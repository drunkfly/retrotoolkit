#ifndef RUNTIMES_SDL2_SCREEN_H
#define RUNTIMES_SDL2_SCREEN_H

#include "Runtimes/SDL2/Common.h"

class Screen
{
public:
    Screen();

    void setBorderColor(uint8_t color) { mBorderColor = color; }
    void setUseShadowScreen(bool flag) { mUseShadowScreen = flag; }

    void onScreenInterrupt() { mFrameCounter++; }

    void draw(uint8_t* pixels, int pitch) const;

private:
    uint8_t mBorderColor;
    int mFrameCounter;
    bool mUseShadowScreen;

    DISABLE_COPY(Screen);
};

#endif
