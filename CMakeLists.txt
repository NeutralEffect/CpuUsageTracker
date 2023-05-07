cmake_minimum_required(VERSION 3.1.0)

project(CpuUsageTracker)

set(SOURCES
	app/app.c)

add_executable(${PROJECT_NAME} ${SOURCES})
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 99)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD_REQUIRED ON)

# Set compiler-specific flags to enforce stricter rules
if (CMAKE_C_COMPILER_ID STREQUAL "Clang")
	target_compile_options(${PROJECT_NAME} PRIVATE
		-Weverything
		-Werror
		-pedantic)
elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
	target_compile_options(${PROJECT_NAME} PRIVATE
		-Wall
		-Wextra
		-Werror
		-Wpedantic)
endif ()