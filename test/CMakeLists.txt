message("weng mark! ${IGUANA_DIR}")
include_directories(
        ${IGUANA_DIR}
)

add_executable(test_ormpp
        main.cpp
        unit_test.hpp
)

target_link_libraries(test_ormpp mysqlclient pq)