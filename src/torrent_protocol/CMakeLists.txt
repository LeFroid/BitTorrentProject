project(TorrentProtocol)
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/bencoding
  ${CMAKE_CURRENT_SOURCE_DIR}/http
  ${CMAKE_CURRENT_SOURCE_DIR}/network
  ${FRAMEWORK_SRC_DIR}
  ${SDL2_INCLUDE_DIRS}
  ${SDL2_GFX_INCLUDE_DIRS}
  ${SDL2_IMAGE_INCLUDE_DIRS}
  ${SDL2_TTF_INCLUDE_DIRS}
  ${OPENSSL_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
)

file(GLOB_RECURSE torrent_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h)

add_library(${torrent_LIB} STATIC
  ${torrent_SRC}
)

target_link_libraries(${torrent_LIB} ${framework_LIB})

cotire(${torrent_LIB})
