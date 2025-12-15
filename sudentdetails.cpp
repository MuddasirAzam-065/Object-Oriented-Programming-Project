#include "sudentdetails.h"
#include "qdatetime.h"
#include "ui_sudentdetails.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QRandomGenerator>

sudentdetails::sudentdetails(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::sudentdetails)
{
    ui->setupUi(this);
}
void sudentdetails::on_addstudent_clicked()
{
    QString name = ui->name->text();
    QString fatherName = ui->fathername->text();
    QString cnic = ui->cnic->text();
    QString contact = ui->contact->text();
    QString email = ui->email->text();
    QString postalAdr = ui->postal->text();
    QString permanentAdr = ui->parmanent->text();
    QString degree = ui->degree->text();
    QString enrollYearStr = ui->year->text();
    QString batch;
    if (ui->spring->isChecked()) {
        batch = "Spring";
    } else if (ui->spring_2->isChecked()) {
        batch = "Fall";
    }
    else {
        QMessageBox::critical(this, "Input Error", "Please select either Fall or Spring batch.");
        return;
    }

    const QString filePath = "students.csv";
    QString enrollYearPart = enrollYearStr.right(2);
    quint32 randomNum = QRandomGenerator::global()->bounded(1000);
    QString randomPart = QString::number(randomNum).rightJustified(3, '0');
    QString enrollmentId = "0113" + enrollYearPart + randomPart;
    int enrollYear = enrollYearStr.toInt();
    int currentYear = QDate::currentDate().year();
    int currentMonth = QDate::currentDate().month();
    int semesterCount = 0;
    int yearsPassed = currentYear - enrollYear;
    semesterCount = yearsPassed * 2;

    if (yearsPassed >= 0) {
        if (currentMonth <= 6) {
            if (batch == "Spring") {
                semesterCount += 1;
            }
            else {
                // If enrolled in Fall, Spring is not yet completed in the current year
                // No change needed as the Spring semester of the current year hasn't been completed
            }
        }
        else {
            semesterCount += 1;
            if (batch == "Fall") {
                semesterCount += 1;
            }
            else {
                // If enrolled in Spring, Fall is not yet completed in the current year
                // No change needed as the Fall semester of the current year hasn't been completed
            }
        }

        int totalSemestersPassed = 0;
        int yearsDiff = currentYear - enrollYear;
        totalSemestersPassed += yearsDiff * 2;
        if (currentMonth > 6) {
            totalSemestersPassed += 1;
            totalSemestersPassed += 1;
        }
        else {
            totalSemestersPassed += 1;
        }

        if (batch == "Fall") {
            int springSemesterStartMonth = 1;
            int fallSemesterStartMonth = 7;
            int totalMonths = (currentYear - enrollYear) * 12 + (currentMonth - springSemesterStartMonth);
            int batchOffset = 0;
            if (batch == "Fall") {
                batchOffset = -6;
            }

            int monthsSinceEnrollmentStart = totalMonths + batchOffset;
            semesterCount = (monthsSinceEnrollmentStart / 6) + 1; // +1 to make it 1-indexed

        }
        else {
            int springSemesterStartMonth = 1; // January
            int totalMonths = (currentYear - enrollYear) * 12 + (currentMonth - springSemesterStartMonth);
            int batchOffset = 0;
            if (batch == "Fall") {
                batchOffset = -6;
            }

            int monthsSinceEnrollmentStart = totalMonths + batchOffset;
            semesterCount = (monthsSinceEnrollmentStart + 5) / 6;
            if (semesterCount < 1) {
                semesterCount = 1;
            }
        }
    }
    else {
        semesterCount = 0;
    }

    QString currentSemester = QString::number(semesterCount);
    QFile file(filePath);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        QMessageBox::critical(this, "File Error", "Could not open file for writing: " + filePath);
        return;
    }

    QTextStream out(&file);
    const QString studentPasswordPlaceholder = "NULL";
    if (file.size() == 0) {
        out << "Name,Enrollement,Father Name,CNIC,Contact,Email,Postal Address,Permanent Address,Degree,Enroll Year,Password,Batch,Current Semester\n";
    }
    out << name << ","
        << enrollmentId << ","
        << fatherName << ","
        << cnic << ","
        << contact << ","
        << email << ","
        << postalAdr << ","
        << permanentAdr << ","
        << degree << ","
        << enrollYearStr << ","
        << studentPasswordPlaceholder << ","
        << batch << ","
        << currentSemester << "\n";

    file.close();
    QMessageBox::information(this, "Success", "Student details saved to " + filePath);
    QMessageBox::information(this, "Success", "Teacher details saved to " + filePath);
    QList<QLineEdit*> lineEdits = this->findChildren<QLineEdit*>();

    for (QLineEdit* le : std::as_const(lineEdits))
        le->clear();
}
sudentdetails::~sudentdetails()
{
    delete ui;
}
