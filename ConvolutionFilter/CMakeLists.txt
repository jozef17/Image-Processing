cmake_minimum_required (VERSION 3.8)

find_package(CUDA)

set(SOURCES
   src/Filter.cpp
   src/Filter.cu
   src/FilterHandler.cpp
   src/HandlerManager.cpp
   src/Kernel.cpp
   src/Main.cpp
)

set(INCLUDES
   include/Filter.cuh
   include/Filter.hpp
   include/FilterHandler.hpp
   include/HandlerManager.hpp
   include/Kernel.hpp
   include/MessageConst.hpp
)

add_executable (ConvolutionFilter ${SOURCES} ${INCLUDES})

IF (${CUDA_FOUND})
   add_definitions(-DUSE_CUDA)
   enable_language(CUDA)
   set(CUDA_SEPARABLE_COMPILATION ON)
   set_property(TARGET ConvolutionFilter PROPERTY CUDA_SEPARABLE_COMPILATION ON)
   set_target_properties(ConvolutionFilter PROPERTIES CUDA_SEPARABLE_COMPILATION ON)   
   target_link_libraries(ConvolutionFilter Common cuda ${CUDA_LIBRARIES})
ELSE (${CUDA_FOUND})
   target_link_libraries(ConvolutionFilter Common)
ENDIF (${CUDA_FOUND})

list (APPEND EXTRA_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include")
list (APPEND EXTRA_INCLUDES "${CMAKE_SOURCE_DIR}/Common/include")

IF (${CUDA_FOUND})
   target_include_directories (ConvolutionFilter PRIVATE ${EXTRA_INCLUDES} ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})
ELSE (${CUDA_FOUND})
   target_include_directories (ConvolutionFilter PRIVATE ${EXTRA_INCLUDES})
ENDIF (${CUDA_FOUND})

add_subdirectory("test")
