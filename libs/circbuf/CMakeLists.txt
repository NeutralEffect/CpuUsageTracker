project(CircularBuffer)

add_library(${PROJECT_NAME} STATIC
	circbuf.c)

add_subdirectory(test)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		include
	PRIVATE
		.)

set_target_properties(${PROJECT_NAME} PROPERTIES
	C_STANDARD 11
	C_STANDARD_REQUIRED ON)

set(CIRCBUF_CLANG_COMPILE_FLAGS
	-Weverything
	-Wno-declaration-after-statement
	-Wno-padded
	-Wno-newline-eof
	-Wno-unused-function
	-Wno-error=pedantic
	-pedantic)

set(CIRCBUF_GCC_COMPILE_FLAGS
	-Wall
	-Wextra
	-Werror
	-Wpedantic
	-Wno-error=pedantic)

# Set compiler-specific flags to enforce stricter rules
if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
	target_compile_options(${PROJECT_NAME} PRIVATE ${CIRCBUF_CLANG_COMPILE_FLAGS})
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
	target_compile_options(${PROJECT_NAME} PRIVATE ${CIRCBUF_GCC_COMPILE_FLAGS})
endif()
