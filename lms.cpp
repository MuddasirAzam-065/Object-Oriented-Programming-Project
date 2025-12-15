#include "lms.h"
#include "adminlogin.h"
#include "ui_lms.h"
#include "login.h"
#include "logins.h"

lms::lms(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::lms)
{
    ui->setupUi(this);
}
void lms::on_admin_clicked()
{
    adminlogin *dialog = new adminlogin(this);
    if (dialog->exec() == QDialog::Accepted)
    {
        close();
    }
    dialog->deleteLater();
}

void lms::on_faculity_clicked()
{
    login *log = new login(this);
    if(log->exec() == QDialog::Accepted)
    {close();}
    log->deleteLater();
}

void lms::on_student_clicked()
{
    logins *log = new logins(this);
    if(log->exec() == QDialog::Accepted)
    {close();}
    log->deleteLater();
}
lms::~lms()
{
    delete ui;
}
