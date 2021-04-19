#include "Game.h"
#include "Emulator/Z80Screen.h"
#include "Emulator/Emulator.h"
#include "Emulator/Snapshot.h"
#include "Runtimes/SDL2/SDL2Core.h"
#include "Runtimes/SDL2/Settings.h"
#include "Runtimes/SDL2/GameData.h"

const int TicksPerFrame = 1000 / 50;

Game::Game()
    : mSDL2(std::make_unique<SDL2Core>())
    , mSettings(std::make_unique<Settings>())
    , mEmulator(std::make_unique<Emulator>())
    , mZoom(0)
    , mBorderSize(0)
    , mLastFrameTicks(0)
{
    createWindow();
    mEmulator->loadSnapshot(&initState, initMemory);
}

Game::~Game()
{
    destroyWindow();
}

void Game::createWindow()
{
    mZoom = mSettings->screenZoom();
    mBorderSize = (mSettings->screenBorder() ? 2 * BORDER_SIZE : 0);
    mSDL2->createWindow(
        SCREEN_WIDTH * mZoom + mBorderSize, SCREEN_HEIGHT * mZoom + mBorderSize, mSettings->fullScreen());
    mSDL2->createRenderer();
    mSDL2->createScreenBuffer(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void Game::destroyWindow()
{
    mSDL2->destroyScreenBuffer();
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
        mEmulator->runFrame();
        mLastFrameTicks += TicksPerFrame;
    }

    int w = 0, h = 0;
    mSDL2->beginFrame(&w, &h);

    int pitch = 0;
    void* pixels = mSDL2->lockScreenBuffer(&pitch);
    mEmulator->screen()->draw(reinterpret_cast<uint8_t*>(pixels), pitch);
    mSDL2->unlockScreenBuffer();

    int scrW = SCREEN_WIDTH * mZoom;
    int scrH = SCREEN_HEIGHT * mZoom;
    int scrX = (w - scrW) / 2;
    int scrY = (h - scrH) / 2;
    mSDL2->drawScreenBuffer(scrX, scrY, scrW, scrH);

    mSDL2->endFrame();
}
