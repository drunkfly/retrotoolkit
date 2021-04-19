#include "SDL2Core.h"
#include "Runtimes/SDL2/Game.h"
#include "Runtimes/SDL2/GameData.h"
#include "Emulator/Snapshot.h"
#include <SDL_main.h>

static std::unique_ptr<Game> game;

static void quit(int code)
{
    game.reset();

  #ifdef __EMSCRIPTEN__
    SDL_ExitProcess(code);
  #else
    exit(code);
  #endif
}

static void error(const char* message)
{
    game.reset();

    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", message);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message, nullptr);

    quit(1);
}

static void runFrame()
{
    try {
        if (!game)
            game = std::make_unique<Game>();
        game->runFrame();
    } catch (const QuitGame&) {
        quit(0);
    } catch (const std::exception& e) {
        error(e.what());
    }
}

int main(int argc, char** argv)
{
  #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(runFrame, 0, TRUE);
  #else
   #ifndef NDEBUG
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            loadSnapshot(argv[i], initState, initMemory);
            break;
        }
    }
   #endif

    for (;;)
        runFrame();
  #endif
}
