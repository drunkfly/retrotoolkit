#include "Screen.h"
#include "Runtimes/SDL2/Memory.h"

static const uint8_t palette[] = {
        0x00, 0x00, 0x00,
        0x00, 0x00, 0xc0,
        0xc0, 0x00, 0x00,
        0xc0, 0x00, 0xc0,
        0x00, 0xc0, 0x00,
        0x00, 0xc0, 0xc0,
        0xc0, 0xc0, 0x00,
        0xc0, 0xc0, 0xc0,
        0x00, 0x00, 0x00,
        0x00, 0x00, 0xff,
        0xff, 0x00, 0x00,
        0xff, 0x00, 0xff,
        0x00, 0xff, 0x00,
        0x00, 0xff, 0xff,
        0xff, 0xff, 0x00,
        0xff, 0xff, 0xff,
    };

Screen::Screen()
    : mBorderColor(0)
    , mFrameCounter(0)
    , mUseShadowScreen(false)
{
}

void Screen::draw(uint8_t* pixels, int pitch) const
{
    int bank = (mUseShadowScreen ? 7 : 5);
    const uint8_t* memPixels = &memory[bank * 16384];
    const uint8_t* memAttrib = pixels + 0x1800;

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            uint8_t attr = memAttrib[(y >> 3) * (SCREEN_WIDTH / 8) + (x >> 3)];
            uint8_t byte = memPixels[y * (SCREEN_WIDTH / 8) + (x >> 3)];

            bool pixel = (byte & (1 << (7 - (x & 7)))) != 0;
            if (attr & 0x80) { // flash?
                if (mFrameCounter & 32)
                    pixel = !pixel;
            }

            uint8_t color;
            if (pixel)
                color = attr & 7; // ink
            else
                color = (attr >> 3) & 7; // paper

            if (attr & 0x40) // bright?
                color |= 8;

            const uint8_t* c = &palette[color * 3];
            int off = y * pitch + (x << 2);
            pixels[off + 0] = c[2];
            pixels[off + 1] = c[1];
            pixels[off + 2] = c[0];
            pixels[off + 3] = 0xff;
        }
    }
}
