#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class login; // Class name changed to 'login'
}

class login : public QDialog // Base class remains QDialog
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();

public slots:
    void on_log_clicked();
    void on_firstlogin_clicked(); // Assuming the same first login logic exists

private:
    Ui::login *ui;
    // Returns the teacher's fields (QStringList) if login is successful.
    QStringList checkCredentials(const QString &email, const QString &password);
};

#endif // LOGIN_H
