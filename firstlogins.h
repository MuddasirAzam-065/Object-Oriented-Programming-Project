#ifndef FIRSTLOGINS_H
#define FIRSTLOGINS_H

#include <QDialog>

namespace Ui {
class firstlogins;
}

class firstlogins : public QDialog
{
    Q_OBJECT

public:
    explicit firstlogins(QWidget *parent = nullptr);
    ~firstlogins();
public slots:
    void on_first_clicked();
private:
    Ui::firstlogins *ui;
     bool updatePasswordInCSV(const QString &name, const QString &email, const QString &password);
};

#endif // FIRSTLOGINS_H
