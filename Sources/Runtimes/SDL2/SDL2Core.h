#ifndef RUNTIMES_SDL2_SDL2CORE_H
#define RUNTIMES_SDL2_SDL2CORE_H

#include "Runtimes/SDL2/Common.h"

class SDL2Core
{
public:
    SDL2Core();
    ~SDL2Core();

    void createWindow(int width, int height, bool fullscreen);
    void destroyWindow();

    void createRenderer();
    void destroyRenderer();

    void createScreenBuffer(int width, int height);
    void destroyScreenBuffer();
    void* lockScreenBuffer(int* pitch);
    void unlockScreenBuffer();

    void beginFrame(int* width, int* height);
    void drawScreenBuffer(int x, int y, int w, int h);
    void endFrame();

private:
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;
    SDL_Texture* mScreenBuffer;
    bool mScreenBufferLocked;

    DISABLE_COPY(SDL2Core);
};

#endif
