#include "main.h"
#include "resource1.h"
#include "utils/libwinapi.hpp"
#include "utils/string_utils.hpp"


const utils::lpsz lpsz(1280);

HINSTANCE main_hinstance;
HWND main_hwnd;

main_buffer* main_ing = nullptr;

utils::my_console console{};

HMENU debug_sub_menu;

std::locale main_loc("zh_CH.UTF-8");

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	std::ios::sync_with_stdio(false);
	if (console.is_open()) {
		init_logging();
		BOOST_LOG_TRIVIAL(debug) << "日志已加载\n";
	}
	else {
		MessageBox(NULL, L"无法创建控制台窗口!", L"错误!",
			MB_ICONERROR | MB_OK);
		return 1;
	}

	BOOST_LOG_TRIVIAL(trace) << "设置未处理异常处理器\n";
	std::set_terminate(unexpected_handler);

	BOOST_LOG_TRIVIAL(trace) << "C++国际化库设置\n";
	std::locale::global(main_loc);

	main_hinstance = hInstance;
	WNDCLASSEX wc{ 0 };
	wc.lpszClassName = mainClassName.c_str();
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.cbSize = sizeof(WNDCLASSEX);
	BOOST_LOG_TRIVIAL(trace) << "窗口类已初始化\n";
	if (!RegisterClassEx(&wc)) {
		BOOST_LOG_TRIVIAL(fatal) << "无法注册窗口类\n" << STACKTRACE;
		MessageBox(NULL, L"无法注册窗口类", L"错误!",
			MB_ICONERROR | MB_OK);
		return 1;
	}

	BOOST_LOG_TRIVIAL(trace) << "窗口类已注册\n";

	main_hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		mainClassName.c_str(),
		mainWindowName.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, lpsz.width(), lpsz.height(),
		NULL, NULL, hInstance, NULL);

	if (main_hwnd == NULL) {
		BOOST_LOG_TRIVIAL(fatal) << "无法创建主窗口\n" << STACKTRACE;
		MessageBox(NULL, L"无法创建主窗口", L"错误!",
			MB_ICONERROR | MB_OK);
		return 1;
	}

	BOOST_LOG_TRIVIAL(trace) << "主窗口已创建\n";

	ShowWindow(main_hwnd, nShowCmd);
	UpdateWindow(main_hwnd);

	BOOST_LOG_TRIVIAL(trace) << "解析参数\n";
	int argc;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (argv == nullptr)
	{
		BOOST_LOG_TRIVIAL(error) << "参数转换失败\n";
	}
	else
	{
		if (argc > 1)
		{
			BOOST_LOG_TRIVIAL(trace) << "参数数大于1, 尝试转换参数指向文件, 并打开该文件\n";
			fs::path file_path = argv[1];
			if (main_ing == nullptr) {
				BOOST_LOG_TRIVIAL(warning) << "config对象未初始化, 无法处理打开文件请求\n";
			}
			else {
				try
				{
					main_ing->open_file(file_path);
				}
				catch (const std::exception& e)
				{
					BOOST_LOG_TRIVIAL(error) << "打开文件失败: " << e.what() << '\n';
				}
			}
		}
		LocalFree(argv);
	}

	BOOST_LOG_TRIVIAL(info) << "初始化完成, 开始消息循环\n";
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	BOOST_LOG_TRIVIAL(debug) << "主线程即将退出, 返回值: " << (int)msg.wParam << '\n';
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_CREATE:
	{
		BOOST_LOG_TRIVIAL(debug) << "创建窗口中, 初始化控件中\n";

		BOOST_LOG_TRIVIAL(trace) << "注册DPI感知\n";
		if (SetProcessDPIAware())
		{
			BOOST_LOG_TRIVIAL(trace) << "注册DPI感知成功\n";
		}
		else
		{
			BOOST_LOG_TRIVIAL(error) << "注册DPI感知失败, 缩放时可能会糊\n" << STACKTRACE;
		}

		BOOST_LOG_TRIVIAL(trace) << "加载菜单控件\n";
		HMENU hMenu = LoadMenu(main_hinstance, MAKEINTRESOURCE(IDR_MENU1));
		if (hMenu == nullptr)
		{
			BOOST_LOG_TRIVIAL(fatal) << "加载菜单控件失败\n" << STACKTRACE;
			MessageBox(NULL, L"加载菜单控件失败", L"错误!",
				MB_ICONERROR | MB_OK);
			DestroyWindow(hwnd);
			break;
		}

		debug_sub_menu = GetSubMenu(hMenu, 1);

		BOOST_LOG_TRIVIAL(trace) << "设置菜单控件\n";
		if (!SetMenu(hwnd, hMenu))
		{
			BOOST_LOG_TRIVIAL(fatal) << "设置菜单控件失败\n" << STACKTRACE;
			MessageBox(NULL, L"置菜单控件失败", L"错误!",
				MB_ICONERROR | MB_OK);
			DestroyWindow(hwnd);
			break;
		}

		

		// 初始化控件
		BOOST_LOG_TRIVIAL(debug) << "初始化其他控件\n";
		static main_buffer my_main_buffer(hwnd, main_hinstance);
		BOOST_LOG_TRIVIAL(debug) << "初始化config对象\n";
		main_ing = &my_main_buffer;

		break;
	}
	case WM_DESTROY:
	{
		BOOST_LOG_TRIVIAL(trace) << "接受到WM_DESTROY信号\n";
		/*
		BOOST_LOG_TRIVIAL(trace) << "COM库注销\n";
		CoUninitialize();
		*/
		BOOST_LOG_TRIVIAL(trace) << "关闭控制台窗口\n";
		FreeConsole();

		BOOST_LOG_TRIVIAL(trace) << "向主线程发送Quit信号\n";
		PostQuitMessage(0);
		break;
	}
	case WM_CLOSE:
	{
		BOOST_LOG_TRIVIAL(trace) << "接受到WM_CLOSE信号\n";
		if (main_ing->是否需要保存)
		{
			int msgboxID = MessageBox(
				hwnd,
				(LPCWSTR)L"您想要保存更改吗？",
				(LPCWSTR)L"提示",
				MB_ICONQUESTION | MB_YESNOCANCEL
			);
			switch (msgboxID)
			{
			case IDYES: // 用户选择了“是”
				BOOST_LOG_TRIVIAL(warning) << "用户选择了\"是\", 保存并销毁窗口\n";
				if (main_ing != nullptr)
				{
					try
					{
						main_ing->save_file();
					}
					catch (const std::exception& e)
					{
						BOOST_LOG_TRIVIAL(error) << "保存文件失败: " << e.what() << '\n' << STACKTRACE;
						std::wstring msg = L"保存文件失败: ";
						msg += utils::to_wstring(e.what());
						MessageBox(NULL, msg.c_str(), L"保存文件失败!",
							MB_ICONERROR | MB_OK);
					}
				}
				DestroyWindow(hwnd);
				break;

			case IDNO: // 用户选择了“否”
				BOOST_LOG_TRIVIAL(warning) << "用户选择了\"否\", 销毁窗口\n";
				DestroyWindow(hwnd);
				break;

			case IDCANCEL: // 用户选择了“取消”
				BOOST_LOG_TRIVIAL(warning) << "用户选择了\"取消\"\n";
				break;

			default:
				// 默认处理其他情况
				BOOST_LOG_TRIVIAL(warning) << "意外的提示返回值, 销毁窗口\n";
				break;
			}
		}
		else
		{
			DestroyWindow(hwnd);
		}
		break;
	}
	case WM_COMMAND:
	{
		auto ID = LOWORD(wParam);
		// 处理菜单命令
		switch (ID)
		{
		case MENU_OPEN_FILE:
		{
			const auto file = get_open_file_path();
			if (!file.has_value())
			{
				BOOST_LOG_TRIVIAL(warning) << "获取打开文件失败\n" << STACKTRACE;
				break;
			}
			BOOST_LOG_TRIVIAL(trace) << "获取打开文件成功: " << utils::to_string(file.value().u8string()) << '\n';
			try
			{
				main_ing->open_file(file.value());
			}
			catch (const std::exception& e)
			{
				BOOST_LOG_TRIVIAL(error) << "打开文件失败: " << e.what() << '\n' << STACKTRACE;
				std::wstring msg = L"打开文件失败: ";
				msg += utils::to_wstring(e.what());
				MessageBox(NULL, msg.c_str(), L"打开文件失败!",
					MB_ICONERROR | MB_OK);
			}
			break;
		}
		case MENU_SAVE:
		{
			try
			{
				main_ing->save_file();
			}
			catch (const std::exception& e)
			{
				BOOST_LOG_TRIVIAL(error) << "保存文件失败: " << e.what() << '\n' << STACKTRACE;
				std::wstring msg = L"保存文件失败: ";
				msg += utils::to_wstring(e.what());
				MessageBox(NULL, msg.c_str(), L"保存文件失败!",
					MB_ICONERROR | MB_OK);
			}
			break;
		}
		case MENU_SAVE_AS:
		{
			try
			{
				main_ing->save_as_file();
			}
			catch (const std::exception& e)
			{
				BOOST_LOG_TRIVIAL(error) << "保存文件失败: " << e.what() << '\n' << STACKTRACE;
				std::wstring msg = L"保存文件失败: ";
				msg += utils::to_wstring(e.what());
				MessageBox(NULL, msg.c_str(), L"保存文件失败!",
					MB_ICONERROR | MB_OK);
			}
			break;
		}
		case MENU_QUIT:
		{
			DestroyWindow(hwnd);
			break;
		}
		case 8000:
		{
			switch (HIWORD(wParam))
			{
			case EN_CHANGE: {
				if (main_ing == nullptr) { break; }
				main_ing->内容被修改();
				break;
			}
			default:
				break;
			}
			break;
		}
		case 8004:
		{
			switch (HIWORD(wParam))
			{
			case EN_CHANGE: {
				if (main_ing == nullptr) { break; }
				main_ing->内容被修改();
				break;
			}
			default:
				break;
			}
			break;
		}
		case 8008:
		{
			switch (HIWORD(wParam))
			{
			case EN_CHANGE: {
				if (main_ing == nullptr) { break; }
				main_ing->内容被修改();
				break;
			}
			default:
				break;
			}
			break;
		}
		case 8012:
		{
			switch (HIWORD(wParam))
			{
			case EN_CHANGE: {
				if (main_ing == nullptr) { break; }
				main_ing->内容被修改();
				break;
			}
			default:
				break;
			}
			break;
		}
		case 8016:
		{
			switch (HIWORD(wParam))
			{
			case EN_CHANGE: {
				if (main_ing == nullptr) { break; }
				main_ing->内容被修改();
				break;
			}
			default:
				break;
			}
			break;
		}
		case 8020:
		{
			switch (HIWORD(wParam))
			{
			case EN_CHANGE: {
				if (main_ing == nullptr) { break; }
				main_ing->内容被修改();
				break;
			}
			default:
				break;
			}
			break;
		}
		case 8024:// main_ing->音量
		{
			switch (HIWORD(wParam))
			{
			case EN_CHANGE:
			{				
				if (main_ing == nullptr) { break; }
				main_ing->内容被修改();
				auto num = utils::stoll(main_ing->音量.get());
				if (num > 100)
				{
					main_ing->音量.set(L"100");
				}
				break;
			}
			default:
			{
				break;
			}
			}

			break;
		}
		case 8028:// main_ing->大小
		{
			switch (HIWORD(wParam))
			{
			case EN_CHANGE:
			{
				if (main_ing == nullptr) { break; }
				main_ing->内容被修改();
				auto num = utils::stoll(main_ing->大小.get());
				if (num < 10)
				{
					main_ing->大小.set(L"10");
				}
				if (num > 1000)
				{
					main_ing->大小.set(L"1000");
				}
				break;
			}
			default:
			{
				break;
			}
			}

			break;
		}
		case 8032:
		{
			switch (HIWORD(wParam))
			{
			case EN_CHANGE: {
				if (main_ing == nullptr) { break; }
				main_ing->内容被修改();
				break;
			}
			default:
				break;
			}
			break;
		}
		case 8036:
		{
			switch (HIWORD(wParam))
			{
			case EN_CHANGE: {
				if (main_ing == nullptr) { break; }
				main_ing->内容被修改();
				break;
			}
			default:
				break;
			}
			break;
		}
		case 8040:
		{
			switch (HIWORD(wParam))
			{
			case EN_CHANGE: {
				if (main_ing == nullptr) { break; }
				main_ing->内容被修改();
				break;
			}
			default:
				break;
			}
			break;
		}
		case 8044:
		{
			switch (HIWORD(wParam))
			{
			case EN_CHANGE: {
				if (main_ing == nullptr) { break; }
				main_ing->内容被修改();
				break;
			}
			default:
				break;
			}
			break;
		}
		case 8048:
		{
			switch (HIWORD(wParam))
			{
			case CBN_SELCHANGE: {
				if (main_ing == nullptr) { break; }
				main_ing->内容被修改();
				break;
			}
			default:
				break;
			}
			break;
		}
		case 8052:
		{
			switch (HIWORD(wParam))
			{
			case CBN_SELCHANGE: {
				if (main_ing == nullptr) { break; }
				main_ing->内容被修改();
				break;
			}
			default:
				break;
			}
			break;
		}
		case 8056:
		{
			switch (HIWORD(wParam))
			{
			case CBN_SELCHANGE: {
				if (main_ing == nullptr) { break; }
				main_ing->内容被修改();
				break;
			}
			default:
				break;
			}
			break;
		}
		case 8060:
		{
			switch (HIWORD(wParam))
			{
			case CBN_SELCHANGE: {
				if (main_ing == nullptr) { break; }
				main_ing->内容被修改();
				break;
			}
			default:
				break;
			}
			break;
		}
		case 8064:
		{
			switch (HIWORD(wParam))
			{
			case CBN_SELCHANGE: {
				if (main_ing == nullptr) { break; }
				main_ing->内容被修改();
				break;
			}
			default:
				break;
			}
			break;
		}
		}
		break;
	}
	default:
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
		break;
	}
	}
	return 0;
}