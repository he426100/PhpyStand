#include "stdafx.h"
#include <main/php.h>
#include <sapi/embed/php_embed.h>
#include <string>
#include <codecvt>
#include <locale>
#include <vector>
#include <shellapi.h>
#include <Windows.h>

// 链接必要的库
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

// 主函数入口点的选择
#ifdef PHPYSTAND_CONSOLE
int main(int argc, char *argv[])
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
    int embed_argc = 0;
    char **embed_argv = nullptr;

    // 隐藏控制台窗口（仅适用于 WinMain）
#ifndef PHPYSTAND_CONSOLE
    HWND hwnd = GetConsoleWindow();
    if (hwnd != NULL)
    {
        ShowWindow(hwnd, SW_HIDE);
    }
#endif

    // 获取命令行参数（仅适用于命令行模式）
#ifdef PHPYSTAND_CONSOLE
    embed_argc = argc;
    embed_argv = argv;
#else
    std::wstring command_line = GetCommandLineW();
    auto argv_wide = CommandLineToArgvW(command_line);
    if (argv_wide.empty())
    {
        return 1;
    }

    // 转换为 UTF-8 编码的 argv
    embed_argc = static_cast<int>(argv_wide.size());
    embed_argv = new char*[embed_argc + 1]; // 注意包含空终止符

    for (size_t i = 0; i < argv_wide.size(); ++i)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::string utf8_arg = converter.to_bytes(argv_wide[i]);
        embed_argv[i] = strdup(utf8_arg.c_str());
    }
    embed_argv[embed_argc] = nullptr; // 添加空终止符
#endif

    // 获取当前目录和模块文件名
    std::wstring current_dir = GetCurrentDirectoryW();
    std::wstring module_path = GetModuleFileNameW();

    // 构建脚本路径
    std::wstring script_path = module_path.substr(0, module_path.find_last_of(L'.'));
    script_path += L".int";

    // 设置环境变量
    SetEnvironmentVariableW(L"PHPYSTAND", module_path.c_str());
    SetEnvironmentVariableW(L"PHPYSTAND_SCRIPT", script_path.c_str());

    // 将宽字符串转换为 UTF-8 编码的 std::string
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string utf8_script = converter.to_bytes(script_path);

    // 模拟命令行环境（仅适用于 WinMain）
#ifndef PHPYSTAND_CONSOLE
    if (AttachConsole(ATTACH_PARENT_PROCESS))
    {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);

        // 设置环境变量模拟标准输入输出
        int fd_stdout = _fileno(stdout);
        if (fd_stdout >= 0)
        {
            char fn_stdout[32];
            sprintf(fn_stdout, "%d", fd_stdout);
            SetEnvironmentVariableA("PHPYSTAND_STDOUT", fn_stdout);
        }

        int fd_stdin = _fileno(stdin);
        if (fd_stdin >= 0)
        {
            char fn_stdin[32];
            sprintf(fn_stdin, "%d", fd_stdin);
            SetEnvironmentVariableA("PHPYSTAND_STDIN", fn_stdin);
        }
    }
#endif

    // 执行 PHP 代码，并处理可能的异常
    PHP_EMBED_START_BLOCK(embed_argc, embed_argv);

    zend_file_handle zfd;
    zend_stream_init_filename(&zfd, utf8_script.c_str());
    if (php_execute_script(&zfd) == FAILURE)
    {
        php_printf("Failed\n");
    }
    zend_destroy_file_handle(&zfd);

    PHP_EMBED_END_BLOCK();

    // 清理命令行参数（仅适用于 GUI 模式）
#ifndef PHPYSTAND_CONSOLE
    for (int i = 0; i < embed_argc; ++i)
    {
        free(embed_argv[i]);
    }
    delete[] embed_argv;
#endif

    return 0;
}