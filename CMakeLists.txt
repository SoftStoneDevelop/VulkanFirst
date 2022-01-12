# CMakeList.txt : CMake project for VulkanEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("VulkanEngine")

# Add source to this project's executable.
add_executable (VulkanEngine   "lve_window.hpp" "lve_window.cpp" "first_app.hpp" "main.cpp" "first_app.cpp" "lve_device.hpp" "lve_device.cpp" "lve_pipeline.hpp" "lve_pipeline.cpp" "lve_swap_chain.hpp" "lve_swap_chain.cpp")

# TODO: Add tests and install targets if needed.

set(PROJECT_NAME "VulkanEngine")
set(VULKAN_SDK_PATH "${CMAKE_SOURCE_DIR}/External/vulkan")

if (DEFINED VULKAN_SDK_PATH)
  set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/Include") # 1.1 Make sure this include path is correct
  message(STATUS "Found Vulkan_INCLUDE_DIRS: ${Vulkan_INCLUDE_DIRS}")
  set(Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/Lib") # 1.2 Make sure lib path is correct
  message(STATUS "Found Vulkan_LIBRARIES: ${Vulkan_LIBRARIES}")
  set(Vulkan_FOUND "True")
else()
  find_package(Vulkan REQUIRED) # throws error if could not find Vulkan
  message(STATUS "Found Vulkan: $ENV{VULKAN_SDK}")
endif()
if (NOT Vulkan_FOUND)
	message(FATAL_ERROR "Could not find Vulkan library!")
else()
	message(STATUS "Using vulkan lib at: ${Vulkan_LIBRARIES}")
endif()

set(GLFW_PATH "${CMAKE_SOURCE_DIR}/External/glfw")

if (DEFINED GLFW_PATH)
  message(STATUS "Using GLFW path specified in .env")
  set(GLFW_INCLUDE_DIRS "${GLFW_PATH}/include")
  if (MSVC)
    set(GLFW_LIB "${GLFW_PATH}/lib-vc2022") # 2.1 Update lib-vc2022 to use same version as your visual studio
  elseif (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    message(STATUS "USING MINGW")
    set(GLFW_LIB "${GLFW_PATH}/lib-mingw-w64") # 2.1 make sure matches glfw mingw subdirectory
  endif()
else()
  find_package(glfw3 3.3 REQUIRED)
  set(GLFW_LIB glfw)
  message(STATUS "Found GLFW")
endif()
if (NOT GLFW_LIB)
	message(FATAL_ERROR "Could not find glfw library!")
else()
	message(STATUS "Using glfw lib at: ${GLFW_LIB}")
endif()


set(GLM_PATH "${CMAKE_SOURCE_DIR}/External/glm-0.9.9.8/glm")

if (WIN32)
  message(STATUS "CREATING BUILD FOR WINDOWS")

  target_include_directories(${PROJECT_NAME} PUBLIC
    ${Vulkan_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${GLM_PATH}
    )

  target_link_directories(${PROJECT_NAME} PUBLIC
    ${Vulkan_LIBRARIES}
    ${GLFW_LIB}
  )

  target_link_libraries(${PROJECT_NAME} glfw3)
  target_link_libraries(${PROJECT_NAME} vulkan-1 -vulkan)
elseif (UNIX)
    message(STATUS "CREATING BUILD FOR UNIX")
    target_include_directories(${PROJECT_NAME} PUBLIC
      ${PROJECT_SOURCE_DIR}/src
      ${TINYOBJ_PATH}
    )
    target_link_libraries(${PROJECT_NAME} glfw ${Vulkan_LIBRARIES})
endif()

set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ShadersSource)
set(SHADER_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/Shaders)

############## Build SHADERS #######################

# Find all vertex and fragment sources within shaders directory
# taken from VBlancos vulkan tutorial
# https://github.com/vblanco20-1/vulkan-guide/blob/all-chapters/CMakeLists.txt
find_program(GLSL_VALIDATOR glslangValidator HINTS 
  ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE} 
  /usr/bin 
  /usr/local/bin 
  ${VULKAN_SDK_PATH}/Bin
  ${VULKAN_SDK_PATH}/Bin32
  $ENV{VULKAN_SDK}/Bin/ 
  $ENV{VULKAN_SDK}/Bin32/
)

message(STATUS "GLSL_EXECUTOR: ${GLSL_EXECUTOR} ")

# get all .vert and .frag files in shaders directory
file(GLOB_RECURSE GLSL_SOURCE_FILES
  "${SHADER_SOURCE_DIR}/*.frag"
  "${SHADER_SOURCE_DIR}/*.vert"
)

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  message(STATUS "Find source shader FILE_NAME: ${FILE_NAME} ")
  set(SPIRV "${SHADER_BINARY_DIR}/${FILE_NAME}.spv")
  message(STATUS "Out path SPIRV: ${SPIRV} ")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    run ALL
    DEPENDS ${SPIRV_BINARY_FILES}
)