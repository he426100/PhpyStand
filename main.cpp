#include "stdafx.h"
#include <sapi/embed/php_embed.h>
#include <string>
#include <string.h>
#include <codecvt>
#include <locale>
#include <vector>
#include <shellapi.h>

// #pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
// #pragma comment(lib, "user32.lib")
#pragma comment(lib, "php8embed.lib")

int main(int argc, char *argv[])
{
    // HWND hwnd = GetConsoleWindow();
    // ShowWindow(hwnd, SW_HIDE);

    // std::wstring _cwd;
    std::wstring _home;
    std::wstring _phpystand;
    std::wstring _args;
    std::vector<std::wstring> _argv;

    LPWSTR *argvw;
	int _argc;
	_args = GetCommandLineW();
	argvw = CommandLineToArgvW(_args.c_str(), &_argc);
	if (argvw == NULL) {
		MessageBoxA(NULL, "Error in CommandLineToArgvW()", "ERROR", MB_OK);
		return false;
	}
	_argv.resize(_argc);
	for (int i = 0; i < _argc; i++) {
		_argv[i] = argvw[i];
	}
	LocalFree(argvw);

    wchar_t path[MAX_PATH + 10];
    GetCurrentDirectoryW(MAX_PATH + 1, path);
    // _cwd = path;

    GetModuleFileNameW(NULL, path, MAX_PATH + 1);
    _phpystand = path;

    SetEnvironmentVariableW(L"PHPYSTAND", _phpystand.c_str());

    int size = (int)_phpystand.size() - 1;
    for (; size >= 0; size--) {
        if (_phpystand[size] == L'.') break;
    }
    if (size < 0) size = (int)_phpystand.size();

    std::wstring main = _phpystand.substr(0, size);
    std::wstring _script = L"require '";
    _script += main + L".int';";

    SetEnvironmentVariableW(L"PHPYSTAND_SCRIPT", _script.c_str());

    // Convert the wide string to UTF-8 encoded std::string
    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
    std::string utf8_script = myconv.to_bytes(_script);

    // Execute PHP code
    PHP_EMBED_START_BLOCK(0, NULL);
    zend_eval_string(utf8_script.c_str(), NULL, "phpy app");
    PHP_EMBED_END_BLOCK();

    return 0;
}
