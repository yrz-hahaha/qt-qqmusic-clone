#ifndef MUSIC_H
#define MUSIC_H

#include <QUrl>
#include <QString>

class Music
{
public:
    Music();

    Music(const QUrl& url);

    void setIsLike(bool isLike);
    void setIsHistory(bool isHistory);
    void setMusicName(const QString& musicName);
    void setSingerName(const QString& singerName);
    void setAlbumName(const QString& albumName);
    void setDuration(const qint64 duration);
    void setMusicUrl(const QUrl& url);
    void setMusicId(const QString& musicId);

    bool getIsLike();
    bool getIsHistory();
    QString getMusicName();
    QString getSingerName();
    QString getAlbumName();
    qint64 getDuration();
    QUrl getMusicUrl();
    QString getMusicId();

private:
    void parseMediaMetaData();

private:
    bool isLike; // 标记⾳乐是否为我喜欢
    bool isHistory; // 标记⾳乐是否播放过

    // ⾳乐的基本信息有：歌曲名称、歌⼿名称、专辑名称、总时⻓
    QString musicName;
    QString singerName;
    QString albumName;
    qint64 duration; // ⾳乐的持续时⻓，即播放总的时⻓

    // 为了标记歌曲的唯⼀性，给歌曲设置id
    // 磁盘上的歌曲⽂件经常删除或者修改位置，导致播放时找不到⽂件，或者重复添加
    // 此处⽤musicId来维护播放列表中⾳乐的唯⼀性
    QString musicId;

    QUrl musicUrl; // ⾳乐在磁盘中的位置
};

#endif // MUSIC_H
