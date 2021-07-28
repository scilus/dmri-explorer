#========== Targets Configurations ============#
file(GLOB SOURCES "src/*.cpp")
add_executable(rtfodfslicer ${SOURCES})

target_link_libraries(rtfodfslicer PRIVATE GLAD)
target_link_libraries(rtfodfslicer PRIVATE glm::glm)
target_link_libraries(rtfodfslicer PRIVATE glfw)
target_link_libraries(rtfodfslicer PRIVATE NIFTI::nifticdf)
target_link_libraries(rtfodfslicer PRIVATE IMGUI)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_compile_definitions(RTFODFSLICER_SHADERS_DIR="${CMAKE_CURRENT_BINARY_DIR}/shaders")

add_custom_command(TARGET rtfodfslicer POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E create_symlink
                   ${CMAKE_CURRENT_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:rtfodfslicer>/shaders
                   COMMENT "Creating symlink to shader directory" VERBATIM)
