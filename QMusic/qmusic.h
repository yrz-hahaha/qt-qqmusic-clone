#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void initUI();

protected:
    // 重写QWidget类的⿏标单击和⿏标移动事件
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    // 为窗⼝添加阴影
    void addShadow();

    // 设置BodyLeft中6个btForm的信息
    void setBtFormInfo();

    // btForm点击槽函数
    void onBtFormClick(int id);

    // 链接信号和槽
    void connectSignalAndSlot();



private:
    Ui::Widget *ui;

    bool isDragging;		// 记录鼠标是否处于按下并准备拖拽的状态
    QPoint dragPosition;	// 记录鼠标按下时，鼠标指针相对于窗口左上角的坐标偏移
};
#endif // WIDGET_H
