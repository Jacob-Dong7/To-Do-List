#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <string>
#include <sqlite3.h>
#include <QComboBox>
#include <QListView>
#include <QTableView>
#include <QStandardItemModel>
#include <filter.h>
#include "createtask.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow{
    Q_OBJECT
    public:
    explicit MainWindow(sqlite3* database, QWidget *parent = nullptr);

    private:
    QString filterData;
    Ui::MainWindow *ui;
    std::string title, description, category, date, time, priority;
    QString datex;
    Create createTask;
    sqlite3* db;
    QStandardItemModel* model;
    QStandardItemModel* modelTaskList;
    void loadData();
    void loadTaskList();
    void resetTaskCreation();
    void resetTaskCompletion();
    void populateCategory(QComboBox*);
    void populateFilter(QComboBox*, std::string);
    char* errorMSG;

    private slots:
    void on_btnAddTasks_clicked();
    void on_btnSave_clicked();
    void on_btnNew_clicked();
    void on_btnSaveC_clicked();
    void on_btnClearTask_clicked();
    void on_btnReturnNewTask_clicked();
    void on_btnTaskList_clicked();
    void on_btnReturnTaskList_clicked();
    void on_btnComplete_clicked();
    void on_btnDeleteTask_clicked();
    void on_btnAddTaskTL_clicked();
    void on_btnReset_clicked();
    void on_btnConfirmFilter_clicked();
    void on_rbCategory_clicked();
    void on_rbPriority_clicked();
};
#endif