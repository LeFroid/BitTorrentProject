cmake_minimum_required(VERSION 2.6)

# Handle policies
cmake_policy(SET CMP0011 NEW)
if(POLICY CMP0058)
  cmake_policy(SET CMP0058 NEW)
endif()

project(BitTorrentProject)

set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  ${CMAKE_SOURCE_DIR}/cmake
)

set(LANGUAGE "CXX")

# Use C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")

# Dependencies

# SDL2 Libraries
find_package(SDL2 REQUIRED)
find_package(SDL2_gfx REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

# OpenSSL or LibreSSL
find_package(OpenSSL REQUIRED)

# PCH support
include(cotire)

# For Unix systems set the rpath so that libraries are found
set(CMAKE_INSTALL_RPATH ${LIBS_DIR})
set(CMAKE_INSTALL_NAME_DIR ${LIBS_DIR})
# Run out of build tree
set(CMAKE_BUILD_WITH_INSTALL_RPATH OFF)

# Project info
set(client_EXEC bitclient)
set(framework_LIB framework)
set(torrent_LIB torrent_protocol)

set(FRAMEWORK_SRC_DIR ${CMAKE_SOURCE_DIR}/src/framework)
set(TORRENT_SRC_DIR ${CMAKE_SOURCE_DIR}/src/torrent_protocol)

# Source Tree
add_subdirectory(src)
