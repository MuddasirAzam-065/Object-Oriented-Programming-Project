#ifndef ASSIGNTEACHER_H
#define ASSIGNTEACHER_H

#include <QDialog>
#include <QString>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QStringList>

namespace Ui {
class assignteacher;
}

class assignteacher : public QDialog
{
    Q_OBJECT

public:
    explicit assignteacher(QWidget *parent = nullptr);
    ~assignteacher();

private slots:
    // Slot connected to the 'Assign' button (e.g., on_assignButton_clicked)
    void on_assign_clicked();

private:
    Ui::assignteacher *ui;
    const QString COURSES_FILE_PATH = "courses.csv";
    const QString TEACHERS_FILE_PATH = "teachers.csv";

    // Helper to check if a teacher exists in teacher.csv
    bool isTeacherAndDepartmentValid(const QString& teacherName, const QString& departmentName);

    // Helper to find the course and update the assigned teacher
    bool updateCourseTeacher(const QString& courseCode, const QString& teacherName);
};

#endif // ASSIGNTEACHER_H
