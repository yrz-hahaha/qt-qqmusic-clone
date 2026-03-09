#include "musiclist.h"

#include <QMimeDatabase>
#include <QMimeData>

MusicList::MusicList()
{

}

void MusicList::addMusicByUrl(const QList<QUrl> &urls)
{
    // 遍历传入的 URL 列表
    for (const auto &musicUrl : urls)
    {
        // 1. 获取文件的本地路径
        QString localFile = musicUrl.toLocalFile();

        // 2. 检测歌曲是否已经存在（去重逻辑）
        // 如果 musicPaths 集合中已经包含该路径，则跳过本次循环
        if (musicPaths.contains(localFile))
        {
            continue;
        }

        // 3. 歌曲还没有加载过，将其标记为已处理
        musicPaths.insert(localFile);

        // 4. 利用 Qt 的 MIME 数据库进行深度类型校验
        // 相比简单的后缀名判断，QMimeDatabase 能够识别文件真实的内容特征
        QMimeDatabase db;
        QMimeType mime = db.mimeTypeForFile(localFile);

        // 5. 严格筛选：仅接受 MPEG（MP3）和 FLAC 无损格式
        // 这一步是确保后续播放引擎 QMediaPlayer 能够正常工作的“防火墙”
        if (mime.name() != "audio/mpeg" && mime.name() != "audio/flac")
        {
            // 如果不是指定的音频格式，跳过该文件
            continue;
        }

        // 6. 数据落地：将合法的音乐文件存入列表容器（如 QList<QUrl>）
        musicList.push_back(musicUrl);
    }
}

iterator MusicList::begin()
{
    return musicList.begin();
}

iterator MusicList::end()
{
    return musicList.end();
}

iterator MusicList::findMusicById(const QString &musicId)
{
    // 1. 开启迭代器遍历流程
    // 从容器的起始位置 (begin) 开始，逐一向后移动，直到末尾 (end)
    for (iterator it = begin(); it != end(); ++it)
    {
        // 2. 匹配核心逻辑
        // 通过迭代器访问当前 Music 对象，并将其 ID 与目标 ID 进行比对
        if (it->getMusicId() == musicId)
        {
            // 3. 命中目标
            // 找到对应的歌曲，立即返回当前位置的迭代器
            return it;
        }
    }

    // 4. 兜底处理
    // 如果遍历完整个列表仍未找到匹配 ID，返回 end() 迭代器
    // 这是 C++ 标准库及 Qt 容器中表示“未找到”或“无效位置”的通用约定
    return end();
}
