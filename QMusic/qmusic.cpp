#include "widget.h"
#include "ui_widget.h"

#include <QTimer>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    initUI();

}

Widget::~Widget()
{
    delete ui;
}

void Widget::initUI()
{
    // 设置窗口为无边框
    setWindowFlag(Qt::WindowType::FramelessWindowHint);

    // 为窗口添加阴影
    addShadow();

    // 设置BodyLeft中6个btForm的信息
    setBtFormInfo();

    // 链接信号和槽
    connectSignalAndSlot();

    // 这里直接调“onBtFormClick(1);”，可能会出现 Bug，就是“推荐”选项卡的频谱能跳动，但颜色并没有变化，还是初始的灰色。
    // 原因是当我们在 Widget 的构造函数里调用 onBtFormClick(1); 时，构造函数运行，
    // 此时，整个窗口其实还没有真正显示到屏幕上（处于隐身状态），在这个“隐身”阶段，Qt 的 QSS 样式引擎有时会比较“懒”，
    // 它接收到了你改变背景颜色的指令，但它觉得“反正还没显示，我先不画了”，结果等窗口真正弹出来时，样式就被忽略了。
    // 所以，我们可以用 Qt 中的下面这个函数解决上面的问题，这个函数的意思是：在指定的延时时间后，执行一次指定的槽函数回调函数。
    // （这里设置为 0 并不是真的 “立刻执行”，而是告诉 Qt：“当前代码执行完后，尽快执行这个操作”）
    QTimer::singleShot(0, this, [=](){
        onBtFormClick(1);
    });
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    QWidget *clickedWidget = childAt(event->pos());

    // 只要鼠标在顶部 80 个像素内，并且没有点到按钮 (max, min, quit, skin) 和 输入框 (search)，条件就达成
    // 注意：必须加上 clickedWidget == nullptr 的判断，防止鼠标刚好点击在了一个连背景 QWidget 都没有铺到的绝对空白边缘时
    // 对空指针解引用，此时程序会崩溃
    if (event->pos().y() < 80 &&
       (clickedWidget == nullptr ||
       (!clickedWidget->inherits("QPushButton") && !clickedWidget->inherits("QLineEdit"))))
    {

        // "==" 仅响应鼠标左键
        if (event->button() == Qt::LeftButton)
        {
            isDragging = true; // 鼠标左键已按下准备拖拽

            /*
            * event->globalPos(): 鼠标相对于屏幕左上角的位置
            * frameGeometry().topLeft(): 整个窗口（含边框）相对于屏幕左上角的位置
            * 两者相减，得到鼠标在窗口内部的相对坐标
            */
            dragPosition = event->globalPos() - frameGeometry().topLeft();

            // 接受事件，不再向下传递（也可以不写，但为了规范还是写上）
            event->accept();
            return;
        }
    }

    isDragging = false;
    // 交给基类处理
    QWidget::mousePressEvent(event);
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    // event->buttons(): 返回鼠标按键状态
    // "&" 只要按下了左键，不管有没有同时按住其他键，条件都成立。
    // 只有在明确标记为“拖拽中”且按住左键时才移动
    if (isDragging && (event->buttons() & Qt::LeftButton))
    {
        // 窗口新位置 = 当前鼠标的全局位置 - 之前记录的偏移量
        move(event->globalPos() - dragPosition);

        event->accept();
        return;
    }

    QWidget::mouseMoveEvent(event);
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    isDragging = false;
    (void)event;
}

void Widget::addShadow()
{
    // 1. 设置窗口背景透明（非常关键，否则阴影会被窗口的背景色遮挡）
    this->setAttribute(Qt::WA_TranslucentBackground);

    // 2. 创建阴影效果对象
    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);

    // 设置阴影偏移：(0,0) 表示阴影向四周均匀发散，没有特定方向的偏移
    shadowEffect->setOffset(0, 0);

    // 设置阴影颜色：使用半透明黑色，视觉效果更柔和
    shadowEffect->setColor(QColor(0, 0, 0, 160));

    // 设置阴影的模糊半径：数值越大，阴影越弥散
    shadowEffect->setBlurRadius(20);

    // 3. 将阴影效果应用到窗口上
    this->setGraphicsEffect(shadowEffect);
}

void Widget::setBtFormInfo()
{
    ui->rec->seticon(":/images/rec.png", "推荐", 1);
    ui->music->seticon(":/images/music.png", "⾳乐馆", 2);
    ui->audio->seticon(":/images/radio.png", "电台", 3);
    ui->like->seticon(":/images/like.png", "我喜欢", 4);
    ui->local->seticon(":/images/local.png", "本地下载", 5);
    ui->recent->seticon(":/images/recent.png", "最近播放", 6);
}

void Widget::onBtFormClick(int id)
{
    // 1.获取当前⻚⾯所有btFrom按钮类型的对象
    // 因为 findChildren 是一个递归搜索操作，会递归地查找当前对象（this，即你的 Widget）对象树上的所有控件，
    // 只要控件的类型是 BtForm（或者是其派生类），它都会被抓取出来。
    // 所以，还是更推荐在 Widget.h 中定义 QList<BtForm*> m_btList;，
    // 并在在构造函数执行一次 m_btList = this->findChildren<BtForm*>();，这样性能会更好。
    QList<BtForm*> buttonList = this->findChildren<BtForm*>();

    // 2.遍历所有对象, 如果不是当前id的按钮,则把之前设置的背景颜⾊清除掉
    foreach (BtForm* btitem, buttonList)
    {
        if (id != btitem->getId())
        {
            btitem->clearBg();
        }
        else
        {
            btitem->setActive();
        }
    }
    // 3.设置当前栈空间显⽰⻚⾯
    ui->stackedWidget->setCurrentIndex(id - 1);
}

void Widget::connectSignalAndSlot()
{
    // ⾃定义的btFrom按钮点击信号，当btForm点击后，设置对应的堆叠窗⼝
    connect(ui->rec, &BtForm::click, this, &Widget::onBtFormClick);
    connect(ui->music, &BtForm::click, this, &Widget::onBtFormClick);
    connect(ui->audio, &BtForm::click, this, &Widget::onBtFormClick);
    connect(ui->like, &BtForm::click, this, &Widget::onBtFormClick);
    connect(ui->local, &BtForm::click, this, &Widget::onBtFormClick);
    connect(ui->recent, &BtForm::click, this, &Widget::onBtFormClick);
}



