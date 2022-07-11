# Image static library
cmake_minimum_required (VERSION 3.8)

set(SOURCES
   src/Pixel.cpp
)

set(INCLUDES
   include/Pixel.hpp
)

add_library(Common STATIC ${SOURCES} ${INCLUDES} )

list (APPEND EXTRA_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include")
target_include_directories (Common PUBLIC ${EXTRA_INCLUDES})

add_subdirectory ("test")
