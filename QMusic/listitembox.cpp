#include "listitembox.h"
#include "ui_listitembox.h"

ListItemBox::ListItemBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListItemBox),
    isLike(false)
{
    ui->setupUi(this);

    // likeBtn按钮连接其点击槽函数
    connect(ui->likeBtn, &QPushButton::clicked, this, &ListItemBox::onLikeBtnClicked);
}

ListItemBox::~ListItemBox()
{
    delete ui;
}

void ListItemBox::enterEvent(QEvent *event)
{
    (void)event;
    setStyleSheet("background-color:#EFEFEF");
}

void ListItemBox::leaveEvent(QEvent *event)
{
    (void)event;
    setStyleSheet("");
}

void ListItemBox::setMusicName(const QString& name)
{
    // 更新歌曲名标签
    ui->musicNameLabel->setText(name);
}

void ListItemBox::setSinger(const QString& singer)
{
    // 更新歌手名标签
    ui->musicSingerLabel->setText(singer);
}

void ListItemBox::setAlbumName(const QString& albumName)
{
    // 更新专辑名标签
    ui->albumNameLabel->setText(albumName);
}

void ListItemBox::setLikeIcon(bool like)
{
    // 同步内部状态变量
    isLike = like;

    // 根据布尔值切换“收藏”按钮的图标
    if (isLike)
    {
        // 状态为 true：显示高亮/实心心形图标
        ui->likeBtn->setIcon(QIcon(":/images/like_2.png"));
    }
    else
    {
        // 状态为 false：显示默认/空心心形图标
        ui->likeBtn->setIcon(QIcon(":/images/like_3.png"));
    }
}

void ListItemBox::onLikeBtnClicked()
{
    isLike = !isLike;
    setLikeIcon(isLike);
    emit setIsLike(isLike);
}
