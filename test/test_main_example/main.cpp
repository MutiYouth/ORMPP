#ifdef _MSC_VER

#ifdef _WIN64
#include <WinSock2.h>
#elif _WIN32
#include <winsock.h>
#endif

#endif



#include <iostream>
#include <thread>


#include "../../src/core/connection_pool.hpp"
#include "../../src/core/dbng.hpp"
#include "../../src/core/ormpp_cfg.hpp"

#ifdef ORMPP_ENABLE_MYSQL

#include "../../src/dbs/mysql.hpp"

#endif
#ifdef ORMPP_ENABLE_SQLITE3

#include "../../src/dbs/sqlite.hpp"

#endif
#ifdef ORMPP_ENABLE_PG

#include "../../src/dbs/postgresql.hpp"

#endif


#define TEST_MAIN

#include "unit_test.hpp"



using namespace std::string_literals;

struct test_tb
{
    int id;
    char name[12];
};
REFLECTION(test_tb, id, name);

struct person
{
    int id;
    std::string name;
    int age;
};
REFLECTION(person, id, name, age
)

struct student
{
    int code; // key
    std::string name;
    char sex;
    int age;
    double dm;
    std::string classroom;
};
REFLECTION(student, code, name, sex, age, dm, classroom
)

struct simple
{
    int id;
    double code;
    int age;
};
REFLECTION(simple, id, code, age
);

using namespace ormpp;
const char *ip = "192.168.0.106"; // your database ip
const char *db_user_pg = "pi";
const char *db_user_mysql = "root";
const char *db_pwd = "weng2022";
const char *db_name = "test";


/*
TEST_CASE(mysql_performance){
   dbng<mysql> mysql;

   TEST_REQUIRE(mysql.connect(ip, db_user_mysql, db_pwd, db_name));
   TEST_REQUIRE(mysql.execute("DROP TABLE IF EXISTS student"));

   ormpp_auto_increment_key auto_key{"code"};
   TEST_REQUIRE(mysql.create_datatable<student>(auto_key));

   using namespace std::chrono;
   auto m_begin = high_resolution_clock::now();
   for (int i = 0; i < 10000; ++i) {
       mysql.insert(student{i, "tom", 0, i, 1.5, "classroom1"});
   }
   auto s = duration_cast<duration<double>>(high_resolution_clock::now() -
   m_begin).count(); std::cout<<s<<'\n';

   m_begin = high_resolution_clock::now();
   std::vector<student> v;
   for (int i = 0; i < 10000; ++i) {
       v.push_back(student{i, "tom", 0, i, 1.5, "classroom1"});
   }
   mysql.insert(v);
   s = duration_cast<duration<double>>(high_resolution_clock::now() -
   m_begin).count(); std::cout<<s<<'\n';

   m_begin = high_resolution_clock::now();
   for (int j = 0; j < 100; ++j) {
       TEST_REQUIRE(!mysql.query<student>("limit 1000").empty());
   }
   s = duration_cast<duration<double>>(high_resolution_clock::now() -
   m_begin).count(); std::cout<<s<<'\n';
}
*/

template<class T, size_t N>
constexpr size_t size(T (&)[N]) {
    return N;
}


// https://github.com/qicosmos/ormpp/commits/master?after=a2a9d0963fe8f7123da4a701c7368d9e17ee516c+244&branch=master&qualified_name=refs%2Fheads%2Fmaster

struct test_order {
    int id;
    std::string name;
};
REFLECTION(test_order, name, id);

struct dummy
{
    int id;
    std::string name;
};
REFLECTION(dummy, id, name);



#ifdef ORMPP_ENABLE_MYSQL

TEST_CASE(random_reflection_order)
{
    dbng<mysql> mysql;
    TEST_REQUIRE(mysql.connect(ip, "root", "12345", "testdb", /*timeout_seconds=*/5, 3306));
    TEST_REQUIRE(mysql.execute("create table if not exists `test_order` (id int, name text);"));
    mysql.delete_records<test_order>();
    int id = 666;
    std::string name = "hello";
    mysql.insert(test_order { id, name });
    auto v = mysql.query<test_order>();
    TEST_REQUIRE(v.size() > 0);
    TEST_CHECK(v.front().id == id);
    TEST_CHECK(v.front().name == name);
}

