#include "recboxitem.h"
#include "ui_recboxitem.h"
#include <QPropertyAnimation>
#include <QDebug>
#include <QEvent>

RecBoxItem::RecBoxItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecBoxItem)
{
    ui->setupUi(this);

    // 1. 安装事件拦截器
    ui->musicImageBox->installEventFilter(this);

    // 2. 初始化动画对象（指定 this 为父对象，程序结束时自动销毁，无需 delete）
    animation = new QPropertyAnimation(ui->musicImageBox, "geometry", this);

    // 注意这里不能直接获取图片框最开始的 Y 坐标，因为此时 C++ 的类构造函数执行正在执行，
    // 这个窗口（RecBoxItem）其实还没有真正显示到屏幕上，所以 Qt 的布局引擎还没有完成最终的排版计算，
    // 在这个时候去问 musicImageBox：“你的 Y 坐标是多少？”
    // 它只会会迷茫地回答你：“我还没排好版呢，默认是 0 吧。”
    // 但我们期望的是排版后的 10 ，所以后续执行动画时会出现图片上移时部分被吞噬的情况
    // 并且当鼠标移出图片后图片还无法回到原位，因为上移动画实际上上移了 20 ，下移动画却只下移了 10
    // originY = ui->musicImageBox->y();

    // 3. 随便给个负数作为还没获取坐标的标记
    originY = -1;
}

RecBoxItem::~RecBoxItem()
{
    delete ui;
}

void RecBoxItem::setText(const QString& text)
{
    ui->recBoxItemText->setText(text);
}

void RecBoxItem::setImage(const QString& Imagepath)
{
    QString imgStyle = "border-image:url("+Imagepath+");";
    ui->recMusicImage->setStyleSheet(imgStyle);
}

bool RecBoxItem::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->musicImageBox)
    {
        // 动态获取当前的宽高和 X 坐标
        int currentX = ui->musicImageBox->x();
        int currentW = ui->musicImageBox->width();
        int currentH = ui->musicImageBox->height();

        if (event->type() == QEvent::Enter)
        {
            // 如果是第一次进入，此时布局已完成，精准抓取真实 Y 坐标
            if (originY == -1)
            {
                originY = ui->musicImageBox->y();
            }

            animation->stop(); // 停止可能正在执行的回弹动画
            animation->setDuration(100);
            // 重点：从“当前所在位置”开始动画，实现丝滑打断
            animation->setStartValue(ui->musicImageBox->geometry());
            // 目标位置：Y 轴向上移动 10 像素
            animation->setEndValue(QRect(currentX, originY - 10, currentW, currentH));
            animation->start();

            return true;
        }
        else if (event->type() == QEvent::Leave)
        {
            animation->stop(); // 停止可能正在执行的上升动画
            animation->setDuration(150);

            animation->setStartValue(ui->musicImageBox->geometry());
            // 目标位置：回到最初记录的 Y 坐标
            animation->setEndValue(QRect(currentX, originY, currentW, currentH));
            animation->start();

            return true;
        }
    }

    return QWidget::eventFilter(watched, event);
}
