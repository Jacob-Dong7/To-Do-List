#include <QMainWindow>
#include <iostream>
#include <string>
#include <QMessageBox>
#include <QWidget>
#include <QComboBox>
#include <sqlite3.h>
#include <QTableView>
#include <QStandardItemModel>
#include <filter.h>
#include <QPushButton>

#include "createtask.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(sqlite3* database, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), db(database), model(new QStandardItemModel(this)), modelTaskList((new QStandardItemModel)) {
    ui->setupUi(this);
    resetTaskCompletion();

    ui->btnSaveC->hide();
    ui->txtNewCategory->hide();
    ui->txtNewCategory->clear();
    populateCategory(ui->cmbCategory);
    ui->dteDate->setMinimumDate(QDate::currentDate());

    ui->tbvTaskList->setModel(modelTaskList);
    ui->tbvTaskList->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->tbvCurrTask->setModel(model);
    ui->tbvCurrTask->setSelectionBehavior(QAbstractItemView::SelectRows);
    loadData();
}

//function that will load in the user tasks into the table view box at the main menu
void MainWindow::loadData() {
    model->clear();
    const char* sql = "SELECT TaskID, Task_Title, Category, Due_Date, Priority FROM task;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return;
    }

    //header
    int columns = sqlite3_column_count(stmt);
    QStringList header = {"ID", "Task Name", "Category", "Due Date", "Priority"};
    model->setHorizontalHeaderLabels(header);

    //rows
    int rows = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        model->insertRow(rows);
        for (int i = 0; i < columns; ++i) {
            const char* data = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            model->setItem(
                rows,
                i,
                new QStandardItem(data)
            );
        }
        ++rows;
    }

    sqlite3_finalize(stmt);
}

//function that will populate all the user created/preexisting category types into the combo box
void MainWindow::populateCategory(QComboBox* cmbBox) {
    const char* sql = "SELECT category FROM Category;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        QMessageBox::warning(this, "Warning", "Cannot access database");
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* name = sqlite3_column_text(stmt, 0);
        ui->cmbCategory->addItem(
            QString::fromUtf8(reinterpret_cast<const char*>(name))
        );
    }

    sqlite3_finalize(stmt);
} 

void MainWindow::on_btnAddTasks_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_btnSave_clicked() {
    title.clear(), category.clear(), description.clear(), priority .clear(), datex.clear(), date.clear();

    title = ui->txtTitle->toPlainText().toStdString();
    category = ui->cmbCategory->currentText().toStdString();
    description = ui->txtDescription->toPlainText().toStdString();
    datex = ui->dteDate->date().toString();
    date = datex.toStdString();

    if (title.size() <= 0) {
        QMessageBox::warning(this, "Warning", "Please enter a title");
        return;
    }
    
    if (ui->rbLow->isChecked()) {
        priority = "Low";
    } else if (ui->rbMed->isChecked()) {
        priority = "Medium";
    } else if (ui->rbHigh->isChecked()) {
        priority = "High";
    } else {
        QMessageBox::warning(this, "Warning", "Please select a priority");
        return;
    }

    createTask.newTask(db, title, description, category, date, priority);
    resetTaskCreation();
    return;
}

void MainWindow::on_btnNew_clicked() {
    ui->btnNew->hide();
    ui->btnSaveC->show();
    ui->cmbCategory->hide();
    ui->txtNewCategory->show();
}

void MainWindow::on_btnSaveC_clicked() {
    std::string sql;
    //validation
    if (ui->txtNewCategory->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a category name");
        ui->txtNewCategory->clear();
        return;
    }

    std::string newCategory = ui->txtNewCategory->toPlainText().toStdString();
    sql = "INSERT INTO Category (Category) Values('" 
    + newCategory + "');";

    sqlite3_exec(db, sql.c_str(), NULL, 0, &errorMSG);

    resetTaskCreation();

    //reload the category combobox
    populateCategory(ui->cmbCategory);
    return;
}

void MainWindow::on_btnClearTask_clicked() {
    resetTaskCreation();
    return;
}

void MainWindow::resetTaskCreation() {
    ui->txtNewCategory->clear();
    ui->txtNewCategory->hide();
    ui->btnSaveC->hide();
    ui->btnNew->show();
    ui->cmbCategory->show();
    ui->txtTitle->clear();
    ui->txtDescription->clear();
    ui->rbLow->setAutoExclusive(false);
    ui->rbMed->setAutoExclusive(false);
    ui->rbHigh->setAutoExclusive(false);
    ui->rbHigh->setChecked(false);
    ui->rbMed->setChecked(false);
    ui->rbLow->setChecked(false);
    ui->rbLow->setAutoExclusive(true);
    ui->rbMed->setAutoExclusive(true);
    ui->rbHigh->setAutoExclusive(true);
    return;
}

