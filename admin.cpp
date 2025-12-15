#include "admin.h"
#include "removecourse.h"
#include "ui_admin.h"
#include "lms.h"
#include "teacherdetails.h"
#include "sudentdetails.h"
#include "remove.h"
#include "addcourse.h"
#include "assignteacher.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QRegularExpression>
#include <Qt>

admin::admin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::admin)
{
    ui->setupUi(this);
    studentModel = new QStandardItemModel(this);
    ui->tableView->setModel(studentModel);
}
void admin::on_logout_clicked()
{
    lms *lmsWindow = new lms();
    lmsWindow->showMaximized();
    this->close();
}

void admin::on_addstudent_clicked()
{
    sudentdetails *dialog = new sudentdetails(this);
    dialog->exec();
    dialog->deleteLater();
}

void admin::on_viewstudents_clicked()
{
    const QString filePath = "students.csv";
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "File Error", "Could not open students.csv for reading.");
        return;
    }

    QTextStream in(&file);
    studentModel->clear();

    int row = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();

        QStringList fields = line.split(',');

        for (int i = 0; i < fields.size(); ++i) {
            QString field = fields.at(i).trimmed();

            if (field.startsWith('"') && field.endsWith('"')) {
                field = field.mid(1, field.length() - 2);
            }
            fields[i] = field;
        }

        if (row == 0) {
            studentModel->setHorizontalHeaderLabels(fields);
        }
        else {
            studentModel->insertRow(row - 1);

            for (int col = 0; col < fields.size(); ++col) {
                QStandardItem *item = new QStandardItem(fields.at(col));
                studentModel->setItem(row - 1, col, item);
            }
        }

        row++;
    }

    file.close();
    ui->tableView->resizeColumnsToContents();
}

void admin::on_addteacher_clicked()
{
    teacherdetails *dialog2 = new teacherdetails();
    dialog2->exec();
    dialog2->deleteLater();
}

void admin::on_viewteachers_clicked()
{
    const QString filePath = "teachers.csv";
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "File Error", "Could not open students.csv for reading.");
        return;
    }

    QTextStream in(&file);
    studentModel->clear();

    int row = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();

        QStringList fields = line.split(',');

        for (int i = 0; i < fields.size(); ++i) {
            QString field = fields.at(i).trimmed();

            if (field.startsWith('"') && field.endsWith('"')) {
                field = field.mid(1, field.length() - 2);
            }
            fields[i] = field;
        }

        if (row == 0) {
            studentModel->setHorizontalHeaderLabels(fields);
        }
        else {
            studentModel->insertRow(row - 1);

            for (int col = 0; col < fields.size(); ++col) {
                QStandardItem *item = new QStandardItem(fields.at(col));
                studentModel->setItem(row - 1, col, item);
            }
        }

        row++;
    }

    file.close();
    ui->tableView->resizeColumnsToContents();
}

void admin::on_removestudent_clicked()
{
    class remove *stdrm = new class remove();
    stdrm->exec();
    stdrm->deleteLater();
}

void admin::on_removeteacher_clicked()
{
    class removet *tearm = new class removet();
    tearm->exec();
    tearm->deleteLater();
}

void admin::on_addcourse_clicked()
{
    class addcourse * course = new addcourse();
    course->exec();
    course->deleteLater();
}

void admin::extracted(QStringList &fields, QList<QStandardItem *> &rowItems)
{
    for (const QString &field : fields) {
        rowItems.append(new QStandardItem(field));
    }
}

void admin::on_allcourses_clicked()
{
    QString filePath = "courses.csv";
    QFile file(filePath);

    if (!file.exists()) {
        QMessageBox::warning(this, "Error", "courses.csv not found!");
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Unable to open courses.csv");
        return;
    }
    QTextStream in(&file);
    studentModel->clear();
    QString headerLine = in.readLine();
    QStringList headers = headerLine.split(",");
    studentModel->setHorizontalHeaderLabels(headers);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(",");
        QList<QStandardItem *> rowItems;
        extracted(fields, rowItems);
        studentModel->appendRow(rowItems);
    }
    file.close();
    ui->tableView->setModel(studentModel);
    ui->tableView->resizeColumnsToContents();
}

void admin::on_removecourse_clicked()
{
    removecourse *remove = new removecourse();
    remove->exec();
    remove->deleteLater();
}

void admin::on_assignteacher_clicked()
{
    assignteacher *teacher = new assignteacher();
    teacher->exec();
    teacher->deleteLater();
}
admin::~admin()
{
    delete ui;
}
