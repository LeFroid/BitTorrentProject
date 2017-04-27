# BitTorrentProject

A BitTorrent client, implementing the standard BitTorrent protocol in C++

Uses a custom GUI framework based on SDL2 libraries for the user interface.

# Software Dependencies

The following libraries are required to build and run the client:
 * Boost.System
 * Boost.Filesystem
 * SDL2
 * SDL2_image
 * SDL2_ttf
 * SDL2_gfx
 * OpenSSL or LibreSSL

In addition, CMake is required to generate the makefiles on Mac, Linux, or any other *nix system 

# How to Get, Build and Run Program

For *nix users, perform the following steps (substitue "../destdir" with the path to any installation directory):
```
> git clone https://github.com/LeFroid/BitTorrentProject.git
> cd BitTorrentProject && mkdir build && cd build
> cmake .. -DCMAKE_INSTALL_PREFIX=../destdir
> make && make install && cd ../destdir/bin
> ./bitclient
```

For Windows users, build and install the program with Visual Studio 2017 or newer by cloning the repository and opening the folder from Visual Studio
as a project. VS2017 will automatically convert the CMake files into visual studio project files.

# Features Implemented

HTTP 1.1 tracker communications (UDP tracker protocol not implemented)

Peer listener service for incoming connections.

Peer fetcher service based on tracker response, programmed to request more peers every 5 minutes for active torrent files.

Piece selection algorithm based on bitfields and have messages sent by peers to local client.

Single and multi file download support.

Configurable color scheme in JSON format for the user interface.

Configurable network settings in JSON format for peer listener service.

Can set download directory through either the GUI or the bitclient.json file.

Asynchronous network stack.

Hierarchical graphical user interface library and layout managers.

Can communicate with hundreds of peers simultaneously.

Can download multiple torrent files at the same time.

# Bugs

The torrent client has not been observed to upload data to peers. This may be due to the low limit on how many peers the
client is currently hardcoded to unchoke, however this issue has not been properly investigated due to time constraints.

On Mac OS X the GUI occasionally has a pulse-like effect - for example, when a button is hovered over by the mouse,
it will toggle back and forth between its active color and inactive color. This appears to only be an issue on Mac Systems,
and is likely because of some quirk in the SDL2 rendering library.

# License

See LICENSE file
