#ifndef OCR_H
#define OCR_H

#include <string>
#include <tesseract/baseapi.h>
#include <tesseract/resultiterator.h>
#include "framegrabber.h"

namespace lhack {

using namespace std;

template <typename DIM=KDXDimensions >
class Recognizer
{
public:
    Recognizer(string modeldir, string lang) :
        modeldir_(modeldir), lang_(lang) {}

    // Recognizes the title and filters the metadata
    // i.e. returns only the title
    string Recognize(Bitmap& image);

private:
    string modeldir_;
    string lang_;
};

template <typename DIM >
string Recognizer<DIM>::Recognize(Bitmap& image)
{
    tesseract::TessBaseAPI api;
    api.Init(modeldir_.c_str(), lang_.c_str());
    api.SetImage((const unsigned char*)image.buffer(),
                 image.width(), image.height(), 1, image.width());
    api.Recognize(0);
    tesseract::ResultIterator *it = api.GetIterator();

    int top, bottom, left=0, right=0;
    int prev_right = right;
    string result;
    do {
        prev_right = right;
        it->BoundingBox(tesseract::RIL_WORD, &left, &top, &right, &bottom);
        if ((left - prev_right) > DIM::kMaxBBGap) {
            break;
        } else {
            if (prev_right)
                result.append(" ");
            result.append(it->GetUTF8Text(tesseract::RIL_WORD));
        }
     } while (it->Next(tesseract::RIL_WORD));

    delete it;

    return result;
}

}; // namespace lhack

#endif // OCR_H
