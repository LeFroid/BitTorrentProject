#include <iostream>

#include <SDL.h>

#include "Engine.h"
#include "LogHelper.h"

#include "bencoding/BenDictionary.h"
#include "bencoding/BenString.h"
#include "bencoding/BenInt.h"
#include "bencoding/BenList.h"

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

    BenString listStrA("String A");
    BenString listStrB("String B");
    BenList list;
    list.push_back(&listStrA);
    list.push_back(&listStrB);

    BenDictionary dict;
    dict["str_key"] = &str;
    dict["int_key"] = &intVal;
    dict["list_key"] = &list;

    LOG_INFO("client", "dict[\"str_key\"] = ", static_cast<BenString*>(dict["str_key"])->getValue());
    LOG_INFO("client", "dict[\"int_key\"] = ", static_cast<BenInt*>(dict["int_key"])->getValue());

    BenList *listPtr = static_cast<BenList*>(dict["list_key"]);
    for (auto it = listPtr->cbegin(); it != listPtr->cend(); ++it)
    {
        LOG_INFO("client", "dict[\"list_key\"]: list const_iterator pointing to: ", static_cast<BenString* const>(*it)->getValue());
    }

    Engine::halt();

    return 0;
}
