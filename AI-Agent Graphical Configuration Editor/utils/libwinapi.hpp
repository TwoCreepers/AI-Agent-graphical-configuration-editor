#pragma once

#include "template_type_aliases.hpp"
#include <string>
#include <memory>
#include <stdexcept>
#include <windowsx.h>
#include <Windows.h>
#include <unordered_map>

namespace utils {
	class lpsz {
	public:
		int m_width;
		int m_height;
		lpsz() noexcept :m_width(1280), m_height(720) {

		}
		lpsz(in<int> width) noexcept :m_width(width), m_height(width / 16 * 9) {

		}
		lpsz(in<int> width, in<int> height) noexcept :m_width(width), m_height(height) {

		}
		int width() const noexcept {
			return m_width;
		}
		int height() const noexcept {
			return m_height;
		}
		~lpsz() noexcept {

		}
		bool operator==(const lpsz& other) const noexcept {
			return m_width == other.m_width && m_height == other.m_height;
		}
	};
	class my_console {
		bool m_is_open = false;
	public:
		my_console(bool e = true) noexcept {
			if (e)
			{
				open();
			}
		}
		~my_console() noexcept {
			close();
		}
		bool is_open() const noexcept {
			return m_is_open;
		}
		bool is_close() const noexcept {
			return !m_is_open;
		}
		bool open() noexcept {
			if (m_is_open) {
				return false;
			}
			if (AllocConsole())
			{
				SetConsoleCP(65001);
				SetConsoleOutputCP(65001);
				auto mystdin = stdin;
				auto mystdout = stdout;
				auto mystderr = stderr;
				freopen_s(&mystdin, "CONIN$", "r", stdin);
				freopen_s(&mystdout, "CONOUT$", "w", stdout);
				freopen_s(&mystderr, "CONOUT$", "w", stderr);
				m_is_open = true;
			}
			else
			{
				m_is_open = false;
			}
			return m_is_open;
		}
		bool close() noexcept {
			if (m_is_open)
			{
				if (FreeConsole())
				{
					m_is_open = false;
				}
				else
				{
					m_is_open = true;
				}
			}
			return m_is_open;
		}
		bool set_consle_cp(in<UINT> wCodePageID) noexcept {
			if (SetConsoleCP(wCodePageID))
			{
				if (SetConsoleOutputCP(wCodePageID))
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
	};
	class myfont {
		HFONT m_hfont = nullptr;
	public:
		int nHeight = 24;										// 高度，以逻辑单位表示字体的高度
		int nWidth = 0;											// 宽度，以逻辑单位表示平均字符宽度，如果设置为0，则由高度决定
		int nEscapement = 0;									// 旋转角度，以十分之一度为单位
		int nOrientation = 0;									// 倾斜角度，以十分之一度为单位
		int fnWeight = FW_NORMAL;								// 字体粗细，如FW_BOLD
		DWORD fdwItalic = FALSE;								// 是否使用斜体，如ITALIC
		DWORD fdwUnderline = FALSE;								// 是否加下划线，如UNDERLINE
		DWORD fdwStrikeOut = FALSE;								// 是否加删除线，如STRIKEOUT
		DWORD fdwCharSet = ANSI_CHARSET;						// 字符集，如ANSI_CHARSET
		DWORD fdwOutputPrecision = OUT_DEFAULT_PRECIS;			// 输出精度，如OUT_DEFAULT_PRECIS
		DWORD fdwClipPrecision = CLIP_DEFAULT_PRECIS;			// 剪裁精度，如CLIP_DEFAULT_PRECIS
		DWORD fdwQuality = DEFAULT_QUALITY;						// 输出质量，如DEFAULT_QUALITY
		DWORD fdwPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;	// 间距和家族，如DEFAULT_PITCH | FF_DONTCARE
		LPCTSTR lpszFace = NULL;								// 字体名称字符串

		myfont()
		{

		}
		operator HFONT() {
			if (m_hfont != nullptr) {
				DeleteObject(m_hfont);
				m_hfont = nullptr;
			}
			m_hfont = CreateFont(
				nHeight,
				nWidth,
				nEscapement,
				nOrientation,
				fnWeight,
				fdwItalic,
				fdwUnderline,
				fdwStrikeOut,
				fdwCharSet,
				fdwOutputPrecision,
				fdwClipPrecision,
				fdwQuality,
				fdwPitchAndFamily,
				lpszFace
			);
			return m_hfont;
		}
		HFONT get() {
			if (m_hfont != nullptr) {
				DeleteObject(m_hfont);
				m_hfont = nullptr;
			}
			m_hfont = CreateFont(
				nHeight,
				nWidth,
				nEscapement,
				nOrientation,
				fnWeight,
				fdwItalic,
				fdwUnderline,
				fdwStrikeOut,
				fdwCharSet,
				fdwOutputPrecision,
				fdwClipPrecision,
				fdwQuality,
				fdwPitchAndFamily,
				lpszFace
			);
			return m_hfont;
		}
		~myfont()noexcept {
			if (m_hfont != nullptr) {
				DeleteObject(m_hfont);
				m_hfont = nullptr;
			}
		}
	};
	constexpr auto 每个字符长几像素 = 20;
	constexpr auto 每个字符高几像素 = 25;
	class myedit {
	public:
		int m_x;
		int m_y;
		int m_width;
		int m_height;
		HWND m_hedit;
		myedit() = delete;
		myedit(in<int> x, in<int> y,
			in<int> width, in<int> height,
			in<::std::wstring_view> wstr,
			in<HWND> hwnd, in<HMENU> hmenu,
			in<DWORD> dwstyle = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL) :
			m_x(x + (static_cast<int>(wstr.size()) * 每个字符长几像素)),
			m_y(y),
			m_width(width - (static_cast<int>(wstr.size()) * 每个字符长几像素)),
			m_height(height),
			m_hedit(CreateWindow(L"Edit", wstr.data(),
				dwstyle,
				x + (static_cast<int>(wstr.size()) * 每个字符长几像素), y, width - (static_cast<int>(wstr.size()) * 每个字符长几像素), height,
				hwnd, hmenu, GetModuleHandle(NULL), nullptr
			))
		{
			CreateWindow(L"Edit", wstr.data(),
				WS_CHILD | WS_VISIBLE | ES_READONLY | WS_BORDER | ES_LEFT,
				x, y, (static_cast<int>(wstr.size()) * 每个字符长几像素), height,
				hwnd, nullptr, GetModuleHandle(NULL), nullptr
			);
		}
		::std::wstring get() {
			const int textLength = Edit_GetTextLength(m_hedit) + 1;
			auto buffer = ::std::make_unique<wchar_t[]>(textLength);
			Edit_GetText(m_hedit, buffer.get(), textLength);
			return ::std::wstring(buffer.get());
		}
		void set(in<::std::wstring_view> wstr) {
			SetWindowText(m_hedit, wstr.data());
		}
		void set_font(myfont& font) {
			SendMessage(m_hedit, WM_SETFONT, (WPARAM)font.get(), TRUE);
		}
	};
	template <typename T>
	class mycombobox {
	public:
		int m_x;
		int m_y;
		int m_width;
		int m_height;
		HWND m_combobox;
		::std::unordered_map<::std::wstring, T> m_buffer;
		mycombobox() = delete;
		mycombobox(in<int> x, in<int> y,
			in<int> width, in<int> height,
			in<::std::wstring_view> wstr,
			in<HWND> hwnd, in<HMENU> hmenu,
			in<DWORD> dwstyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST) :
			m_x(x + (static_cast<int>(wstr.size()) * 每个字符长几像素)),
			m_y(y),
			m_width(width - (static_cast<int>(wstr.size()) * 每个字符长几像素)),
			m_height(height),
			m_combobox(CreateWindow(L"COMBOBOX", nullptr,
				WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
				x + (static_cast<int>(wstr.size()) * 每个字符长几像素), y, width - (static_cast<int>(wstr.size()) * 每个字符长几像素), height,
				hwnd, hmenu, GetModuleHandle(NULL), nullptr
			))
		{
			CreateWindow(L"Edit", wstr.data(),
				WS_CHILD | WS_VISIBLE | ES_READONLY | WS_BORDER | ES_LEFT,
				x, y, (static_cast<int>(wstr.size()) * 每个字符长几像素), height,
				hwnd, nullptr, GetModuleHandle(NULL), nullptr
			);
		}
		void add(in<::std::wstring> text, in<T> buffer) {
			if (m_buffer.find(text) != m_buffer.end())
			{
				return;
			}
			if (ComboBox_AddString(m_combobox, text.c_str()) == CB_ERR) {
				throw ::std::runtime_error("添加CB选项失败");
			}
			m_buffer[text] = buffer;
			MoveWindow(m_combobox, m_x, m_y, m_width, m_height + (static_cast<int>(m_buffer.size()) * 每个字符高几像素), TRUE);
		}
		void set(in<::std::wstring> text) {
			if (ComboBox_SelectString(m_combobox, NULL, text.c_str()) == CB_ERR) {
				throw ::std::runtime_error("设置CB失败");
			}
		}
		T get() {
			auto Index = ComboBox_GetCurSel(m_combobox);
			int nTextLen = ComboBox_GetLBTextLen(m_combobox, Index) + 1;
			auto buffer = std::make_unique<wchar_t[]>(static_cast<int>(nTextLen));
			if (ComboBox_GetLBText(m_combobox, Index, buffer.get()) == CB_ERR)
			{
				throw ::std::runtime_error("未找到CB索引");
				return T{};
			}
			return m_buffer.at(std::wstring(buffer.get()));
		}
		void set_font(myfont& font) {
			SendMessage(m_combobox, WM_SETFONT, (WPARAM)font.get(), TRUE);
		}
		void clear() {
			ComboBox_ResetContent(m_combobox);
			m_buffer.clear();
		}
		bool empty() {
			return m_buffer.empty();
		}
	};
}