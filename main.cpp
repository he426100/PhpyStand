#include "stdafx.h"
#include <sapi/embed/php_embed.h>
#include <string>
#include <codecvt>
#include <locale>
#include <vector>
#include <shellapi.h>

// 链接必要的库
// #pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
// #pragma comment(lib, "user32.lib")
#pragma comment(lib, "php8embed.lib")

// 使用 wstring 而不是 LPWSTR，提高代码的可读性和一致性
std::vector<std::wstring> CommandLineToArgvW(const std::wstring &command_line)
{
    int argc;
    LPWSTR *argvw = CommandLineToArgvW(command_line.c_str(), &argc);
    if (!argvw)
    {
        MessageBoxA(NULL, "Error in CommandLineToArgvW()", "ERROR", MB_OK);
        return {};
    }

    std::vector<std::wstring> argv;
    for (int i = 0; i < argc; ++i)
    {
        argv.push_back(argvw[i]);
    }
    LocalFree(argvw);
    return argv;
}

// 获取当前目录和模块文件名
std::wstring GetCurrentDirectoryW()
{
    wchar_t path[MAX_PATH + 1];
    GetCurrentDirectoryW(MAX_PATH + 1, path);
    return path;
}

std::wstring GetModuleFileNameW()
{
    wchar_t path[MAX_PATH + 1];
    GetModuleFileNameW(NULL, path, MAX_PATH + 1);
    return path;
}

// 主函数
int main()
{
    // 隐藏控制台窗口
    // HWND hwnd = GetConsoleWindow();
    // ShowWindow(hwnd, SW_HIDE);

    // 获取命令行参数
    std::wstring command_line = GetCommandLineW();
    auto argv = CommandLineToArgvW(command_line);
    if (argv.empty())
        return 1; // 如果转换失败，则退出程序

    // 获取当前目录和模块文件名
    std::wstring current_dir = GetCurrentDirectoryW();
    std::wstring module_path = GetModuleFileNameW();

    // 构建脚本路径
    std::wstring script_path = module_path.substr(0, module_path.find_last_of(L'.'));
    script_path += L".int";

    // 构建要执行的 PHP 脚本命令
    std::wstring script_command = L"require '";
    script_command += script_path;
    script_command += L"';";

    // 设置环境变量
    SetEnvironmentVariableW(L"PHPYSTAND", module_path.c_str());
    SetEnvironmentVariableW(L"PHPYSTAND_SCRIPT", script_path.c_str());

    // 将宽字符串转换为 UTF-8 编码的 std::string
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string utf8_script = converter.to_bytes(script_command);

    // 执行 PHP 代码，并处理可能的异常
    PHP_EMBED_START_BLOCK(0, NULL);

    zend_try
    {
        zend_eval_string(utf8_script.c_str(), NULL, "phpy app");
    }
    zend_end_try();

    if (EG(exception))
    {
        wprintf(L"run %s error\n", script_path.c_str());
    }

    PHP_EMBED_END_BLOCK();

    return 0;
}
