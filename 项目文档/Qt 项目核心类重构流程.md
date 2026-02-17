
很多开发者在新建 Qt 项目时，为了快速启动往往会直接跳过向导设置，一路狂点‘下一步’，导致主类名和源文件被默认命名为 `Widget` 和 `widget.cpp`。在项目初期这或许无伤大雅，但当代码积累到一定规模后，尝试手动逐一重命名就会面临极高的风险。在这个过程中，稍有不慎就可能漏改某个 `#include` 路径、忘记替换 `Ui::` 命名空间，或是破坏 `.pro` 工程文件中的关联配置，进而引发大量的编译报错，严重影响开发进度。

幸运的是，我们完全不必采取这种低效且易错的手工排查。Qt Creator 内部提供了一套极其强大的 **‘重构（Refactoring）’工具** 。借助这套工具，我们能够安全、平滑地完成项目核心类与文件的全局替换，彻底解决这类命名的历史遗留问题。

### **第一步：修改 UI 文件里的类名**

UI 文件里的大写名字，决定了 Qt 底层生成的 `Ui::` 命名空间里的类名。

1. 双击打开 **`widget.ui`** 进入设计师界面。 
2. 在右上角的 **“对象查看器（Object Inspector）”** 中，选中最顶层的那个根节点（默认名字就叫 `Widget`）。 
3. 在右下角的 **“属性编辑器（Property Editor）”** 中，找到 **`objectName`** 属性。
4. 将它的值从 `Widget` 改为我们想要的类名（比如：**`QQMusic`**）。   
5. 按 `Ctrl + S` 保存。

### 第二步：利用 Qt 的“重构快捷键”重命名 C++ 类

这一步会让 Qt 帮你自动去寻找所有用到这个类的地方（包括 `main.cpp` 里的调用），并统一替换。

1. 打开 **`widget.h`** 文件。
2. 找到声明类名的地方：`class Widget : public QWidget`。
3. 把光标放在 `Widget` 这个单词上。
4. **按下快捷键：`Ctrl + Shift + R`**（或者右键选择 `Refactor` -> `Rename Symbol Under Cursor`）。
5. 此时 `Widget` 单词会被一个红框框住。你直接输入新类名（比如：**`QQMusic`**）。
6. 输入完后，按下 **回车键（Enter）**。

### 第三步：重命名物理文件

类名改好后，我们需要修改左侧项目树里的文件名。

1. 在左侧项目树中，右键点击 **`widget.h`**，选择 **“重命名（Rename）...”**，改为我们的新项目文件名（比如：**`qqmusic.h`**）。
2. 同样的方法，重命名其他文件名。（比如：把 **`widget.cpp`** 重命名为 **`qqmusic.cpp`**，把 **`widget.ui`** 重命名为 **`qqmusic.ui`**）。

### 第四步：手动修改最后残留的 `Widget`

1. 打开 `qqmusic.h`，修改前置声明和指针：
```cpp
// 修改前：
namespace Ui { class Widget; }
class QQMusic : public QWidget {
// ...
private:
    Ui::Widget *ui;
};

// ========================
// 修改后：
namespace Ui { class QQMusic; }  // 换成 QQMusic
class QQMusic : public QWidget {
// ...
private:
    Ui::QQMusic *ui;             // 换成 QQMusic
};
```
2. 打开 `qqmusic.cpp`，修改头文件和构造函数：
```cpp
// 修改前：
#include "ui_widget.h"
QQMusic::QQMusic(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)

// ========================
// 修改后：
#include "ui_qqmusic.h"          // 换成 ui_qqmusic.h
QQMusic::QQMusic(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QQMusic)        // 换成 QQMusic
{
    ui->setupUi(this);
    initUI();
}
```

通过上述四步后就能完成项目的重构了。

### 补充：为什么`.ui` 文件已经改名了，但我们仍然能够使用 `#include "ui_widget.h"`？

当我们只完成前三步，不做第四步时，会发现程序依然能正常编译运行，甚至在 `qqmusic.h` 和 `qqmusic.cpp` 中继续包含 `#include "ui_widget.h"` 并使用 `Widget` 类也没有任何报错。按理说，既然文件已经改名，系统就不该认识 `ui_widget.h`，也不该认识 `Widget` 类了才对。

原因在于：在 Qt 中，我们写的代码（源码目录）和最终编译生成的文件（构建目录）是严格分开的。那个带着长长名字的 `build-QQMusic-Desktop_xxx-Debug` 文件夹，就是**构建目录**。

当我们第一次运行项目时，Qt 会在项目文件夹的同级目录下生成一个构建目录，并在这个目录下根据我们项目中的 `.ui` 文件**生成**一个 `.h` 文件。比如：我们原本的 `widget.ui` 就会生成 `ui_widget.h` 头文件。

当我们改名后，Qt 看到了新名字 **`qqmusic.ui`**，于是它立刻在构建目录下生成了一个全新的 `ui_qqmusic.h`。但是！**它并没有去主动删除原本就躺在构建目录里的那个旧的 `ui_widget.h`。** 此时，我们的构建目录里**同时存在** `ui_qqmusic.h` 和 `ui_widget.h` 两个文件。只要这两个文件都在，编译器找到哪个就能用哪个。

但这会带来一个极其严重的隐患：虽然现在程序能跑，但它其实处于一种危险的“假死”状态。

因为我们的 C++ 代码绑定的依然是那个**已经被遗弃的旧文件（`ui_widget.h`）**。 如果此时你去 Qt Designer 里打开 `qqmusic.ui`，往里面拖入一个新的按钮，保存后运行，你会发现：**我们的 C++ 代码根本找不到这个新按钮！** 因为新按钮的信息被写进了 `ui_qqmusic.h`，而我们的代码还在傻傻地读取那个永远不会再更新的旧文件。

解决办法就是执行上面的第四步。最好将改名前的生成的构建目录一并删除掉，这样如果包含错误的头文件(比如`ui_widget.h`）和使用错误的类（比如`Widget`）就会报错。