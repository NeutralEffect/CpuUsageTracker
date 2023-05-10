cmake_minimum_required(VERSION 3.1.0)

project(CpuUsageTracker
	DESCRIPTION "CPU usage tracker for Linux-based systems"
	LANGUAGES C)
	
add_executable(${PROJECT_NAME} app/app.c)

# Those directives could be moved to CMakeLists.txt inside /libs
add_dependencies(${PROJECT_NAME} CircularBuffer)
target_include_directories(${PROJECT_NAME} PRIVATE circbuf)
target_link_libraries(${PROJECT_NAME} CircularBuffer)

add_subdirectory(libs)
add_subdirectory(src)

set_target_properties(${PROJECT_NAME} PROPERTIES
	C_STANDARD 11
	C_STANDARD_REQUIRED ON)

set_target_properties(${PROJECT_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out")

# Set compiler-specific flags to enforce stricter rules
if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
	target_compile_options(${PROJECT_NAME} PRIVATE
		-Weverything
		-Werror
		-pedantic)
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
	target_compile_options(${PROJECT_NAME} PRIVATE
		-Wall
		-Wextra
		-Werror
		-Wpedantic
		-Wno-error=pedantic)
endif()