#include <boost/predef.h>
#include <iostream>
#include <signal.h>

#include <SDL.h>

#include "Engine.h"
#include "LogHelper.h"

#include "SHA1Hash.h"
#include "TorrentState.h"
#include "TorrentFile.h"
#include "TorrentMgr.h"

#include "EventHandler.h"
#include "FontStorage.h"
#include "Keybinder.h"
#include "ObjectManager.h"

#include "Window.h"

#include "MainScreen.h"

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

    // Start io_service
    eTorrentMgr.run();

    // Spawn the main window and screen
    ObjectManager *objMgr = ObjectManager::getInstance();
    Window *win = objMgr->createObject<Window>("BitTorrent Client", 1280, 800, false);
    win->draw();
    win->setBackgroundColor(Color(255, 255, 255, 255));

    std::string themeFile = CLIENT_CONFIG_DIR + std::string("bitclient_gui.json");
    MainScreen *screen = objMgr->createObject<MainScreen>(win, themeFile);
    screen->draw();

    EventHandler *eventHandler = EventHandler::getInstance();
    eventHandler->setWindowPtr(win);
    eventHandler->pollForEvents();

    FontStorage::destroyInstance();
    //Keybinder::destroyInstance();
    EventHandler::destroyInstance();
    ObjectManager::destroyInstance();

    Engine::halt();

    // Kill io service with SIGTERM
    raise(SIGTERM);

    return 0;
}
