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

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "framegrabber.h"
#include "ocr.h"
#include "filematch.h"

int main(int argc, char **argv)
{
    using namespace lhack;
    if (argc < 4) {
        std::cerr << "Syntax: lhack rootdir comma-sep-filters similarity-coeff" << std::endl;
        return 2;
    }

    const char *fbdev = "/dev/fb/0";
#ifdef LHACK_DEVEL_HOST
    if (argc < 5) {
        std::cerr << "You must provide a grayscale image as the final arg. to be used instead of fbdev\n";
    }
    fbdev = argv[4];
#endif
    FrameGrabber<KDXDimensions> fgrab(fbdev);
    Bitmap image = fgrab.GrabSelected();
    if (!image.IsValid())
        return 2;

#ifdef LHACK_DEBUG_GRABBER
    if (title.IsValid()) {
        char dmpname[80];
        snprintf(dmpname, 80, "titledump-%dx%d.gray", image.width(), image.height());
        std::ofstream bmdump(dmpname);
        bmdump.write(image.buffer(), image.height()*image.width());
        bmdump.close();
    }
#endif

#ifdef LHACK_DEVEL_HOST
    Recognizer<KDXDimensions> ocr("/mnt/x86/share", "eng");
#else
    Recognizer<KDXDimensions> ocr("/mnt/us/launchpad/share", "eng");
#endif
    string ocr_result = ocr.Recognize(image);

    std::vector<std::string> filters;
    char* fbegin = argv[2];
    char* fend = fbegin;
    while (*fend) {
        if (*fend == ',') {
            filters.push_back(string(fbegin, fend));
            fbegin = ++fend;
        }
        else {
            ++ fend;
        }
    }
    if (fend > fbegin)
        filters.push_back(string(fbegin, fend));

    if (filters.empty())
        return 2;

    string match = Search(argv[1], filters, ocr_result, atof(argv[3]));
    if (match.empty())
        return 3;

    std::cout << match << std::endl;

    return 0;
}
