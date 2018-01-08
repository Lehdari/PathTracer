cmake_minimum_required(VERSION 3.8)
project(PathTracer)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES)

add_subdirectory(src)

set(EXECUTABLE_NAME PathTracer)
add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

target_link_libraries(${EXECUTABLE_NAME})

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

#Find and link OpenGL
find_package(OpenGL REQUIRED)
if (OPENGL_FOUND)
    include_directories(${OPENGL_INCLUDE_DIRS})
    target_link_libraries(${EXECUTABLE_NAME} ${OPENGL_LIBRARIES})
endif()

#Find and link GLEW
list(APPEND CMAKE_PREFIX_PATH "C:/Program Files (x86)/glew")
find_package(GLEW REQUIRED)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
    target_link_libraries(${EXECUTABLE_NAME} ${GLEW_LIBRARIES})
endif()

#Find and link SFML
find_package(SFML 2 REQUIRED graphics window system)
if(SFML_FOUND)
    include_directories("${SFML_INCLUDE_DIR}")
    target_link_libraries(${EXECUTABLE_NAME} debug "${SFML_SYSTEM_LIBRARY_DEBUG}" "${SFML_WINDOW_LIBRARY_DEBUG}" "${SFML_GRAPHICS_LIBRARY_DEBUG}")
    target_link_libraries(${EXECUTABLE_NAME} optimized "${SFML_SYSTEM_LIBRARY_RELEASE}" "${SFML_WINDOW_LIBRARY_RELEASE}" "${SFML_GRAPHICS_LIBRARY_RELEASE}")
endif()