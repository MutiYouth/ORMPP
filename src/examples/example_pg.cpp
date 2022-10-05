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
    pg_db.connect("192.168.10.106", "pi", "weng2022", "test");
    pg_db.create_datatable<person>();

    pg_db.insert(p);
    pg_db.insert(v);

    p.name = "test_1111";
    // WENG TODO 22-10-5: 更新失败; 更新时，若没有KEY，则会删除所有。
    pg_db.update(p);
    pg_db.update(v);

    // WENG TODO 22-10-5: 查询失败
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