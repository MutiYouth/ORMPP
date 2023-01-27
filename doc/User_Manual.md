
# 接口介绍
ormpp屏蔽了不同数据库操作接口的差异，提供了统一简单的数据库操作接口，具体提供了数据库连接、断开连接、创建数据表、插入数据、更新数据、删除数据、查询数据和事务相关的接口。

## 接口概览

	//连接数据库
	template <typename... Args>
	bool connect(Args&&... args);
	
	//断开数据库连接
	bool disconnect();
	
	//创建数据表
	template<typename T, typename... Args>
	bool create_datatable(Args&&... args);
	
	//插入单条数据
	template<typename T, typename... Args>
	int insert(const T& t, Args&&... args);
	
	//插入多条数据
	template<typename T, typename... Args>
	int insert(const std::vector<T>& t, Args&&... args);
	
	//更新单条数据
	template<typename T, typename... Args>
	int update(const T& t, Args&&... args);
	
	//更新多条数据
	template<typename T, typename... Args>
	int update(const std::vector<T>& t, Args&&... args);
	
	//删除数据
	template<typename T, typename... Args>
	bool delete_records(Args&&... where_conditon);
	
	//查询数据，包括单表查询和多表查询
	template<typename T, typename... Args>
	auto query(Args&&... args);
	
	//执行原生的sql语句
	int execute(const std::string& sql);
	
	//开始事务
	bool begin();
	
	//提交事务
	bool commit();
	
	//回滚
	bool rollback();

## 具体的接口使用介绍
先在entity.hpp中定义业务实体（和数据库的表对应），接着定义数据库对象：

	#include "dbng.hpp"
	using namespace ormpp;
	
	struct person
	{
		int id;
		std::string name;
		int age;
	};
	REFLECTION(person, id, name, age)

	int main(){

		dbng<mysql> mysql;
	    dbng<sqlite> sqlite;
	    dbng<postgresql> postgres;
		//......
	}

1. 连接数据库

   template <typename... Args>
   bool connect(Args&&... args);

connect exmple:

	mysql.connect("127.0.0.1", "root", "12345", "testdb")

	postgres.connect("127.0.0.1", "root", "12345", "testdb")

	sqlite.connect("127.0.0.1", "root", "12345", "testdb")

返回值：bool，成功返回true，失败返回false.

2. 断开数据库连接

   bool disconnect();

disconnect exmple:

	mysql.disconnect();

	postgres.disconnect();

	sqlite.disconnect();

注意：用户可以不用显式调用，在数据库对象析构时会自动调用disconnect接口。

返回值：bool，成功返回true，失败返回false.

3.创建数据表

	template<typename T, typename... Args>
	bool create_datatable(Args&&... args);

create_datatable example:

	//创建不含主键的表
	mysql.create_datatable<student>();
	
	postgres.create_datatable<student>();
	
	sqlite.create_datatable<student>();

	//创建含主键和not null属性的表
	ormpp_key key1{"id"};
	ormpp_not_null not_null{{"id", "age"}};

    person p = {1, "test1", 2};
    person p1 = {2, "test2", 3};
    person p2 = {3, "test3", 4};

    mysql.create_datatable<person>(key1, not_null);
    postgres.create_datatable<person>(key1, not_null);
    sqlite.create_datatable<person>(key1);

注意：目前只支持了key和not null属性，并且只支持单键，还不支持组合键，将在下一个版本中支持组合键。

返回值：bool，成功返回true，失败返回false.

4.插入单条数据

	template<typename T, typename... Args>
	int insert(const T& t, Args&&... args);

insert example:

	person p = {1, "test1", 2};
	TEST_CHECK(mysql.insert(p)==1);
    TEST_CHECK(postgres.insert(p)==1);
    TEST_CHECK(sqlite.insert(p)==1);

返回值：int，成功返回插入数据的条数1，失败返回INT_MIN.

5.插入多条数据

	template<typename T, typename... Args>
	int insert(const std::vector<T>& t, Args&&... args);

multiple insert example:

	person p = {1, "test1", 2};
    person p1 = {2, "test2", 3};
    person p2 = {3, "test3", 4};
    std::vector<person> v1{p, p1, p2};

    TEST_CHECK(mysql.insert(v1)==3);
    TEST_CHECK(postgres.insert(v1)==3);
    TEST_CHECK(sqlite.insert(v1)==3);

返回值：int，成功返回插入数据的条数N，失败返回INT_MIN.

6. 更新单条数据


	template<typename T, typename... Args>
	int update(const T& t, Args&&... args);

update example:

	person p = {1, "test1", 2};
	TEST_CHECK(mysql.update(p)==1);
    TEST_CHECK(postgres.update(p)==1);
    TEST_CHECK(sqlite.update(p)==1);

注意：更新会根据表的key字段去更新，如果表没有key字段的时候，需要指定一个更新依据字段名，比如

	TEST_CHECK(mysql.update(p, "age")==1);
    TEST_CHECK(postgres.update(p, "age")==1);
    TEST_CHECK(sqlite.update(p, "age")==1);

返回值：int，成功返回更新数据的条数1，失败返回INT_MIN.

5.插入多条数据

	template<typename T, typename... Args>
	int update(const std::vector<T>& t, Args&&... args);

multiple insert example:

	person p = {1, "test1", 2};
    person p1 = {2, "test2", 3};
    person p2 = {3, "test3", 4};
    std::vector<person> v1{p, p1, p2};

    TEST_CHECK(mysql.insert(v1)==3);
    TEST_CHECK(postgres.insert(v1)==3);
    TEST_CHECK(sqlite.insert(v1)==3);

注意：更新会根据表的key字段去更新，如果表没有key字段的时候，需要指定一个更新依据字段名，用法同上。

