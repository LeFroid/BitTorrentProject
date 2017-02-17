project(TorrentProtocol)
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${SDL2_INCLUDE_DIRS}
  ${SDL2_GFX_INCLUDE_DIRS}
  ${SDL2_IMAGE_INCLUDE_DIRS}
  ${SDL2_TTF_INCLUDE_DIRS}
  #  ${TinyXML_INCLUDE_DIRS}
)

file(GLOB_RECURSE torrent_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h)

add_library(${torrent_LIB} STATIC
  ${torrent_SRC}
)

cotire(${torrent_LIB})