TEST_CASE(mysql_exist_tb) {
    dbng<mysql> mysql;
    TEST_REQUIRE(mysql.connect(ip, db_user_mysql, db_pwd, db_name, 3306, /*timeout_seconds=*/5));
    dummy d{0, "tom"};
    dummy d1{0, "jerry"};
    if (mysql.insert(d) < 0) {
        std::cout << "table `dummy` not exit, create it. create result: ";
        auto result_c = mysql.create_datatable<dummy>();
        std::cout << result_c << std::endl;
    }

    auto result_1 = mysql.insert(d);
    auto result_2 = mysql.insert(d1);
    auto v = mysql.query<dummy>("limit 1, 1");
    TEST_CHECK(v.size() > 0);
    std::cout << "mysql_exist_tb test: " << v.size() << "\n";
}

TEST_CASE(mysql_pool) {
    /*
    dbng<sqlite> sqlite;
    sqlite.connect(db_name);
    sqlite.create_datatable<test_tb>(ormpp_unique{"name"});
    test_tb tb{1, "aa"};
    sqlite.insert(tb);
    auto vt = sqlite.query<test_tb>();
    auto vt1 = sqlite.query<std::tuple<test_tb>>("select * from test_tb");
    auto &pool = connection_pool<dbng<mysql>>::instance();
    try {
        pool.init(1, ip, db_user_mysql, db_pwd, db_name, 2);
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        return;
    }
    auto con = pool.get();
    auto v = con->query<std::tuple<test_tb>>("select * from test_tb");
    con->create_datatable<test_tb>(ormpp_unique{"name"});
    for (int i = 0; i < 10; ++i) {
        auto conn = pool.get();
        //        conn_guard guard(conn);
        if (conn == nullptr) {
            std::cout << "no available conneciton" << std::endl;
            break;
        }

        bool r = conn->create_datatable<person>();
    }
    */
}

#endif

TEST_CASE(test_ormpp_cfg) {
    ormpp_cfg cfg{};
    bool ret = config_manager::from_file(cfg, "../../../data/config/ormpp.cfg");
    if (!ret) {
        return;
    }

#ifdef ORMPP_ENABLE_MYSQL
    auto &pool = connection_pool<dbng<mysql>>::instance();
    try {
        cfg.db_port = 3306;

        pool.init(cfg.db_conn_num, cfg.db_ip.data(), cfg.user_name.data(), cfg.pwd.data(), cfg.db_name.data(), cfg.db_port);
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        return;
    }

    auto conn1 = pool.get();
    auto result1 = conn1->query<student>();
    TEST_CHECK(result1.size() > 0);
    std::cout << "cfg file parse test: " << result1.size() << std::endl;
#endif
}

#ifdef ORMPP_ENABLE_PG
TEST_CASE(postgres_pool) {
    auto &pool = connection_pool<dbng<postgresql >>::instance();
    try {
        pool.init(3, ip, db_user_mysql, db_pwd, db_name, 2);
        pool.init(7, ip, db_user_mysql, db_pwd, db_name, 2);
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        return;
    }

    auto conn1 = pool.get();
    auto conn2 = pool.get();
    auto conn3 = pool.get();

    std::thread thd([conn2, &pool] {
        std::this_thread::sleep_for(std::chrono::seconds(15));
        pool.return_back(conn2);
    });

    auto conn4 = pool.get(); // 10s later, timeout
    TEST_CHECK(conn4 == nullptr);
    auto conn5 = pool.get();
    TEST_CHECK(conn5 != nullptr);

    thd.join();

    for (int i = 0; i < 10; ++i) {
        auto conn = pool.get();
        // conn_guard guard(conn);
        if (conn == nullptr) {
            std::cout << "no available conneciton" << std::endl;
            continue;
        }

        bool r = conn->create_datatable<person>();
    }
}

#endif

TEST_CASE(orm_connect) {
    int timeout = 5;

#ifdef ORMPP_ENABLE_PG
    dbng<postgresql> postgres;
    TEST_REQUIRE(postgres.connect(ip, db_user_pg, db_pwd, db_name));
    TEST_REQUIRE(postgres.disconnect());
    TEST_REQUIRE(postgres.connect(ip, "xxxx", db_pwd, db_name, timeout));
#endif

#ifdef ORMPP_ENABLE_MYSQL
    dbng<mysql> mysql;
    TEST_REQUIRE(mysql.connect(ip, db_user_mysql, db_pwd, db_name));
    TEST_REQUIRE(mysql.disconnect());
    TEST_REQUIRE(mysql.connect(ip, db_user_mysql, db_pwd, db_name, timeout));
#endif

#ifdef ORMPP_ENABLE_SQLITE3
    dbng<sqlite> sqlite;
    TEST_REQUIRE(sqlite.connect("test.db"));
    TEST_REQUIRE(sqlite.disconnect());
#endif
}

