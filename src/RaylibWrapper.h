#ifndef __RAYLIB_WRAPPER_H__
#define __RAYLIB_WRAPPER_H__

struct RaylibWrapper
{
    enum class ColorEnum : int {
        RAYLIB_WHITE,
        RAYLIB_BLACK
    };
    // Taken from raylib.h
    static void InitWindow(int width, int height, const char *title);  // Initialize window and OpenGL context
    static void CloseWindow(void);                                     // Close window and unload OpenGL context
    static bool WindowShouldClose(void);                               // Check if application should close (KEY_ESCAPE pressed or windows close icon clicked)
    static void SetTargetFPS(int fps);                                 // Set target FPS (maximum)
    static void ClearBackground(ColorEnum color);                          // Set background color (framebuffer clear color)
    static void BeginDrawing(void);                                    // Setup canvas (framebuffer) to start drawing
    static void EndDrawing(void);                                      // End canvas drawing and swap buffers (double buffering)
    static void DrawText(const char *text, int posX, int posY, int fontSize, ColorEnum color);       // Draw text (using default font)


};

#endif
