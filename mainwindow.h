#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <string>
#include <sqlite3.h>
#include <QComboBox>

#include "createtask.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow{
    Q_OBJECT
    public:
    explicit MainWindow(sqlite3* database, QWidget *parent = nullptr);

    private:
    Ui::MainWindow *ui;
    std::string title, description, category, date, time, priority;
    QString datex;
    Create createTask;
    sqlite3* db;
    char* errorMSG;
    void populateCategory(QComboBox*);

    private slots:
    void on_btnAddTasks_clicked();
    void on_btnSave_clicked();
    void on_btnNew_clicked();
    void on_btnSaveC_clicked();
};
#endif