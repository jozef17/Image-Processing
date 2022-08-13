# Image static library
cmake_minimum_required(VERSION 3.8)

set(SOURCES
   src/BitmapImage.cpp
   src/CommandLineParser.cpp
   src/Image.cpp
   src/Pixel.cpp
   src/PrintMessageHandler.cpp
   src/RAWImage.cpp
   src/Window.cpp
   src/png/BitStream.cpp
   src/png/Inflate.cpp 
   src/png/PngImage.cpp
#   src/png/Type00Decoder.cpp
   src/png/Type01Decoder.cpp
#   src/png/Type10Decoder.cpp
)

set(INCLUDES
   include/ArgumentHandler.hpp
   include/BitmapImage.hpp
   include/CommandLineParser.hpp
   include/Common.hpp
   include/Exception.hpp
   include/Image.hpp
   include/Pixel.hpp
   include/PrintMessageHandler.hpp
   include/RAWImage.hpp
   include/Window.hpp
   include/png/BitStream.hpp
   include/png/BlockDecoder.hpp
   include/png/Inflate.hpp 
   include/png/PngImage.hpp
#   include/png/Type00Decoder.hpp
   include/png/Type01Decoder.hpp
#   include/png/Type10Decoder.hpp
)

add_library(Common STATIC ${SOURCES} ${INCLUDES} )

target_link_libraries(Common PRIVATE glfw ${OPENGL_gl_LIBRARY})

list(APPEND EXTRA_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include")
target_include_directories(Common PRIVATE ${EXTRA_INCLUDES})

add_subdirectory("test")