cmake_minimum_required(VERSION 3.1.0)

project(CpuUsageTracker
	DESCRIPTION "CPU usage tracker for Linux-based systems"
	LANGUAGES C)
	
enable_testing()

add_executable(${PROJECT_NAME} app/app.c)

# Those directives could be moved to CMakeLists.txt inside /libs
add_dependencies(${PROJECT_NAME} CircularBuffer)
target_include_directories(${PROJECT_NAME} PRIVATE circbuf)
target_link_libraries(${PROJECT_NAME} CircularBuffer)

add_subdirectory(libs)
add_subdirectory(src)
add_subdirectory(test)

set_target_properties(${PROJECT_NAME} PROPERTIES
	C_STANDARD 11
	C_STANDARD_REQUIRED ON)

set_target_properties(${PROJECT_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out"
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out")

# Set compiler-specific flags to enforce stricter rules
if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
	target_compile_options(${PROJECT_NAME} PRIVATE
		-Weverything
		-Werror
		-Wno-declaration-after-statement
		-Wno-padded
		-Wno-newline-eof
		-Wno-error=pedantic
		-Wno-format-nonliteral
		-Wno-atomic-implicit-seq-cst
		-Wno-sign-conversion
		-Wno-cast-qual
		-Wno-disabled-macro-expansion
		-Wno-error=implicit-int-float-conversion
		-Wno-error=shorten-64-to-32
		-pedantic)
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
	target_compile_options(${PROJECT_NAME} PRIVATE
		-Wall
		-Wextra
		-Wpedantic
		-Werror
		-Wno-error=pedantic)
endif()