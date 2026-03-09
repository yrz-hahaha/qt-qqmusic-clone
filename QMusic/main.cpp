#include "qmusic.h"

#include <QApplication>
#include <QMessageBox>
#include <QSharedMemory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 1. 为将要创建的共享内存定义一个唯一的标识符
    // 这个字符串（"QMusic"）在操作系统底层是全局唯一的
    QSharedMemory sharedMem("QMusic");

    // 2. 尝试将当前进程“挂载”到这个共享内存上
    // 如果 attach() 成功，说明已经有另一个 QMusic 进程创建并持有这块内存了
    if (sharedMem.attach())
    {
        // 弹出提示框告知用户
        QMessageBox::information(nullptr, "QQMusic", "QQMusic已经在运行...");

        // 直接退出当前进程，不进入主循环
        return 0;
    }

    // 3. 如果 attach() 失败，说明我是第一个运行的实例
    // 于是我正式“创建”这块共享内存，大小设为 1 字节即可（仅用于占位）
    sharedMem.create(1);

    // 4. 正常启动主界面
    QMusic w;
    w.show();

    return a.exec();
}
