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

# License

See LICENSE file
