#include "commonpage.h"
#include "ui_commonpage.h"
#include "listitembox.h"

CommonPage::CommonPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommonPage)
{
    ui->setupUi(this);
}

CommonPage::~CommonPage()
{
    delete ui;
}

void CommonPage::setCommonPageUI(const QString &title, const QString &image)
{
    // 设置标题
    ui->pageTittle->setText(title);

    // 设置封⾯栏
    ui->musicImageLabel->setPixmap(QPixmap(image));
    ui->musicImageLabel->setScaledContents(true);

    // test
    //ListItemBox* listItemBox = new ListItemBox(this);
    //QListWidgetItem* listWidgetItem = new QListWidgetItem(ui->pageMusicList);
    //listWidgetItem->setSizeHint(QSize(ui->pageMusicList->width(), 45));
    //ui->pageMusicList->setItemWidget(listWidgetItem, listItemBox);
}
