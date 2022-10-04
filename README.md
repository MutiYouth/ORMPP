# 一个很酷的Modern C++ ORM库----ormpp
## ormpp的目标
ormpp最重要的目标就是让c++中的数据库编程变得简单，为用户提供统一的接口，支持多种数据库，降低用户使用数据库的难度。

## ormpp的特点
ormpp是modern c++(c++11/14/17)开发的ORM库，目前支持了三种数据库：mysql, postgresql和sqlite，ormpp主要有以下几个特点：

1. header only
2. cross-platform 
3. unified interface 
4. easy to use 
5. easy to change database

你通过ormpp可以很容易地实现数据库的各种操作了，大部情况下甚至都不需要写sql语句。ormpp是基于编译期反射的，会帮你实现自动化的实体映射，你再也不用写对象到数据表相互赋值的繁琐易出错的代码了，更酷的是你可以很方便地切换数据库，如果需要从mysql切换到postgresql或sqlite只需要修改一下数据库类型就可以了，无需修改其他代码。

## 快速示例

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

更多使用说明，见[UserManual](User_Manual.md)。


## 如何编译
下载源码
```
git clone https://github.com/MutiYouth/ormpp.git --recursive
```
然后，Windows下可使用CMake、VisualStudio 进行后续的编译处理; 或者linux下，使用常规的编译过程编译即可。
```bash
cd path_to_clone_root_dir
mkdir build && cd build
cmake ..
make -j6
# sudo make install
```

## 使用pg时
postgresql.hpp编译过程中，如果`postgresql/libpq-fe.h`找不到，可尝试安装libpq：
```
apt-get install libpq-dev
```


备注：
* 编译器支持<br/>
需要支持C++17的编译器, 符合要求的编译器版本有：<br/>
linux gcc7.2, clang4.0; <br/>
windows >vs2017 update5

* 数据库的安装
因为ormpp支持mysql, postgresql和sqlite，所以需要安装mysql，postgresql，postgresql官方提供的libpq以及sqlite3，安装之后，在CMakeLists.txt配置目录和库路径。

* 依赖的第三方库
部分用的是[iguana](https://github.com/qicosmos/iguana.git).


# roadmap

1. 支持组合键。 
2. 多表查询时增加一些诸如where, group, oder by, join, limit等常用的谓词，避免直接写sql语句。 
3. 增加日志 
4. 增加获取错误消息的接口 
5. 支持更多的数据库、数据类型
6. 增加数据库链接池


# 联系方式
## 当前分支维护
WENG, Start From 22.8.

## 原始作者
purecpp@163.com  <br/>
[https://github.com/qicosmos/ormpp](https://github.com/qicosmos/ormpp "ormpp") <br/>
