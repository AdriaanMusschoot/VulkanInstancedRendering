# Source files
set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Shaders")
set(SHADER_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/Shaders")

file(GLOB_RECURSE GLSL_SOURCE_FILES
    "${SHADER_SOURCE_DIR}/*.frag"
    "${SHADER_SOURCE_DIR}/*.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
    set(SPIRV "${SHADER_BINARY_DIR}/${FILE_NAME}.spv")
    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL}
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders 
    DEPENDS ${SPIRV_BINARY_FILES}
)


set(SOURCES 
    "Engine/main.cpp" 
    "Engine/Configuration.h"

    "Engine/VulkanEngine.cpp"       "Engine/VulkanEngine.h"
    "Engine/App.cpp"                "Engine/App.h"
    "Engine/Clock.cpp"              "Engine/Clock.h"

    "Device/Instance.h" 
    "Device/Device.h" 

    "Utils/Logging.h"               
    "Utils/FileReader.h"      
    
    "Utils/QueueFamilies.cpp"       "Utils/QueueFamilies.h"
    "Utils/Frame.cpp"               "Utils/Frame.h"
    "Utils/RenderStructs.cpp"       "Utils/RenderStructs.h"
    "Utils/Buffer.cpp"              "Utils/Buffer.h"
    "Utils/Camera.cpp"              "Utils/Camera.h"
    

    "Pipeline/Shader.cpp"           "Pipeline/Shader.h"
    "Pipeline/Descriptor.cpp"       "Pipeline/Descriptor.h"
    "Pipeline/RenderPass.cpp"       "Pipeline/RenderPass.h"
    
    "Rendering/Swapchain.h"
    "Rendering/Synchronization.h"
    "Rendering/Scene.h"

    "Rendering/FrameBuffer.cpp"     "Rendering/FrameBuffer.h"
    "Rendering/Commands.cpp"        "Rendering/Commands.h"
    "Rendering/Image.cpp"           "Rendering/Image.h"
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES} ${GLSL_SOURCE_FILES})
add_dependencies(${PROJECT_NAME} Shaders)
# Link libraries
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES} glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES} tinyobjloader)

set(RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Resources")
set(RESOURCES_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/Resources")

add_custom_command(
    TARGET Assignment PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${RESOURCES_DIR} ${RESOURCES_BINARY_DIR}
)