TEST_CASE(orm_create_table) {
    ormpp_key key{"id"};
    ormpp_not_null not_null{{"id", "age"}};
    ormpp_auto_key auto_key{"id"};

#ifdef ORMPP_ENABLE_PG
    dbng<postgresql> postgres;
    TEST_REQUIRE(postgres.connect(ip, db_user_pg, db_pwd, db_name));
    TEST_REQUIRE(postgres.create_datatable<person>());
    TEST_REQUIRE(postgres.create_datatable<person>(key));
    TEST_REQUIRE(postgres.create_datatable<person>(not_null));
    TEST_REQUIRE(postgres.create_datatable<person>(key, not_null));
    TEST_REQUIRE(postgres.create_datatable<person>(not_null, key));
    TEST_REQUIRE(postgres.create_datatable<person>(auto_key));
    TEST_REQUIRE(postgres.create_datatable<person>(auto_key, not_null));
    TEST_REQUIRE(postgres.create_datatable<person>(not_null, auto_key));
#endif

#ifdef ORMPP_ENABLE_SQLITE3
    dbng<sqlite> sqlite;
    TEST_REQUIRE(sqlite.connect("test.db"));
    TEST_REQUIRE(sqlite.create_datatable<person>());
    TEST_REQUIRE(sqlite.create_datatable<person>(key));
    TEST_REQUIRE(sqlite.create_datatable<person>(not_null));
    TEST_REQUIRE(sqlite.create_datatable<person>(key, not_null));
    TEST_REQUIRE(sqlite.create_datatable<person>(not_null, key));
    TEST_REQUIRE(sqlite.create_datatable<person>(auto_key));
    TEST_REQUIRE(sqlite.create_datatable<person>(auto_key, not_null));
    TEST_REQUIRE(sqlite.create_datatable<person>(not_null, auto_key));
#endif

#ifdef ORMPP_ENABLE_MYSQL
    dbng<mysql> mysql;
    TEST_REQUIRE(mysql.connect(ip, db_user_mysql, db_pwd, db_name));
    TEST_REQUIRE(mysql.create_datatable<person>());
    TEST_REQUIRE(mysql.create_datatable<person>(key));
    TEST_REQUIRE(mysql.create_datatable<person>(not_null));
    TEST_REQUIRE(mysql.create_datatable<person>(key, not_null));
    TEST_REQUIRE(mysql.create_datatable<person>(not_null, key));
    TEST_REQUIRE(mysql.create_datatable<person>(auto_key));
    TEST_REQUIRE(mysql.create_datatable<person>(auto_key, not_null));
    TEST_REQUIRE(mysql.create_datatable<person>(not_null, auto_key));
#endif
}

