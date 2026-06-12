#include <algorithm>
#include <cmath>

#include "textoverlay.h"
#include "stb_easy_font.h"

namespace textoverlay {

void drawText(Image<float> & image, int x, int y, const std::string & text,
               const ColorRGB & color, float scale)
{
    // stb_easy_font emits a quad (4 vertices, 16 bytes each: 3 floats + RGBA8)
    // per stroke segment of the text, laid out as if drawn at the origin.
    char vertexBuffer[99999];
    int numQuads = stb_easy_font_print(0, 0, const_cast<char *>(text.c_str()),
                                        nullptr, vertexBuffer, sizeof(vertexBuffer));

    for(int q = 0; q < numQuads; ++q) {
        float minX = std::numeric_limits<float>::max(), maxX = std::numeric_limits<float>::lowest();
        float minY = std::numeric_limits<float>::max(), maxY = std::numeric_limits<float>::lowest();

        for(int v = 0; v < 4; ++v) {
            const char * vptr = vertexBuffer + (q * 4 + v) * 16;
            float vx = *reinterpret_cast<const float *>(vptr + 0);
            float vy = *reinterpret_cast<const float *>(vptr + 4);
            minX = std::min(minX, vx);
            maxX = std::max(maxX, vx);
            minY = std::min(minY, vy);
            maxY = std::max(maxY, vy);
        }

        int px0 = x + (int) std::floor(minX * scale);
        int px1 = x + (int) std::ceil(maxX * scale);
        int py0 = y + (int) std::floor(minY * scale);
        int py1 = y + (int) std::ceil(maxY * scale);

        for(int py = py0; py < py1; ++py) {
            if(py < 0 || py >= (int) image.height) {
                continue;
            }
            for(int px = px0; px < px1; ++px) {
                if(px < 0 || px >= (int) image.width) {
                    continue;
                }
                image.set3(px, py, color);
            }
        }
    }
}

void annotateImage(Image<float> & image, const std::vector<std::string> & lines, float scale)
{
    if(lines.empty()) {
        return;
    }

    const int margin = (int) std::ceil(4 * scale);
    const int lineHeight = (int) std::ceil(12 * scale);

    int maxTextWidth = 0;
    for(const auto & line : lines) {
        int textWidth = stb_easy_font_width(const_cast<char *>(line.c_str()));
        maxTextWidth = std::max(maxTextWidth, (int) std::ceil(textWidth * scale));
    }

    int boxWidth = std::min((int) image.width, maxTextWidth + 2 * margin);
    int boxHeight = std::min((int) image.height, (int) lines.size() * lineHeight + 2 * margin);
    int boxY0 = (int) image.height - boxHeight;

    for(int y = boxY0; y < (int) image.height; ++y) {
        for(int x = 0; x < boxWidth; ++x) {
            image.set3(x, y, ColorRGB::BLACK());
        }
    }

    for(size_t i = 0; i < lines.size(); ++i) {
        drawText(image, margin, boxY0 + margin + (int) i * lineHeight, lines[i], ColorRGB::WHITE(), scale);
    }
}

} // namespace textoverlay
