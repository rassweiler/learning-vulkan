cmake_minimum_required(VERSION 3.0.0)
project(LearningVulkan VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(X11 REQUIRED)

include_directories(
	${Vulkan_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIRS} 
   ${GLFW_INCLUDE_DIRS}
   ${GLEW_INCLUDE_DIRS}
	${X11_INCLUDE_DIR}
   ${PROJECT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/include/
)

include(CTest)
enable_testing()

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++20 -O2")
set (source_dir "${PROJECT_SOURCE_DIR}/src/")
set (include_dir "${CMAKE_SOURCE_DIR}/include/")
set (shader_dir "${CMAKE_SOURCE_DIR}/shaders/")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

file (GLOB source_files "${source_dir}/*.cpp" "${include_dir}/*.hpp" "${shader_dir}/*.*")

add_executable(LearningVulkan ${source_files})

target_include_directories(LearningVulkan PUBLIC "${include_dir}" "${shader_dir}")
target_link_libraries(LearningVulkan glfw vulkan dl pthread X11 Xxf86vm Xrandr Xi)
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND cd ${CMAKE_SOURCE_DIR}/shaders/ && ./compile.sh)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND mkdir -p ${PROJECT_SOURCE_DIR}/bin/shaders/ && cp -r ${PROJECT_SOURCE_DIR}/shaders/*.spv ${PROJECT_SOURCE_DIR}/bin/shaders/)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
