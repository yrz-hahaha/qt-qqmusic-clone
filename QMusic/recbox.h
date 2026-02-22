#ifndef RECBOX_H
#define RECBOX_H

#include <QWidget>
#include <QJsonArray>

namespace Ui {
class RecBox;
}

class RecBox : public QWidget
{
    Q_OBJECT

public:
    explicit RecBox(QWidget *parent = nullptr);
    ~RecBox();

    // 初始化并向 RecBox 中添加元素
    void initRecBoxUi(QJsonArray data, int row);
    void createRecBoxItem();

private slots:
    // 翻页按钮的槽函数
    void on_btUp_clicked();
    void on_btDown_clicked();

private:
    Ui::RecBox *ui;

    int row; // 记录当前RecBox实际总⾏数
    int col; // 记录当前RecBox实际每⾏有⼏个元素
    QJsonArray imageList; // 保存界⾯上的图⽚, ⾥⾯实际为key、value键值对

    int currentIndex; // 当前显示到了第几页（从 0 开始计）
    int count; // 标记imageList中元素按照col分组总数
};

#endif // RECBOX_H
