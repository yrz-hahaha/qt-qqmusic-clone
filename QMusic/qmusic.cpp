#include "qmusic.h"
#include "ui_qmusic.h"

#include <QTimer>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QFileDialog>

QMusic::QMusic(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QMusic)
{
    ui->setupUi(this);

    initUI();
}

QMusic::~QMusic()
{
    delete ui;
}

void QMusic::initUI()
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

    // 向 recBox 中添加元素
    srand(time(NULL));
    ui->recMusicBox->initRecBoxUi(randomPicture(), 1);
    ui->supplyMusicBox->initRecBoxUi(randomPicture(), 2);

    // 设置我喜欢、本地⾳乐、最近播放⻚⾯
    ui->likePage->setMusicListType(PageType::LIKE_PAGE);
    ui->likePage->setCommonPageUI("我喜欢", ":/images/ilikebg.png");
    ui->localPage->setMusicListType(PageType::LOCAL_PAGE);
    ui->localPage->setCommonPageUI("本地⾳乐", ":/images/localbg.png");
    ui->recentPage->setMusicListType(PageType::HISTORY_PAGE);
    ui->recentPage->setCommonPageUI("最近播放", ":/images/recentbg.png");

    // 创建⾳量调节窗⼝对象并挂到对象树
    volumeTool = new VolumeTool(this);
}

void QMusic::mousePressEvent(QMouseEvent *event)
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

void QMusic::mouseMoveEvent(QMouseEvent *event)
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

void QMusic::mouseReleaseEvent(QMouseEvent *event)
{
    isDragging = false;
    (void)event;
}

void QMusic::addShadow()
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

void QMusic::setBtFormInfo()
{
    ui->rec->seticon(":/images/rec.png", "推荐", 1);
    ui->music->seticon(":/images/music.png", "⾳乐馆", 2);
    ui->audio->seticon(":/images/radio.png", "电台", 3);
    ui->like->seticon(":/images/like.png", "我喜欢", 4);
    ui->local->seticon(":/images/local.png", "本地下载", 5);
    ui->recent->seticon(":/images/recent.png", "最近播放", 6);
}

void QMusic::onBtFormClick(int id)
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
    // ui->stackedWidget->setCurrentIndex(0);
}

void QMusic::connectSignalAndSlot()
{
    // ⾃定义的btFrom按钮点击信号，当btForm点击后，设置对应的堆叠窗⼝
    connect(ui->rec, &BtForm::click, this, &QMusic::onBtFormClick);
    connect(ui->music, &BtForm::click, this, &QMusic::onBtFormClick);
    connect(ui->audio, &BtForm::click, this, &QMusic::onBtFormClick);
    connect(ui->like, &BtForm::click, this, &QMusic::onBtFormClick);
    connect(ui->local, &BtForm::click, this, &QMusic::onBtFormClick);
    connect(ui->recent, &BtForm::click, this, &QMusic::onBtFormClick);

    // 关联CommonPage发射的updateLikeMusic信号
    connect(ui->likePage, &CommonPage::updateLikeMusic, this, &QMusic::onUpdateLikeMusic);
    connect(ui->localPage, &CommonPage::updateLikeMusic, this, &QMusic::onUpdateLikeMusic);
    connect(ui->recentPage, &CommonPage::updateLikeMusic, this, &QMusic::onUpdateLikeMusic);
}

QJsonArray QMusic::randomPicture()
{
    // 1. 初始化推荐图片路径列表
    QVector<QString> vecImageName;
    vecImageName << "001.png" << "002.png" << "003.png" << "004.png" << "005.png"
                 << "006.png" << "007.png" << "008.png" << "009.png" << "010.png"
                 << "011.png" << "012.png" << "013.png" << "014.png" << "015.png"
                 << "016.png" << "017.png" << "018.png" << "019.png" << "020.png"
                 << "021.png" << "022.png" << "023.png" << "024.png" << "025.png"
                 << "026.png" << "027.png" << "028.png" << "029.png" << "030.png"
                 << "031.png" << "032.png" << "033.png" << "034.png" << "035.png"
                 << "036.png" << "037.png" << "038.png" << "039.png" << "040.png";

    // 2. 打乱图片顺序，实现每次打开都是“随机推荐”的效果
    std::random_shuffle(vecImageName.begin(), vecImageName.end());

    QJsonArray objArray;

    // 3. 循环构造 Json 对象并存入数组
    for (int i = 0; i < vecImageName.size(); ++i) {
        QJsonObject obj;

        // 拼接资源文件路径，例如：":/images/rec/001.png"
        obj.insert("path", ":/images/rec/" + vecImageName[i]);

        /* * 格式化推荐文本内容：
         * %1：占位符
         * arg(i, 3, 10, QChar('0')) 解析：
         * i：当前循环索引
         * 3：总共占 3 位
         * 10：使用 10 进制
         * QChar('0')：不足 3 位的前面补 '0'，例如：推荐-000, 推荐-001...
         */
        QString strText = QString("推荐-%1").arg(i, 3, 10, QChar('0'));
        obj.insert("text", strText);

        objArray.append(obj);
    }

    return objArray;
}

