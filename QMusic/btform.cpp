#include "btform.h"
#include "ui_btform.h"

#include <QMouseEvent>
#include <QTimer>
#include <QRandomGenerator>

BtForm::BtForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BtForm)
{
    ui->setupUi(this);

    // 实例化定时器
    timer = new QTimer(this);

    // 连接定时器的“超时信号”与的“跳动函数”
    connect(timer, &QTimer::timeout, this, &BtForm::updateSpectrum);
}

BtForm::~BtForm()
{
    delete ui;
}

void BtForm::seticon(QString btIcon, QString content, int mid)
{
    // 设置⾃定义按钮的图⽚、⽂字、以及id
    ui->btIcon->setPixmap(QPixmap(btIcon));
    ui->btText->setText(content);
    this->id = mid;
}

void BtForm::setActive()
{
    // 背景变为绿⾊，⽂字变为⽩⾊
    ui->btStyle->setStyleSheet("#btStyle{ background:rgb(30,206,154);}*{color:#F6F6F6;}");

    // 启动定时器，参数是毫秒
    timer->start(100);
}

void BtForm::clearBg()
{
    // 清除上⼀个按钮点击的背景效果,恢复之前的样式
    ui->btStyle->setStyleSheet("#btStyle:hover{ background:#D8D8D8;} ");

    // 取消选中时：停止动画
    timer->stop();

    // 把 4 个竖条的高度打回原形（比如恢复成静止时的 4 像素高）
    ui->line1->setFixedHeight(4);
    ui->line2->setFixedHeight(4);
    ui->line3->setFixedHeight(4);
    ui->line4->setFixedHeight(4);
}

int BtForm::getId()
{
    return id;
}

void BtForm::mousePressEvent(QMouseEvent *event)
{
    // 告诉编译器不要触发警告
    (void)event;

    emit click(this->id); // 发送⿏标点击信号
    event->accept();
}

void BtForm::updateSpectrum()
{
    // 生成 4 到 16 像素之间的随机高度 (你可以根据你 UI 的实际大小调整 16 这个最大值)
    int h1 = QRandomGenerator::global()->bounded(4, 17);
    int h2 = QRandomGenerator::global()->bounded(4, 17);
    int h3 = QRandomGenerator::global()->bounded(4, 17);
    int h4 = QRandomGenerator::global()->bounded(4, 17);

    // 瞬间改变四个小竖条的高度，制造顿挫的律动感
    ui->line1->setFixedHeight(h1);
    ui->line2->setFixedHeight(h2);
    ui->line3->setFixedHeight(h3);
    ui->line4->setFixedHeight(h4);
}
