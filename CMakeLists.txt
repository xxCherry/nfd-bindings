cmake_minimum_required(VERSION 3.28)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(nfd_bindings)

set(CMAKE_CXX_STANDARD          26)
set(CMAKE_CXX_EXTENSIONS        ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(NODE_DOWNLOAD_URL https://nodejs.org/dist/v24.10.0/win-x64/node.lib)
set(NODE_API_INCLUDE_URL https://github.com/nodejs/node-api-headers/raw/refs/heads/main/include)
set(NODE_ADDON_API_INCLUDE_URL https://raw.githubusercontent.com/nodejs/node-addon-api/refs/heads/main/)

# node headers download

file(DOWNLOAD
    ${NODE_DOWNLOAD_URL}
    ${CMAKE_SOURCE_DIR}/lib/node.lib)

file(DOWNLOAD
    ${NODE_API_INCLUDE_URL}/js_native_api.h
    ${CMAKE_SOURCE_DIR}/include/js_native_api.h)

file(DOWNLOAD
    ${NODE_API_INCLUDE_URL}/js_native_api_types.h
    ${CMAKE_SOURCE_DIR}/include/js_native_api_types.h)

file(DOWNLOAD
    ${NODE_API_INCLUDE_URL}/node_api.h
    ${CMAKE_SOURCE_DIR}/include/node_api.h)

file(DOWNLOAD
    ${NODE_API_INCLUDE_URL}/node_api_types.h
    ${CMAKE_SOURCE_DIR}/include/node_api_types.h)

file(DOWNLOAD
    ${NODE_ADDON_API_INCLUDE_URL}/napi-inl.h
    ${CMAKE_SOURCE_DIR}/include/napi-inl.h)

file(DOWNLOAD
    ${NODE_ADDON_API_INCLUDE_URL}/napi.h
    ${CMAKE_SOURCE_DIR}/include/napi.h)

add_compile_definitions(NODE_ADDON_API_DISABLE_DEPRECATED)

add_compile_options(
  -Wall
  -Wextra
  -fno-rtti
)

file(
  GLOB_RECURSE
  SOURCES
  src/*.cpp
)

add_library(
  ${PROJECT_NAME}
  SHARED
  ${SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_directories(${PROJECT_NAME} PRIVATE lib)

target_link_libraries(${PROJECT_NAME} PRIVATE node.lib)