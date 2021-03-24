#========== Targets Configurations ============#

add_executable(main src/main.cpp)

target_link_libraries(main PRIVATE glad::glad)
target_link_libraries(main PRIVATE glm::glm)
target_link_libraries(main PRIVATE glfw)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_custom_command(TARGET main POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR}/shaders/
                   $<TARGET_FILE_DIR:main>/shaders/)
