What is it?
-----------
This kludge can be used along with LaunchPad to initiate some kind of action on the file which is currently selected in Kindle's stock shell. Currently works only with Kindle DXG, but I believe it can be relatively easily modified for the other models(although I don't own and have no experience with the smaller devices).

How it works?
------------
The program has three main steps/parts:

* First the framebuffer is scanned to find the bold underline of the currently selected file and the title of this file is cropped and is sent to Tesseract OCR engine

* Tesseract then converts the image to a string

* Finally the OCR result is approximately matched to (a subset) of the files and the best-matching file's absolute path is written to the standard output. The matching algorithm is using the ideas from [SimString](http://www.chokkan.org/software/simstring/) 
