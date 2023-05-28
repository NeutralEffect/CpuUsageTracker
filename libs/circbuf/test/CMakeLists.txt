# Circular buffer tests
project(CircularBufferTests)

set(CBUFTESTS_CLANG_COMPILE_FLAGS ${CUT_CLANG_FLAGS})
set(CBUFTESTS_GCC_COMPILE_FLAGS ${CUT_GCC_FLAGS})

add_executable(CircularBufferTests circbuf_tests.c)

add_test(
	NAME 	CircularBufferTests
	COMMAND CircularBufferTests
)

target_include_directories(CircularBufferTests
 	PRIVATE
		${CMAKE_SOURCE_DIR}/libs/circbuf/include
		${CMAKE_SOURCE_DIR}/libs/circbuf
)

target_sources(CircularBufferTests PRIVATE
 	${CMAKE_SOURCE_DIR}/libs/circbuf/circbuf.c)

set_target_properties(CircularBufferTests PROPERTIES
	C_STANDARD 11
 	C_STANDARD_REQUIRED ON
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/out"
)

target_compile_definitions(CircularBufferTests PRIVATE
	CUT_DISABLE_LOGGING)

 if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
 	target_compile_options(CircularBufferTests PRIVATE ${CBUFTESTS_CLANG_COMPILE_FLAGS})
 elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
 	target_compile_options(CircularBufferTests PRIVATE ${CBUFTESTS_GCC_COMPILE_FLAGS})
 endif()
