cmake_minimum_required(VERSION 3.21)
project(caldera)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glad REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(spdlog REQUIRED)

add_executable(caldera
   src/main.cpp
)

target_link_libraries(caldera
   glad::glad
   glfw
   glm::glm
   spdlog::spdlog
)

target_compile_definitions(caldera PRIVATE GLFW_INCLUDE_NONE)
add_custom_target(copy_assets
   COMMAND
   ${CMAKE_COMMAND} -E copy_directory
   "${CMAKE_SOURCE_DIR}/src/shaders"
   "${CMAKE_CURRENT_BINARY_DIR}/assets"
)
add_dependencies(caldera copy_assets)
