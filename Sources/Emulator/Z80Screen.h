#ifndef EMULATOR_SCREEN_H
#define EMULATOR_SCREEN_H

#include "Emulator/Common.h"

class Z80Screen
{
public:
    static const uint8_t palette[];

    explicit Z80Screen(Emulator* emulator);

    void setBorderColor(uint8_t color) { mBorderColor = color; }
    void setUseShadowScreen(bool flag) { mUseShadowScreen = flag; }

    void onScreenInterrupt() { mFrameCounter++; }

    void draw(uint8_t* pixels, int pitch, bool inactiveScreen = false) const;

private:
    Emulator* mEmulator;
    uint8_t mBorderColor;
    int mFrameCounter;
    bool mUseShadowScreen;

    DISABLE_COPY(Z80Screen);
};

#endif
