cmake_minimum_required (VERSION 3.8)

set(SOURCES
   src/HandlerManager.cpp
   src/Kernel.cpp
   src/Main.cpp
)

set(INCLUDES
   include/HandlerManager.hpp
   include/Kernel.hpp
   include/MessageConst.hpp
)

add_executable (ConvolutionFilter ${SOURCES} ${INCLUDES} )

target_link_libraries(ConvolutionFilter Common)

list (APPEND EXTRA_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include")
list (APPEND EXTRA_INCLUDES "${CMAKE_SOURCE_DIR}/Common/include")
target_include_directories (ConvolutionFilter PRIVATE ${EXTRA_INCLUDES})
