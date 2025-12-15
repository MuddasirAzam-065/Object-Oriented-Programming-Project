#ifndef ADMIN_H
#define ADMIN_H
#include "lms.h"
#include "remove.h"
#include "removet.h"
#include <QMainWindow>
#include <QStandardItemModel>

namespace Ui {
class admin;
}

class admin : public QMainWindow
{
    Q_OBJECT

public:
    explicit admin(QWidget *parent = nullptr);
    ~admin();
public slots:
    void on_logout_clicked();
    void on_addstudent_clicked();
    void on_viewstudents_clicked();
    void on_addteacher_clicked();
    void on_viewteachers_clicked();
    void on_removestudent_clicked();
    void on_removeteacher_clicked();
    void on_addcourse_clicked();
    void extracted(QStringList &fields, QList<QStandardItem *> &rowItems);
    void on_allcourses_clicked();
    void on_removecourse_clicked();
    void on_assignteacher_clicked();

private:
    Ui::admin *ui;
    QStandardItemModel *studentModel;
};

#endif // ADMIN_H
