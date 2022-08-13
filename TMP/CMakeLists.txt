cmake_minimum_required (VERSION 3.8)

set(SOURCES
   Main.cpp   
)

add_executable (Tmp ${SOURCES} )

target_link_libraries(Tmp Common)

list (APPEND EXTRA_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include")
list (APPEND EXTRA_INCLUDES "${CMAKE_SOURCE_DIR}/Common/include")
target_include_directories (Tmp PRIVATE ${EXTRA_INCLUDES})
