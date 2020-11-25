# @file bsp/CMakeLists.txt
# @author HinsShum hinsshum@qq.com
# @date 2020/11/25 20:21:18
# @encoding utf-8
# @brief Here's the first line of every CMakeLists.txt,
#        which is the required name of the file CMake looks for:
cmake_minimum_required(VERSION 3.1)

if(DEFINED BOARD_NAME)
    execute_process(COMMAND find ${CMAKE_CURRENT_SOURCE_DIR} -name "${BOARD_NAME}" OUTPUT_VARIABLE ARCH_DIR)
    if("${ARCH_DIR}" STREQUAL "")
        message(FATAL_ERROR "No arch dir find, please check the board name")
    endif()
    add_subdirectory(${BOARD_NAME})
    set(C_SOURCE_FILES ${C_SOURCE_FILES} PARENT_SCOPE)
endif()
