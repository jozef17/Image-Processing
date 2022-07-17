cmake_minimum_required (VERSION 3.8)

set(SOURCES
	CommandLineParserTest.cpp
	ImageTest.cpp
	PixelTest.cpp
)

add_executable(CommonTests ${SOURCES})

target_link_libraries(CommonTests 
	gtest_main
	Common
)

add_test(NAME CommonTests COMMAND example)

list (APPEND EXTRA_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/../include")
target_include_directories (CommonTests PUBLIC ${EXTRA_INCLUDES})
