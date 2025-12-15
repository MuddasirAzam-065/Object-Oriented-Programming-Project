#include "adminlogin.h"
#include "ui_adminlogin.h"
#include <QMessageBox>
#include <QLineEdit>
#include "admin.h"

adminlogin::adminlogin(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::adminlogin)
{
    ui->setupUi(this);
    ui->password->setEchoMode(QLineEdit::Password);
}

adminlogin::~adminlogin()
{
    delete ui;
}

void adminlogin::on_login_clicked()
{
    QString enteredEmail = ui->email->text();
    QString enteredPassword = ui->password->text();
    const QString correctEmail = "admin@gmail.com";
    const QString correctPassword = "admin12345";

    if (enteredEmail == correctEmail && enteredPassword == correctPassword)
    {
        admin *adminWindow = new admin();
        adminWindow->showMaximized();
        accept();
    }
    else
    {
        QMessageBox::warning(this, "Login Failed", "Invalid email or password.");
        ui->password->clear();
    }
}
