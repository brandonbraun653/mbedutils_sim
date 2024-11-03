# STL Based Library Integration for MbedUtils
# Targets are defined in: https://github.com/brandonbraun653/mbedutils
file(GLOB STL_INTF_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library(mbedutils_lib_sim STATIC ${STL_INTF_SOURCES})
target_link_libraries(mbedutils_lib_sim PRIVATE
    mbedutils_headers
    mbedutils_internal_headers
)
