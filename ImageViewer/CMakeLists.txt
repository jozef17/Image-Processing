cmake_minimum_required (VERSION 3.8)

set(SOURCES
   src/HandlerManager.cpp
   src/HelpHandler.cpp
   src/ImageHandler.cpp
   src/InvalidInputHandler.cpp
   src/Main.cpp
)

set(INCLUDES
   include/HandlerManager.hpp
   include/HelpHandler.hpp
   include/ImageHandler.hpp
   include/InvalidInputHandler.hpp
)

add_executable (ImageViewer ${SOURCES} ${INCLUDES} )

target_link_libraries(ImageViewer Common)

list (APPEND EXTRA_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include")
list (APPEND EXTRA_INCLUDES "${CMAKE_SOURCE_DIR}/Common/include")
target_include_directories (ImageViewer PRIVATE ${EXTRA_INCLUDES})
