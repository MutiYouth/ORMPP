//
// Created by weng on 22-10-4.
//
#include <string>
#include "../core/dbng.hpp"
#include "../dbs/postgresql.hpp"

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


    dbng<postgresql> pg_db;
    // ormpp_key key{"id"};
    pg_db.connect("192.168.10.106", "pi", "weng2022", "test");
    pg_db.create_datatable<person>();

    pg_db.insert(p);
    pg_db.insert(v);

    p.name = "test_111";
    p1.name = "test_2222";
    p2.name = "test_3333";
    // WENG notice 22-10-5 12:31:  如果对象设置了key，则更新时不需要再进行加入条件列名了。
    //  否则一定要加入足以证明这个对象的条件列名们。
    pg_db.update(p, "id");
    pg_db.update(v, "id");

    auto result = pg_db.query<person>();
    for (auto &person: result) {
        std::cout << person.id << " " << person.name << " " << person.age << std::endl;
    }

    pg_db.delete_records<person>();

    // transaction
    pg_db.begin();
    for (int i = 0; i < 10; ++i) {
        person s = {i, "tom", 19};
        if (!pg_db.insert(s)) {
            pg_db.rollback();
            return -1;
        }
    }
    pg_db.commit();
}