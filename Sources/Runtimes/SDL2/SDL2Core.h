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

    void beginFrame(int* width, int* height);
    void endFrame();

private:
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;

    DISABLE_COPY(SDL2Core);
};

#endif
