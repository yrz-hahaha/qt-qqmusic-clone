#ifndef RECBOXITEM_H
#define RECBOXITEM_H

#include <QWidget>
#include <QPropertyAnimation>

namespace Ui {
class RecBoxItem;
}

class RecBoxItem : public QWidget
{
    Q_OBJECT

public:
    explicit RecBoxItem(QWidget *parent = nullptr);
    ~RecBoxItem();

    // 设置推荐文本和图片
    void setText(const QString& text);
    void setImage(const QString& Imagepath);

protected:
    // 声明事件过滤器，用于拦截子控件的鼠标进出事件
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Ui::RecBoxItem *ui;

    // 动画对象和初始Y坐标
    QPropertyAnimation *animation;
    int originY;
};

#endif // RECBOXITEM_H
