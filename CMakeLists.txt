# Image processing
cmake_minimum_required (VERSION 3.8)

project ("ImageProcessing")

# Google Test
if (NOT googletest)
	include(FetchContent)
	FetchContent_Declare(
	  googletest
	  URL https://github.com/google/googletest/archive/refs/heads/main.zip
	)
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)
endif()

# Projects
add_subdirectory ("Common")
