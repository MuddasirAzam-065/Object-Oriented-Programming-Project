#ifndef REMOVECOURSE_H
#define REMOVECOURSE_H

#include <QDialog>

namespace Ui {
class removecourse;
}

class removecourse : public QDialog
{
    Q_OBJECT

public:
    explicit removecourse(QWidget *parent = nullptr);
    ~removecourse();
public slots:
    void on_fetch_clicked();
    void extracted(QStringList &remainingLines, QTextStream &out);
    void on_remove_clicked();

private:
    Ui::removecourse *ui;
    const QString CSV_FILE_PATH = "courses.csv";

    // Helper function to read the CSV file
    bool readCourses(const QString& courseCode, QString& courseTitle, QStringList& remainingLines, bool isRemoval);
};

#endif // REMOVECOURSE_H
