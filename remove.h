#ifndef REMOVE_H
#define REMOVE_H

#include <QDialog>

namespace Ui {
class remove;
}

class remove : public QDialog
{
    Q_OBJECT

public:
    explicit remove(QWidget *parent = nullptr);
    ~remove();
public slots:
    void on_fetch_clicked();
    void on_delete_2_clicked();
private:
    Ui::remove *ui;
};

#endif // REMOVE_H
