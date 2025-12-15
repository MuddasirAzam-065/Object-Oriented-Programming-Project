#ifndef FIRSTLOGIN_H
#define FIRSTLOGIN_H

#include <QDialog>

namespace Ui {
class firstlogin;
}

class firstlogin : public QDialog
{
    Q_OBJECT

public:
    explicit firstlogin(QWidget *parent = nullptr);
    ~firstlogin();
public slots:
    void on_first_clicked();
private:
    Ui::firstlogin *ui;
    bool updatePasswordInCSV(const QString &name, const QString &email, const QString &password);
};

#endif // FIRSTLOGIN_H
