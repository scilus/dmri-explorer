set(CMAKE_GENERATOR "Unix Makefiles")
set(CMAKE_TOOLCHAIN_FILE ./third/vcpkg-export-20210512-131839/scripts/buildsystems/vcpkg.cmake)

#========== Global Configurations =============#
#----------------------------------------------#
cmake_minimum_required(VERSION 3.11)
project(rtfodfslicer)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# glad include
add_library(GLAD "third/glad/src/glad.c")
include_directories("third/glad/include/")

# nifti clib library
add_subdirectory("third/nifti_clib")

# imgui include
file(GLOB IMGUI_SRC "third/imgui/src/*.cpp")
add_library(IMGUI ${IMGUI_SRC})
target_link_libraries(IMGUI PRIVATE glfw GLAD)
include_directories("third/imgui/include/")
add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLAD)

add_subdirectory("Engine")