void MainWindow::on_btnReturnNewTask_clicked() {

    resetTaskCreation();
    ui->stackedWidget->setCurrentIndex(0);
    loadData();
    return;
}

void MainWindow::loadTaskList() {
    modelTaskList->clear();
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT TaskID, Task_Title, Category, Description, Due_Date, Priority FROM task;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return;
    }
    int cols = sqlite3_column_count(stmt);
    modelTaskList->setColumnCount(cols);

    QStringList header = {"ID", "Task Name", "Category", "Description", "Due Date", "Priority"};
    modelTaskList->setHorizontalHeaderLabels(header);

    int row = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        modelTaskList->insertRow(row);
        for (int i = 0; i < cols; ++i) {
            const char* data =reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            modelTaskList->setItem(
                row,
                i,
                new QStandardItem(data ? data : "")
            );
        }
        ++row;
    }

    sqlite3_finalize(stmt);
    
}

void MainWindow::on_btnTaskList_clicked() {
    ui->stackedWidget->setCurrentIndex(2);
    loadTaskList();
}

void MainWindow::on_btnReturnTaskList_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_btnComplete_clicked() {
    ui->lblTaskID->show();
    ui->txtTaskID->show();
    ui->btnDeleteTask->show();
    ui->btnComplete->hide();
}

void MainWindow::resetTaskCompletion() {
    ui->lblTaskID->hide();
    ui->txtTaskID->hide();
    ui->btnDeleteTask->hide();
    ui->txtTaskID->clear();
    ui->btnComplete->show();
    return;
}

void MainWindow::on_btnDeleteTask_clicked() {
    if (ui->txtTaskID->toPlainText().isEmpty()) {
        return;
    }

    std::string sql = "DELETE FROM task WHERE TaskID = " + ui->txtTaskID->toPlainText().toStdString() + ";";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return;
    }


    sqlite3_exec(db, sql.c_str(), NULL, 0, NULL);
    loadData();
    resetTaskCompletion();
}

void MainWindow::on_btnAddTaskTL_clicked() {
    resetTaskCompletion();
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_btnReset_clicked() {
    loadData();
    resetTaskCompletion();
    loadTaskList();
}

void MainWindow::on_rbCategory_clicked() {
    populateFilter(ui->cmbFilter, "Category");
}

void MainWindow::on_rbPriority_clicked() {
    populateFilter(ui->cmbFilter, "Priority");
}

void MainWindow::populateFilter(QComboBox* cmbFilter, std::string filter) {
    cmbFilter->clear();
    const char* sql;
     QStringList List = {"Low", "Medium", "High"};
    if (filter == "Category") {
        sql = "SELECT category from Category;";
    } else if (filter == "Priority") {
        cmbFilter->insertItems(
            0,
            List
        );
        return;
    }

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return;
    }

    int colCount = sqlite3_column_count(stmt);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        for (int i = 0; i < colCount; ++i) {
            const char* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            cmbFilter->insertItem(
                i,
                text
            );
        }
    }
    sqlite3_finalize(stmt);
}

void MainWindow::on_btnConfirmFilter_clicked() {
    modelTaskList->clear();
    std::string sql;
    sqlite3_stmt* stmt = nullptr;
    if (ui->rbCategory->isChecked()) {
        std::string filter = ui->cmbFilter->currentText().toStdString();
        sql = "SELECT TaskID, Task_Title, Category, Description, Due_Date, Priority FROM task WHERE category = '" + filter + "';";
    } else if (ui->rbPriority->isChecked()) {
        std::string filter = ui->cmbFilter->currentText().toStdString();
        sql = "SELECT TaskID, Task_Title, Category, Description, Due_Date, Priority FROM task WHERE priority = '" + filter + "';";
    }

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return;
    }

    //header
    QStringList header = {"ID", "Task Name", "Category", "Description", "Due Date", "Priority"};
    modelTaskList->setHorizontalHeaderLabels(
        header
    );

    int cols = sqlite3_column_count(stmt);
    modelTaskList->setColumnCount(cols);

    //rows
    int row = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        modelTaskList->insertRow(row);
        for (int i = 0; i < cols; ++i) {
            const char* data = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            modelTaskList->setItem(
                row,
                i,
                new QStandardItem(data ? data : "")
            );
        }
        ++row;
    }

    sqlite3_finalize(stmt);

    return;
}
