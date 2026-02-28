#ifndef LISTITEMBOX_H
#define LISTITEMBOX_H

#include <QWidget>

namespace Ui {
class ListItemBox;
}

class ListItemBox : public QWidget
{
    Q_OBJECT

public:
    explicit ListItemBox(QWidget *parent = nullptr);
    ~ListItemBox();

    void setMusicName(const QString& name);
    void setSinger(const QString& singer);
    void setAlbumName(const QString& albumName);
    void setLikeIcon(bool like);

    void onLikeBtnClicked(); // 按钮点击槽函数

signals:
    void setIsLike(bool); // 通知更新歌曲数据信号

protected:
    // 重写鼠标进入和离开事件处理函数
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

private:
    Ui::ListItemBox *ui;

    bool isLike;
};

#endif // LISTITEMBOX_H
