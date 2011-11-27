#include <iostream>
#include <fstream>

#include "framegrabber.h"


int main(int argc, char **argv)
{
    using namespace lhack;

    const char *fbdev = "/home/vassil/devel/vision/ocr/images/kindle/1.gray";
    FrameGrabber<KDXDimensions> fgrab(fbdev);
    Bitmap title = fgrab.GrabSelected(8);
    std::cout << "Bitmap valid? " << title.IsValid() << std::endl;
#ifdef LHACK_DEBUG_GRABBER
    if (title.IsValid()) {
        char dmpname[80];
        snprintf(dmpname, 80, "titledump-%dx%d.gray", title.width(), title.height());
        std::ofstream bmdump(dmpname);
        bmdump.write(title.buffer(), title.height()*title.width());
        bmdump.close();
    }
#endif
}
