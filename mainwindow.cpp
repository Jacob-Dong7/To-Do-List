#include <QMainWindow>
#include <iostream>
#include <string>
#include <QMessageBox>
#include <QWidget>
#include <QComboBox>
#include <sqlite3.h>

#include "createtask.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(sqlite3* database, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), db(database) {
    ui->setupUi(this);
    ui->btnSaveC->hide();
    ui->txtNewCategory->hide();
    ui->txtNewCategory->clear();
    populateCategory(ui->cmbCategory);
}

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
    if (ui->txtNewCategory->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a category name");
        ui->txtNewCategory->clear();
        return;
    }

    std::string newCategory = ui->txtNewCategory->toPlainText().toStdString();
    sql = "INSERT INTO Category (Category) Values('" 
    + newCategory + "');";

    sqlite3_exec(db, sql.c_str(), NULL, 0, &errorMSG);

    ui->txtNewCategory->clear();
    ui->txtNewCategory->hide();
    ui->btnSaveC->hide();
    ui->btnNew->show();
    ui->cmbCategory->show();
    return;
}