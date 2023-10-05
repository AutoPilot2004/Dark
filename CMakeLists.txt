cmake_minimum_required (VERSION 3.27.0)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Dark")

find_package(glfw3 CONFIG REQUIRED)
find_package(glew REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(box2d CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(stb REQUIRED)

add_subdirectory ("Dark")
add_subdirectory ("DarkEditor")