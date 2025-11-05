module spt.infrastructure.repositories:repository;

import std;
import spt.infrastructure.sql;
import :repository;

using std::format;
using std::getenv;
using std::string;
using std::string_view;
using spt::infrastructure::sql::Database;
using spt::infrastructure::repositories::Repository;

Repository::Repository() 
    : _db { format("{0}\\Blendwerk\\SPT\\spt.db", getenv("USERPROFILE")) } 
{
}
Database& Repository::getDB() {
    return _db;
}

const Database& Repository::getDB() const {
    return _db;
}

