target_include_directories(${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/utils
		${CMAKE_CURRENT_SOURCE_DIR}/threads)

target_sources(${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/threads/analyzer.c
		${CMAKE_CURRENT_SOURCE_DIR}/threads/printer.c
		${CMAKE_CURRENT_SOURCE_DIR}/threads/reader.c
		${CMAKE_CURRENT_SOURCE_DIR}/utils/file.c
		${CMAKE_CURRENT_SOURCE_DIR}/utils/log.c
		${CMAKE_CURRENT_SOURCE_DIR}/utils/procstat.c)