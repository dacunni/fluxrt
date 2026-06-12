#ifndef __TEXTOVERLAY_H__
#define __TEXTOVERLAY_H__

#include <string>
#include <vector>

#include "image.h"
#include "color.h"

namespace textoverlay {

// Draws a single line of text into the image with its top-left corner at (x, y).
// Pixels of the glyph strokes are set to the given color.
void drawText(Image<float> & image, int x, int y, const std::string & text,
               const ColorRGB & color = ColorRGB::WHITE(), float scale = 2.0f);

// Draws a block of lines of text over a filled background box in the
// bottom-left corner of the image, for annotating output images with
// metadata such as date/time and commit hash.
void annotateImage(Image<float> & image, const std::vector<std::string> & lines,
                    float scale = 2.0f);

} // namespace textoverlay

#endif
