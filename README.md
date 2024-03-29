# PhpyStand
[swoole/phpy](https://github.com/swoole/phpy) 独立部署环境，这个项目是搭配 [swoole/phpy](https://github.com/swoole/phpy) 使用的，参考了[PyStand](https://github.com/skywind3000/PyStand)

## 功能说明

- Windows 下独立 [swoole/phpy](https://github.com/swoole/phpy) 环境的启动器。
- 自动加载 `PhpyStand.exe` 同级目录下面 的 Embed PHP 和 Embedded Python。
- 自动启动 `PhpyStand.exe` 同级目录下面的 `PhpyStand.int` 程序（PHP 代码）。
- 如果改名，会加载对应名称的 `.int` 文件，比如改为 `MyDemo.exe` 就会加载 `MyDemo.int`。
- 窗口程序，无 Console，但是如果在 cmd.exe 内运行，可以看到 print 的内容。


## 编译指南
- 按 [build-on-windows](https://github.com/swoole/phpy/blob/main/docs/cn/php/build-on-windows.md) 编译，编译配置增加`--enbale-embed`
- 复制 `php-8.1.27-src\x64\Release` 中的 `php-8.1.27-devel-vs16-x64` 和 `php-8.1.27` 到项目的 `php` 目录下
- 复制 `php8embed.lib` 到项目的 `build` 目录下
- `mkdir build && cd build && cmake .. && cmake --build .`


## 使用方式

- 用 CMake 生成 `PhpyStand.exe`。
- 下载 [swoole/phpy](https://github.com/swoole/phpy) windows包 和 Python Embedded 版本，用embed python代替其中的python文件，放到 `PhpyStand.exe` 所在目录内。
- 在 `PhpyStand.exe` 所在目录创建 PHP 源代码 PhpyStand.int。
- 双击 `PhpyStand.exe` 就会运行 `PhpyStand.int` 里的代码。
- 可以编译成命令行版方便调试，CMake 的时候加 `-DPHPYSTAND_CONSOLE=ON` 即可。
- 可用 `Enigma Virtual Box` 打包成单个exe文件

### 更换图标

可以替换 appicon.ico 文件并重新编译 PyStand.exe ，或者使用 Resource Hacker 直接
替换 Release 内下载的 PyStand.exe 文件的程序图标。

## 未解决问题
- 无法像 `PyStand` 那样把所有 python 文件放在 `runtime` 目录下 
- 暂不支持 `PyStand` 那样的环境检测，如果目录下无 `phpy` 环境会无法运行

## 使用例子
1. console+phar
```php
<?php

define('STDOUT', fopen('php://stdout', 'wb'));

try {
    PyCore::import('加载中...');
    require 'app.phar';
} finally {
    fclose(STDOUT);
}
```

2. gui [手把手教你玩qt](docs/%E6%89%8B%E6%8A%8A%E6%89%8B%E6%95%99%E4%BD%A0%E7%8E%A9qt.md)
```php
<?php

PyCore::import('site')->addsitedir(__DIR__ . '/Lib/site-packages');
$sys = PyCore::import('sys');
$sys->path->append(__DIR__);

$QtWidgets = PyCore::import('PySide6.QtWidgets');
$MainWindow = PyCore::import('mainwindow')->Ui_MainWindow;

$app = $QtWidgets->QApplication($sys->argv);
$window = $QtWidgets->QMainWindow();

// php中无法继承python的类
$ui = $MainWindow();
$ui->setupUi($window);

$window->show();
exit($app->exec());
```

3. MessageBox
```php
<?php

$ffi = FFI::cdef('
    typedef char TCHAR;
    typedef unsigned int UINT;
    typedef const TCHAR *LPCTSTR;
    typedef LPCTSTR LPCSTR;
    typedef UINT HWND;

    int MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);
', 'user32.dll');

$ffi->MessageBoxA(null, 'Message', 'Hello, World!', 0);
```

## 参考文档
- [https://github.com/skywind3000/PyStand](https://github.com/skywind3000/PyStand)  
- [https://github.com/swoole/phpy](https://github.com/swoole/phpy)  
- [https://github.com/php/php-src/tree/master/sapi/embed](https://github.com/php/php-src/tree/master/sapi/embed)  
- [https://github.com/llgoer/php-embed-windows/](https://github.com/llgoer/php-embed-windows/)  
