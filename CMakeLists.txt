cmake_minimum_required(VERSION 3.2)
project(ormpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++17")
set(ENABLE_MYSQL ON)
set(ENABLE_PG OFF)
set(ENABLE_SQLITE3 OFF)

set(SOURCE_FILES
        src/main.cpp
        src/dbng.hpp
        src/unit_test.hpp
        src/pg_types.h
        src/type_mapping.hpp
        src/utility.hpp
        src/entity.hpp
        src/connection_pool.hpp
        src/ormpp_cfg.hpp)
if (ENABLE_MYSQL)
    add_definitions(-DORMPP_ENABLE_MYSQL)
    list(APPEND SOURCE_FILES src/mysql.hpp)
endif ()

if (ENABLE_SQLITE3)
    add_definitions(-DORMPP_ENABLE_SQLITE3)
    list(APPEND SOURCE_FILES src/sqlite.hpp)
endif ()

if (ENABLE_PG)
    add_definitions(-DORMPP_ENABLE_PG)
    list(APPEND SOURCE_FILES src/postgresql.hpp)
endif ()




include_directories(
    ${CMAKE_SOURCE_DIR}/thirdparty/iguana
)
add_executable(ormpp ${SOURCE_FILES})

if (ENABLE_MYSQL)
    target_link_libraries(ormpp mysqlclient)
endif ()
if (ENABLE_SQLITE3)
    target_link_libraries(ormpp sqlite3)
endif ()
if (ENABLE_PG)
    target_link_libraries(ormpp pq)
endif ()
