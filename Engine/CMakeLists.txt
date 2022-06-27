# dmriexplorer executable
file(GLOB SOURCES "src/*.cpp")
add_executable(dmriexplorer ${SOURCES})

target_link_libraries(dmriexplorer PRIVATE GLAD)
target_link_libraries(dmriexplorer PRIVATE glfw)
target_link_libraries(dmriexplorer PRIVATE NIFTI_LIB)
target_link_libraries(dmriexplorer PRIVATE IMGUI)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# /!\ quote symbols are critical and MUST be passed to the compiler. /!\
add_compile_definitions(DMRI_EXPLORER_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")

# copy shaders to build directory
file(GLOB_RECURSE SHADERS_SRC
     RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
     "shaders/**")

# copy shader files to build directory
foreach(f ${SHADERS_SRC})
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${f} ${CMAKE_CURRENT_BINARY_DIR}/${f} COPYONLY)
endforeach()

# copy icons to build directory
foreach(f ${DMRI_EXP_ICONS})
    configure_file(${DMRI_EXP_ICONS_ABS_ROOT}/${f} ${CMAKE_CURRENT_BINARY_DIR}/${f} COPYONLY)
endforeach()
