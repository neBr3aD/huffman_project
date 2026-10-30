cmake_minimum_required(VERSION 3.16)

project(huffman_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT Python3_EXECUTABLE)
    set(_venv_python "${CMAKE_CURRENT_SOURCE_DIR}/venv/bin/python")
    if(EXISTS "${_venv_python}")
        set(Python3_EXECUTABLE "${_venv_python}" CACHE FILEPATH
            "Python interpreter used to build the Huffman extension")
    endif()
endif()

find_package(Python3 COMPONENTS Interpreter Development.Module REQUIRED)

execute_process(
    COMMAND "${Python3_EXECUTABLE}" -m pybind11 --cmakedir
    OUTPUT_VARIABLE PYBIND11_CMAKE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE PYBIND11_CMAKE_DIR_RESULT
)
if(NOT PYBIND11_CMAKE_DIR_RESULT EQUAL 0)
    message(FATAL_ERROR
        "pybind11 is not installed for ${Python3_EXECUTABLE}. "
        "Install it with: ${Python3_EXECUTABLE} -m pip install pybind11")
endif()

find_package(pybind11 CONFIG REQUIRED PATHS "${PYBIND11_CMAKE_DIR}" NO_DEFAULT_PATH)

pybind11_add_module(huffman_archiver src/pybind_wrapper.cpp)
set_target_properties(huffman_archiver PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
