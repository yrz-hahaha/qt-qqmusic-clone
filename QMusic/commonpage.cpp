#include "commonpage.h"
#include "ui_commonpage.h"
#include "listitembox.h"

CommonPage::CommonPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommonPage)
{
    ui->setupUi(this);

    ui->pageMusicList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // playAllBtn按钮的信号槽处理
    // 当播放按钮点击时，发射playAll信号，播放当前⻚⾯的所有歌曲
    // playAll信号交由QQMusic中处理
    connect(ui->playAllBtn, &QPushButton::clicked, this, [=](){
        emit playAll(pageType);
    });

    connect(ui->pageMusicList, &QListWidget::doubleClicked, this, [=](const QModelIndex &index){
        // ⿏标双击后，发射信号告诉QQMusic，播放this⻚⾯中共被双击的歌曲
        emit playMusicByIndex(this, index.row());
    });
}

CommonPage::~CommonPage()
{
    delete ui;
}

void CommonPage::setCommonPageUI(const QString &title, const QString &image)
{
    // 设置标题
    ui->pageTittle->setText(title);

    // 设置封⾯栏
    ui->musicImageLabel->setPixmap(QPixmap(image));
    ui->musicImageLabel->setScaledContents(true);

    // test
    //ListItemBox* listItemBox = new ListItemBox(this);
    //QListWidgetItem* listWidgetItem = new QListWidgetItem(ui->pageMusicList);
    //listWidgetItem->setSizeHint(QSize(ui->pageMusicList->width(), 45));
    //ui->pageMusicList->setItemWidget(listWidgetItem, listItemBox);
}

void CommonPage::setMusicListType(PageType pageType)
{
    this->pageType = pageType;
}

void CommonPage::reFresh(MusicList& musicList)
{
    ui->pageMusicList->clear();

    // 1. 数据过滤：从全局资源池中提取出属于当前页面的音乐 ID 列表
    addMusicToMusicPage(musicList);

    // 2. 渲染循环：遍历过滤后的 ID 列表，动态创建 UI 控件
    for (auto musicId : musicListOfPage)
    {
        // 从全局列表中查找对应的音乐实体对象
        auto it = musicList.findMusicById(musicId);
        if (it == musicList.end())
        {
            continue; // 如果 ID 失效，跳过该项，增强鲁棒性
        }

        // 3. 核心步骤：创建自定义列表项 UI (ListItemBox)
        // ListItemBox 是你设计的包含歌名、歌手、操作按钮的自定义 QWidget
        ListItemBox* listItemBox = new ListItemBox(ui->pageMusicList);
        listItemBox->setMusicName(it->getMusicName());
        listItemBox->setSinger(it->getSingerName());
        listItemBox->setAlbumName(it->getAlbumName());
        listItemBox->setLikeIcon(it->getIsLike());

        // 4. 将自定义控件嵌入 QListWidget
        // 先创建一个标准的占位项
        QListWidgetItem* listWidgetItem = new QListWidgetItem(ui->pageMusicList);

        // 设置占位项的高度，确保能完整容纳我们的自定义控件 (固定高度 45px)
        listWidgetItem->setSizeHint(QSize(ui->pageMusicList->width(), 45));

        // 将自定义的渲染控件关联到该行
        ui->pageMusicList->setItemWidget(listWidgetItem, listItemBox);

        // 接收ListItemBox发射的setLikeMusic信号
        connect(listItemBox, &ListItemBox::setIsLike, this, [=](bool isLike){
            emit updateLikeMusic(isLike, it->getMusicId());
        });
    }

    // 5. 视图更新：强制触发界面重绘，确保所有动态生成的控件立即显示
    repaint();
}

void CommonPage::addMusicToMusicPage(MusicList &musicList)
{ 
    // 1. 视图层重置
    // 在重新加载前清空当前页面的索引列表，确保 UI 显示与逻辑状态一致
    musicListOfPage.clear();

    // 2. 遍历全局音乐资源池
    // 使用 auto& 引用可以避免在循环中产生对象拷贝，提升处理海量歌单时的效率
    for (auto &music : musicList)
    {
        // 3. 根据页面类型执行过滤算法
        switch (pageType)
        {
        case LOCAL_PAGE:
            // 本地列表：无需过滤，直接获取所有音乐 ID
            musicListOfPage.push_back(music.getMusicId());
            break;

        case LIKE_PAGE:
            // 收藏列表：仅筛选 isLike 标记为 true 的数据项
            if (music.getIsLike())
            {
                musicListOfPage.push_back(music.getMusicId());
            }
            break;

        case HISTORY_PAGE:
            // 历史列表：仅筛选 isHistory 标记为 true 的数据项
            if (music.getIsHistory())
            {
                musicListOfPage.push_back(music.getMusicId());
            }
            break;

        default:
            // 默认兜底，防止出现未定义类型的渲染异常
            break;
        }
    }
}

void CommonPage::addMusicToPlayer(MusicList &musicList, QMediaPlaylist *playList)
{
    // 1. 播放列表预处理
    // 在添加新曲目之前，通常需要清空旧的播放列表，确保播放顺序与当前页面显示一致
    playList->clear();

    // 2. 遍历全局音乐资源池
    // 建议使用 const auto &music 以提升性能，避免在循环中产生对象拷贝
    for (auto &music : musicList)
    {
        // 3. 根据当前页面类型（PageType）进行逻辑过滤
        // 只有符合当前页面属性的音乐才会被加入到播放队列中
        switch (this->pageType)
        {
        case LOCAL_PAGE:
            // 本地页面：加载所有已识别的本地音乐
            playList->addMedia(music.getMusicUrl());
            break;

        case LIKE_PAGE:
            // “我喜欢”页面：仅加载标记为 Like 的歌曲
            if (music.getIsLike())
            {
                playList->addMedia(music.getMusicUrl());
            }
            break;

        case HISTORY_PAGE:
            // “最近播放”页面：仅加载有历史记录标记的歌曲
            if (music.getIsHistory())
            {
                playList->addMedia(music.getMusicUrl());
            }
            break;

        default:
            // 兜底处理
            break;
        }
    }
}

const QString &CommonPage::getMusicIdByIndex(int index) const
{
    if(index >= musicListOfPage.size())
    {
        qDebug()<<"⽆此歌曲";
        return "";
    }

    return musicListOfPage[index];
}
