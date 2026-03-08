#ifndef LRCPAGE_H
#define LRCPAGE_H

#include <QWidget>
#include <QPropertyAnimation>

#include "QString"

// LRC行歌词结构
struct LyricLine
{
    qint64 time; // 时间
    QString text; // 歌词内容

    LyricLine(qint64 qtime, QString qtext)
        : time(qtime)
        , text(qtext)
    {}
};

namespace Ui {
class LrcPage;
}

class LrcPage : public QWidget
{
    Q_OBJECT

public:
    explicit LrcPage(QWidget *parent = nullptr);
    ~LrcPage();

    bool parseLrc(const QString& lrcPath);

    int getLineLrcWordIndex(qint64 pos);

    QString getLineLrcWord(qint64 index);

    void showLrcWord(int time);

    void setMusicName(const QString musicName) const;
    void setMusicSinger(const QString musicSinger) const;
private:
    Ui::LrcPage *ui;

    QPropertyAnimation* lrcAnimation;

    QVector<LyricLine> lrcLines; // 按照时间的先后次序保存每⾏歌词
};

#endif // LRCPAGE_H
