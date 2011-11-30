#include <iostream>
#include <fstream>

#include "framegrabber.h"
#include "ocr.h"

int main(int argc, char **argv)
{
    using namespace lhack;

    if (argc < 2) {
        std::cerr << "You should provide a gray image or fbdev as parameter" << std::endl;
        return 2;
    }
    //const char *fbdev = "/home/vassil/devel/vision/ocr/images/kindle/1.gray";
    //const char *fbdev = "/dev/fb/0";
    FrameGrabber<KDXDimensions> fgrab(argv[1]);
    Bitmap image = fgrab.GrabSelected(8);
    std::cout << "Bitmap valid? " << image.IsValid() << std::endl;
#ifdef LHACK_DEBUG_GRABBER
    if (title.IsValid()) {
        char dmpname[80];
        snprintf(dmpname, 80, "titledump-%dx%d.gray", image.width(), image.height());
        std::ofstream bmdump(dmpname);
        bmdump.write(image.buffer(), image.height()*image.width());
        bmdump.close();
    }
#endif

    Recognizer<KDXDimensions> ocr("/mnt/x86/share", "eng");
    std::cout << ocr.Recognize(image) << std::endl;

    return 0;
}
