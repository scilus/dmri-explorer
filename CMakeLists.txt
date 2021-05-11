set(CMAKE_TOOLCHAIN_FILE ./third/vcpkg/scripts/buildsystems/vcpkg.cmake)

#========== Global Configurations =============#
#----------------------------------------------#
cmake_minimum_required(VERSION 3.9)
project(stunningsuccotash)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# glad include
add_library(GLAD "third/glad/src/glad.c")
include_directories("third/glad/include/")

add_subdirectory("third/nifti_clib")
add_subdirectory("Engine")
