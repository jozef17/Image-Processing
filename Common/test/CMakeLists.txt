cmake_minimum_required (VERSION 3.8)

set(SOURCES
    BitmapLoaderTest.cpp
	CommandLineParserTest.cpp
	ImageLoaderTest.cpp
	ImageTest.cpp
	PixelTest.cpp
	PngLoaderTest.cpp
	RAWLoaderTest.cpp
	Type01DecoderTest.cpp
	Type10DecoderTest.cpp
	UtilsTest.cpp
)

add_executable(CommonTests ${SOURCES})

target_link_libraries(CommonTests 
	gtest_main
	Common
)

add_test(NAME CommonTests COMMAND example)

list (APPEND EXTRA_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/../include")
set_property(TARGET CommonTests PROPERTY CXX_STANDARD 17)
target_include_directories (CommonTests PUBLIC ${EXTRA_INCLUDES})
