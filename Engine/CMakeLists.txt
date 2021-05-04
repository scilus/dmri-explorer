#========== Targets Configurations ============#
file(GLOB SOURCES "src/*.cpp")
add_executable(main ${SOURCES})

target_link_libraries(main PRIVATE GLAD)
target_link_libraries(main PRIVATE glm::glm)
target_link_libraries(main PRIVATE glfw)
target_link_libraries(main PRIVATE NIFTI::nifticdf)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_custom_command(TARGET main POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E create_symlink
                   ${CMAKE_CURRENT_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:main>/shaders
                   COMMENT "Creating symlink to shader directory" VERBATIM)
