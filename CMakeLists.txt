set(CMAKE_GENERATOR "Unix Makefiles")

#========== Global Configurations =============#
#----------------------------------------------#
cmake_minimum_required(VERSION 3.11)
project(dmriexplorer)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE ON)

# glfw include
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("deps/glfw-3.3.6")

# glad include
add_library(GLAD "deps/glad/src/glad.c")
include_directories("deps/glad/include/")

# nifti clib library
add_subdirectory("deps/nifti_clib")

# imgui include
file(GLOB IMGUI_SRC "deps/imgui/src/*.cpp")
add_library(IMGUI ${IMGUI_SRC})
target_link_libraries(IMGUI PRIVATE glfw GLAD)
include_directories("deps/imgui/include/")
add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLAD)

# std image loader include
include_directories("deps/stb/include/")

# glm math library include
include_directories("deps/glm/")

add_subdirectory("Engine")
