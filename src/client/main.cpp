#include <iostream>

#include <SDL.h>

#include "Engine.h"
#include "LogHelper.h"

#include "bencoding/BenDictionary.h"
#include "bencoding/BenString.h"
#include "bencoding/BenInt.h"
#include "bencoding/BenList.h"
#include "bencoding/Decoder.h"
#include "bencoding/Encoder.h"

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
    Decoder decoder;
    std::string bencodedStr = "d4:spami42ee";
    auto benPtr = decoder.decode(bencodedStr);
    auto dictPtr = std::static_pointer_cast<BenDictionary>(benPtr);
    LOG_INFO("client", "size of dictionary = ", dictPtr->size());
    LOG_INFO("client", "value for key \"spam\" in dictionary = ", static_cast<BenInt*>(((*dictPtr)["spam"]).get())->getValue());
    
    std::shared_ptr<BenObjectBase> str(std::make_shared<BenString>("Hello, World!"));
    std::shared_ptr<BenObjectBase> intVal(std::make_shared<BenInt>(42));

    auto list = std::make_shared<BenList>();
    list->push_back(str);
    list->push_back(intVal);

    BenDictionary dict;
    dict["str_key"] = intVal;
    dict["list_key"] = list; 

    Encoder encoder;
    dict.accept(encoder);
    LOG_INFO("client", "encoded dictionary represented as: ", encoder.getData());

    Engine::halt();

    return 0;
}
