cmake_minimum_required (VERSION 3.8)

set(SOURCES
	# Tests
	DCTTest.cpp
	FloatImageTest.cpp
	# Units under test
	../src/DCT.cpp
	../src/FloatImage.cpp
)

add_executable(CompressionTests ${SOURCES})

target_link_libraries(CompressionTests 
	gtest_main
	Common
)

add_test(NAME CompressionTests COMMAND example)

list (APPEND EXTRA_INCLUDES "${CMAKE_SOURCE_DIR}/Common/include")
list (APPEND EXTRA_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/../include")
target_include_directories (CompressionTests PUBLIC ${EXTRA_INCLUDES})
