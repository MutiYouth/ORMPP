set(ORMPP_VERSION_MAJOR 0)
set(ORMPP_VERSION_MINOR 1)
set(ORMPP_VERSION ${ORMPP_VERSION_MAJOR}.${ORMPP_VERSION_MINOR})

# Prevent in source builds
if (EXISTS "${PROJECT_BINARY_DIR}/CMakeLists.txt")
    message(FATAL_ERROR "Source build detected: please use a subdir. You may remove 'CMakeCache.txt' and 'CMakeFiles'.")
endif ()

# Default build type (Override with cmake .. -DCMAKE_BUILD_TYPE=...)
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
    message(STATUS "Setting build type to '${CMAKE_BUILD_TYPE}' as none was specified.")
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif ()

