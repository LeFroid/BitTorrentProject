#include <iostream>

#include <SDL.h>

#include "Engine.h"
#include "LogHelper.h"

#include "bencoding/BenDictionary.h"
#include "bencoding/BenString.h"
#include "bencoding/BenInt.h"
using namespace bencoding;

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

    // bencoding test
    BenString str("Hello, World!");
    BenInt intVal(42);

    BenDictionary dict;
    dict["str_key"] = &str;
    dict["int_key"] = &intVal;

    LOG_INFO("client", "dict[\"str_key\"] = ", static_cast<BenString*>(dict["str_key"])->getValue());
    LOG_INFO("client", "dict[\"int_key\"] = ", static_cast<BenInt*>(dict["int_key"])->getValue());

    Engine::halt();

    return 0;
}
