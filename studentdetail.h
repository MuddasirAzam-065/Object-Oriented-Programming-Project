#ifndef STUDENTDETAIL_H
#define STUDENTDETAIL_H
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QRegularExpression>
#include <Qt>
struct StudentData {
    QString name;
    QString enrollment;
    QString fatherName;
    QString cnic;
    QString contact;
    QString email;
    QString postalAddress;
    QString permanentAddress;
    QString degree;
    QString enrollYear;
    QString password;

    // Converts the stored data into a list format suitable for display
    QStringList toQStringList() const {
        return QStringList()
        << "Name: " + name
        << "Enrollment: " + enrollment
        << "Father Name: " + fatherName
        << "CNIC: " + cnic
        << "Contact: " + contact
        << "Email: " + email
        << "Postal Address: " + postalAddress
        << "Permanent Address: " + permanentAddress
        << "Degree: " + degree
        << "Enroll Year: " + enrollYear
            // Password is omitted from this display list
            ;
    }
};

#endif // STUDENTDETAIL_H
