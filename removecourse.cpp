#include "removecourse.h"
#include "ui_removecourse.h"
#include <QDebug>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <QMessageBox>

removecourse::removecourse(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::removecourse)
{
    ui->setupUi(this);
    ui->title->setReadOnly(true);
}

removecourse::~removecourse()
{
    delete ui;
}

bool removecourse::readCourses(const QString& courseCode, QString& courseTitle, QStringList& remainingLines, bool isRemoval)
{
    QFile file(CSV_FILE_PATH);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "File Error", "Could not open courses.csv for reading.");
        return false;
    }

    QTextStream in(&file);
    bool found = false;
    remainingLines.clear();
    if (!in.atEnd()) {
        remainingLines.append(in.readLine());
    }
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');
        if (fields.count() >= 2) {
            QString currentCode = fields[1].trimmed();
            if (currentCode == courseCode) {
                found = true;
                if (!isRemoval) {
                    courseTitle = fields[0].trimmed();
                }
            }
            else {
                remainingLines.append(line);
            }
        }
        else {
            remainingLines.append(line);
        }
    }

    file.close();
    return found;
}

void removecourse::on_fetch_clicked()
{
    QString courseCode = ui->code->text().trimmed();

    if (courseCode.isEmpty()) {
        QMessageBox::warning(this, "Input Required", "Please enter the Course Code to fetch.");
        ui->title->clear();
        return;
    }

    QString courseTitle;
    QStringList remainingLines;
    bool found = readCourses(courseCode, courseTitle, remainingLines, false);

    if (found) {
        ui->title->setText(courseTitle);
        QMessageBox::information(this, "Success", "Course found: " + courseTitle);
    } else {
        ui->title->setText("Not Found");
        QMessageBox::warning(this, "Not Found", "Course code '" + courseCode + "' was not found in the records.");
    }
}

void removecourse::extracted(QStringList &remainingLines, QTextStream &out) {
    for (const QString &line : remainingLines) {
        out << line << "\n";
    }
}

void removecourse::on_remove_clicked() {
    QString courseCode = ui->code->text().trimmed();
    if (courseCode.isEmpty()) {
        QMessageBox::warning(this, "Input Required",
                             "Please enter the Course Code to remove.");
        return;
    }
    QString courseTitle;
    QStringList remainingLines;
    bool found = readCourses(courseCode, courseTitle, remainingLines,true);
    if (!found) {
        QMessageBox::warning(this, "Not Found",
                             "Course code '" + courseCode +
                                 "' was not found. Nothing to remove.");
        ui->title->setText("Not Found");
        return;
    }
    QFile file(CSV_FILE_PATH);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text |
                   QIODevice::Truncate)) {
        QMessageBox::critical(
            this, "File Error",
            "Could not open courses.csv for writing. Removal failed.");
        return;
    }
    QTextStream out(&file);
    extracted(remainingLines, out);
    file.close();
    QMessageBox::information(this, "Success", "Course with code '" + courseCode + "' has been successfully removed.");
    ui->code->clear();
    ui->title->clear();
}
