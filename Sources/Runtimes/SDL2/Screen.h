#ifndef RUNTIMES_SDL2_SCREEN_H
#define RUNTIMES_SDL2_SCREEN_H

#include "Runtimes/SDL2/Common.h"

class Screen
{
public:
    Screen();

    void setUseShadowScreen(bool flag) { mUseShadowScreen = flag; }

    void draw(uint8_t* pixels, int pitch) const;

private:
    bool mUseShadowScreen;

    DISABLE_COPY(Screen);
};

#endif
