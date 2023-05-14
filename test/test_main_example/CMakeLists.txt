# add old test case
include_directories(
        ${IGUANA_DIR}
)

add_executable(test_ormpp
        main.cpp
        unit_test.hpp
        )

target_link_libraries(test_ormpp ${MYSQL_LIBRARY} pq sqlite3)

if (ENABLE_MYSQL AND MSVC)
    set_target_properties(test_ormpp PROPERTIES COMPILE_FLAGS "/MD")
endif ()
