#ifndef ADDCOURSE_H
#define ADDCOURSE_H

#include <QDialog>

namespace Ui {
class addcourse;
}

class addcourse : public QDialog
{
    Q_OBJECT

public:
    explicit addcourse(QWidget *parent = nullptr);
    ~addcourse();
public slots:
    void on_add_clicked();
private:
    Ui::addcourse *ui;
    void saveCourse();
    void saveRegisteredStudents();
    void sortCSVBySemester(const QString &filePath, int semesterColumn);
};

#endif // ADDCOURSE_H
