#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QPropertyAnimation>

#include "volumetool.h"
#include "musiclist.h"
#include "commonpage.h"
#include "lrcpage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class QMusic; }
QT_END_NAMESPACE

class QMusic : public QWidget
{
    Q_OBJECT

public:
    QMusic(QWidget *parent = nullptr);
    ~QMusic();

    void initUI();

private slots:
    void on_volume_clicked();

    void on_addLocal_clicked();


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

    // 图片路径和推荐文本准备
    QJsonArray randomPicture();

    void onUpdateLikeMusic(bool isLike, QString musicId); // 响应CommonPage发射

    void initPlayer(); // 初始化媒体对象

    void onPlayClicked(); // 播放按钮

    // 播放状态发⽣改变
    void onPlayStateChanged();

    void onPlayUpClicked(); // 上⼀曲
    void onPlayDownClicked(); // 下⼀曲

    void onPlaybackModeClicked(); // 播放模式设置

    void onPlaybackModeChanged(QMediaPlaylist::PlaybackMode playbackMode);

    void onPlayAll(PageType pageType);
    void playAllOfCommonPage(CommonPage* commonPage, int index);

    void playMusicByIndex(CommonPage* page, int index);

    void onCurrentIndexChanged(int index);

    void setMusicSilence(bool isMuted);

    void setPlayerVolume(int volume); // 设置⾳量⼤⼩

    // 歌曲持续时⻓改变时[歌曲切换]
    void onDurationChanged(qint64 duration);

    // 播放位置改变，即持续播放时间改变
    void onPositionChanged(qint64 duration);

    void onMusicSliderChanged(float value); // 进度条改变

    void onMetaDataAvailableChanged(bool available);

    void onLrcWordClicked();

private:
    Ui::QMusic *ui;

    bool isDragging;		// 记录鼠标是否处于按下并准备拖拽的状态
    QPoint dragPosition;	// 记录鼠标按下时，鼠标指针相对于窗口左上角的坐标偏移

    VolumeTool* volumeTool;

    MusicList musicList;   // 程序用来组织歌曲文件

    //播放器相关
    QMediaPlayer* player;

    // 要多⾸歌曲播放，以及更复杂的播放设置，需要给播放器设置媒体列表
    QMediaPlaylist* playList;

    CommonPage* curPage;

    qint64 totalTime;         // 记录媒体源的总时间
    int currentIndex;         // 记录当前正在播放歌曲在媒体列表中的索引

    LrcPage* lrcPage;
    QPropertyAnimation* lrcPageAnimal;
};
#endif // WIDGET_H
