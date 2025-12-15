#ifndef SUDENTDETAILS_H
#define SUDENTDETAILS_H

#include <QDialog>

namespace Ui {
class sudentdetails;
}

class sudentdetails : public QDialog
{
    Q_OBJECT

public:
    explicit sudentdetails(QWidget *parent = nullptr);
    ~sudentdetails();
public slots:
    void on_addstudent_clicked();
private:
    Ui::sudentdetails *ui;
};

#endif // SUDENTDETAILS_H
