file(GLOB_RECURSE SRC_FILES "src/*.cpp")

add_library(Dark ${SRC_FILES})

target_include_directories(Dark PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/Dark ${Stb_INCLUDE_DIR})
target_link_libraries(Dark PUBLIC glfw GLEW::GLEW glm::glm box2d::box2d yaml-cpp)

add_custom_command(TARGET Dark POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/resources" $<TARGET_FILE_DIR:Dark>/resources
  COMMAND_EXPAND_LISTS
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Dark PROPERTY CXX_STANDARD 20)
endif()