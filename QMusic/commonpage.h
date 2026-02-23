#ifndef COMMONPAGE_H
#define COMMONPAGE_H

#include <QWidget>

namespace Ui {
class CommonPage;
}

class CommonPage : public QWidget
{
    Q_OBJECT

public:
    explicit CommonPage(QWidget *parent = nullptr);
    ~CommonPage();

    void setCommonPageUI(const QString &title, const QString &image);

private:
    Ui::CommonPage *ui;
};

#endif // COMMONPAGE_H
