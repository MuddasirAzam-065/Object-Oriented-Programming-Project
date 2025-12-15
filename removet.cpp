#include "removet.h"
#include "ui_removet.h"
#include <QFile>
#include <QMessageBox>
#include <Qt>


removet::removet(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::removet)
{
    ui->setupUi(this);
}

void removet::on_fetch_clicked()
{
    QString cnicToFind = ui->cnic->text().trimmed();
    if (cnicToFind.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter a CNIC to fetch.");
        return;
    }

    QFile file("teachers.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "File Error", "Could not open teachers.csv for reading.");
        return;
    }
    QTextStream in(&file);
    in.readLine();
    bool found = false;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields;
        bool inQuotes = false;
        int start = 0;
        for (int i = 0; i < line.length(); ++i) {
            QChar c = line.at(i);
            if (c == '"')
                inQuotes = !inQuotes;
            if (c == ',' && !inQuotes) {
                QString field = line.mid(start, i - start).trimmed();
                if (field.startsWith('"') && field.endsWith('"') && field.length() > 1)
                    field = field.mid(1, field.length() - 2);
                fields.append(field);
                start = i + 1;
            }
        }
        QString lastField = line.mid(start).trimmed();
        if (lastField.startsWith('"') && lastField.endsWith('"') && lastField.length() > 1)
            lastField = lastField.mid(1, lastField.length() - 2);
        fields.append(lastField);
        if (fields.size() > 3 && fields[3].trimmed() == cnicToFind) {
            ui->name->setText(fields[0].trimmed());
            ui->ID->setText(fields[1].trimmed());
            found = true;
            break;
        }
    }
    file.close();
    if (!found) {
        QMessageBox::information(this, "Search Result", "CNIC not found in teacher records.");
        ui->name->clear();
        ui->ID->clear();
    }
}

void removet::on_cut_clicked()
{
    QString cnicToDelete = ui->cnic->text().trimmed();
    if (cnicToDelete.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter a CNIC to delete.");
        return;
    }

    QFile file("teachers.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "File Error", "Could not open teachers.csv for reading.");
        return;
    }
    QTextStream in(&file);
    QString header = in.readLine();
    QList<QString> lines;
    bool deleted = false;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields;
        bool inQuotes = false;
        int start = 0;
        for (int i = 0; i < line.length(); ++i) {
            QChar c = line.at(i);
            if (c == '"')
                inQuotes = !inQuotes;
            if (c == ',' && !inQuotes) {
                QString field = line.mid(start, i - start).trimmed();
                if (field.startsWith('"') && field.endsWith('"') && field.length() > 1)
                    field = field.mid(1, field.length() - 2);
                fields.append(field);
                start = i + 1;
            }
        }

        QString lastField = line.mid(start).trimmed();
        if (lastField.startsWith('"') && lastField.endsWith('"') && lastField.length() > 1)
            lastField = lastField.mid(1, lastField.length() - 2);
        fields.append(lastField);
        if (fields.size() > 3 && fields[3].trimmed() == cnicToDelete) {
            deleted = true;
            continue;
        }
        lines.append(line);
    }
    file.close();

    if (!deleted) {
        QMessageBox::information(this, "Delete Failed", "No teacher found with this CNIC.");
        return;
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QMessageBox::critical(this, "File Error", "Could not open teachers.csv for writing.");
        return;
    }

    QTextStream out(&file);
    out << header << "\n";

    for (const QString &l : lines)
        out << l << "\n";
    file.close();
    QMessageBox::information(this, "Success", "Teacher record deleted successfully!");
    ui->name->clear();
    ui->ID->clear();
}

removet::~removet()
{
    delete ui;
}