TEST_CASE(orm_insert_query) {
    ormpp_key key{"code"};
    ormpp_not_null not_null{{"code", "age"}};
    ormpp_auto_key auto_key{"code"};

    student s = {0, "tom", 0, 19, 1.5, "room2"};
    student s1 = {0, "jack", 1, 20, 2.5, "room3"};
    student s2 = {0, "mke", 2, 21, 3.5, "room4"};
    std::vector<student> v{s1, s2};

    // mysql
#ifdef ORMPP_ENABLE_MYSQL
    dbng<mysql> mysql;
    TEST_REQUIRE(mysql.connect(ip, db_user_mysql, db_pwd, db_name));
    auto vv0 = mysql.query(FID(simple::id), "<", "5");
    auto vv = mysql.query(FID(simple::id), "<", 5);
    auto vv3 = mysql.query(FID(person::name), "<", "5");
    auto vv5 = mysql.query(FID(person::name), "<", 5);
    mysql.delete_records(FID(simple::id), "=", 3);
#endif


    // pg
#ifdef ORMPP_ENABLE_PG
    dbng<postgresql> pg_db;
    TEST_REQUIRE(pg_db.connect(ip, db_user_pg, db_pwd, db_name));
    auto vv1 = pg_db.query(FID(simple::id), "<", "5");
#endif

    // sqlite
#ifdef ORMPP_ENABLE_SQLITE3
    dbng<sqlite> sqlite;
    TEST_REQUIRE(sqlite.connect("test.db"));
    auto vv2 = sqlite.query(FID(simple::id), "<", "5");
#endif

    // auto key
    {
#ifdef ORMPP_ENABLE_PG
        TEST_REQUIRE(pg_db.create_datatable<student>(auto_key, not_null));
        // WENG bug found 22-10-5 23:44: 这里的insert报错。  done, 23-1-26 20:50. 不影响运行。
        auto result1 = pg_db.insert(s);
        TEST_CHECK(result1 == 1);
        auto result2 = pg_db.query<student>();
        TEST_CHECK(result2.size() == 1);
        auto result3 = pg_db.insert(v);
        TEST_CHECK(result3 == 2);
#endif

#ifdef ORMPP_ENABLE_SQLITE3
        TEST_REQUIRE(sqlite.create_datatable<student>(auto_key));
        TEST_CHECK(sqlite.insert(s) == 1);
        auto result4 = sqlite.query<student>();
        TEST_CHECK(result4.size() == 1);
        TEST_CHECK(sqlite.insert(v) == 2);
        auto result6 = sqlite.query<student>();
        TEST_CHECK(result6.size() == 4);
        auto v2 = sqlite.query(FID(student::code), "<", "5");
        auto v3 = sqlite.query<student>("limit 2");
#endif
    }

    // key
    {
#ifdef ORMPP_ENABLE_MYSQL
        TEST_REQUIRE(mysql.create_datatable<student>(auto_key, not_null));
        TEST_CHECK(mysql.insert(s) == 1);
        auto result1 = mysql.query<student>("limit 3");
        TEST_CHECK(result1.size() == 1);
        TEST_CHECK(mysql.insert(s) == 1);
        TEST_CHECK(mysql.insert(v) == 2);
        auto result4 = mysql.query<student>();
        TEST_CHECK(result4.size() == 4);
        auto result5 = mysql.query<student>();
        TEST_CHECK(result5.size() == 4);

        TEST_REQUIRE(mysql.create_datatable<student>(key, not_null));
        v[0].code = 1;
        v[1].code = 2;
        TEST_CHECK(mysql.insert(s) == 1);
        auto result11 = mysql.query<student>();
        TEST_CHECK(result11.size() == 1);
        TEST_CHECK(mysql.insert(s) < 0);
        TEST_CHECK(mysql.delete_records<student>());
        TEST_CHECK(mysql.insert(v) == 2);
        auto result44 = mysql.query<student>();
        TEST_CHECK(result44.size() == 2);
        auto result55 = mysql.query<student>();
        TEST_CHECK(result55.size() == 2);
        auto result6 = mysql.query<student>();
        TEST_CHECK(result6.size() == 2);
#endif

#ifdef ORMPP_ENABLE_PG
        TEST_REQUIRE(pg_db.create_datatable<student>(key, not_null));
        TEST_CHECK(pg_db.insert(s) == 1);
        auto result2 = pg_db.query<student>();
        TEST_CHECK(result2.size() == 1);
        TEST_CHECK(pg_db.delete_records<student>());
        TEST_CHECK(pg_db.insert(v) == 2);
#endif

#ifdef ORMPP_ENABLE_SQLITE3
        TEST_REQUIRE(sqlite.create_datatable<student>(key));
        TEST_CHECK(sqlite.insert(s) == 1);
        auto result3 = sqlite.query<student>();
        TEST_CHECK(result3.size() == 1);
        TEST_CHECK(sqlite.delete_records<student>());
        TEST_CHECK(sqlite.insert(v) == 2);
#endif
    }
}

TEST_CASE(orm_update) {
    ormpp_key key{"code"};
    ormpp_not_null not_null{{"code", "age"}};
    ormpp_auto_key auto_key{"code"};

    student s = {1, "tom", 0, 19, 1.5, "room2"};
    student s1 = {2, "jack", 1, 20, 2.5, "room3"};
    student s2 = {3, "mke", 2, 21, 3.5, "room4"};
    std::vector<student> v{s, s1, s2};

#ifdef ORMPP_ENABLE_MYSQL
    dbng<mysql> mysql;
    TEST_REQUIRE(mysql.connect(ip, db_user_mysql, db_pwd, db_name));
    TEST_REQUIRE(mysql.create_datatable<student>(key, not_null));
    TEST_CHECK(mysql.insert(v) == 3);

    v[0].name = "test1";
    v[1].name = "test2";

    TEST_CHECK(mysql.update(v[0]) == 1);
    auto result = mysql.query<student>();
    TEST_CHECK(mysql.update(v[1]) == 1);
    auto result1 = mysql.query<student>();
#endif

#ifdef ORMPP_ENABLE_PG
    dbng<postgresql> postgres;
    TEST_REQUIRE(postgres.connect(ip, db_user_pg, db_pwd, db_name));
    TEST_REQUIRE(postgres.create_datatable<student>(key, not_null));
    TEST_CHECK(postgres.insert(v) == 3);
    TEST_CHECK(postgres.update(v[0]) == 1);
    auto result2 = postgres.query<student>();
    TEST_CHECK(postgres.update(v[1]) == 1);
    auto result3 = postgres.query<student>();
#endif

#ifdef ORMPP_ENABLE_SQLITE3
    dbng<sqlite> sqlite;
    TEST_REQUIRE(sqlite.connect("test.db"));
    TEST_REQUIRE(sqlite.create_datatable<student>(key));
    TEST_CHECK(sqlite.insert(v) == 3);
    TEST_CHECK(sqlite.update(v[0]) == 1);
    auto result4 = sqlite.query<student>();
    TEST_CHECK(sqlite.update(v[1]) == 1);
    auto result5 = sqlite.query<student>();
#endif
}

