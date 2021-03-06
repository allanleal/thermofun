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

# Download and install the Reaktoro library
ExternalProject_Add(REAKTORO
    PREFIX thirdparty
    GIT_REPOSITORY https://allanleal@bitbucket.org/reaktoro/reaktoro.git
    GIT_TAG ${REAKTORO_TAG}
    UPDATE_COMMAND ""
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${THIRDPARTY_DIR}
               -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
               -DBUILD_SHARED_LIBS=ON
               -DLINK_GEMS=ON
)

# Create the install target for the third-party libraries
install(DIRECTORY ${THIRDPARTY_DIR}/lib 
    DESTINATION /usr/local)
install(DIRECTORY ${THIRDPARTY_DIR}/include 
    DESTINATION /usr/local)
