# Include the ExternalProject module
include(ExternalProject)

# Ignore all warning from the third-party libraries
if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions("-w")
endif()

# Set the list of compiler flags for the external projects
if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(CXXFLAGS "-fPIC")
endif()

option(BUILD_THIRDPARTY "Build thirdparty libaries." OFF)
set(BUILD_THIRDPARTY OFF)

if (REFRESH_THIRDPARTY) 
	set(BUILD_THIRDPARTY ON)
else ()
	find_library(JSONUI_LIB jsonui PATHS /usr/local/lib NO_DEFAULT_PATH)
	if(NOT JSONUI_LIB)
        	set(BUILD_THIRDPARTY ON)
	endif()
endif()

if (BUILD_THIRDPARTY)
# Download and install the bsonui library
ExternalProject_Add(JSONUI
    PREFIX thirdparty
    GIT_REPOSITORY https://bitbucket.org/gems4/jsonui.git
#    GIT_TAG impex-move
    UPDATE_COMMAND ""
    CMAKE_ARGS -DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${THIRDPARTY_DIR}
               -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
               -DCMAKE_CXX_FLAGS=${CXXFLAGS}               
               -DCMAKE_INSTALL_INCLUDEDIR=include
               -DCMAKE_INSTALL_LIBDIR=lib
               -DCMAKE_INSTALL_BINDIR=bin
               -DBUILD_SHARED_LIBS=ON
)


# Create the install target for the third-party libraries
install(DIRECTORY ${THIRDPARTY_DIR}/lib
    DESTINATION /usr/local)
install(DIRECTORY ${THIRDPARTY_DIR}/include
    DESTINATION /usr/local)
#install(DIRECTORY ${THIRDPARTY_DIR}/lib
#   DESTINATION .)
#install(DIRECTORY ${THIRDPARTY_DIR}/include
#   DESTINATION .)

else ()
	message(STATUS "JSONUI already present at /usr/local/lib. CMake will Stop. \n Use -DREFRESH_THIRDPARTY=ON to reinstall ThirdParty")
endif()
