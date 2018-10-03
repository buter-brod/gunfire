cmake_minimum_required(VERSION 3.1)

project(gunfire)

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR 3rdparty/SFML/lib/cmake/SFML)

file(GLOB SRC 
	${CMAKE_CURRENT_LIST_DIR}/src/*.cpp
	${CMAKE_CURRENT_LIST_DIR}/src/include/*.h
	)

add_executable(gunfire src/main.cpp)
target_sources(gunfire PRIVATE ${SRC})


INCLUDE_DIRECTORIES(src/include)
INCLUDE_DIRECTORIES(3rdparty/thor/include)
LINK_DIRECTORIES(3rdparty/thor/lib)

find_library(THOR_LIBRARY_D thor-s-d.lib HINTS 3rdparty/thor/lib)
find_library(THOR_LIBRARY thor-s.lib HINTS 3rdparty/thor/lib)

target_link_libraries(gunfire debug ${THOR_LIBRARY_D})
target_link_libraries(gunfire ${THOR_LIBRARY})

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)
target_link_libraries(gunfire sfml-graphics sfml-audio)
