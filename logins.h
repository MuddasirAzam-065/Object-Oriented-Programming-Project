#ifndef LOGINS_H
#define LOGINS_H

#include <QDialog>
#include <QStringList>
namespace Ui {
class logins;
}

class logins : public QDialog
{
    Q_OBJECT

public:
    explicit logins(QWidget *parent = nullptr);
    ~logins();
public slots:
    void on_log_clicked();
    void on_firstlogin_clicked();
private:
    Ui::logins *ui;
    QStringList checkCredentials(const QString &email, const QString &password);
};

#endif // LOGINS_H
