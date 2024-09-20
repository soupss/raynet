cmake_minimum_required(VERSION 3.13)
project(Raynet_client C)

set(CMAKE_C_COMPILER emcc)
set(CMAKE_OSX_ARCHITECTURES "")
set(CMAKE_SYSTEM_NAME Generic)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/inc")
set(BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin")

set(RAYLIB_DIR "/Users/anton/Development/raylib/src")
set(RAYLIB_WEB_DIR "${RAYLIB_DIR}/web")
set(RAYLIB_WEB_LIB "${RAYLIB_WEB_DIR}/libraylib.a")

file(GLOB SOURCES "${SRC_DIR}/*.c")

add_executable(out ${SOURCES})

set_target_properties(out PROPERTIES
    OUTPUT_NAME "out"
    SUFFIX ".html"
    RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}"
)

target_include_directories(out PRIVATE "${INC_DIR}" "${RAYLIB_DIR}")

target_compile_options(out PRIVATE -Wall -Wextra)

target_link_libraries(out PRIVATE "${RAYLIB_WEB_LIB}")

target_link_options(out PRIVATE
    "-L${RAYLIB_DIR}"
    "-sUSE_GLFW=3"
    "-sASYNCIFY"
    "--shell-file ${RAYLIB_DIR}/minshell.html"
)

add_custom_target(clean-all
    COMMAND ${CMAKE_BUILD_TOOL} clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${BIN_DIR}"
)
