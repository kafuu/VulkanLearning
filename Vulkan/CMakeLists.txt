project(Vulkan)


file(GLOB src "D:/program/Vulkan/src/*.cpp" )
add_executable(Vulkan "Vulkan.cpp" ${src})
target_include_directories(
	Vulkan
	PUBLIC
	"D:/program/Vulkan/header"
	"C:/VulkanSDK/1.3.261.1/Include"
)
target_link_libraries(
	Vulkan
	PUBLIC
	"C:/VulkanSDK/1.3.261.1/Lib/SDL2.lib"
	"C:/VulkanSDK/1.3.261.1/Lib/SDL2main.lib"
	"C:/VulkanSDK/1.3.261.1/Lib/vulkan-1.lib"
)

find_program(GLSLC_PROGRAM glslc REQUIRED)

message(STATUS "run glslc to compile shaders ...")
execute_process(COMMAND ${GLSLC_PROGRAM} ${CMAKE_SOURCE_DIR}/shaders/shader.vert -o ${CMAKE_SOURCE_DIR}/vert.spv)
execute_process(COMMAND ${GLSLC_PROGRAM} ${CMAKE_SOURCE_DIR}/shaders/shader.frag -o ${CMAKE_SOURCE_DIR}/frag.spv)
message(STATUS "compile shader OK")

add_custom_target(
	Vulkan1
	ALL
	COMMAND powershell "copy C:/VulkanSDK/1.3.261.1/Bin/SDL2.dll ${CMAKE_CURRENT_BINARY_DIR}"
)
