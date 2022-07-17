# Image static library
cmake_minimum_required(VERSION 3.8)

set(SOURCES
   src/BitmapImage.cpp
   src/CommandLineOption.cpp
   src/CommandLineParser.cpp
   src/Image.cpp
   src/Pixel.cpp
   src/RAWImage.cpp
   src/Window.cpp
)

set(INCLUDES
   include/BitmapImage.hpp
   include/CommandLineOption.hpp
   include/CommandLineParser.hpp
   include/Exception.hpp
   include/Image.hpp
   include/Pixel.hpp
   include/RAWImage.hpp
   include/Window.hpp
)

add_library(Common STATIC ${SOURCES} ${INCLUDES} )

target_link_libraries(Common PRIVATE glfw ${OPENGL_gl_LIBRARY})

list(APPEND EXTRA_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include")
target_include_directories(Common PRIVATE ${EXTRA_INCLUDES})

add_subdirectory("test")