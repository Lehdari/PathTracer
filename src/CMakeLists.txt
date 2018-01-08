aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SUB_SOURCES)

set(SOURCE_FILES
        ${SOURCE_FILES}
        ${SUB_SOURCES}
        PARENT_SCOPE)