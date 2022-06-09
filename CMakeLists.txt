set(CMAKE_TOOLCHAIN_FILE "deps/zlib/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_GENERATOR_PLATFORM "x64")

if(UNIX) # force the buildsystem to unix makefiles on linux
    set(CMAKE_GENERATOR "Unix Makefiles")
endif()

cmake_minimum_required(VERSION 3.11)
project(dmriexplorer LANGUAGES CXX)
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

# nifti i/o library
add_subdirectory("deps/nifti")
include_directories("deps/nifti/include/")

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

#args parser library include
include_directories("deps/args/")

# get icon images; will be passed on to Engine
file(GLOB DMRI_EXP_ICONS
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     "icons/*")
set(DMRI_EXP_ICONS_ABS_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory("Engine")
