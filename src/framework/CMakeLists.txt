project(Framework)
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${SDL2_INCLUDE_DIRS}
  ${SDL2_GFX_INCLUDE_DIRS}
  ${SDL2_IMAGE_INCLUDE_DIRS}
  ${SDL2_TTF_INCLUDE_DIRS}
  #  ${TinyXML_INCLUDE_DIRS}
)

file(GLOB_RECURSE framework_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h)

add_library(${framework_LIB} STATIC
  ${framework_SRC}
)

cotire(${framework_LIB})
