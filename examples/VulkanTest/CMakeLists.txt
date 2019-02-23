set(TARGET VulkanTest)
set(SOURCES VulkanTest.cpp)

set(DEPENDENCIES
	${glfw_LIBRARIES}
	${glm_LIBRARIES}
	${Vulkan_LIBRARIES}
)

include_directories(
	${glfw_INCLUDE_DIRS}
	${glm_INCLUDE_DIRS}
	${Vulkan_INCLUDE_DIRS}
)

add_executable(${TARGET} ${SOURCES})
target_link_libraries(${TARGET} ${DEPENDENCIES})

install(
	TARGETS ${TARGET}
	DESTINATION ${CMAKE_BINARY_DIR}/bin
)