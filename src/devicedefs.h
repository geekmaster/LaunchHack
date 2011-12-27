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

// Kindle 3 Keyboard parameters
struct K3Dimensions {
    // Whole screen dimensions
    static const int kScreenWidth = 600;
    static const int kScreenHeight = 800;

    // ... and bits per pixel
    static const int kBPP = 4;

    // Number of titles per page when browsing a collection
    static const int kEntryPerPgCol = 9;

    // Number of titles/page when NOT browsing a collection
    static const int kEntryPerPg = 10;

    // Vertical offset of the topmost pixel of the 1st title,
    // when the contents of a collection are shown
    static const int kOffsetYCol = 159;

    // Vertical offset of the topmost pixel of the 1st title,
    // when NOT browsing a collection
    static const int kOffsetY = 100;

    // The vertical offset of the collection's name underline
    static const int kOffsetUlineCol = 125;

    // Offset from the left margin of the first char in a title
    static const int kOffsetX = 60;

    // The gap between the underline of one title and the topmost pixels
    // of the next.
    static const int kEntryGap = 34;

    // Maximum length of a title in pixels
    static const int kEntryLen = 500;

    // Maximum height of the title font
    static const int kFontHeight = 18;

    // Vertical distance in pixels from the bottommost pixel in a title to
    // the bottommost pixel of the underlining squares
    static const int kUlineBaseOffset = 12;

    // The minimum distance from the font's baseline to the topmost pixels of the underline
    static const int kUlineMinOffset = 7;

    // The color of the solid underline selection pointer
    static const unsigned kUlineColor = UINT_MAX;

    // Maximum gap between words in a title beyond which next words are considered to be metadata
    static const int kMaxBBGap = 40;
};

};
#endif // DEVICEDEFS_H
