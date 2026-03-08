#include "lrcpage.h"
#include "ui_lrcpage.h"

#include <QFile>
#include <QDebug>

LrcPage::LrcPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LrcPage)
{
    ui->setupUi(this);

    setWindowFlag(Qt::FramelessWindowHint);

    // 添加动画效果
    lrcAnimation = new QPropertyAnimation(this, "geometry", this);
    lrcAnimation->setDuration(250);
    lrcAnimation->setStartValue(QRect(10, 10, width(), height()));
    lrcAnimation->setEndValue(QRect(10, 10 + height(), width(), height()));

    // 点击设置下拉按钮时开启动画
    connect(ui->hideBtn, &QPushButton::clicked, this, [=]{
        lrcAnimation->start();
    });

    // 动画结束时，将窗⼝隐藏
    connect(lrcAnimation, &QPropertyAnimation::finished, this, [=]{
        hide();
    });

    ui->hideBtn->setIcon(QIcon(":/images/xiala.png"));

    showLrcWord(-1);
}

LrcPage::~LrcPage()
{
    delete ui;
}

bool LrcPage::parseLrc(const QString& lrcPath)
{
    lrcLines.clear();

    // 1. 打开歌词文件
    QFile lrcFile(lrcPath);
    if (!lrcFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "无法打开文件:" << lrcPath;
        return false;
    }

    // 2. 逐行读取并解析
    while (!lrcFile.atEnd())
    {
        // 读取一行数据，转换成 QString 处理更方便
        QString lrcLineRaw = QString::fromUtf8(lrcFile.readLine());
        if (lrcLineRaw.trimmed().isEmpty()) continue;

        // 查找时间标签的边界 [00:17.94]
        int left  = lrcLineRaw.indexOf('[');
        int right = lrcLineRaw.indexOf(']');

        if (left == -1 || right == -1) continue; // 跳过不符合格式的行

        // 提取时间字符串内容 (例如 "00:17.94")
        QString timeStr = lrcLineRaw.mid(left + 1, right - left - 1);

        qint64 lineTime = 0;
        int start = 0;
        int end = 0;

        // --- 解析分钟 ---
        end = timeStr.indexOf(':');
        lineTime += timeStr.mid(start, end - start).toInt() * 60 * 1000;

        // --- 解析秒 ---
        start = end + 1;
        end = timeStr.indexOf('.', start);
        lineTime += timeStr.mid(start, end - start).toInt() * 1000;

        // --- 解析毫秒 ---
        start = end + 1;
        // 注意：有的歌词是 [00:12.34]，有的是 [00:12.345]
        lineTime += timeStr.mid(start).toInt();

        // 3. 解析歌词文本
        // 取 ']' 之后的所有内容并去除首尾空格
        QString wordText = lrcLineRaw.mid(right + 1).trimmed();

        // 存入容器
        lrcLines.push_back(LyricLine(lineTime, wordText));
    }

    // 4. 测试验证输出
    for (const auto& line : lrcLines)
    {
        qDebug() << "时间(ms):" << line.time << " 内容:" << line.text;
    }

    return true;
}

int LrcPage::getLineLrcWordIndex(qint64 pos)
{
    // 1. 如果歌词列表为空，直接返回 -1
    if (lrcLines.isEmpty())
    {
        return -1;
    }

    // 2. 特殊情况：如果当前时间还没到第一句歌词的时间
    if (lrcLines[0].time > pos)
    {
        return 0;
    }

    // 3. 核心逻辑：通过时间区间对比，获取当前在哪一行
    // 从第二行开始遍历，对比 [i-1].time 和 [i].time
    for (int i = 1; i < lrcLines.size(); ++i)
    {
        if (pos >= lrcLines[i - 1].time && pos < lrcLines[i].time)
        {
            return i - 1;
        }
    }

    // 4. 如果遍历结束仍未找到（说明进度超过了最后一句歌词的时间）
    // 返回最后一行下标
    return lrcLines.size() - 1;
}

QString LrcPage::getLineLrcWord(qint64 index)
{
    // 1. 边界检查：防止索引越界导致程序崩溃
    // 检查索引是否小于 0 或者超过了歌词列表的最大长度
    if (index < 0 || index >= lrcLines.size())
    {
        return ""; // 如果越界，返回空字符串
    }

    // 2. 返回对应索引的歌词文本
    return lrcLines[index].text;
}

void LrcPage::showLrcWord(int time)
{
    // 1. 根据当前播放时间获取对应的歌词索引
    int index = getLineLrcWordIndex(time);

    // 2. 如果返回 -1，说明歌词库为空或解析失败
    if (-1 == index)
    {
        ui->line1->setText("");
        ui->line2->setText("");
        ui->line3->setText("");
        ui->lineCenter->setText("当前歌曲无歌词");
        ui->line4->setText("");
        ui->line5->setText("");
        ui->line6->setText("");
    }
    else
    {
        // 3. 核心显示逻辑：以 index 为中心，向上取 3 行，向下取 3 行
        // 这里巧妙地利用了之前写的 getLineLrcWord(index) 函数
        // 因为该函数内部有边界检查（index < 0 会返回空串），所以这里不需要手动判断越界
        ui->line1->setText(getLineLrcWord(index - 3));
        ui->line2->setText(getLineLrcWord(index - 2));
        ui->line3->setText(getLineLrcWord(index - 1));

        ui->lineCenter->setText(getLineLrcWord(index)); // 当前正在唱的这一句

        ui->line4->setText(getLineLrcWord(index + 1));
        ui->line5->setText(getLineLrcWord(index + 2));
        ui->line6->setText(getLineLrcWord(index + 3));
    }
}

void LrcPage::setMusicName(const QString musicName) const
{
    ui->musicName->setText(musicName);
}

void LrcPage::setMusicSinger(const QString musicSinger) const
{
    ui->musicSinger->setText(musicSinger);
}
