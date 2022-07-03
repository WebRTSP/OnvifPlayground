cmake_minimum_required(VERSION 3.16)

project(OnvifPlayground)

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_search_module(GSOAP REQUIRED gsoap++)
set(GSOAP_SHARE_DIR /usr/share)
set(GSOAP_PLUGIN_DIR ${GSOAP_SHARE_DIR}/gsoap/plugin)


file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp
    *.h
    onvif/*.h
    onvif/*.cpp
    onvif/*.nsmap
    onvif/*.sh
#    discovery/*.h
#    discovery/*.cpp
#    discovery/*.nsmap
#    discovery/*.sh
#    ${GSOAP_PLUGIN_DIR}/wsaapi.h
#    ${GSOAP_PLUGIN_DIR}/wsaapi.c
#    ${GSOAP_PLUGIN_DIR}/wsddapi.h
#    ${GSOAP_PLUGIN_DIR}/wsddapi.c
    *.cmake)

set_source_files_properties(${GSOAP_PLUGIN_DIR}/wsaapi.c PROPERTIES LANGUAGE CXX )
set_source_files_properties(${GSOAP_PLUGIN_DIR}/wsddapi.c PROPERTIES LANGUAGE CXX )

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE
    ${GSOAP_INCLUDE_DIRS}
    ${GSOAP_SHARE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/discovery)
target_link_libraries(${PROJECT_NAME}
    ${GSOAP_LDFLAGS}
    Threads::Threads)
#target_compile_definitions(${PROJECT_NAME} PUBLIC SOAP_H_FILE=wsddH.h)

target_precompile_headers(${PROJECT_NAME} PRIVATE pch.h)

#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
