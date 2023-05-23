project(CpuUsageTrackerTests)

set(CUTTESTS_CLANG_COMPILE_FLAGS ${CUT_CLANG_FLAGS})
set(CUTTESTS_GCC_COMPILE_FLAGS ${CUT_GCC_FLAGS})


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
	target_compile_options(FileTests PRIVATE ${CUTTESTS_CLANG_COMPILE_FLAGS})
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
	target_compile_options(FileTests PRIVATE ${CUTTESTS_GCC_COMPILE_FLAGS})
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
	${CMAKE_SOURCE_DIR}/src/utils/sync.c)

set_target_properties(ThreadUtilsTests PROPERTIES
	C_STANDARD 11
	C_STANDARD_REQUIRED ON
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out")

	
# Set compiler-specific flags to enforce stricter rules
if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
	target_compile_options(ThreadUtilsTests PRIVATE ${CUTTESTS_CLANG_COMPILE_FLAGS})
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
	target_compile_options(ThreadUtilsTests PRIVATE ${CUTTESTS_GCC_COMPILE_FLAGS})
endif()
