#ifndef MUSICLIST_H
#define MUSICLIST_H

#include "music.h"

#include <QVector>

typedef typename QVector<Music>::iterator iterator;

class MusicList
{
public:
    MusicList();

    // 将QQMusic⻚⾯中读取到的⾳乐⽂件，检测是⾳乐⽂件后添加到musicList中
    void addMusicByUrl(const QList<QUrl>& urls);

    iterator begin();
    iterator end();

    iterator findMusicById(const QString& musicId);
private:
    QVector<Music> musicList;
};

#endif // MUSICLIST_H
