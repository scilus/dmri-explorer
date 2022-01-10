#========== Targets Configurations ============#
file(GLOB SOURCES "src/*.cpp")
add_executable(dmriexplorer ${SOURCES})

target_link_libraries(dmriexplorer PRIVATE GLAD)
target_link_libraries(dmriexplorer PRIVATE glfw)
target_link_libraries(dmriexplorer PRIVATE NIFTI::nifticdf)
target_link_libraries(dmriexplorer PRIVATE IMGUI)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_compile_definitions(DMRI_EXPLORER_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")

add_custom_command(TARGET dmriexplorer POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E create_symlink
                   ${CMAKE_CURRENT_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:dmriexplorer>/shaders
                   COMMENT "Creating symlink to shader directory" VERBATIM)

add_custom_command(TARGET dmriexplorer POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E create_symlink
                   ${CMAKE_SOURCE_DIR}/icons/ $<TARGET_FILE_DIR:dmriexplorer>/icons
                   COMMENT "Creating symlink to icons directory" VERBATIM)
