What is this?
-------------
This kludge can be used along with LaunchPad to initiate some kind of action on the file which is currently selected in Kindle's stock shell. Currently works only with Kindle DXG, but I believe it can be relatively easily modified for the other models(although I don't own and have no experience with the smaller devices).

How it works?
-------------
The program has three main steps/parts:

* First the framebuffer is scanned to find the bold underline of the currently selected file and the title of this file is cropped and is sent to Tesseract OCR engine

* Tesseract then converts the image to a string

* Finally the OCR result is approximately matched to (a subset) of the files and the best-matching file's absolute path is written to the standard output. The matching algorithm is using the ideas from [SimString](http://www.chokkan.org/software/simstring/) 

How to run?
-----------
This tool is mostly intended to be used in launchpad-initiated scripts. It takes three parameters:

* The root directory to be searched for a matching file

* A comma-separated list of filters which could be used to narrow the search to specific types of files (e.g "*.pdf,*.mobi")

* A similarity coefficient in the range 0.0 - 1.0. This number could be thought of as the minimum fraction of overlapping letter tri-grams between the OCR derived string and matched files. If this coefficient is too low, the search will be slower, and if too high the true matching file can be rejected/not found. About 0.5-0.6 seems to work OK.