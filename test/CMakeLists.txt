project(CpuUsageTrackerTests)


# File utilities tests
add_executable(FileTests file_tests.c)

add_test(
	NAME 	FileTests
	COMMAND FileTests)

target_include_directories(FileTests
	PRIVATE
		${CMAKE_SOURCE_DIR}/src/utils
		.)

target_sources(FileTests PRIVATE
	${CMAKE_SOURCE_DIR}/src/utils/file.c)

set_target_properties(FileTests PROPERTIES
	C_STANDARD 11
	C_STANDARD_REQUIRED ON
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out")


if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
	target_compile_options(FileTests PRIVATE
		-Weverything
		-Werror
		-Wno-declaration-after-statement
		-Wno-unused-macros
		-Wno-unused-function
		-Wno-padded
		-Wno-newline-eof
		-Wno-error=pedantic
		-Wno-format-nonliteral
		-Wno-atomic-implicit-seq-cst
		-Wno-sign-conversion
		-Wno-cast-qual
		-Wno-disabled-macro-expansion
		-Wno-missing-prototypes
		-Wno-error=implicit-int-float-conversion
		-Wno-error=shorten-64-to-32
		-pedantic
		-pthread)
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
	target_compile_options(FileTests PRIVATE
		-Wall
		-Wextra
		-Wpedantic
		-Werror
		-Wno-error=pedantic
		-pthread)
endif()


# Thread utilities tests
add_executable(ThreadUtilsTests thread_utils_tests.c)

add_test(
	NAME 	ThreadUtilsTests
	COMMAND ThreadUtilsTests)

target_include_directories(ThreadUtilsTests
	PRIVATE
		${CMAKE_SOURCE_DIR}/src/utils
		.)

target_sources(ThreadUtilsTests PRIVATE
	${CMAKE_SOURCE_DIR}/src/utils/thread_utils.c)

set_target_properties(ThreadUtilsTests PROPERTIES
	C_STANDARD 11
	C_STANDARD_REQUIRED ON
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out")

# Set compiler-specific flags to enforce stricter rules
if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
	target_compile_options(ThreadUtilsTests PRIVATE
		-Weverything
		-Werror
		-Wno-declaration-after-statement
		-Wno-unused-macros
		-Wno-unused-function
		-Wno-padded
		-Wno-newline-eof
		-Wno-error=pedantic
		-Wno-format-nonliteral
		-Wno-atomic-implicit-seq-cst
		-Wno-sign-conversion
		-Wno-cast-qual
		-Wno-disabled-macro-expansion
		-Wno-missing-prototypes
		-Wno-error=implicit-int-float-conversion
		-Wno-error=shorten-64-to-32
		-pedantic
		-pthread)
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
	target_compile_options(ThreadUtilsTests PRIVATE
		-Wall
		-Wextra
		-Wpedantic
		-Werror
		-Wno-error=pedantic
		-pthread)
endif()