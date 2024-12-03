# Image static library
cmake_minimum_required(VERSION 3.8)

set(SOURCES
   src/BitmapLoader.cpp
   src/BitStream.cpp
   src/CommandLineParser.cpp
   src/Image.cpp
   src/ImageLoader.cpp   
   src/Pixel.cpp
   src/PrintMessageHandler.cpp
   src/RAWLoader.cpp
   src/Utils.cpp
   src/Window.cpp
   src/gif/GifLoader.cpp
   src/png/Adler32.cpp
   src/png/HuffmanDecoder.cpp
   src/png/Inflate.cpp 
   src/png/PngLoader.cpp
   src/png/Type01Decoder.cpp
   src/png/Type10Decoder.cpp
)

set(INCLUDES
   include/ArgumentHandler.hpp
   include/BitmapLoader.hpp
   include/BitStream.hpp
   include/CommandLineParser.hpp
   include/Common.hpp
   include/Exception.hpp
   include/Image.hpp
   include/ImageLoader.hpp
   include/Pixel.hpp
   include/PrintMessageHandler.hpp
   include/RAWLoader.hpp
   include/Utils.hpp
   include/Window.hpp
   include/gif/GifLoader.hpp
   include/png/Adler32.hpp
   include/png/BlockDecoder.hpp
   include/png/HuffmanDecoder.hpp
   include/png/Inflate.hpp 
   include/png/PngLoader.hpp
   include/png/Type01Decoder.hpp
   include/png/Type10Decoder.hpp
)

add_library(Common STATIC ${SOURCES} ${INCLUDES} )

target_link_libraries(Common PRIVATE glfw ${OPENGL_gl_LIBRARY})

list(APPEND EXTRA_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include")
target_include_directories(Common PRIVATE ${EXTRA_INCLUDES})

add_subdirectory("test")