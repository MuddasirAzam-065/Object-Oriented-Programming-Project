#ifndef TEACHERDETAILS_H
#define TEACHERDETAILS_H

#include <QDialog>

namespace Ui {
class teacherdetails;
}

class teacherdetails : public QDialog
{
    Q_OBJECT

public:
    explicit teacherdetails(QWidget *parent = nullptr);
    ~teacherdetails();
public slots:
    void on_addteacher_clicked();
private:
    Ui::teacherdetails *ui;
};

#endif // TEACHERDETAILS_H
