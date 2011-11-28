#include <iostream>
#include <tesseract/baseapi.h>

#include "ocr.h"

namespace lhack {

std::string RecognizeTitle(Bitmap& bmTitle)
{
    tesseract::TessBaseAPI api;

    api.Init("/mnt/x86/share", "eng");
    api.SetPageSegMode(tesseract::PSM_AUTO);
    api.SetImage((const unsigned char *) (bmTitle.buffer()),
                 bmTitle.width(), bmTitle.height(),
                 1, bmTitle.width());
    int rec_failure = api.Recognize(0);
    if (!rec_failure) {
        std::cout << "Recognized: " << api.GetUTF8Text() << std::endl;
    }
    else {
        std::cout << "Recognition failure" << std::endl;
    }

    return std::string(api.GetUTF8Text());
}

}
