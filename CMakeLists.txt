CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(demo)
 
LIST( APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ext/cmake-lib" )
INCLUDE( pch_support )
INCLUDE( compiler_option )

OPTION( AUTO_DOWNLOAD_EXT "When needed external library is not found, Automatically download it." OFF )
OPTION( SUBMODULE_UPDATE "Try to update submodules" ON )

IF( SUBMODULES_UPDATE )
	MESSAGE( "Git Submodule update..." )
	EXECUTE_PROCESS( COMMAND git submodule update --init
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/.. )
	MESSAGE( "Git Submodule update complete" )
ENDIF(  )

SET( DEMO_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src )
SET( DEMO_RES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/res )
SET( DEMO_EXT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext )
SET( DEMO_EXE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/exe )
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin" )

#ADD_DEFINITIONS(-DUNICODE -D_UNICODE)
#ADD_DEFINITIONS(-DOGRE_WCHAR_T_STRINGS=1)

IF(WIN32)
	SET(CMAKE_MODULE_PATH "$ENV{OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
	SET(OGRE_SAMPLES_INCLUDEPATH
		$ENV{OGRE_HOME}/Samples/include
	)
ENDIF(WIN32)

IF(UNIX)
	IF(EXISTS "/usr/local/lib/OGRE/cmake")

		SET(CMAKE_MODULE_PATH "/usr/local/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
		SET(OGRE_SAMPLES_INCLUDEPATH "/usr/local/share/OGRE/samples/Common/include/") # We could just *assume* that developers uses this basepath : /usr/local

	ELSEIF(EXISTS "/usr/lib/OGRE/cmake")

		SET(CMAKE_MODULE_PATH "/usr/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
		SET(OGRE_SAMPLES_INCLUDEPATH "/usr/share/OGRE/samples/Common/include/") # Otherwise, this one

	ELSE ()
		MESSAGE(SEND_ERROR "Failed to find module path.")
	ENDIF(EXISTS "/usr/local/lib/OGRE")
ENDIF(UNIX)

IF (CMAKE_BUILD_TYPE STREQUAL "")
	# CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
	# differentiation between debug and release builds.
	SET(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
ENDIF ()
 
SET(CMAKE_DEBUG_POSTFIX "_d")
 
SET(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/dist")
 
FIND_PACKAGE(OGRE REQUIRED)
 
#IF(NOT "${OGRE_VERSION_NAME}" STREQUAL "Cthugha")
#  message(SEND_ERROR "You need Ogre 1.7 Cthugha to build this.")
#ENDIF()

FIND_PACKAGE(OIS REQUIRED)
 
IF(NOT OIS_FOUND)
	MESSAGE(SEND_ERROR "Failed to find OIS.")
ENDIF()

# Find Boost
IF (NOT OGRE_BUILD_PLATFORM_IPHONE)
	IF (WIN32 OR APPLE)
		SET(Boost_USE_STATIC_LIBS TRUE)
	ELSE ()
		# Statically linking boost to a dynamic Ogre build doesn't work on Linux 64bit
		SET(Boost_USE_STATIC_LIBS ${OGRE_STATIC})
	ENDIF ()
	IF (MINGW)
		# this is probably a bug in CMake: the boost find module tries to look for
		# boost libraries with name libboost_*, but CMake already prefixes library
		# search names with "lib". This is the workaround.
		SET(CMAKE_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES} "")
	ENDIF ()
	SET(Boost_ADDITIONAL_VERSIONS "1.44" "1.44.0" "1.42" "1.42.0" "1.41.0" "1.41" "1.40.0" "1.40" "1.39.0" "1.39" "1.38.0" "1.38" "1.37.0" "1.37" )
	# Components that need linking (NB does not include header-only components like bind)
	SET(OGRE_BOOST_COMPONENTS thread date_time)
	FIND_PACKAGE(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
	IF (NOT Boost_FOUND)
		# Try again with the other type of libs
		SET(Boost_USE_STATIC_LIBS NOT ${Boost_USE_STATIC_LIBS})
		FIND_PACKAGE(Boost COMPONENTS ${OGRE_BOOST_COMPONENTS} QUIET)
	ENDIF()
	FIND_PACKAGE(Boost QUIET)
 
	# Set up referencing of Boost
	#include_directories(${Boost_INCLUDE_DIR})
	#add_definitions(-DBOOST_ALL_NO_LIB)
	#SET(OGRE_LIBRARIES ${OGRE_LIBRARIES} ${Boost_LIBRARIES})
ENDIF()

IF( APPLE )
	SET( CMAKE_FIND_ROOT_PATH ${CMAKE_OSX_SYSROOT} )
ENDIF(  )

MACRO( APPEND_LINK_LIBS )
	LIST( APPEND DEMO_LINK_LIBS ${ARGN} )
ENDMACRO( APPEND_LINK_LIBS )

MACRO( APPEND_LINK_EXT_LIBS OPTIMIZED_LIB DEBUG_LIB )
	LIST( APPEND DEMO_LINK_EXT_LIBS optimized ${OPTIMIZED_LIB} debug ${DEBUG_LIB} )
ENDMACRO( APPEND_LINK_EXT_LIBS OPTIMIZED_LIB DEBUG_LIB )

IF( MSVC )
	APPEND_LINK_LIBS( Setupapi winmm )
ENDIF()

# LibOVR
# Enable ovr assert
ADD_DEFINITIONS(-DOVR_BUILD_DEBUG=1)
ADD_SUBDIRECTORY( ${DEMO_EXT_DIR}/libovr )
INCLUDE_DIRECTORIES( ${LIBOVR_INC_DIR} )
APPEND_LINK_LIBS( LibOVR )
USE_CPP11( TARGET LibOVR )

FILE( GLOB DEMO_SRC_FILES "${DEMO_SRC_DIR}/*.*" )
FILE( GLOB DEMO_EXE_FILES "${DEMO_EXE_DIR}/*.*" )

INCLUDE_DIRECTORIES( ${OIS_INCLUDE_DIRS}
	${OGRE_INCLUDE_DIRS}
	${DEMO_SRC_DIR} )

# demo common library
ADD_LIBRARY( demo_lib
	${DEMO_SRC_FILES}
	"${DEMO_SRC_DIR}/stdafx.h"
	"${DEMO_SRC_DIR}/stdafx.cpp"	)
	
MSVC_SET_PCH( demo_lib
	"${DEMO_SRC_DIR}/stdafx.h"
	"${DEMO_SRC_DIR}/stdafx.cpp"	)
	
SOURCE_GROUP( "" FILES 
	${DEMO_SRC_FILES} )

# demo executable
# subsystem = window
#IF( WIN32 )
#	LIST( APPEND DEMO_EXE_OPTION WIN32 )
#ENDIF(  )
ADD_EXECUTABLE( demo ${DEMO_EXE_OPTION} ${DEMO_EXE_FILES} )
	
SOURCE_GROUP( "" FILES ${DEMO_EXE_FILES} )

USE_CPP11( TARGET demo_lib demo )

SET_TARGET_PROPERTIES(demo_lib PROPERTIES DEBUG_POSTFIX _d)
SET_TARGET_PROPERTIES(demo PROPERTIES DEBUG_POSTFIX _d)

TARGET_LINK_LIBRARIES(demo 
	${OGRE_LIBRARIES} 
	${OIS_LIBRARIES} 
	${DEMO_LINK_LIBS} 
	demo_lib 
	${DEMO_LINK_EXT_LIBS} )

FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/bin)
FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist/media)
 
# post-build copy for win32
IF(WIN32 AND NOT MINGW)
	ADD_CUSTOM_COMMAND( TARGET demo PRE_BUILD
		COMMAND if not exist .\\dist\\bin mkdir .\\dist\\bin )
	ADD_CUSTOM_COMMAND( TARGET demo POST_BUILD
		COMMAND copy \"$(TargetPath)\" .\\dist\\bin )
ENDIF(WIN32 AND NOT MINGW)

IF(MINGW OR UNIX)
	SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/dist/bin)
ENDIF(MINGW OR UNIX)
 
IF(WIN32)
 
	INSTALL(TARGETS demo
		RUNTIME DESTINATION bin
		CONFIGURATIONS All)
 
	#Resources are too big to copy
	#INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/dist/Media
	#	DESTINATION ./
	#	CONFIGURATIONS Release RelWithDebInfo Debug
	#)
 
	INSTALL(FILES ${CMAKE_SOURCE_DIR}/cfg/win32/plugins.cfg
		${CMAKE_SOURCE_DIR}/cfg/win32/resources.cfg
		DESTINATION bin
		CONFIGURATIONS Release RelWithDebInfo
	)
 
	INSTALL(FILES ${CMAKE_SOURCE_DIR}/cfg/win32/plugins_d.cfg
		${CMAKE_SOURCE_DIR}/cfg/win32/resources_d.cfg
		DESTINATION bin
		CONFIGURATIONS Debug
	)
 
	INSTALL(FILES ${OGRE_PLUGIN_DIR_REL}/OgreMain.dll
		${OGRE_PLUGIN_DIR_REL}/RenderSystem_Direct3D9.dll
		${OGRE_PLUGIN_DIR_REL}/RenderSystem_GL.dll
		${OGRE_PLUGIN_DIR_REL}/Plugin_CgProgramManager.dll
		${OGRE_PLUGIN_DIR_REL}/cg.dll
		${OGRE_PLUGIN_DIR_REL}/OIS.dll
		DESTINATION bin
		CONFIGURATIONS Release RelWithDebInfo
	)
 
	INSTALL(FILES ${OGRE_PLUGIN_DIR_DBG}/OgreMain_d.dll
		${OGRE_PLUGIN_DIR_DBG}/RenderSystem_Direct3D9_d.dll
		${OGRE_PLUGIN_DIR_DBG}/RenderSystem_GL_d.dll
		${OGRE_PLUGIN_DIR_DBG}/Plugin_CgProgramManager_d.dll
		${OGRE_PLUGIN_DIR_DBG}/cg.dll
		${OGRE_PLUGIN_DIR_DBG}/OIS_d.dll
		DESTINATION bin
		CONFIGURATIONS Debug
	)
ENDIF(WIN32)

IF(UNIX)
	INSTALL(TARGETS demo
		RUNTIME DESTINATION bin
		CONFIGURATIONS All)
 
	INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/dist/media
		DESTINATION ./
		CONFIGURATIONS Release RelWithDebInfo Debug
	)
 
	INSTALL(FILES ${CMAKE_SOURCE_DIR}/cfg/linux/plugins.cfg
		${CMAKE_SOURCE_DIR}/cfg/linux/resources.cfg
		DESTINATION bin
		CONFIGURATIONS Release RelWithDebInfo Debug
	)
 
ENDIF(UNIX)