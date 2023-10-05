file(GLOB_RECURSE SRC_FILES "src/*.cpp")
file(GLOB_RECURSE INI_FILES "${CMAKE_SOURCE_DIR}/imgui*.ini")

add_executable(DarkEditor ${SRC_FILES})

target_include_directories(DarkEditor PUBLIC ${CMAKE_SOURCE_DIR}/Dark/src ${Stb_INCLUDE_DIR})
target_link_libraries(DarkEditor PUBLIC Dark)

add_custom_command(TARGET DarkEditor POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/resources" $<TARGET_FILE_DIR:DarkEditor>/resources
  COMMAND_EXPAND_LISTS
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/imgui_views" $<TARGET_FILE_DIR:DarkEditor>/imgui_views
  COMMAND_EXPAND_LISTS
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET DarkEditor PROPERTY CXX_STANDARD 20)
endif()