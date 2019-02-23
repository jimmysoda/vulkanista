# FindVulkan module available since CMake v3.7
# C++17 standard support available since CMake v3.8
cmake_minimum_required(VERSION 3.8)

project(vulkanista CXX)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use "d" suffix for debug builds
# Do not use a suffix for RelWithDebInfo
set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "CMake debug suffix")
set(CMAKE_RELWITHDEBINFO_POSTFIX "" CACHE STRING "CMake RelWithDebInfo suffix")

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED CONFIG)
find_package(glm REQUIRED CONFIG)

# vcpkg adds glfw as another target
# Set glfw_LIBRARIES and glfw_INCLUDE_DIRS to be consistent with other dependencies
set(glfw_LIBRARIES glfw glfw)
get_target_property(glfw_INCLUDE_DIRS glfw INTERFACE_INCLUDE_DIRECTORIES)

add_subdirectory(examples)
