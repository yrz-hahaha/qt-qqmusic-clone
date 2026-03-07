#include "volumetool.h"
#include "ui_volumetool.h"

#include <QGraphicsDropShadowEffect>
#include <QPainter>

VolumeTool::VolumeTool(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VolumeTool)
    , isMuted(false) // 默认静⾳
    , volumeRatio(20) // 默认⾳量为20%
{
    ui->setupUi(this);

    // 1. 设置窗口标志（Window Flags）
    // 通过位掩码（Bitmask）配置窗口的交互行为与外观样式
    // Qt::Popup：将窗口设置为弹出式。核心特性是“失焦自隐”，即点击窗口外部区域时自动关闭（Hide）。
    // Qt::FramelessWindowHint：移除标题栏、缩放按钮等原生装饰。这是实现自定义圆角或异形 UI 的前提。
    // (注意：在 Windows 平台，开启透明属性后必须配合此标志，否则非控件区域将渲染为黑色背景)
    // Qt::NoDropShadowWindowHint：禁用 OS 层级的原生阴影，规避系统阴影与后续自定义阴影的渲染冲突。
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);

    // 2. 开启跨平台透明支持
    // 允许窗口背景渲染为透明，配合 QSS 即可实现圆角效果
    setAttribute(Qt::WA_TranslucentBackground);

    // 3. 配置自定义图形阴影（QGraphicsDropShadowEffect）
    // 比起系统阴影，自定义阴影可控性更高，能有效增强界面的视觉层级感
    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setOffset(0, 0);       // 偏移量为0，形成四周均匀发散的呼吸灯感
    shadowEffect->setColor("#646464");   // 柔和的深灰色
    shadowEffect->setBlurRadius(10);     // 模糊半径，数值越大阴影越虚化
    this->setGraphicsEffect(shadowEffect);

    // 4. 加载资源文件
    ui->silenceBtn->setIcon(QIcon(":/images/volumn.png"));

    // 5. 初始化音量条布局（默认音量 20%）
    // 核心逻辑：由于 Qt 坐标系 Y 轴向下增长，而音量条需“自底向上”绘制，需进行逆向几何映射。
    // 计算公式：y_pos = y_start + (总高度 - 当前进度高度)
    // 169 = 25 + (180 - 36)
    ui->outSlider->setGeometry(ui->outSlider->x(), 25 + 180 - 36, ui->outSlider->width(), 36);

    // 联动计算滑块位置：将滑块中心点与进度条顶部对齐
    ui->sliderBtn->move(ui->sliderBtn->x(), ui->outSlider->y() - ui->sliderBtn->height() / 2);

    // 同步 UI 文本显示
    ui->volumeRatio->setText("20%");

    // 关联静⾳按钮的信号槽
    connect(ui->silenceBtn, &QPushButton::clicked, this, &VolumeTool::onSilenceBtnClicked);

    // 安装事件过滤器
    ui->sliderBox->installEventFilter(this);
}

VolumeTool::~VolumeTool()
{
    delete ui;
}

void VolumeTool::paintEvent(QPaintEvent *event)
{
    (void)event; // 明确告诉编译器该参数暂不使用，避免警告

    // 1. 创建绘图对象：指定在当前窗口 (this) 上画图
    QPainter painter(this);

    // 2. 设置抗锯齿：让画出来的图形边缘更平滑，不带“毛边”
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 3. 设置画笔：设置为 NoPen 代表不需要边框线
    painter.setPen(Qt::NoPen);

    // 4. 设置画刷：填充颜色为白色
    painter.setBrush(Qt::white);

    // 5. 定义三角形的三个顶点 (QPolygon 多边形)
    QPolygon polygon;
    polygon.append(QPoint(30, 300)); // 左顶点
    polygon.append(QPoint(70, 300)); // 右顶点
    polygon.append(QPoint(50, 320)); // 下顶点（尖角向下）

    // 6. 绘制三角形
    painter.drawPolygon(polygon);
}

void VolumeTool::onSilenceBtnClicked()
{
    // 1. 状态翻转：在静音与非静音之间切换
    isMuted = !isMuted;

    // 2. UI 图标切换：根据状态显示不同的喇叭图标
    if (isMuted)
    {
        ui->silenceBtn->setIcon(QIcon(":/images/silent.png"));
    }
    else
    {
        ui->silenceBtn->setIcon(QIcon(":/images/volumn.png"));
    }

    // 3. 发射信号：通知主窗口或其他监听者更新播放器状态
    emit setSilence(isMuted);
}

bool VolumeTool::eventFilter(QObject *object, QEvent *event)
{
    // 1. 目标判定：仅处理针对 volumeBox（音量容器）的事件
    if (object == ui->sliderBox)
    {
        // 获取事件类型进行分支处理
        if (event->type() == QEvent::MouseButtonPress)
        {
            // A. 鼠标按下事件：实现“点哪跳哪”
            // 立即修改滑块按钮 (sliderBtn) 和外轮廓 (outLine) 的位置
            // 重新计算音量比例 (volumeRatio) 并调用 setVolume() 更新 UI 显示
            setVolume();
        }
        else if (event->type() == QEvent::MouseMove)
        {
            // B. 鼠标移动事件：实现滑动调节
            // 在拖拽过程中实时修改位置、计算比例并更新界面
            setVolume();

            // 关键点：实时向外部发射信号，确保播放器音量能够跟随手指即时变化
            emit setMusicVolume(volumeRatio);
        }
        else if (event->type() == QEvent::MouseButtonRelease)
        {
            // C. 鼠标释放事件：交互结束
            // 确保在松开鼠标时，最后一次确定的音量值被准确同步到播放器
            emit setMusicVolume(volumeRatio);
        }

        // 表示事件已被处理，不再向下传递
        return true;
    }

    // 2. 非目标对象：交给父类处理默认逻辑
    return QObject::eventFilter(object, event);
}

void VolumeTool::setVolume()
{
    // 1. 坐标转换：将鼠标在全局（屏幕）的位置转换为在 sliderBox 上的相对 y 坐标
    int height = ui->sliderBox->mapFromGlobal(QCursor::pos()).y();

    // 2. 边界限制：鼠标在 sliderBox 中可移动的有效 y 范围在 [25, 205] 之间
    // 这种“硬编码”限制确保了滑块不会飞出预设的槽位
    height = (height < 25) ? 25 : height;
    height = (height > 205) ? 205 : height;

    // 3. 调整滑块按钮位置：保持 x 轴不变，更新 y 轴，并做居中补偿处理
    ui->sliderBtn->move(ui->sliderBtn->x(), height - ui->sliderBtn->height() / 2);

    // 4. 更新进度条 (outLine) 的位置和大小
    // 随着滑块上移，进度条的高度会增加（205 - height）
    ui->outSlider->setGeometry(ui->outSlider->x(), height, ui->outSlider->width(), 205 - height);

    // 5. 计算音量比率
    // 180 是你定义的有效滑动总长度（205 - 25 = 180）
    // 通过当前进度条高度占总长度的比例计算出 0-100 的整数值
    volumeRatio = (int)((float)ui->outSlider->height() / 180 * 100);

    // 6. UI 反馈：将计算出的音量百分比显示在 Label 上
    ui->volumeRatio->setText(QString::number(volumeRatio) + "%");
}
