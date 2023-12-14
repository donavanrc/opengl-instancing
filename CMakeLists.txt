cmake_minimum_required(VERSION 3.20)

project(opengl-instancing C CXX)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")

include_directories(ThirdParty/glad/include)
include_directories(ThirdParty/glfw/include)
include_directories(ThirdParty/glm)

add_subdirectory(ThirdParty/glad)
add_subdirectory(ThirdParty/glfw)

add_executable(opengl-instancing Source/main.cpp)

set_property(TARGET opengl-instancing PROPERTY CXX_STANDARD 20)
set_property(TARGET opengl-instancing PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(opengl-instancing PRIVATE glad glfw)