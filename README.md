# vulkanista
A learning playground for Vulkan, today's trendiest graphics API

Build Instructions - Windows
----------------------------

1. Install the following tools:

* CMake 3.8 or later
* vcpkg
* Visual Studio 2015 or later
* Vulkan SDK

2. Set the following environment variables:
* `VCPKG_ROOT` to the root of your vcpkg repository
* `VULKAN_SDK` to the root of Vulkan SDK version install, e.g., _C:\VulkanSDK\1.1.97.0_

3. Install the following packages with vcpkg:
* `vulkan:x64-windows`
* `glm:x64-windows`
* `glfw3:x64-windows`

4. Configure CMake project
Run _make.bat_ or open the repository folder in Visual Studio Code and run the
_vcpkg-x64-windows-debug_ or _vcpkg-x64-windows-release_ build task.

5. Run _VulkanTest_
Locate and execute VulkanTest.exe, making sure that its dependencies are accessible,
e.g., add the required dependency binary directory to `PATH` environment variable or launch
VulkanTest.exe from the dependency binary directory as the current working directory.
The vcpkg-generated binaries are located in `%VCPKG_ROOT%\installed\x64-windows\bin` for
release and `%VCPKG_ROOT%\installed\x64-windows\debug\bin` for debug.

You may also run the _VulkanTest x64-windows_ debug configuration from Visual Studio Code.