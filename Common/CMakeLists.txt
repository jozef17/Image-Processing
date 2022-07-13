# Image static library
cmake_minimum_required (VERSION 3.8)

set(SOURCES
   src/BitmapImage.cpp
   src/Image.cpp
   src/Pixel.cpp
   src/RAWImage.cpp
)

set(INCLUDES
   include/BitmapImage.hpp
   include/Exception.hpp
   include/Image.hpp
   include/Pixel.hpp
   include/RAWImage.hpp
)

add_library(Common STATIC ${SOURCES} ${INCLUDES} )

list (APPEND EXTRA_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include")
target_include_directories (Common PUBLIC ${EXTRA_INCLUDES})

add_subdirectory ("test")
