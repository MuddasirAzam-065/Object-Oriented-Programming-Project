#include "firstlogins.h"
#include "ui_firstlogins.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

firstlogins::firstlogins(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::firstlogins)
{
    ui->setupUi(this);
}

firstlogins::~firstlogins()
{
    delete ui;
}

void firstlogins::on_first_clicked()
{
    QString name = ui->name->text().trimmed();
    QString email = ui->email->text().trimmed();
    QString password = ui->password->text().trimmed();
    if (name.isEmpty() || email.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter Name, Email and Password.");
        return;
    }
    if (updatePasswordInCSV(name, email, password)) {
        QMessageBox::information(this, "Success", "Password assigned successfully!");
        this->close();
    }
    else {
        QMessageBox::critical(this, "Failed", "Name and Email not found!");
    }
}

bool firstlogins::updatePasswordInCSV(const QString &name, const QString &email, const QString &password)
{
    QFile file("students.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    QTextStream in(&file);
    QStringList lines;
    bool updated = false;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList cols = line.split(",");
        if (cols.size() < 11) {
            lines.append(line);
            continue;
        }
        QString csvName = cols[0].trimmed();
        QString csvEmail = cols[5].trimmed();
        if (csvName.toLower() == name.toLower() &&
            csvEmail == email) {

            cols[10] = password;
            updated = true;
        }
        lines.append(cols.join(","));
    }
    file.close();
    if (!updated)
        return false;
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        return false;
    QTextStream out(&file);
    for (const QString &l : lines)
        out << l << "\n";
    file.close();
    return true;
}
