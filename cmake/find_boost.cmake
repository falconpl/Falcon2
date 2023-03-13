

# Look for Boost in common installation directories on Linux and macOS
find_path(BOOST_ROOT version.hpp
    /usr/local/include/boost
    /usr/include/boost
    /opt/local/include/boost
    /usr/local/boost
    /usr/boost
    /opt/local/boost
)

# If Boost is found, set the Boost include and library paths
if (BOOST_ROOT)
    message(STATUS "Boost found: ${BOOST_ROOT}")
    #set(Boost_INCLUDE_DIRS ${BOOST_ROOT})
    #set(Boost_LIBRARY_DIRS ${BOOST_ROOT}/lib)
    #set(Boost_NO_SYSTEM_PATHS ON)
else()
    message(STATUS "Can't find Boost")
endif()
