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

#include "TorrentFile.h"
#include "TrackerClient.h"
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>

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

    // Begin test on torrent file & tracker
    if (argc > 1)
    {
    std::shared_ptr<TorrentFile> testFile(std::make_shared<TorrentFile>(argv[1]));
    http::URL announce = testFile->getAnnounceURL();
    boost::asio::io_service ioService;
    boost::asio::signal_set sigs(ioService, SIGINT, SIGTERM);
    sigs.async_wait(std::bind(&boost::asio::io_service::stop, &ioService));

    std::shared_ptr<network::TrackerClient> client = std::make_shared<network::TrackerClient>(ioService, network::Socket::Mode::TCP);
    client->setTorrentFile(testFile);

    std::string host = announce.getHost().substr(0, announce.getHost().find_last_of(':'));
    std::string port = announce.getHost().substr(announce.getHost().find_last_of(':') + 1);

    boost::asio::ip::tcp::resolver resolver(ioService);
    boost::asio::ip::tcp::resolver::query query(host, port);
    boost::asio::ip::tcp::endpoint remote_endpoint = *resolver.resolve(query);
    client->connect(remote_endpoint);
    ioService.run();
    }
    // End test on torrent file & tracker

    Engine::halt();

    return 0;
}
