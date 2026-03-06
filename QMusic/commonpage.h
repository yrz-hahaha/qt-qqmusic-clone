#ifndef COMMONPAGE_H
#define COMMONPAGE_H

#include "musiclist.h"

#include <QWidget>
#include <QMediaPlaylist>

namespace Ui {
class CommonPage;
}

enum PageType
{
    LIKE_PAGE, // 我喜欢⻚⾯
    LOCAL_PAGE, // 本地下载⻚⾯
    HISTORY_PAGE // 最近播放⻚⾯
};

class CommonPage : public QWidget
{
    Q_OBJECT

public:
    explicit CommonPage(QWidget *parent = nullptr);
    ~CommonPage();

    void setCommonPageUI(const QString &title, const QString &image);

    void setMusicListType(PageType pageType);

    void reFresh(MusicList& musicList);

    void addMusicToPlayer(MusicList &musicList, QMediaPlaylist *playList);

    const QString& getMusicIdByIndex(int index) const;

signals:
    void updateLikeMusic(bool isLike, QString musicId);

    // 该信号由QQMusic处理
    void playAll(PageType pageType);

    void playMusicByIndex(CommonPage*, int);

private:
    void addMusicToMusicPage(MusicList &musicList);

private:
    Ui::CommonPage *ui;

    // 歌单列表
    QVector<QString> musicListOfPage; // 具体某个⻚⾯的⾳乐，将来只需要存储⾳乐的 id 即可
    PageType pageType; // 标记属于likePage、localPage、recentPage哪个⻚⾯
};

#endif // COMMONPAGE_H
