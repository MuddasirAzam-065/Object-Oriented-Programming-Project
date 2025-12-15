#include "teacherdetails.h"
#include "ui_teacherdetails.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QRandomGenerator>

teacherdetails::teacherdetails(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::teacherdetails)
{
    ui->setupUi(this);
}

void teacherdetails::on_addteacher_clicked()
{
    QString name = ui->name->text();
    QString qualification = ui->qualification->text();
    QString cnic = ui->cnic->text();
    QString contact = ui->contact->text();
    QString email = ui->email->text();
    QString postalAdr = ui->postal->text();
    QString permanentAdr = ui->parmanent->text();
    QString degree = ui->degree->text();
    QString enrollYear = ui->year->text();
    const QString filePath = "teachers.csv";
    QString cnicpart = cnic.right(3);
    quint32 randomNum = QRandomGenerator::global()->bounded(1000);
    QString randomPart = QString::number(randomNum).rightJustified(3, '0');
    QString Id = "0113" + cnicpart + randomPart;
    QFile file(filePath);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        QMessageBox::critical(this, "File Error", "Could not open file for writing: " + filePath);
        return;
    }
    QString status;
    if(ui->visiting->isChecked())
        status = "Visiting";
    else if(ui->permanent->isChecked())
        status = "Permanent";
    else
    {QMessageBox::warning(this, "Input Error", "Please select either Visiting or Permanent address type.");
        return;}
    QTextStream out(&file);
    const QString teacherPasswordPlaceholder = "NULL";
    if (file.size() == 0) {
        out << "Name,ID,Qualification,CNIC,Contact,Email,Postal Address,Permanent Address,Departement,Enroll Year,Status,Password\n";
    }

    out << name << ","
        << Id << ","
        << qualification << ","
        << cnic << ","
        << contact << ","
        << email << ","
        << postalAdr << ","
        << permanentAdr << ","
        << degree << ","
        << enrollYear << ","
        << status << ","
        << teacherPasswordPlaceholder << "\n";

    file.close();
    QMessageBox::information(this, "Success", "Teacher details saved to " + filePath);
    QList<QLineEdit*> lineEdits = this->findChildren<QLineEdit*>();

    for (QLineEdit* le : lineEdits)
        le->clear();
}

teacherdetails::~teacherdetails()
{
    delete ui;
}
