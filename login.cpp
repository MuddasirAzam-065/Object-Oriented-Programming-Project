#include "login.h"
#include "ui_login.h"
#include "teacher.h"
#include "firstlogin.h"
#include <QFile>
#include <QMessageBox>
#include <Qt>

login::login(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::login)
{
    ui->setupUi(this);
}

login::~login()
{
    delete ui;
}

QStringList login::checkCredentials(const QString &email, const QString &password)
{
    const int EMAIL_COLUMN = 5;
    const int PASSWORD_COLUMN = 11;
    QFile file("teachers.csv");
    QStringList teacherFields;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "File Error", "Could not open teachers.csv. Check if it exists.");
        return teacherFields;
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
                    return teacherFields;
                }
                if (filePassword == password) {
                    for (QString &field : fields) {
                        teacherFields.append(field.trimmed());
                    }
                    return teacherFields;
                }
                else {
                    return teacherFields;
                }
            }
        }
    }

    file.close();
    return teacherFields;
}

void login::on_log_clicked()
{
    QString enteredEmail = ui->email->text().trimmed();
    QString enteredPassword = ui->password->text().trimmed();

    if (enteredEmail.isEmpty() || enteredPassword.isEmpty()) {
        QMessageBox::warning(this, "Input Required", "Please enter both Email and Password.");
        return;
    }
    QStringList studentData = checkCredentials(enteredEmail, enteredPassword);
    if (!studentData.isEmpty()) {
        teacher *studentWindow = new teacher(studentData);
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

void login::on_firstlogin_clicked()
{
    firstlogin *s = new firstlogin();
    s->exec();
    s->deleteLater();
}