TEST_CASE(orm_multi_update) {
    ormpp_key key{"code"};
    ormpp_not_null not_null{{"code", "age"}};
    ormpp_auto_key auto_key{"code"};

    student s = {1, "tom", 0, 19, 1.5, "room2"};
    student s1 = {2, "jack", 1, 20, 2.5, "room3"};
    student s2 = {3, "mike", 2, 21, 3.5, "room4"};
    std::vector<student> v{s, s1, s2};

#ifdef ORMPP_ENABLE_MYSQL
    dbng<mysql> mysql;
    TEST_REQUIRE(mysql.connect(ip, db_user_mysql, db_pwd, db_name));
    TEST_REQUIRE(mysql.create_datatable<student>(key, not_null));
    TEST_CHECK(mysql.insert(v) == 3);
    v[0].name = "test1";
    v[1].name = "test2";
    v[2].name = "test3";

    TEST_CHECK(mysql.update(v) == 3);
    auto result = mysql.query<student>();
    TEST_CHECK(result.size() == 3);
#endif

#ifdef ORMPP_ENABLE_SQLITE3
    dbng<sqlite> sqlite;
    TEST_REQUIRE(sqlite.connect("test.db"));
    TEST_REQUIRE(sqlite.create_datatable<student>(key));
    TEST_CHECK(sqlite.insert(v) == 3);
    TEST_CHECK(sqlite.update(v) == 3);
    auto result4 = sqlite.query<student>();
    TEST_CHECK(result4.size() == 3);
#endif

#ifdef ORMPP_ENABLE_PG
    dbng<postgresql> postgres;
    TEST_REQUIRE(postgres.connect(ip, db_user_pg, db_pwd, db_name));
    TEST_REQUIRE(postgres.create_datatable<student>(key, not_null));
    TEST_CHECK(postgres.insert(v) == 3);
    TEST_CHECK(postgres.update(v) == 3);
    auto result2 = postgres.query<student>();
    TEST_CHECK(result2.size() == 3);
#endif
}

TEST_CASE(orm_delete) {
    ormpp_key key{"code"};
    ormpp_not_null not_null{{"code", "age"}};
    ormpp_auto_key auto_key{"code"};

    student s = {1, "tom", 0, 19, 1.5, "room2"};
    student s1 = {2, "jack", 1, 20, 2.5, "room3"};
    student s2 = {3, "mike", 2, 21, 3.5, "room4"};
    std::vector<student> v{s, s1, s2};

#ifdef ORMPP_ENABLE_MYSQL
    dbng<mysql> mysql;
    TEST_REQUIRE(mysql.connect(ip, db_user_mysql, db_pwd, db_name));
    TEST_REQUIRE(mysql.create_datatable<student>(key, not_null));
    TEST_CHECK(mysql.insert(v) == 3);
    TEST_REQUIRE(mysql.delete_records<student>("code=1"));
    TEST_CHECK(mysql.query<student>().size() == 2);
    TEST_REQUIRE(mysql.delete_records<student>(""));
    auto result = mysql.query<student>();
    TEST_CHECK(result.size() == 0);
#endif

#ifdef ORMPP_ENABLE_PG
    dbng<postgresql> postgres;
    TEST_REQUIRE(postgres.connect(ip, db_user_pg, db_pwd, db_name));
    TEST_REQUIRE(postgres.create_datatable<student>(key, not_null));
    TEST_CHECK(postgres.insert(v) == 3);
    TEST_REQUIRE(postgres.delete_records<student>("code=1"));
    TEST_CHECK(postgres.query<student>().size() == 2);
    TEST_REQUIRE(postgres.delete_records<student>(""));
    auto result1 = postgres.query<student>();
    TEST_CHECK(result1.size() == 0);
#endif

#ifdef ORMPP_ENABLE_SQLITE3
    dbng<sqlite> sqlite;
    TEST_REQUIRE(sqlite.connect("test.db"));
    TEST_REQUIRE(sqlite.create_datatable<student>(key));
    TEST_CHECK(sqlite.insert(v) == 3);
    TEST_REQUIRE(sqlite.delete_records<student>("code=1"));
    TEST_CHECK(sqlite.query<student>().size() == 2);
    TEST_REQUIRE(sqlite.delete_records<student>(""));
    auto result2 = sqlite.query<student>();
    TEST_CHECK(result2.size() == 0);
#endif
}

