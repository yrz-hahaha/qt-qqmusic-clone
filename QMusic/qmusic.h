#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>

#include "volumetool.h"
#include "musiclist.h"

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

private:
    Ui::QMusic *ui;

    bool isDragging;		// 记录鼠标是否处于按下并准备拖拽的状态
    QPoint dragPosition;	// 记录鼠标按下时，鼠标指针相对于窗口左上角的坐标偏移

    VolumeTool* volumeTool;

    MusicList musicList;   // 程序用来组织歌曲文件
};
#endif // WIDGET_H
