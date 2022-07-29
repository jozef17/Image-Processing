cmake_minimum_required (VERSION 3.8)

set(SOURCES
	KernelTest.cpp
	../src/Kernel.cpp
)

add_executable(ConvolutionFilterTests ${SOURCES})

target_link_libraries(ConvolutionFilterTests 
	gtest_main
	Common
)

add_test(NAME ConvolutionFilterTests COMMAND example)

list (APPEND EXTRA_INCLUDES "${CMAKE_SOURCE_DIR}/Common/include")
list (APPEND EXTRA_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/../include")
target_include_directories (ConvolutionFilterTests PUBLIC ${EXTRA_INCLUDES})
