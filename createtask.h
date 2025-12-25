#ifndef CREATETASK_H
#define CREATETASK_H
#include <string>
#include <sqlite3.h>
class Create {
    public:
    void newTask(sqlite3* db, std::string, std::string, std::string, std::string, std::string);

    private:
    std::string sql;
    char* errorMSG;
};

#endif