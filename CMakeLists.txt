
# ZeroMQ Integration
set(ZeroMQ_INCLUDE_DIR "/usr/include")
set(ZeroMQ_LIBRARY "/usr/lib/x86_64-linux-gnu/libzmq.a")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/cppzmq)

# MbedUtils Simulation Headers
add_library(mbedutils_sim_headers INTERFACE)
target_include_directories(mbedutils_sim_headers INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ZeroMQ_INCLUDE_DIR}
)


# STL Based Library Integration for MbedUtils
# Targets are defined in: https://github.com/brandonbraun653/mbedutils
file(GLOB STL_INTF_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library(mbedutils_lib_sim STATIC ${STL_INTF_SOURCES})
target_link_libraries(mbedutils_lib_sim
    PUBLIC
        cppzmq
    PRIVATE
        gssapi_krb5
        mbedutils_headers
        mbedutils_internal_headers
        norm
        pgm
        pthread
        sodium
        zmq
)
