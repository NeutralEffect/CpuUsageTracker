project(CpuUsageTrackerTests)

set(CUTTESTS_CLANG_COMPILE_FLAGS ${CUT_CLANG_FLAGS})
set(CUTTESTS_GCC_COMPILE_FLAGS ${CUT_GCC_FLAGS})

# CpuCount tests
add_executable(CpuCountTests cpucount_tests.c)

add_test(
	NAME 	CpuCountTests
	COMMAND CpuCountTests
)

target_include_directories(CpuCountTests
 	PRIVATE
		${CMAKE_SOURCE_DIR}/src/utils
		.
)

target_sources(CpuCountTests PRIVATE
 	${CMAKE_SOURCE_DIR}/src/utils/cpucount.c)

set_target_properties(CpuCountTests PROPERTIES
	C_STANDARD 11
 	C_STANDARD_REQUIRED ON
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out"
)

target_compile_definitions(CpuCountTests PRIVATE
	CUT_DISABLE_LOGGING)

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
	target_compile_options(CpuCountTests PRIVATE ${CUTTESTS_CLANG_COMPILE_FLAGS})
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
	target_compile_options(CpuCountTests PRIVATE ${CUTTESTS_GCC_COMPILE_FLAGS})
endif()


# Threadctl tests
add_executable(ThreadctlTests threadctl_tests.c)

add_test(
	NAME 	ThreadctlTests
	COMMAND ThreadctlTests
)

target_include_directories(ThreadctlTests
 	PRIVATE
		${CMAKE_SOURCE_DIR}/src/utils
		${CMAKE_SOURCE_DIR}/src/threads
)

target_sources(ThreadctlTests PRIVATE
 	${CMAKE_SOURCE_DIR}/src/utils/threadctl.c)

set_target_properties(ThreadctlTests PROPERTIES
	C_STANDARD 11
 	C_STANDARD_REQUIRED ON
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out"
)

target_compile_definitions(ThreadctlTests PRIVATE
	CUT_DISABLE_LOGGING)

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
	target_compile_options(ThreadctlTests PRIVATE ${CUTTESTS_CLANG_COMPILE_FLAGS})
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
	target_compile_options(ThreadctlTests PRIVATE ${CUTTESTS_GCC_COMPILE_FLAGS})
endif()
