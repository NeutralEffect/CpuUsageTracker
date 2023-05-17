project(CircularBuffer)

add_library(${PROJECT_NAME} STATIC
	circbuf.c)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		./include
	PRIVATE
		.)

set_target_properties(${PROJECT_NAME} PROPERTIES
	C_STANDARD 11
	C_STANDARD_REQUIRED ON)

	# Set compiler-specific flags to enforce stricter rules
if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
target_compile_options(${PROJECT_NAME} PRIVATE
	-Weverything
	-Wno-declaration-after-statement
	-Wno-padded
	-Wno-newline-eof
	-Wno-error=pedantic
	-pedantic)
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
target_compile_options(${PROJECT_NAME} PRIVATE
	-Wall
	-Wextra
	-Werror
	-Wpedantic)
endif()