TEST_CASE(orm_query) {
    ormpp_key key{"id"};
    simple s1 = {1, 2.5, 3};
    simple s2 = {2, 3.5, 4};
    simple s3 = {3, 4.5, 5};
    std::vector<simple> v{s1, s2, s3};

#ifdef ORMPP_ENABLE_MYSQL
    dbng<mysql> mysql;
    TEST_REQUIRE(mysql.connect(ip, db_user_mysql, db_pwd, db_name));
    TEST_REQUIRE(mysql.create_datatable<simple>(key));
    TEST_CHECK(mysql.insert(v) == 3);
    auto result = mysql.query<simple>();
    TEST_CHECK(result.size() == 3);
    auto result3 = mysql.query<simple>("where id=1");
    TEST_CHECK(result3.size() == 1);
#endif

#ifdef ORMPP_ENABLE_SQLITE3
    dbng<sqlite> sqlite;
    TEST_REQUIRE(sqlite.connect("test.db"));
    TEST_REQUIRE(sqlite.create_datatable<simple>(key));
    TEST_CHECK(sqlite.insert(v) == 3);
    auto result2 = sqlite.query<simple>();
    TEST_CHECK(result2.size() == 3);
    auto result5 = sqlite.query<simple>("where id=3");
    TEST_CHECK(result5.size() == 1);
#endif

#ifdef ORMPP_ENABLE_PG
    dbng<postgresql> postgres;
    TEST_REQUIRE(postgres.connect(ip, db_user_pg, db_pwd, db_name));
    TEST_REQUIRE(postgres.create_datatable<simple>(key));
    TEST_CHECK(postgres.insert(v) == 3);
    auto result1 = postgres.query<simple>();
    TEST_CHECK(result1.size() == 3);
    auto result4 = postgres.query<simple>("where id=2");
    TEST_CHECK(result4.size() == 1);
#endif
}

TEST_CASE(orm_query_some) {
    ormpp_key key{"code"};
    ormpp_not_null not_null{{"code", "age"}};
    ormpp_auto_key auto_key{"code"};

    student s = {1, "tom", 0, 19, 1.5, "room2"};
    student s1 = {2, "jack", 1, 20, 2.5, "room3"};
    student s2 = {3, "mike", 2, 21, 3.5, "room4"};
    std::vector<student> v{s, s1, s2};

#ifdef ORMPP_ENABLE_MYSQL
    dbng<mysql> mysql;
    TEST_REQUIRE(mysql.connect(ip, db_user_mysql, db_pwd, db_name));
    TEST_REQUIRE(mysql.create_datatable<student>(key, not_null));
    TEST_CHECK(mysql.insert(v) == 3);
    auto result3 = mysql.query<std::tuple<int>>("select count(1) from student");
    TEST_CHECK(result3.size() == 1);
    TEST_CHECK(std::get<0>(result3[0]) == 3);

    auto result4 = mysql.query<std::tuple<int>>("select count(1) from student");
    TEST_CHECK(result4.size() == 1);
    TEST_CHECK(std::get<0>(result4[0]) == 3);

    auto result5 = mysql.query<std::tuple<int>>("select count(1) from student");
    TEST_CHECK(result5.size() == 1);
    TEST_CHECK(std::get<0>(result5[0]) == 3);
    auto result = mysql.query<std::tuple<int, std::string, double>>("select code, name, dm from student");
    TEST_CHECK(result.size() == 3);
#endif

#ifdef ORMPP_ENABLE_PG
    dbng<postgresql> postgres;
    TEST_REQUIRE(postgres.connect(ip, db_user_pg, db_pwd, db_name));
    TEST_REQUIRE(postgres.create_datatable<student>(key, not_null));
    TEST_CHECK(postgres.insert(v) == 3);
    auto result1 = postgres.query<std::tuple<int, std::string, double>>(
            "select code, name, dm from student");
    TEST_CHECK(result1.size() == 3);
#endif

#ifdef ORMPP_ENABLE_SQLITE3
    dbng<sqlite> sqlite;
    TEST_REQUIRE(sqlite.connect("test.db"));
    TEST_CHECK(sqlite.insert(v) == 3);
    TEST_REQUIRE(sqlite.create_datatable<student>(key));
    auto result2 = sqlite.query<std::tuple<int, std::string, double>>(
            "select code, name, dm from student");
    TEST_CHECK(result2.size() == 3);
#endif
}

