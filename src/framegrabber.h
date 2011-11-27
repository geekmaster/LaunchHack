#ifndef FRAMEGRABBER_H
#define FRAMEGRABBER_H

#include <string>
#include <fstream>

namespace lhack {

// Various dimensions relevant to DX devices
struct KDXDimensions {
    // Whole screen dimensions
    static const int kScreenWidth = 824;
    static const int kScreenHeight = 1200;

    // ... and bits per pixel
    static const int kBPP = 4;

    // Number of titles per page
    static const int kEntryPerPg = 15;

    // Vertical offset of the topmost pixel of the 1st title
    static const int kOffsetY = 161;

    // Offset from the left margin of the first char in a title
    static const int kOffsetX = 61;

    // The gap between the underline of one title and the topmost pixels
    // of the next.
    static const int kEntryGap = 36;

    // Maximum length of a title in pixels
    static const int kEntryLen = 700;

    // Maximum height of the title font
    static const int kFontHeight = 16;

    // Vertical distance in pixels from the bottommost pixel in a title to
    // the bottommost pixel of the underlining squares
    static const int kUlineBaseOffset = 12;

    // The minimum distance from the font's baseline to the topmost pixels of the underline
    static const int kUlineMinOffset = 7;

    // The side length of the (bigger) underlining squares
    //static const int kUlineSquareSz = 3;

};

// A bitmap to be passed around ...
struct Bitmap {
    Bitmap(int width, int height, int bpp):
        width_(width), height_(height), bpp_(bpp)
    {
        buffer_ = new char[((width*bpp)/8) * height];
        refcnt_ = new int;
        ++ *refcnt_;
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
     */
    Bitmap GrabSelected(int target_bpp);

private:
    const char *fbdev_;
};

template <typename DIM >
Bitmap FrameGrabber<DIM>::GrabSelected(int target_bpp)
{
    using namespace std;

#ifdef LHACK_DEBUG_GRABBER
    int chkline_idx = 1;
#endif

    int i, j;
    int bytes_row = (DIM::kScreenWidth * DIM::kBPP) / 8;
    int bytes_skip =  bytes_row * DIM::kOffsetY; // skip the lines before the 1st title
    int bytes_entry = bytes_row * (DIM::kFontHeight + DIM::kUlineBaseOffset + DIM::kEntryGap);
    int off_uline = bytes_row * (DIM::kFontHeight + DIM::kUlineBaseOffset + 1);
    int bytes_row_title = (DIM::kEntryLen * DIM::kBPP) / 8;
    int title_height = DIM::kFontHeight + DIM::kUlineMinOffset;

    int check_start = ((DIM::kOffsetX + 2*sizeof(unsigned))*DIM::kBPP) / (8 * sizeof(unsigned));
    int check_len = ((DIM::kEntryLen - 2*sizeof(unsigned))*DIM::kBPP) / (8 * sizeof(unsigned));


    char *linebuf = new char[bytes_row];

    Bitmap title(DIM::kEntryLen, title_height, target_bpp);

    ifstream fb(fbdev_, ios::in | ios::binary);

    for (i = 0; i < DIM::kEntryPerPg; i++) {
        // Check for the solid underline
        fb.seekg(bytes_skip + off_uline, ios::beg);
        fb.read(linebuf, bytes_row);
#ifdef LHACK_DEBUG_GRABBER
        char dumpfile[80];
        snprintf(dumpfile, 80, "chkline%02d.gray", chkline_idx++);
        ofstream chkdump(dumpfile, ios::out | ios::binary);
        chkdump.write(linebuf, bytes_row);
        chkdump.close();
#endif
        for (j = check_start; j < check_start + check_len; j++) {
            if (((unsigned*) linebuf)[j] != 0) {
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

                // Convert the bit-depth (well ... it assumes a 4bpp FB and 8bpp target :)
                for (int h = 0; h < bytes_row_title; h++) {
                    buffer[buff_off++] = 0xf0 & linebuf[h];
                    buffer[buff_off++] = linebuf[h] << 4; // << (target_bpp - DIM::kBPP)
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

    return title;
}

}; // namespace
#endif // FRAMEGRABBER_H
