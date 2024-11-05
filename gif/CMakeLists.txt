cmake_minimum_required (VERSION 3.8)

set(SOURCES
   src/Main.cpp   
)

set(INCLUDES
)

add_executable (gif ${SOURCES} ${INCLUDES} )

target_link_libraries(gif Common)

list (APPEND EXTRA_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include")
list (APPEND EXTRA_INCLUDES "${CMAKE_SOURCE_DIR}/Common/include")
target_include_directories (gif PRIVATE ${EXTRA_INCLUDES})
