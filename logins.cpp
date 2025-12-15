#include "logins.h"
#include "ui_logins.h"
#include "student.h"
#include "firstlogins.h"
#include "student.h"
#include <QFile>
#include <QMessageBox>
#include <Qt>
logins::logins(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::logins)
{
    ui->setupUi(this);
}

logins::~logins()
{
    delete ui;
}

QStringList logins::checkCredentials(const QString &email, const QString &password)
{
    const int EMAIL_COLUMN = 5;
    const int PASSWORD_COLUMN = 10;

    QFile file("students.csv");
    QStringList studentFields;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "File Error", "Could not open students.csv. Check if it exists.");
        return studentFields;
    }
    QTextStream in(&file);
    if (!in.atEnd()) {
        in.readLine();
    }
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');
        if (fields.size() > PASSWORD_COLUMN) {
            QString fileEmail = fields.at(EMAIL_COLUMN).trimmed();
            QString filePassword = fields.at(PASSWORD_COLUMN).trimmed();
            if (fileEmail == email) {
                file.close();
                if (filePassword == "NULL")
                {
                    QMessageBox::critical(this, "Set Password First", "Press First Login To set New password");
                    return studentFields;
                }
                if (filePassword == password) {
                    for (QString &field : fields) {
                        studentFields.append(field.trimmed());
                    }
                    return studentFields;
                }
                else {
                    return studentFields;
                }
            }
        }
    }

    file.close();
    return studentFields;
}

void logins::on_log_clicked()
{
    QString enteredEmail = ui->email->text().trimmed();
    QString enteredPassword = ui->password->text().trimmed();
    if (enteredEmail.isEmpty() || enteredPassword.isEmpty()) {
        QMessageBox::warning(this, "Input Required", "Please enter both Email and Password.");
        return;
    }
    QStringList studentData = checkCredentials(enteredEmail, enteredPassword);
    if (!studentData.isEmpty()) {
        student *studentWindow = new student(studentData);
        studentWindow->showMaximized();
        accept();

    }
    else {
        if (QMessageBox::warning(this, "Login Failed", "Invalid Email or Password. Please try again.", QMessageBox::NoButton) == QMessageBox::NoButton)
        {
            // Do nothing, but this line structure is sometimes needed to avoid compiler warnings
        }
    }
}

void logins::on_firstlogin_clicked()
{
    firstlogins *s = new firstlogins();
    s->exec();
    s->deleteLater();
}
