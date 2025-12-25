#include <QApplication>
#include <sqlite3.h>
#include "mainwindow.h"
#include <iostream>

int main(int argc, char *argv[]) {
    sqlite3* db;
    int rc = sqlite3_open("../database.db", &db);
    if(rc) {
        return 0;
    } else {
        std::cout << "opened" << std::endl;
    }
    QApplication app(argc, argv);
    MainWindow main(db);
    main.show();

    int ret = app.exec();
    sqlite3_close(db);

    return ret;
}