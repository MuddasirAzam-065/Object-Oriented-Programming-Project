#include "assignteacher.h"
#include "ui_assignteacher.h"
#include <QDebug>
#include <QMessageBox>

assignteacher::assignteacher(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::assignteacher)
{
    ui->setupUi(this);
}

assignteacher::~assignteacher()
{
    delete ui;
}

QString cleanCsvField(const QString& field) {
    QString cleaned = field.trimmed();
    if (cleaned.startsWith('"') && cleaned.endsWith('"') && cleaned.length() > 1) {
        return cleaned.mid(1, cleaned.length() - 2).trimmed();
    }
    return cleaned;
}

bool assignteacher::isTeacherAndDepartmentValid(const QString& teacherName, const QString& departmentName)
{
    QFile file(TEACHERS_FILE_PATH);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "File Error", "Could not open teacher.csv for reading.");
        return false;
    }

    QTextStream in(&file);
    in.readLine();
    bool found = false;
    const QString targetTeacherName = teacherName.toLower();
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');
        if (fields.count() > 0) {
            QString fileTeacherName = cleanCsvField(fields[0]).toLower();

            if (fileTeacherName == targetTeacherName) {
                found = true;
                break;
            }
        }
    }
    file.close();
    return found;
}

bool assignteacher::updateCourseTeacher(const QString& courseCode, const QString& teacherName)
{
    QStringList allLines;
    bool found = false;
    QFile readFile(COURSES_FILE_PATH);
    if (!readFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "File Error", "Could not open courses.csv for reading.");
        return false;
    }
    QTextStream in(&readFile);
    int lineNumber = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');
        if (lineNumber == 0) {
            allLines.append(line);
        }
        else if (fields.count() >= 5) {
            if (fields[1].trimmed().toLower() == courseCode.toLower()) {
                fields[5] = teacherName;
                line = fields.join(',');
                found = true;
            }
            allLines.append(line);
        }
        else {
            allLines.append(line);
        }
        lineNumber++;
    }
    readFile.close();
    if (!found) {
        return false;
    }

    QFile writeFile(COURSES_FILE_PATH);
    if (!writeFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QMessageBox::critical(this, "File Error", "Could not open courses.csv for writing. Update failed.");
        return false;
    }
    QTextStream out(&writeFile);
    for (const QString& line : allLines) {
        out << line << "\n";
    }
    writeFile.close();
    return true;
}

void assignteacher::on_assign_clicked()
{
    QString courseCode = ui->code->text().trimmed();
    QString teacherName = ui->teacher->text().trimmed();
    QString departmentName = ui->departement->text().trimmed();
    QString missingFields;
    if (courseCode.isEmpty()) missingFields += "Course Code, ";
    if (teacherName.isEmpty()) missingFields += "Teacher Name, ";
    if (departmentName.isEmpty()) missingFields += "Department, ";

    if (!missingFields.isEmpty()) {
        missingFields.chop(2);
        QMessageBox::warning(this, "Input Required", "Please enter the following missing information: " + missingFields + ".");
        return;
    }
    if (!isTeacherAndDepartmentValid(teacherName, departmentName)) {
        QMessageBox::warning(this, "Validation Error", "Teacher '" + teacherName + "' was not found in " + TEACHERS_FILE_PATH + ".");
        return;
    }

    if (updateCourseTeacher(courseCode, teacherName)) {
        QMessageBox::information(this, "Success", "Teacher " + teacherName + " has been successfully assigned to course " + courseCode + ".");
        ui->code->clear();
        ui->teacher->clear();
        ui->departement->clear();

    }
    else {
        QMessageBox::warning(this, "Course Error", "Course code '" + courseCode + "' was not found in " + COURSES_FILE_PATH + ". Assignment failed.");
    }
}