void QMusic::on_volume_clicked()
{
    // 由于弹出窗口默认在屏幕的 (0, 0) 位置显示，为了让它精准出现在按钮上方，我们必须重新计算它的全局坐标

    // 1. 获取音量按钮在屏幕（全局）中的坐标
    // 这样弹出的音量条才能精准定位在按钮附近，而不是主窗口左上角
    QPoint point = ui->volume->mapToGlobal(QPoint(0, 0));

    // 2. 计算 volumeTool 窗口的左上角位置
    // 逻辑：将弹出窗口的底部中心点对齐到按钮的左上角
    // 弹出窗口左上角 = 按钮全局坐标 - (窗口宽度的1/2, 窗口总高度)
    QPoint volumeLeftTop = point - QPoint(volumeTool->width() / 2, volumeTool->height());

    // 3. 微调窗口位置
    // 根据实际 UI 视觉效果进行手动偏移，确保遮盖或间距美观
    volumeLeftTop.setY(volumeLeftTop.y() + 30);
    volumeLeftTop.setX(volumeLeftTop.x() + 15);

    // 4. 执行移动并显示
    volumeTool->move(volumeLeftTop);
    volumeTool->show();
}

void QMusic::on_addLocal_clicked()
{
    // 1. 实例化文件对话框
    QFileDialog fileDialog(this);
    fileDialog.setWindowTitle("添加本地音乐");

    // 2. 设置对话框交互模式：文件打开模式
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);

    // 3. 设置文件选择模式：ExistingFiles 支持一次性选择多个已存在的文件
    fileDialog.setFileMode(QFileDialog::ExistingFiles);

    // 4. 配置 MIME 类型过滤器
    // 使用 MIME 类型（如 application/octet-stream）来限制用户选择的文件类型
    QStringList mimeList;
    mimeList << "application/octet-stream";
    fileDialog.setMimeTypeFilters(mimeList);

    // 5. 动态设置默认打开路径
    // 获取当前程序运行目录并向上跳转一级，定位到工程预设的音乐文件夹
    QDir dir(QDir::currentPath());
    dir.cdUp();
    QString musicPath = dir.path() + "/QMusic/musics/";
    fileDialog.setDirectory(musicPath);

    // 6. 以模态方式显示对话框并处理返回结果
    // exec() 会阻塞当前线程，直到用户点击“打开”或“取消”
    if (fileDialog.exec() == QFileDialog::Accepted)
    {
        // 跳转至“本地音乐”展示界面（StackedWidget 索引为 4）
        onBtFormClick(5);

        // 获取选中的文件 URL 列表
        QList<QUrl> urls = fileDialog.selectedUrls();

        // 拿到歌曲⽂件后，将歌曲⽂件交由musicList进⾏管理
        musicList.addMusicByUrl(urls);

        // 更新到本地⾳乐列表
        ui->localPage->reFresh(musicList);
    }
}

void QMusic::onUpdateLikeMusic(bool isLike, QString musicId)
{
    // 1. 核心数据同步：定位目标歌曲并更新内存对象信息
    // 在全局 musicList 中通过唯一 ID 查找对应的歌曲实体
    auto it = musicList.findMusicById(musicId);
    if (it != musicList.end())
    {
        // 更新 Music 对象的 isLike 状态
        // 这一步确保了 Model 层的数据是最新的
        it->setIsLike(isLike);
    }

    // 2. 视图级联刷新：通知所有相关页面更新显示
    // 由于“收藏”状态的改变会影响多个页面的列表内容或图标状态
    // 我们需要依次调用各个页面的 reFresh 接口进行全量或局部重绘
    ui->likePage->reFresh(musicList);   // 刷新“我喜欢”页面：实时增加或移除歌曲项
    ui->localPage->reFresh(musicList);  // 刷新“本地下载”页面：同步心形图标状态
    ui->recentPage->reFresh(musicList); // 刷新“最近播放”页面：同步收藏标记
}
