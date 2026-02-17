#ifndef BTFORM_H
#define BTFORM_H

#include <QWidget>

namespace Ui {
class BtForm;
}

class BtForm : public QWidget
{
    Q_OBJECT

public:
    explicit BtForm(QWidget *parent = nullptr);
    ~BtForm();

    // 设置图标 ⽂字 id
    void seticon(QString btIcon,QString content,int mid);

    // 点亮当前按钮并发出超时信号使频谱跳动起来
    void setActive();

    // 清除上⼀次按钮点击留下的样式
    void clearBg();

    // 获取id
    int getId();

protected:
    // ⿏标点击事件
    virtual void mousePressEvent(QMouseEvent *event);

signals:
    // 自定义一个点击信号
    void click(int mid);

private slots:
    // 频谱跳动函数
    void updateSpectrum();

private:
    Ui::BtForm *ui;

    // 按钮id：该按钮对应的page⻚
    int id = 0;

    // 定时器指针
    QTimer *timer;
};

#endif // BTFORM_H
