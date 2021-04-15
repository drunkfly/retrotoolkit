#include "SDL2Core.h"

SDL2Core::SDL2Core()
    : mWindow(nullptr)
    , mRenderer(nullptr)
{
    int r = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    if (r != 0) {
        std::stringstream ss;
        ss << "Unable to initialize SDL: " << SDL_GetError();
        throw std::runtime_error(ss.str());
    }
}

SDL2Core::~SDL2Core()
{
    destroyRenderer();
    destroyWindow();
    SDL_Quit();
}

void SDL2Core::createWindow(int width, int height, bool fullscreen)
{
    destroyWindow();

    Uint32 flags = 0;
    if (fullscreen)
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_INPUT_GRABBED | SDL_WINDOW_ALLOW_HIGHDPI;

    SDL_ClearError();
    mWindow = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
    if (!mWindow) {
        std::stringstream ss;
        ss << "Unable to initialize video mode " << width << 'x' << height;
        ss << '(' << (fullscreen ? "fullscreen" : "windowed") << "): " << SDL_GetError();
        throw std::runtime_error(ss.str());
    }
}

void SDL2Core::destroyWindow()
{
    destroyRenderer();

    if (mWindow) {
        SDL_DestroyWindow(mWindow);
        mWindow = nullptr;
    }
}

void SDL2Core::createRenderer()
{
    destroyRenderer();

    SDL_ClearError();
    mRenderer = SDL_CreateRenderer(mWindow, -1, 0);
    if (!mRenderer) {
        std::stringstream ss;
        ss << "Unable to initialize renderer: " << SDL_GetError();
    }
}

void SDL2Core::destroyRenderer()
{
    if (mRenderer) {
        SDL_DestroyRenderer(mRenderer);
        mRenderer = nullptr;
    }
}

void SDL2Core::beginFrame(int* width, int* height)
{
    int r = SDL_GetRendererOutputSize(mRenderer, width, height);
    if (r != 0) {
        std::stringstream ss;
        ss << "Unable to get renderer output size: " << SDL_GetError();
        throw std::runtime_error(ss.str());
    }

    SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, 255);
    SDL_RenderClear(mRenderer);
}

void SDL2Core::endFrame()
{
    SDL_RenderPresent(mRenderer);
}
