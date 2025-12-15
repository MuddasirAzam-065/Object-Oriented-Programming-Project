#ifndef REMOVET_H
#define REMOVET_H

#include <QDialog>

namespace Ui {
class removet;
}

class removet : public QDialog
{
    Q_OBJECT

public:
    explicit removet(QWidget *parent = nullptr);
    ~removet();
public slots:
    void on_fetch_clicked();
    void on_cut_clicked();
private:
    Ui::removet *ui;
};

#endif // REMOVET_H
