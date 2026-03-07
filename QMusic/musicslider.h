#ifndef MUSICSLIDER_H
#define MUSICSLIDER_H

#include <QWidget>

namespace Ui {
class MusicSlider;
}

class MusicSlider : public QWidget
{
    Q_OBJECT

public:
    explicit MusicSlider(QWidget *parent = nullptr);
    ~MusicSlider();

    void mousePressEvent(QMouseEvent *event); // 重写⿏标按下事件
    void mouseMoveEvent(QMouseEvent *event); // 重写⿏标滚动事件
    void mouseReleaseEvent(QMouseEvent *event); // 重写⿏标释放事件

    void moveSilder(); // 修改outLine的宽度为currentPos

    void setStep(float bf);

signals:
    void setMusicSliderPosition(float);

private:
    Ui::MusicSlider *ui;

    int currentPos; // 滑动条当前位置
    int maxWidth; // 滑动条总宽度
};

#endif // MUSICSLIDER_H
