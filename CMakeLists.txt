cmake_minimum_required(VERSION 3.1)

project(cleansingFire)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

if(UNIX AND NOT APPLE)
	set(LINUX TRUE)
else()
endif()

if(LINUX)
	set (DIR_PLATFORM linux)
else()
	set (DIR_PLATFORM win)
    set(SFML_STATIC_LIBRARIES TRUE)
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

if(LINUX)
	target_compile_definitions(maverick_sfml PRIVATE NO_THOR)
else()
	target_compile_definitions(maverick_sfml PRIVATE SFML_STATIC)
	target_compile_definitions(cleansingFire_sfml PRIVATE SFML_STATIC)

	set_property(DIRECTORY ${CMAKE_CURRENT_LIST_DIR} PROPERTY VS_STARTUP_PROJECT cleansingFireApp)
	set(VS_STARTUP_PROJECT cleansingFireApp)
endif()

INCLUDE_DIRECTORIES(${MAVERICK_DIR}/include)
INCLUDE_DIRECTORIES(${MAVERICK_DIR}/specific/sfml/include)

INCLUDE_DIRECTORIES(${CLEANSINGFIRE_DIR}/include)
INCLUDE_DIRECTORIES(${CLEANSINGFIRE_DIR}/specific/sfml/include)

INCLUDE_DIRECTORIES(3rdparty/SFML-${DIR_PLATFORM}/include)
INCLUDE_DIRECTORIES(3rdparty/thor-${DIR_PLATFORM}/include)

if(LINUX)
	#thor binaries are not compatible with target sfml ver on linux. Too sad.

    #find_library(THOR_LIBRARY_D libthor-d.so HINTS 3rdparty/thor-${DIR_PLATFORM}/lib)
    #find_library(THOR_LIBRARY libthor.so HINTS 3rdparty/thor-${DIR_PLATFORM}/lib)
else()
    find_library(THOR_LIBRARY_D thor-s-d.lib HINTS 3rdparty/thor-${DIR_PLATFORM}/lib)
    find_library(THOR_LIBRARY thor-s.lib HINTS 3rdparty/thor-${DIR_PLATFORM}/lib)
endif()

add_executable(cleansingFireApp src/main.cpp)

find_package(SFML 2.5 COMPONENTS audio graphics REQUIRED)

add_dependencies(cleansingFire maverick)
add_dependencies(cleansingFire_sfml cleansingFire)
add_dependencies(cleansingFireApp cleansingFire_sfml)

target_link_libraries(cleansingFireApp cleansingFire_sfml maverick_sfml cleansingFire maverick)

if (NOT LINUX)
	target_link_libraries(cleansingFireApp debug ${THOR_LIBRARY_D} ${THOR_LIBRARY} )
endif()

target_link_libraries(cleansingFireApp sfml-audio sfml-graphics)


if(LINUX)
else()
    ADD_CUSTOM_COMMAND(
        TARGET cleansingFireApp
        POST_BUILD

    	COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_LIST_DIR}/Resources/openal32.dll
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )

endif()

