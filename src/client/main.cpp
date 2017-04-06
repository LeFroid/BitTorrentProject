#include <boost/predef.h>
#include <iostream>

#include <SDL.h>

#include "Configuration.h"
#include "Engine.h"
#include "LogHelper.h"

#include "BenDictionary.h"
#include "BenString.h"
#include "BenInt.h"
#include "BenList.h"
#include "Decoder.h"
#include "Encoder.h"

#include "SHA1Hash.h"
#include "TorrentState.h"
#include "TorrentFile.h"
#include "TorrentMgr.h"

#include "EventHandler.h"
#include "FontStorage.h"
#include "Keybinder.h"
#include "ObjectManager.h"
#include "Window.h"

using namespace bencoding;
using namespace gui;

// Log helper
LogHelper sLog;

// Platform-dependent definitions
#if BOOST_OS_WINDOWS
#  define CLIENT_CONFIG_DIR "./"
#  define CLIENT_FONTS_DIR "./Fonts/"
#else
#  define CLIENT_CONFIG_DIR "../etc/"
#  define CLIENT_FONTS_DIR "../shared/bitclient/Fonts/"
#endif

void loadResources()
{
    // load fonts
    FontStorage::getInstance()->loadFonts(CLIENT_FONTS_DIR);
}

int main(int argc, char **argv)
{
    // Output logs to current directory
    sLog.setLogDir("./");

    if (!Engine::initialize())
    {
        LOG_FATAL("client", "Graphics library has failed to initialize. SDL Error: ", SDL_GetError());
        return 1;
    }

    // Load fonts
    loadResources();

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

    // Read from configuration file
    Configuration config;
    std::string configPath = CLIENT_CONFIG_DIR;
    configPath.append("bitclient.json");
    config.loadFile(configPath);
    boost::optional<int> port = config.getValue<int>("network.listen_port");
    if (port)
        LOG_INFO("client", "port = ", *port);

    // Begin test on torrent file & tracker
    std::shared_ptr<TorrentState> testFile;
    TorrentMgr mgr;
    if (argc > 1)
    {
        std::string torrentFilePath(argv[1]);
        testFile = mgr.addTorrent(torrentFilePath);

        mgr.run();

        LOG_INFO("client", "Torrent file size = ", testFile->getTorrentFile()->getFileSize());
    }
    // End test on torrent file & tracker

    // Spawn an empty window
    Window *win = ObjectManager::getInstance()->createObject<Window>("BitTorrent Client", 1280, 800, false);
    win->draw();
    win->setBackgroundColor(Color(255, 255, 255, 255));

    EventHandler *eventHandler = EventHandler::getInstance();
    eventHandler->setWindowPtr(win);
    eventHandler->pollForEvents();

    FontStorage::destroyInstance();
    //Keybinder::destroyInstance();
    EventHandler::destroyInstance();
    ObjectManager::destroyInstance();

    Engine::halt();

    return 0;
}
