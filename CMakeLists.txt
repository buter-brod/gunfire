cmake_minimum_required(VERSION 3.1)

project(cleansingFire)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

set(SFML_STATIC_LIBRARIES TRUE)

if(UNIX AND NOT APPLE)
	set (DIR_PLATFORM linux)
else()
	set (DIR_PLATFORM win)
endif()

set (SFML_DIR 3rdparty/SFML-${DIR_PLATFORM}/lib/cmake/SFML)

set(CLEANSINGFIRE_DIR ${CMAKE_CURRENT_LIST_DIR}/src/cleansingFire)
set(MAVERICK_DIR ${CMAKE_CURRENT_LIST_DIR}/src/maverick)

file(GLOB MAVERICK_SRC 
	${MAVERICK_DIR}/*.cpp
	${MAVERICK_DIR}/include/*.h
	)
	
file(GLOB MAVERICK_SFML_SRC 
	${MAVERICK_DIR}/specific/sfml/*.cpp
	${MAVERICK_DIR}/specific/sfml/include/*.h
	)
	
file(GLOB CLEANSINGFIRE_SRC 
	${CLEANSINGFIRE_DIR}/*.cpp
	${CLEANSINGFIRE_DIR}/include/*.h
	)
	
file(GLOB CLEANSINGFIRE_SFML_SRC 
	${CLEANSINGFIRE_DIR}/specific/sfml/*.cpp
	${CLEANSINGFIRE_DIR}/specific/sfml/include/*.h
	)

add_library(maverick STATIC ${MAVERICK_SRC} )
add_library(maverick_sfml STATIC ${MAVERICK_SFML_SRC} )
add_library(cleansingFire STATIC ${CLEANSINGFIRE_SRC} )
add_library(cleansingFire_sfml STATIC ${CLEANSINGFIRE_SFML_SRC} )

target_compile_definitions(maverick_sfml PRIVATE SFML_STATIC)
target_compile_definitions(cleansingFire_sfml PRIVATE SFML_STATIC)

add_executable(cleansingFireApp src/main.cpp)

set_property(DIRECTORY ${CMAKE_CURRENT_LIST_DIR} PROPERTY VS_STARTUP_PROJECT cleansingFireApp)

set(VS_STARTUP_PROJECT cleansingFireApp)

INCLUDE_DIRECTORIES(${MAVERICK_DIR}/include)
INCLUDE_DIRECTORIES(${MAVERICK_DIR}/specific/sfml/include)

INCLUDE_DIRECTORIES(${CLEANSINGFIRE_DIR}/include)
INCLUDE_DIRECTORIES(${CLEANSINGFIRE_DIR}/specific/sfml/include)

INCLUDE_DIRECTORIES(3rdparty/SFML-${DIR_PLATFORM}/include)
INCLUDE_DIRECTORIES(3rdparty/thor-${DIR_PLATFORM}/include)
LINK_DIRECTORIES(3rdparty/thor/${DIR_PLATFORM}/lib)

find_library(THOR_LIBRARY_D thor-s-d.lib HINTS 3rdparty/thor-${DIR_PLATFORM}/lib)
find_library(THOR_LIBRARY thor-s.lib HINTS 3rdparty/thor-${DIR_PLATFORM}/lib)

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)
target_link_libraries(cleansingFireApp sfml-graphics sfml-audio)

target_link_libraries(cleansingFireApp maverick)
target_link_libraries(cleansingFireApp maverick_sfml)
target_link_libraries(cleansingFireApp cleansingFire)
target_link_libraries(cleansingFireApp cleansingFire_sfml)

target_link_libraries(cleansingFireApp debug ${THOR_LIBRARY_D})
target_link_libraries(cleansingFireApp ${THOR_LIBRARY})

ADD_CUSTOM_COMMAND(
    TARGET cleansingFire
    POST_BUILD
	
	COMMAND ${CMAKE_COMMAND} -E copy
${CMAKE_CURRENT_LIST_DIR}/Resources/openal32.dll
${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)