TEST_CASE(orm_query_multi_table) {
    ormpp_key key{"code"};
    ormpp_not_null not_null{{"code", "age"}};
    ormpp_auto_key auto_key{"code"};

    student s = {1, "tom", 0, 19, 1.5, "room2"};
    student s1 = {2, "jack", 1, 20, 2.5, "room3"};
    student s2 = {3, "mike", 2, 21, 3.5, "room4"};
    std::vector<student> v{s, s1, s2};

    ormpp_key key1{"id"};
    person p = {1, "test1", 2};
    person p1 = {2, "test2", 3};
    person p2 = {3, "test3", 4};
    std::vector<person> v1{p, p1, p2};

#ifdef ORMPP_ENABLE_MYSQL
    dbng<mysql> mysql;
    TEST_REQUIRE(mysql.connect(ip, db_user_mysql, db_pwd, db_name));
    TEST_REQUIRE(mysql.create_datatable<student>(key, not_null));
    TEST_CHECK(mysql.insert(v) == 3);
    TEST_REQUIRE(mysql.create_datatable<person>(key1, not_null));
    TEST_CHECK(mysql.insert(v1) == 3);
    auto result = mysql.query<std::tuple<person, std::string, int>>(
            "select person.*, student.name, student.age from person, student"s);
    TEST_CHECK(result.size() == 9);
    auto result3 = mysql.query<std::tuple<person, student>>(
            "select * from person, student"s);
    TEST_CHECK(result.size() == 9);
#endif

#ifdef ORMPP_ENABLE_PG
    dbng<postgresql> postgres;
    TEST_REQUIRE(postgres.connect(ip, db_user_pg, db_pwd, db_name));
    TEST_REQUIRE(postgres.create_datatable<student>(key, not_null));
    TEST_CHECK(postgres.insert(v) == 3);
    TEST_REQUIRE(postgres.create_datatable<person>(key1, not_null));
    TEST_CHECK(postgres.insert(v1) == 3);
    TEST_CHECK(postgres.insert(v1) == 3);
    auto result1 = postgres.query<std::tuple<int, std::string, double>>(
            "select person.*, student.name, student.age from person, student"s);
    TEST_CHECK(result1.size() == 9);
    auto result4 = postgres.query<std::tuple<person, student>>(
            "select * from person, student"s);
    TEST_CHECK(result1.size() == 9);
#endif

#ifdef ORMPP_ENABLE_SQLITE3
    dbng<sqlite> sqlite;
    TEST_REQUIRE(sqlite.connect("test.db"));
    TEST_REQUIRE(sqlite.create_datatable<student>(key));
    TEST_CHECK(sqlite.insert(v) == 3);
    TEST_REQUIRE(sqlite.create_datatable<person>(key1));
    auto result2 = sqlite.query<std::tuple<int, std::string, double>>(
            "select person.*, student.name, student.age from person, student"s);
    TEST_CHECK(result2.size() == 9);
    auto result5 = sqlite.query<std::tuple<person, student>>(
            "select * from person, student"s);
    TEST_CHECK(result2.size() == 9);
#endif
}

TEST_CASE(orm_transaction) {
    ormpp_key key{"code"};
    ormpp_not_null not_null{{"code", "age"}};
    ormpp_auto_key auto_key{"code"};

    student s = {1, "tom", 0, 19, 1.5, "room2"};
    student s1 = {2, "jack", 1, 20, 2.5, "room3"};
    student s2 = {3, "mike", 2, 21, 3.5, "room4"};
    std::vector<student> v{s, s1, s2};

#ifdef ORMPP_ENABLE_MYSQL
    dbng<mysql> mysql;
    TEST_REQUIRE(mysql.connect(ip, db_user_mysql, db_pwd, db_name));
    TEST_REQUIRE(mysql.create_datatable<student>(key, not_null));

    TEST_REQUIRE(mysql.begin());
    for (int i = 0; i < 10; ++i) {
        student st = {i, "tom", 0, 19, 1.5, "room2"};
        if (!mysql.insert(st)) {
            mysql.rollback();
            return;
        }
    }
    TEST_REQUIRE(mysql.commit());
    auto result = mysql.query<student>();
    TEST_CHECK(result.size() == 10);
#endif

#ifdef ORMPP_ENABLE_PG
    dbng<postgresql> postgres;
    TEST_REQUIRE(postgres.connect(ip, db_user_pg, db_pwd, db_name));
    TEST_REQUIRE(postgres.create_datatable<student>(key, not_null));
    TEST_REQUIRE(postgres.begin());
    for (int i = 0; i < 10; ++i) {
        student s = {i, "tom", 0, 19, 1.5, "room2"};
        if (!postgres.insert(s)) {
            postgres.rollback();
            return;
        }
    }
    TEST_REQUIRE(postgres.commit());
    auto result1 = postgres.query<student>();
    TEST_CHECK(result1.size() == 10);
#endif

#ifdef ORMPP_ENABLE_SQLITE3
    dbng<sqlite> sqlite;
    TEST_REQUIRE(sqlite.connect("test.db"));
    TEST_REQUIRE(sqlite.create_datatable<student>(key));
    TEST_REQUIRE(sqlite.begin());
    for (int i = 0; i < 10; ++i) {
        student st = {i, "tom", 0, 19, 1.5, "room2"};
        if (!sqlite.insert(st)) {
            sqlite.rollback();
            return;
        }
    }
    TEST_REQUIRE(sqlite.commit());
    auto result2 = sqlite.query<student>();
    TEST_CHECK(result2.size() == 10);
#endif
}


