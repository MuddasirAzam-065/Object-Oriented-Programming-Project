#include "addcourse.h"
#include "ui_addcourse.h"
#include <QTextStream>
#include <QFile>
#include <QMessageBox>

addcourse::addcourse(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addcourse)
{
    ui->setupUi(this);
}

void addcourse::on_add_clicked()
{
    saveCourse();
    QMessageBox::information(this, "Success", "Course added successfully!");
}

void addcourse::saveCourse()
{
    QString title = ui->title->text();
    QString code = ui->code->text();
    QString hours = ui->hours->text();
    QString semester = ui->semester->text();
    QString degree = ui->degree->text();
    QString filePath = "courses.csv";
    QFile file(filePath);

    bool newFile = !file.exists();

    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);

        if (newFile) {
            out << "Title,Code,Credit Hours,Semester,Degree,Assigned Teacher\n";
        }

        out << title << "," << code << "," << hours << ","
            << semester<< ","<< degree << ",NULL\n";

        file.close();
    }
    sortCSVBySemester(filePath, 3);
}

void addcourse::sortCSVBySemester(const QString &filePath, int semesterColumn)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    QString header = in.readLine();

    QList<QStringList> rows;

    while (!in.atEnd()) {
        QString line = in.readLine();
        rows.append(line.split(","));
    }
    file.close();

    std::sort(rows.begin(), rows.end(), [semesterColumn](const QStringList &a, const QStringList &b) {
        return a[semesterColumn].toInt() < b[semesterColumn].toInt();
    });

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << header << "\n";
        for (const auto &row : rows)
            out << row.join(",") << "\n";
        file.close();
    }
}

addcourse::~addcourse()
{
    delete ui;
}
