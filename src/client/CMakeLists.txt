project(Client)

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FRAMEWORK_SRC_DIR}
    ${FRAMEWORK_SRC_DIR}/GUI
    ${TORRENT_SRC_DIR}
    ${TORRENT_SRC_DIR}/bencoding
    ${TORRENT_SRC_DIR}/http
    ${TORRENT_SRC_DIR}/network
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_GFX_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
)

file(GLOB_RECURSE client_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h)

add_executable(${client_EXEC}
    ${client_SRC}
    bitclient.json
)

TARGET_LINK_LIBRARIES(${client_EXEC}
    ${framework_LIB}
    ${torrent_LIB}
    ${SDL2_LIBRARY}
    ${SDL2_GFX_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${OPENSSL_LIBRARIES}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
)

if (UNIX)
    install(TARGETS ${client_EXEC} DESTINATION bin COMPONENT binaries)
    install(FILES bitclient.json DESTINATION etc COMPONENT config)
    add_custom_target(copy-fonts ALL
        COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/Fonts ${CMAKE_INSTALL_PREFIX}/shared/bitclient/Fonts
        DEPENDS ${client_EXEC})
else()
    install(TARGETS ${client_EXEC} COMPONENT binaries)
    install(FILES bitclient.json COMPONENT config)
    add_custom_target(copy-fonts ALL
        COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/Fonts ${CMAKE_INSTALL_PREFIX}/Fonts
        DEPENDS ${client_EXEC})
endif() 

cotire(${client_EXEC})
