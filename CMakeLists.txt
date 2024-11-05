# Image processing
cmake_minimum_required (VERSION 3.8)

project ("ImageProcessing")

find_package(OpenGL REQUIRED)

# glfw
if (NOT glfw)
	include(FetchContent)
	FetchContent_Declare(glfw
	  URL https://github.com/glfw/glfw/archive/refs/heads/master.zip
	)
	set(glfw_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(glfw)
endif()

# Google Test
if (NOT googletest)
	include(FetchContent)
	FetchContent_Declare(googletest
		URL https://github.com/google/googletest/archive/refs/heads/main.zip
	)
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)
endif()

# Projects
add_subdirectory ("Common")
add_subdirectory ("Compression")
add_subdirectory ("ConvolutionFilter")
add_subdirectory ("ImageViewer")
add_subdirectory ("gif")
