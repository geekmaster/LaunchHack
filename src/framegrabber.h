/*
*   Copyright 2011 Vassil Panayotov <vd.panayotov@gmail.com>
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*/

#ifndef FRAMEGRABBER_H
#define FRAMEGRABBER_H

#include <string>
#include <fstream>

#include "linux/fb.h"
#include "devicedefs.h"

namespace lhack {


// A bitmap to be passed around ...
struct Bitmap {
    Bitmap(int width, int height, int bpp):
        width_(width), height_(height), bpp_(bpp)
    {
        buffer_ = new char[((width*bpp)/8) * height];
        refcnt_ = new int;
        *refcnt_ = 1;
    }

    Bitmap(const Bitmap& other)
    {
        this->buffer_ = other.buffer_;
        this->width_ = other.width_;
        this->height_ = other.height_;
        this->bpp_ = other.bpp_;

        this->refcnt_ = other.refcnt_;
        ++ *refcnt_;
    }

    Bitmap& operator=(const Bitmap& other)
    {
        if (! (-- *refcnt_)) {
            delete[] buffer_;
            delete refcnt_;
        }

        this->buffer_ = other.buffer_;
        this->width_ = other.width_;
        this->height_ = other.height_;
        this->bpp_ = other.bpp_;

        this->refcnt_ = other.refcnt_;
        ++ *refcnt_;
    }

    void SetInvalid()
    {
        width_ = -1;
    }

    bool IsValid()
    {
        return width_ != -1;
    }

    ~Bitmap()
    {
        if (! (-- *refcnt_)) {
            delete[] buffer_;
            delete refcnt_;
        }
    }

    int width() { return width_; }
    int height() { return height_; }
    int bpp() { return bpp_; }
    char* buffer() {return buffer_; }

private:
    int width_;
    int height_;
    int bpp_;
    char *buffer_;
    int *refcnt_;
};

/**
 * Reads the frame buffer and finds the entries there.
 **/
template <typename DIM=KDXDimensions >
class FrameGrabber
{
public:
    FrameGrabber(const char *fbdev): fbdev_(fbdev) {};

    /**
     * Try to find the selected (underlined) title and return its bitmap image.
     * Check the result using IsValid() method of Bitmap.
     */
    Bitmap GrabSelected();

private:
    const char *fbdev_;
};

template <typename DIM >
Bitmap FrameGrabber<DIM>::GrabSelected()
{
    using namespace std;

#ifdef LHACK_DEBUG_GRABBER
    int chkline_idx = 1;
#endif

    int i, j;
    int bytes_row = (DIM::kScreenWidth * DIM::kBPP) / 8;
    int bytes_skip =  bytes_row * DIM::kOffsetYCol; // skip the lines before the 1st title
    int bytes_entry = bytes_row * (DIM::kFontHeight + DIM::kUlineBaseOffset + DIM::kEntryGap);
    int off_uline = bytes_row * (DIM::kFontHeight + DIM::kUlineBaseOffset + 1);
    int bytes_row_title = (DIM::kEntryLen * DIM::kBPP) / 8;
    int title_height = DIM::kFontHeight + DIM::kUlineMinOffset;

    int check_start = ((DIM::kOffsetX + 2*sizeof(unsigned))*DIM::kBPP) / (8 * sizeof(unsigned));
    int check_len = ((DIM::kEntryLen - 2*sizeof(unsigned))*DIM::kBPP) / (8 * sizeof(unsigned));

    char *linebuf = new char[bytes_row];

    Bitmap title(DIM::kEntryLen, title_height, 8);

    ifstream fb(fbdev_, ios::in | ios::binary);

    // Check whether we are browsing a collection
    // (relies on the fact that the collection name's underline is longer)
    int entries_page = DIM::kEntryPerPgCol;
    fb.seekg(bytes_row * DIM::kOffsetUlineCol);
    fb.read(linebuf, bytes_row);
    for (i = check_start - 4; i < check_start + check_len + 3; i++) {
        if (((unsigned*) linebuf)[i] != DIM::kUlineColor) {
            entries_page = DIM::kEntryPerPg;
            bytes_skip = bytes_row * DIM::kOffsetY;
            break;
        }
    }

#ifdef LHACK_DEBUG_GRABBER
    {
        std::cout << "Bytes per line: " << bytes_row << std::endl;
        std::cout << "Collection name underline offset: " << (bytes_row * DIM::kOffsetUlineCol) << std::endl;
        std::cout << "Entries per page: " << entries_page << std::endl;
        std::cout << "Check length: " << check_len << std::endl;
        ofstream chkdump("chkcollection.gray", ios::out | ios::binary);
        chkdump.write(linebuf, bytes_row);
        chkdump.close();

    }
#endif


    for (i = 0; i < entries_page; i++) {
        // Check for the solid underline
        fb.seekg(bytes_skip + off_uline, ios::beg);
        fb.read(linebuf, bytes_row);
#ifdef LHACK_DEBUG_GRABBER
        {
            char dumpfile[80];
            snprintf(dumpfile, 80, "chkline%02d.gray", chkline_idx++);
            ofstream chkdump(dumpfile, ios::out | ios::binary);
            chkdump.write(linebuf, bytes_row);
            chkdump.close();
        }
#endif
        for (j = check_start; j < check_start + check_len; j++) {
            if (((unsigned*) linebuf)[j] != DIM::kUlineColor) {
                break;
            }
        }

        // Selected item has been found
        if (j == check_start + check_len) {
            bytes_skip += (DIM::kOffsetX * DIM::kBPP) / 8;
            fb.seekg(bytes_skip, ios::beg);

            char *buffer = title.buffer();
            int buff_off = 0;
            for (int k = 0; k < title_height; k++) {
                fb.read(linebuf, bytes_row_title);

                // Convert the bit-depth. Assumes a 4bpp FB and 8bpp target
                for (int h = 0; h < bytes_row_title; h++) {
                    buffer[buff_off++] = 0xf0 & linebuf[h];
                    buffer[buff_off++] = linebuf[h] << 4;
                }

                bytes_skip += bytes_row;
                fb.seekg(bytes_skip, ios::beg);
            }

            break;
        }

        bytes_skip += bytes_entry;
    }

    if (i == DIM::kEntryPerPg)
        title.SetInvalid();

    delete[] linebuf;
    fb.close();

    return title;
}

}; // namespace
#endif // FRAMEGRABBER_H