返回值：int，成功返回更新数据的条数N，失败返回INT_MIN.

6. 删除数据

   template<typename T, typename... Args>
   bool delete_records(Args&&... where_conditon);

delete_records example:

	//删除所有数据
	TEST_REQUIRE(mysql.delete_records<person>());
	TEST_REQUIRE(postgres.delete_records<person>());
	TEST_REQUIRE(sqlite.delete_records<person>());

	//根据条件删除数据
	TEST_REQUIRE(mysql.delete_records<person>("id=1"));
	TEST_REQUIRE(postgres.delete_records<person>("id=1"));
	TEST_REQUIRE(sqlite.delete_records<person>("id=1"));

返回值：bool，成功返回true，失败返回false.

7.单表查询

	template<typename T, typename... Args>
	auto query(Args&&... args);

	//如果T是一个反射对象则返回的是单表查询结果vector<T>
	template<typename T, typename... Args>
	std::vector<T> query(Args&&... args);

single table query example:

    auto result = mysql.query<person>();
    TEST_CHECK(result.size()==3);

    auto result1 = postgres.query<person>();
    TEST_CHECK(result1.size()==3);

    auto result2 = sqlite.query<person>();
    TEST_CHECK(result2.size()==3);

	//可以根据条件查询
    auto result3 = mysql.query<person>("where id=1");
    TEST_CHECK(result3.size()==1);

    auto result4 = postgres.query<person>("where id=2");
    TEST_CHECK(result4.size()==1);

    auto result5 = sqlite.query<person>("where id=3");

返回值：std::vector<T>，成功vector不为空，失败则为空.

8.多表或特定列查询

	template<typename T, typename... Args>
	auto query(Args&&... args);

	//如果T是一个tuple类型则返回的是多表或特定列查询，结果vector<tuple<T>>
	template<typename T, typename... Args>
	std::vector<std::tuple<T>> query(Args&&... args);

multiple or some fields query example:

    auto result = mysql.query<std::tuple<int, std::string, int>>("select code, name, dm from person");
    TEST_CHECK(result.size()==3);

    auto result1 = postgres.query<std::tuple<int, std::string, int>>("select code, name, dm from person");
    TEST_CHECK(result1.size()==3);

    auto result2 = sqlite.query<std::tuple<int, std::string, int>>("select code, name, dm from person");
    TEST_CHECK(result2.size()==3);

    auto result3 = mysql.query<std::tuple<int>>("select count(1) from person");
    TEST_CHECK(result3.size()==1);
    TEST_CHECK(std::get<0>(result3[0])==3);

    auto result4 = postgres.query<std::tuple<int>>("select count(1) from person");
    TEST_CHECK(result4.size()==1);
    TEST_CHECK(std::get<0>(result4[0])==3);

    auto result5 = sqlite.query<std::tuple<int>>("select count(1) from person");
    TEST_CHECK(result5.size()==1);
    TEST_CHECK(std::get<0>(result5[0])==3);

返回值：std::vector<std::tuple<T>>，成功vector不为空，失败则为空.

9.执行原生sql语句

	int execute(const std::string& sql);

execute example:

	r = mysql.execute("drop table if exists person");
    TEST_REQUIRE(r);

    r = postgres("drop table if exists person");
    TEST_REQUIRE(r);

    r = sqlite.execute("drop table if exists person");
    TEST_REQUIRE(r);

注意：execute接口支持的原生sql语句是不带占位符的，是一条完整的sql语句。

返回值：int，成功返回更新数据的条数1，失败返回INT_MIN.

10.事务接口

开始事务，提交事务，回滚

	//transaction
	mysql.begin();
	for (int i = 0; i < 10; ++i) {
        person s = {i, "tom", 19};
            if(!mysql.insert(s)){
                mysql.rollback();
                return -1;
            }
	}
	mysql.commit();
返回值：bool，成功返回true，失败返回false.

11.面向切面编程AOP

定义切面：

	struct log{
		//args...是业务逻辑函数的入参
	    template<typename... Args>
	    bool before(Args... args){
	        std::cout<<"log before"<<std::endl;
	        return true;
	    }
	
		//T的类型是业务逻辑返回值，后面的参数则是业务逻辑函数的入参
	    template<typename T, typename... Args>
	    bool after(T t, Args... args){
	        std::cout<<"log after"<<std::endl;
	        return true;
	    }
	};
	
	struct validate{
		//args...是业务逻辑函数的入参
	    template<typename... Args>
	    bool before(Args... args){
	        std::cout<<"validate before"<<std::endl;
	        return true;
	    }
	
		//T的类型是业务逻辑返回值，后面的参数则是业务逻辑函数的入参
	    template<typename T, typename... Args>
	    bool after(T t, Args... args){
	        std::cout<<"validate after"<<std::endl;
	        return true;
	    }
	};

注意：切面的定义中，允许你只定义before或after，或者二者都定义。

	//增加日志和校验的切面
	dbng<mysql> mysql;
    auto r = mysql.warper_connect<log, validate>("127.0.0.1", "root", "12345", "testdb");
    TEST_REQUIRE(r);









# 其他依赖库的配置
## include path
D:\ormpp_deps\mysql;D:\ormpp_deps\postgresql\pgsql;D:\ormpp_deps\sqlite3;..\iguana
## library path
D:\ormpp_deps\mysql\lib;D:\ormpp_deps\postgresql\pgsql\lib;D:\ormpp_deps\sqlite3

## AdditionalDependencies
libmysql_x86.lib;libpq.lib;libpgport.lib;libecpg.lib;postgres.lib;libeay32.lib;sqlite3_x86.lib

libmysql_x64.lib;sqlite3.lib

libmysql.lib;libpq.lib;libpgport.lib;libecpg.lib;postgres.lib;libeay32.lib;sqlite3.lib;
