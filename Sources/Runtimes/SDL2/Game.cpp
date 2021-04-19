#include "Game.h"
#include "Runtimes/SDL2/SDL2Core.h"
#include "Runtimes/SDL2/Settings.h"
#include "Runtimes/SDL2/Cpu.h"
#include "Runtimes/SDL2/Screen.h"

const int TicksPerFrame = 1000 / 50;

Game::Game()
    : mSDL2(std::make_unique<SDL2Core>())
    , mSettings(std::make_unique<Settings>())
    , mScreen(std::make_unique<Screen>())
    , mCpu(std::make_unique<Cpu>(mScreen.get()))
    , mLastFrameTicks(0)
{
    createWindow();
}

Game::~Game()
{
    destroyWindow();
}

void Game::createWindow()
{
    int zoom = mSettings->screenZoom();
    mBorderSize = (mSettings->screenBorder() ? 2 * BORDER_SIZE : 0);
    mSDL2->createWindow(SCREEN_WIDTH * zoom + mBorderSize, SCREEN_HEIGHT * zoom + mBorderSize, mSettings->fullScreen());
    mSDL2->createRenderer();
}

void Game::destroyWindow()
{
    mSDL2->destroyRenderer();
    mSDL2->destroyWindow();
}

void Game::runFrame()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                throw QuitGame();
        }
    }

    Uint32 ticks = SDL_GetTicks();
    if (ticks < mLastFrameTicks) // check for overflow
        mLastFrameTicks = ticks;

    while (ticks - mLastFrameTicks >= TicksPerFrame) {
        mCpu->runFrame();
        mLastFrameTicks += TicksPerFrame;
    }

    int w = 0, h = 0;
    mSDL2->beginFrame(&w, &h);
    mScreen->draw();
    mSDL2->endFrame();
}
