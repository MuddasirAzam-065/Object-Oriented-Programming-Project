#ifndef TEACHER_H
#define TEACHER_H

#include "qlineedit.h"
#include <QMainWindow> // Base class is QMainWindow
#include <QStringList>
#include <QStandardItemModel>

namespace Ui {
class teacher; // Class name changed to 'teacher'
}

class teacher : public QMainWindow // <<< Inherits QMainWindow
{
    Q_OBJECT
    struct GradeResult {
        QString grade;
        QString gpa;
    };

    struct GradeRowUI {
        QString courseCode;
        QString studentName;
        QLineEdit *marksInput;
    };
public:
    // New constructor to accept the teacher details
    explicit teacher(const QStringList &teacherData, QWidget *parent = nullptr);
    ~teacher();
public slots:
    void on_logout_clicked();
    void on_viewstudents_clicked();
    void on_attendence_clicked();
    void on_grades_clicked();
    void on_assignment_clicked();
    void on_viewassignments_clicked();

private:
    Ui::teacher *ui;
    QStringList m_teacherData;
    void displayTeacherDetails();
    QStringList getAssignedCourseCodes(const QString &teacherName);
    QStandardItemModel *getStudentsInCourses(const QStringList &courseCodes);
    QString getStudentEnrollment(const QString &studentName);
    int getCourseCreditHours(const QString &courseCode);
    void saveAttendanceToCSV(const QList<struct AttendanceRecord> &records);
    GradeResult calculateGradeAndGPA(double marks);
    void saveGradesToCSV(const QList<GradeRowUI> &rows);
    QString getCourseTitle(const QString &courseCode);
    void updateAssignmentsCSV(const QString &courseCode, const QString &courseTitle, const QString &assignmentName, const QString &fileName);
    QStringList getAssignmentsForCourse(const QString &courseCode);
    void showAssignmentsTable(const QString &courseCode, const QStringList &assignmentList);
    void downloadAssignmentFile(const QString &fileName);
};

#endif // TEACHER_H
