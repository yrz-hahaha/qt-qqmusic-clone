#include "recbox.h"
#include "ui_recbox.h"

#include "recboxitem.h"
#include <QJsonObject>
#include <cmath>

RecBox::RecBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecBox),
    row(1),
    col(4)
{
    ui->setupUi(this);
}

RecBox::~RecBox()
{
    delete ui;
}

void RecBox::initRecBoxUi(QJsonArray data, int row)
{
    // 1. 判断布局模式
    if (2 == row)
    {
        // 如果是两行，说明当前 RecBox 是主界面上的 supplyMusicBox（音乐补给站）
        this->row = row;
        this->col = 8;
    } else
    {
        // 否则：只有一行，作为主界面上的 recMusicBox（每日推荐）
        // 隐藏底部容器，确保只显示一排
        ui->recListDown->hide();
    }

    // 2. 将传入的图片/文本数据保存到成员变量 imageList 中
    this->imageList = data;

    // 默认显⽰第0组
    currentIndex = 0;

    // 计算总共有⼏组图⽚，ceil表⽰向上取整
    count = ceil((double)imageList.size() / col);

    // 3. 调用函数开始创建具体的歌单项
    createRecBoxItem();
}

void RecBox::createRecBoxItem()
{
    // 溢出掉之前旧元素
    QList<RecBoxItem*> recUpList = ui->recListUp->findChildren<RecBoxItem*>();
    for(auto e : recUpList)
    {
        ui->recListUpHLayout->removeWidget(e);
        delete e;
    }

    QList<RecBoxItem*> recDownList = ui->recListDown->findChildren<RecBoxItem*>();
    for(auto e : recDownList)
    {
        ui->recListDownHLayout->removeWidget(e);
        delete e;
    }

    // 循环创建 RecBoxItem 对象，并根据逻辑分配到不同的布局中
    int index = 0;
    for (int i = currentIndex*col; (i < col + currentIndex*col) && (i < imageList.size()); ++i)
    {
        RecBoxItem* item = new RecBoxItem();

        // 1. 从数据列表中提取当前索引的 JSON 对象
        QJsonObject obj = imageList[i].toObject();

        // 2. 为自定义控件设置对应的音乐图片路径与文本
        item->setText(obj.value("text").toString());
        item->setImage(obj.value("path").toString());

        /* * 3. 布局分配逻辑：
         * - 情况 A：recMusicBox (每日推荐)
         * row 为 1，col 为 4。条件 (row == 2) 恒为假，
         * 因此所有 4 个元素都会通过 else 添加到 ui->recListUpHLayout 中。
         *
         * - 情况 B：supplyMusicBox (音乐补给站)
         * row 为 2，col 为 8。
         * 当 index < 4 (即 0, 1, 2, 3) 时：进入 else，添加到上层布局 (ui->recListUpHLayout)。
         * 当 index >= 4 (即 4, 5, 6, 7) 时：满足 if 条件，添加到下层布局 (ui->recListDownHLayout)。
         */
        if (index >= col / 2 && row == 2)
        {
            ui->recListDownHLayout->addWidget(item);
        }
        else
        {
            ui->recListUpHLayout->addWidget(item);
        }

        ++index;
    }
}

void RecBox::on_btUp_clicked()
{
    // 点击btUp按钮，显⽰前⼀组图⽚，如果已经是第⼀组图⽚，显⽰最后⼀组
    currentIndex--;
    if(currentIndex < 0)
    {
        currentIndex = count - 1;
    }
    createRecBoxItem();
}

void RecBox::on_btDown_clicked()
{
    // 点击btDown按钮，显⽰下⼀组图⽚，如果已经是最后⼀组图⽚，显⽰第0组
    currentIndex++;
    if(currentIndex >= count)
    {
        currentIndex = 0;
    }
    createRecBoxItem();
}
