#include "createtask.h"
#include <string>
#include <sqlite3.h>
void Create::newTask(sqlite3* db, std::string title, std::string description, std::string category, std::string date, std::string priority) {
    int id;
    int exit = 0;
    sql = "INSERT INTO task (Task_Title, Category, Description, Priority, Due_Date) VALUES ('"
    + title + "', '"
    + category + "', '"
    + description + "', '" 
    + priority + "', '"
    + date + "');'";


    exit = sqlite3_exec(db, sql.c_str(), NULL, 0, &errorMSG);

    if (exit != SQLITE_OK) {
        return;
    }
    return;
}
