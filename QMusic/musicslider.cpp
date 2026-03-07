#include "musicslider.h"
#include "ui_musicslider.h"

#include <QMouseEvent>

MusicSlider::MusicSlider(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MusicSlider)
{
    ui->setupUi(this);

    // 初始情况下，还没有开始播放，将当前播放进度设置为0
    currentPos = 0;
    maxWidth = width();
    moveSilder();
}

MusicSlider::~MusicSlider()
{
    delete ui;
}

void MusicSlider::mousePressEvent(QMouseEvent *event)
{
    // 获取点击位置：QMouseEvent 中的 pos() 为鼠标相对于 widget 的本地坐标
    // 因此鼠标位置的 x 坐标可以直接作为进度条 (outLine) 的宽度
    currentPos = event->pos().x();

    // 执行 UI 更新
    moveSilder();
}

void MusicSlider::mouseMoveEvent(QMouseEvent *event)
{
    // 范围判定：定义当前控件的矩形区域，如果鼠标滑出范围，停止处理拖拽
    QRect rect = QRect(0, 0, width(), height());
    QPoint pos = event->pos();

    if (!rect.contains(pos))
    {
        return;
    }

    // 拖拽逻辑：仅当鼠标左键按下并移动时触发
    if (event->buttons() == Qt::LeftButton)
    {
        currentPos = event->pos().x();

        // 越界检查：确保进度位置不会小于 0 或超过最大宽度限制
        if (currentPos < 0)
        {
            currentPos = 0;
        }
        if (currentPos > maxWidth)
        {
            currentPos = maxWidth;
        }

        moveSilder();
    }
}

void MusicSlider::mouseReleaseEvent(QMouseEvent *event)
{
    // 释放确认：在鼠标松开时，最后一次更新进度位置
    currentPos = event->pos().x();
    moveSilder();

    emit setMusicSliderPosition((float)currentPos / (float)maxWidth);
}

void MusicSlider::moveSilder()
{
    // UI 渲染同步：根据计算出的 currentPos 动态调整进度条的宽度和几何布局
    ui->outLine->setMaximumWidth(currentPos);

    // 设置进度条的具体位置和尺寸 (y=8, height=4)
    ui->outLine->setGeometry(0, 8, currentPos, 4);
}

void MusicSlider::setStep(float bf)
{
    currentPos = maxWidth * bf;
    moveSilder();
}
