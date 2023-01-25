cmake_minimum_required(VERSION 3.2)
project(ORMPP)


######################################################
## settings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++20")  # use c++20, because of iguana used.
set(ENABLE_MYSQL ON)
set(ENABLE_PG ON)
set(ENABLE_SQLITE3 OFF)

# Make our own cmake imports accessible
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(common_starter)
include(set_platform_vars)



######################################################
## prepare
# add iguana env of include
set(IGUANA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/iguana)
include_directories(
        ${IGUANA_DIR}/third_party/frozen/include
)

set(SOURCE_FILES
        src/core/dbng.hpp
        src/core/type_mapping.hpp
        src/core/utility.hpp
        src/core/entity.hpp
        src/core/connection_pool.hpp
        src/core/ormpp_cfg.hpp
        src/dbs/pg_types.h
        test/main.cpp
        )
if (ENABLE_MYSQL)
    add_definitions(-DORMPP_ENABLE_MYSQL)
    list(APPEND SOURCE_FILES src/dbs/mysql.hpp)
endif ()

if (ENABLE_SQLITE3)
    add_definitions(-DORMPP_ENABLE_SQLITE3)
    list(APPEND SOURCE_FILES src/dbs/sqlite.hpp)
endif ()

if (ENABLE_PG)
    add_definitions(-DORMPP_ENABLE_PG)
    list(APPEND SOURCE_FILES src/dbs/postgresql.hpp)
endif ()


######################################################
## add test
include_directories(
    ${CMAKE_SOURCE_DIR}/thirdparty/iguana
)
add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

# link libraries
if (ENABLE_MYSQL)
    if(MSVC)
        target_link_libraries(${CMAKE_PROJECT_NAME} libmysql)
    else()
        target_link_libraries(${CMAKE_PROJECT_NAME} mysqlclient)
    endif()
endif ()
if (ENABLE_SQLITE3)
    target_link_libraries(${CMAKE_PROJECT_NAME} sqlite3)
endif ()
if (ENABLE_PG)
    target_link_libraries(${CMAKE_PROJECT_NAME} pq)
endif ()


#######################################################
## others

# install and uninstall 
set(ormpp_components_list "")
list(APPEND ormpp_components_list ${CMAKE_PROJECT_NAME})
include(cmake_install_abouts)

# add
add_subdirectory(test)

# add examples
add_subdirectory(src/examples)

# ref
add_subdirectory(ref/xorm)
