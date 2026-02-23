#ifndef LISTITEMBOX_H
#define LISTITEMBOX_H

#include <QWidget>

namespace Ui {
class ListItemBox;
}

class ListItemBox : public QWidget
{
    Q_OBJECT

public:
    explicit ListItemBox(QWidget *parent = nullptr);
    ~ListItemBox();

protected:
    // 重写鼠标进入和离开事件处理函数
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

private:
    Ui::ListItemBox *ui;
};

#endif // LISTITEMBOX_H
