#ifndef LMS_H
#define LMS_H

#include <QMainWindow>
namespace Ui {
class lms;
}

class lms : public QMainWindow
{
    Q_OBJECT

public:
    explicit lms(QWidget *parent = nullptr);
    ~lms();
public slots:
    void on_admin_clicked();
    void on_faculity_clicked();
    void on_student_clicked();
private:
    Ui::lms *ui;
};
#endif // LMS_H
