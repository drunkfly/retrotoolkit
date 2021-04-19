#ifndef RUNTIMES_SDL2_GAME_H
#define RUNTIMES_SDL2_GAME_H

#include "Runtimes/SDL2/Common.h"

class Cpu;
class Screen;

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
    std::unique_ptr<Screen> mScreen;
    std::unique_ptr<Cpu> mCpu;
    int mZoom;
    int mBorderSize;
    Uint32 mLastFrameTicks;

    DISABLE_COPY(Game);
};

#endif
