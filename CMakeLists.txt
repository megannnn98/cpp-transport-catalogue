# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

cmake_minimum_required(VERSION 3.5)
project(transport)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

include_directories(
    .
)

set (SRCS
    domain.cpp
    domain.h
    geo.cpp
    geo.h
    graph.h
    json_builder.cpp
    json_builder.h
    json_reader.cpp
    json_reader.h
    json.cpp
    json.h
    main.cpp
    map_renderer.cpp
    map_renderer.h
    ranges.h
    request_handler.cpp
    request_handler.h
    router.h
    svg.cpp
    svg.h
    transport_catalogue.cpp
    transport_catalogue.h
    transport_router.cpp
    transport_router.h
)


add_executable(${CMAKE_PROJECT_NAME} ${SRCS})

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets)
