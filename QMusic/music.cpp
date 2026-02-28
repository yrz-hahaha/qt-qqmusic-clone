#include "music.h"

#include <QMediaPlayer>
#include <QCoreApplication>
#include <QUuid>

Music::Music()
    : isLike(false)
    , isHistory(false)
{

}

Music::Music(const QUrl &url)
    : isLike(false)
    , isHistory(false)
    , musicUrl(url)
{
    musicId = QUuid::createUuid().toString();
    parseMediaMetaData();
}

void Music::setIsLike(bool isLike)
{
    this->isLike = isLike;
}

void Music::setIsHistory(bool isHistory)
{
    this->isHistory = isHistory;
}

void Music::setMusicName(const QString &musicName)
{
    // 使用 const 引用传递 QString，避免不必要的内存拷贝，提升性能
    this->musicName = musicName;
}

void Music::setSingerName(const QString &singerName)
{
    this->singerName = singerName;
}

void Music::setAlbumName(const QString &albumName)
{
    this->albumName = albumName;
}

void Music::setDuration(const qint64 duration)
{
    // qint64 是 Qt 跨平台的 64 位整型，常用于存储以毫秒为单位的音频时长
    this->duration = duration;
}

void Music::setMusicUrl(const QUrl &url)
{
    this->musicUrl = url;
}

void Music::setMusicId(const QString &musicId)
{
    this->musicId = musicId;
}

bool Music::getIsLike()
{
    return isLike;
}

bool Music::getIsHistory()
{
    return isHistory;
}

QString Music::getMusicName()
{
    return musicName;
}

QString Music::getSingerName()
{
    return singerName;
}

QString Music::getAlbumName()
{
    return albumName;
}

qint64 Music::getDuration()
{
    // 返回以毫秒为单位的音频时长
    return duration;
}

QUrl Music::getMusicUrl()
{
    return musicUrl;
}

QString Music::getMusicId()
{
    return musicId;
}

void Music::parseMediaMetaData()
{
    // 1. 实例化媒体播放对象
    // 解析元数据需要读取文件流，QMediaPlayer 是最便捷的封装类
    QMediaPlayer player;
    player.setMedia(musicUrl);

    // 2. 阻塞式解析与事件分发
    // 媒体元数据的解析是异步进行的，通常需要 IO 时间。
    // 在死循环中检查 isMetaDataAvailable()，直到元数据加载完成。
    while (!player.isMetaDataAvailable())
    {
        // 关键点：在循环等待期间手动调用进程事件处理函数。
        // 这样可以确保在解析音乐信息时，主界面（GUI）不会陷入“假死”状态，依然能响应鼠标和绘制事件。
        QCoreApplication::processEvents();
    }

    // 3. 提取元数据信息
    if (player.isMetaDataAvailable())
    {
        // 从多媒体容器中提取标签信息并转换为 QString
        musicName  = player.metaData("Title").toString();
        singerName = player.metaData("Author").toString();
        albumName  = player.metaData("AlbumTitle").toString();
        duration   = player.duration();

        QString fileName = musicUrl.fileName();
        int index = fileName.indexOf('-');


        if(musicName.isEmpty())
        {
            if(index != -1)
            {
                // "2002年的第一场雪 - 刀郎.mp3"
                musicName = fileName.mid(0, index).trimmed();
            }
            else
            {
                // "2002年的第一场雪.mp3"
                musicName = fileName.mid(0, fileName.indexOf('.')).trimmed();
            }
        }

        // 作者为空
        if(singerName.isEmpty())
        {
            if(index != -1)
            {
                singerName = fileName.mid(index+1, fileName.indexOf('.')-index-1).trimmed();
            }
            else
            {
                singerName = "未知歌手";
            }
        }

        // 专辑名为空
        if(albumName.isEmpty())
        {
            albumName = "未知专辑";
        }
        // 调试输出，确认解析结果
        qDebug() << musicName << " " << singerName << " " << albumName << " " << duration;
    }
}