struct log
{
    template<typename... Args>
    bool before(Args... args) {
        std::cout << "log before" << std::endl;
        return true;
    }

    template<typename T, typename... Args>
    bool after(T t, Args... args) {
        std::cout << "log after" << std::endl;
        return true;
    }
};


struct validate
{
    template<typename... Args>
    bool before(Args... args) {
        std::cout << "validate before" << std::endl;
        return true;
    }

    template<typename T, typename... Args>
    bool after(T t, Args... args) {
        std::cout << "validate after" << std::endl;
        return true;
    }
};


TEST_CASE(orm_aop) {
    /*
    dbng<mysql> mysql;
    auto r = mysql.wraper_connect<log, validate>("127.0.0.1", db_user_mysql, db_pwd,
    db_name); TEST_REQUIRE(r);

    r = mysql.wraper_execute("drop table if exists person");
    TEST_REQUIRE(r);

    r = mysql.wraper_execute<log>("drop table if exists person");
    TEST_REQUIRE(r);

    r = mysql.wraper_execute<validate>("drop table if exists person");
    TEST_REQUIRE(r);

    r = mysql.wraper_execute<validate, log>("drop table if exists person");
    TEST_REQUIRE(r);
    */
}


// support blob data type for mysql
#ifdef ORMPP_ENABLE_MYSQL
struct Image
{
    int id;
    ormpp::blob bin;
};

REFLECTION(Image, id, bin);

TEST_CASE(orm_mysql_blob) {
    dbng<mysql> mysql;

    TEST_REQUIRE(mysql.connect("127.0.0.1", "root", "", "testdb"));
    TEST_REQUIRE(mysql.execute("DROP TABLE IF EXISTS Image"));

    TEST_REQUIRE(mysql.create_datatable<Image>());

    auto data = "this is a  test binary stream\0, and ?...";
    auto size = 42;

    Image img;
    img.id = 1;
    img.bin.assign(data, data + size);

    TEST_REQUIRE(mysql.insert(img) == 1);

    auto result = mysql.query<Image>("id=1");
    TEST_REQUIRE(result.size() == 1);
    TEST_REQUIRE(result[0].bin.size() == size);
}

struct image_ex {
    int id;
    ormpp::blob bin;
    std::string time;
};

REFLECTION(image_ex, id, bin, time);


TEST_CASE(orm_mysql_blob_tuple) {
    dbng<mysql> mysql;

    TEST_REQUIRE(mysql.connect("127.0.0.1", "root", "", "testdb"));
    TEST_REQUIRE(mysql.execute("DROP TABLE IF EXISTS image_ex"));

    TEST_REQUIRE(mysql.create_datatable<image_ex>());

    auto data = "this is a test binary stream\0, and ?...";
    auto size = 40;

    image_ex img_ex;
    img_ex.id = 1;
    img_ex.bin.assign(data, data + size);
    img_ex.time = "2023-03-29 13:55:00";

    TEST_REQUIRE(mysql.insert(img_ex) == 1);

    auto result = mysql.query<image_ex>("id=1");
    TEST_REQUIRE(result.size() == 1);
    TEST_REQUIRE(result[0].bin.size() == size);

    using image_t = std::tuple<Image, std::string>;
    auto ex_results = mysql.query<image_t>("select id,bin,time from image_ex where id=1;");
    TEST_REQUIRE(ex_results.size() == 1);

    auto &img = std::get<0>(ex_results[0]);
    auto &time = std::get<1>(ex_results[0]);

    TEST_REQUIRE(img.id == 1);
    TEST_REQUIRE(img.bin.size() == size);
    TEST_REQUIRE(time == img_ex.time);
}

#endif

