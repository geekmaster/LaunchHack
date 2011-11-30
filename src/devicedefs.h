#ifndef DEVICEDEFS_H
#define DEVICEDEFS_H

#include <climits>

namespace lhack {


// Kindle DX parameters
struct KDXDimensions {
    // Whole screen dimensions
    static const int kScreenWidth = 824;
    static const int kScreenHeight = 1200;

    // ... and bits per pixel
    static const int kBPP = 4;

    // Number of titles per page when browsing a collection
    static const int kEntryPerPgCol = 15;

    // Number of titles/page when NOT browsing a collection
    static const int kEntryPerPg = 16;

    // Vertical offset of the topmost pixel of the 1st title,
    // when the contents of a collection are shown
    static const int kOffsetYCol = 161;

    // Vertical offset of the topmost pixel of the 1st title,
    // when NOT browsing a collection
    static const int kOffsetY = 101;

    // The vertical offset of the collection's name underline
    static const int kOffsetUlineCol = 122;

    // Offset from the left margin of the first char in a title
    static const int kOffsetX = 61;

    // The gap between the underline of one title and the topmost pixels
    // of the next.
    static const int kEntryGap = 36;

    // Maximum length of a title in pixels
    static const int kEntryLen = 720;

    // Maximum height of the title font
    static const int kFontHeight = 16;

    // Vertical distance in pixels from the bottommost pixel in a title to
    // the bottommost pixel of the underlining squares
    static const int kUlineBaseOffset = 12;

    // The minimum distance from the font's baseline to the topmost pixels of the underline
    static const int kUlineMinOffset = 7;

    // The side length of the (bigger) underlining squares
    //static const int kUlineSquareSz = 3;

    // The color of the solid underline selection pointer
    static const unsigned kUlineColor = UINT_MAX;

    // Maximum gap between words in a title beyond which next words are considered to be metadata
    static const int kMaxBBGap = 33;
};

};
#endif // DEVICEDEFS_H
