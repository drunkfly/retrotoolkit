#ifndef RUNTIMES_SDL2_GAME_H
#define RUNTIMES_SDL2_GAME_H

#include "Runtimes/SDL2/Common.h"

class Game
{
public:
    Game();
    ~Game();

    void createWindow();
    void destroyWindow();

    void runFrame();

private:
    std::unique_ptr<SDL2Core> mSDL2;
    std::unique_ptr<Settings> mSettings;
    int mBorderSize;
    Uint32 mLastFrameTicks;

    DISABLE_COPY(Game);
};

#endif
