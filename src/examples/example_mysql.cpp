//
// Created by weng on 22-10-4.
//
#include <string>
#include "../core/dbng.hpp"
#include "../dbs/mysql.hpp"

using namespace ormpp;


struct person
{
    int id;
    std::string name;
    int age;
};
REFLECTION(person, id, name, age)

int main() {
    person p = {1, "test1", 2};
    person p1 = {2, "test2", 3};
    person p2 = {3, "test3", 4};
    std::vector<person> v{p1, p2};


    dbng<mysql> mysql_db;
    // ormpp_key key{"id"};
    mysql_db.connect("192.168.10.106", "root", "weng2022", "test");
    mysql_db.drop_table<person>();
    mysql_db.create_datatable<person>();

    mysql_db.insert(p);
    mysql_db.insert(v);

    p.name = "test_111";
    p1.name = "test_2222";
    p2.name = "test_3333";
    mysql_db.update(p);
    mysql_db.update(v);

    auto result = mysql_db.query<person>();
    for (auto &person: result) {
        std::cout << person.id << " " << person.name << " " << person.age << std::endl;
    }

    mysql_db.delete_records<person>();

    // transaction
    mysql_db.begin();
    for (int i = 0; i < 10; ++i) {
        person s = {i, "tom", 19};
        if (!mysql_db.insert(s)) {
            mysql_db.rollback();
            return -1;
        }
    }
    mysql_db.commit();
}