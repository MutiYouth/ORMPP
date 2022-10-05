//
// Created by weng on 22-10-4.
//

#ifndef ORMPP_DB_COMMON_HPP
#define ORMPP_DB_COMMON_HPP

#include <string>


namespace ormpp {

class DBCommon
{
public:
    static std::string pg_db_conn_name;
};


std::string DBCommon::pg_db_conn_name = "";


}

#endif //ORMPP_DB_COMMON_HPP
