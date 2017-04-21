#include <boost/predef.h>
#include <iostream>

#include <SDL.h>

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

std::string configFile = CLIENT_CONFIG_DIR + std::string("bitclient.json");
TorrentMgr eTorrentMgr(configFile);

void loadResources()
{
    // load fonts
    FontStorage::getInstance()->loadFonts(CLIENT_FONTS_DIR);
}

int main(int argc, char **argv)
{
    // Output logs to current directory
    sLog.setLogDir("./");

    // set log level of network (log file will be enormous if set to debug)
    sLog.get("torrent_protocol.network")->SetLogLevel(LOG_ERROR);

    if (!Engine::initialize())
    {
        LOG_FATAL("client", "Graphics library has failed to initialize. SDL Error: ", SDL_GetError());
        return 1;
    }

    // Load fonts
    loadResources();

    // bencoding test
    /*
    Decoder decoder;
    std::string bencodedStr = "d4:spami42ee";
    LOG_INFO("client.test", "Decoding bencoded string \"", bencodedStr, "\" into Dictionary");
    auto benPtr = decoder.decode(bencodedStr);
    auto dictPtr = std::static_pointer_cast<BenDictionary>(benPtr);
    LOG_INFO("client.test", "Size of dictionary = ", dictPtr->size());
    LOG_INFO("client.test", "Value of key \"spam\" in dictionary = ", bencast<BenInt*>(((*dictPtr)["spam"]))->getValue());
    
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
    LOG_INFO("client.test", "Visitor pattern test");
    LOG_INFO("client.test", "Bencoding dictionary containing key-value pairs (\"str_key\", \"Hello, World!\"), ",
             "(\"list_key\", [ \"Hello, World!\", 42 ])");
    LOG_INFO("client.test", "Encoded dictionary represented as: ", encoder.getData());
    */

    // Begin test on torrent file & tracker
    std::shared_ptr<TorrentState> testFile;
    if (argc > 1)
    {
        LOG_INFO("client.test", "Beginning download sequence of file: ", argv[1]);
        std::string torrentFilePath(argv[1]);
        testFile = eTorrentMgr.addTorrent(torrentFilePath);

        eTorrentMgr.run();

        auto torrentPtr = testFile->getTorrentFile();
        auto announceURL = torrentPtr->getAnnounceURL();
        LOG_INFO("client.test", "Torrent file size = ", torrentPtr->getFileSize());
        LOG_INFO("client.test", "Announce URL from torrent file: ", announceURL.getHost(), '/', announceURL.getPageName());
        LOG_INFO("client.test", "There are ", torrentPtr->getNumPieces(), " pieces in the file");
    }
    // End test on torrent file & tracker

    // Spawn the main window
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
