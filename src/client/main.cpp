#include <iostream>

#include <SDL.h>

#include "Engine.h"
#include "LogHelper.h"

using namespace gui;

// Log helper
LogHelper sLog;

int main(int argc, char **argv)
{
    if (!Engine::initialize())
    {
        LOG_FATAL("client", "Graphics library has failed to initialize. SDL Error: ", SDL_GetError());
        return 1;
    }

    Engine::halt();

    return 0;
}
