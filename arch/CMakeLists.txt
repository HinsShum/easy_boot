# @file arch/CMakeLists.txt
# @author HinsShum hinsshum@qq.com
# @date 2020/11/25 19:27:32
# @encoding utf-8
# @brief Here's the first line of every CMakeLists.txt,
#        which is the required name of the file CMake looks for:
cmake_minimum_required(VERSION 3.1)

# find the right directory by BOARD_NAME
if(DEFINED BOARD_NAME)
    execute_process(COMMAND find ${CMAKE_CURRENT_SOURCE_DIR} -name "${BOARD_NAME}" OUTPUT_VARIABLE ARCH_DIR)
    if("${ARCH_DIR}" STREQUAL "")
        message(FATAL_ERROR "No arch dir find, please check the board name")
    endif()
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" ARCH_DIR "${ARCH_DIR}")
    string(REPLACE "/board/${BOARD_NAME}\n" "" ARCH_DIR "${ARCH_DIR}")
    add_subdirectory(${ARCH_DIR})
    set(C_SOURCE_FILES ${C_SOURCE_FILES} PARENT_SCOPE)
endif()
