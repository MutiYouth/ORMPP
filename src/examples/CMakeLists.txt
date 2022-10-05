# pg
add_executable(example_pg
        example_pg.cpp
        )
target_link_libraries(example_pg pq)


# mysql
add_executable(example_mysql
        example_mysql.cpp
        )
target_link_libraries(example_mysql mysqlclient)
