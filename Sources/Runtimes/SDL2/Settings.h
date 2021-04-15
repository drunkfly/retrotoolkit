#ifndef RUNTIMES_SDL2_SETTINGS_H
#define RUNTIMES_SDL2_SETTINGS_H

#include "Runtimes/SDL2/Common.h"

class Settings
{
public:
    Settings();
    ~Settings();

    bool screenBorder() const { return mScreenBorder; }
    int screenZoom() const { return mScreenZoom; }
    bool fullScreen() const { return false; } // FIXME

private:
    bool mScreenBorder;
    int mScreenZoom;

    DISABLE_COPY(Settings);
};

#endif
