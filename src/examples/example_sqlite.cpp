//
// Created by weng on 23-1-26.
//
#include <string>
#include "../core/dbng.hpp"
#include "../dbs/sqlite.hpp"

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


    dbng<sqlite> sqlite_db;
    sqlite_db.connect("./../../../data/db/test_230126.db");
    // ormpp_key key{"id"};
    
    std::cout << "table exist: " << sqlite_db.exist_table<person>() << std::endl;
    sqlite_db.create_datatable<person>();

    sqlite_db.insert(p);
    sqlite_db.insert(v);

    p.name = "test_111";
    p1.name = "test_2222";
    p2.name = "test_3333";
    // WENG notice 22-10-5 12:31:  如果对象设置了key，则更新时不需要再进行加入条件列名了。
    //  否则一定要加入足以证明这个对象的条件列名们。
    sqlite_db.update(p, "id");
    sqlite_db.update(v, "id");

    auto result = sqlite_db.query<person>();
    for (auto &person: result) {
        std::cout << person.id << " " << person.name << " " << person.age << std::endl;
    }

    sqlite_db.delete_records<person>();

    // transaction
    sqlite_db.begin();
    for (int i = 0; i < 10; ++i) {
        person s = {i, "tom", 19};
        if (!sqlite_db.insert(s)) {
            sqlite_db.rollback();
            return -1;
        }
    }
    sqlite_db.commit();

    sqlite_db.disconnect();
    std::cout << "Done!" << std::endl;
}