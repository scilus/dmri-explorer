#========== Targets Configurations ============#
file(GLOB SOURCES "src/*.cpp")
add_executable(stunning_succotash ${SOURCES})

target_link_libraries(stunning_succotash PRIVATE GLAD)
target_link_libraries(stunning_succotash PRIVATE glm::glm)
target_link_libraries(stunning_succotash PRIVATE glfw)
target_link_libraries(stunning_succotash PRIVATE NIFTI::nifticdf)
target_link_libraries(stunning_succotash PRIVATE IMGUI)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_custom_command(TARGET stunning_succotash POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E create_symlink
                   ${CMAKE_CURRENT_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:stunning_succotash>/shaders
                   COMMENT "Creating symlink to shader directory" VERBATIM)
