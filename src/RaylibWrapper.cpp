#include <raylib.h>

#include "RaylibWrapper.h"

Color ColorEnumToRaylib(RaylibWrapper::ColorEnum color)
{
    switch(color) {
        case RaylibWrapper::ColorEnum::RAYLIB_WHITE: return WHITE;
        case RaylibWrapper::ColorEnum::RAYLIB_BLACK: return BLACK;
        default: return BLACK;
    }
}

void RaylibWrapper::InitWindow(int width, int height, const char *title)
{
    ::InitWindow(width, height, title);
}

void RaylibWrapper::CloseWindow(void) { ::CloseWindow(); }
bool RaylibWrapper::WindowShouldClose(void) { return ::WindowShouldClose(); }
void RaylibWrapper::SetTargetFPS(int fps) { ::SetTargetFPS(fps); }
void RaylibWrapper::ClearBackground(ColorEnum color) { ::ClearBackground(ColorEnumToRaylib(color)); }
void RaylibWrapper::BeginDrawing(void) { ::BeginDrawing(); }
void RaylibWrapper::EndDrawing(void) { ::EndDrawing(); }
void RaylibWrapper::DrawText(const char *text, int posX, int posY, int fontSize, ColorEnum color)
{
    ::DrawText(text, posX, posY, fontSize, ColorEnumToRaylib(color));
}

