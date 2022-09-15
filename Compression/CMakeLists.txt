cmake_minimum_required (VERSION 3.8)

set(SOURCES
   src/CompressionHandler.cpp
   src/DCT.cpp
   src/FloatImage.cpp
   src/HandlerManager.cpp
   src/ImageCompressor.cpp
   src/Main.cpp   
)

set(INCLUDES
   include/CompressionHandler.hpp
   include/DCT.hpp
   include/FloatImage.hpp
   include/HandlerManager.hpp
   include/ImageCompressor.hpp
   include/MessageConst.hpp
)

add_executable (Compression ${SOURCES} ${INCLUDES} )

target_link_libraries(Compression Common)

list (APPEND EXTRA_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include")
list (APPEND EXTRA_INCLUDES "${CMAKE_SOURCE_DIR}/Common/include")
target_include_directories (Compression PRIVATE ${EXTRA_INCLUDES})

add_subdirectory("test")
