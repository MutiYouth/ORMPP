# A Very Cool Modern C++ ORM LIB ---- ORMPP
## ORMPP's targets
ormpp最重要的目标就是让c++中的数据库编程变得简单，为用户提供统一的接口，支持多种数据库，降低用户使用数据库的难度。

## Features
ormpp是modern c++(c++11/14/17)开发的ORM库，目前支持了三种数据库：mysql, postgresql和sqlite，ormpp主要有以下几个特点：

1. header only
2. cross-platform 
3. unified interface 
4. easy to use 
5. easy to change database

你通过ormpp可以很容易地实现数据库的各种操作了，大部情况下甚至都不需要写sql语句。ormpp是基于编译期反射的，会帮你实现自动化的实体映射，你再也不用写对象到数据表相互赋值的繁琐易出错的代码了，更酷的是你可以很方便地切换数据库，如果需要从mysql切换到postgresql或sqlite只需要修改一下数据库类型就可以了，无需修改其他代码。

## Quick start

这个例子展示如何使用ormpp实现数据库的增删改查之类的操作，无需写sql语句。

```c++
#include "dbng.hpp"
using namespace ormpp;

struct person
{
	int id;
	std::string name;
	int age;
};
REFLECTION(person, id, name, age)

int main()
{
	person p = {1, "test1", 2};
	person p1 = {2, "test2", 3};
	person p2 = {3, "test3", 4};
	std::vector<person> v{p1, p2};

	dbng<mysql> mysql;
	mysql.connect("127.0.0.1", "dbuser", "yourpwd", "testdb");
	mysql.create_datatable<person>();

	mysql.insert(p);
	mysql.insert(v);

	mysql.update(p);
	mysql.update(v);

	auto result = mysql.query<person>(); //vector<person>
	for(auto& person : result){
		std::cout<<person.id<<" "<<person.name<<" "<<person.age<<std::endl;
	}

	mysql.delete_records<person>();

	// transaction
	mysql.begin();
	for (int i = 0; i < 10; ++i) {
		person s = {i, "tom", 19};
			if(!mysql.insert(s)){
				mysql.rollback();
				return -1;
			}
	}
	mysql.commit();
}
```

More detailed examples，see `src/examples`, or [UserManual](User_Manual.md)。


# Compile
Download source code before start operation.
```
git clone https://github.com/MutiYouth/ormpp.git --recursive
```
And then:
```bash
cd path_to_clone_root_dir
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX="../../ormpp_install"
make -j6
make install
# make uninstall
```

##  Using postgresql
postgresql.hpp编译过程中，如果`postgresql/libpq-fe.h`找不到，可尝试安装libpq：
```
apt-get install libpq-dev
```

## Using mysql
```
apt-get install mysql-client
```


## Warnings:
* c++ compilers<br/>
compilers should support at least C++20.

* 数据库的安装
因为ormpp支持mysql, postgresql和sqlite，所以需要安装mysql，postgresql官方提供的libpq, 以及sqlite3，安装之后，在CMakeLists.txt配置目录和库路径。

* 依赖的第三方库
部分用的是[iguana](https://github.com/qicosmos/iguana.git).


# Roadmap

1. 支持组合键。 
2. 多表查询时增加一些诸如where, group, oder by, join, limit等常用的谓词，避免直接写sql语句。 
3. 增加日志 
4. 增加获取错误消息的接口 
5. 支持更多的数据库、数据类型
6. 增加数据库链接池


# Contact
Current Branch:<br/>
[ORMPP](https://github.com/MutiYouth/ormpp) @ 
WENG, Start From 22.8.

Original Author:
purecpp@163.com  <br/>
[https://github.com/qicosmos/ormpp](https://github.com/qicosmos/ormpp "ormpp") <br/>


# REF
* [xmh0511/xorm](https://github.com/xmh0511/xorm)
* cmake安装、卸载参考: [yaml-cpp](https://github.com/jbeder/yaml-cpp/blob/master/CMakeLists.txt),
  [pangolin](https://github.com/stevenlovegrove/Pangolin),
  [drogon](https://github.com/drogonframework/drogon)
