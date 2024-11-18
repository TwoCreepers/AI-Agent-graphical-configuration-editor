#pragma once

#include "utils/template_type_aliases.hpp"
#include <string>
#include <Windows.h>

namespace utils {
	std::string to_string(in<std::u8string> u8str);
	std::string to_string(in<std::wstring> wstr);
	std::wstring to_wstring(in<std::string> str);

	long long stoll(in<std::wstring> wstr) {
		long long num = 0;
		for (const auto& wch : wstr) {
			if (wch <= L'9' && wch >= L'0') {
				num = num * 10 + (wch - L'0');
			}
		}
		return num;
	}

	class str_conv_ex : public std::runtime_error {
	private:
		std::string m_remark;
	public:
		explicit str_conv_ex(in<std::string> message)
			: std::runtime_error(message) {}
		explicit str_conv_ex(in<std::string> message, in<std::string> remark)
			: std::runtime_error(message), m_remark(remark){}
		std::string remark() const {
			return m_remark;
		}
		std::string get_name() const noexcept {
			return std::string("str_conv_ex: ") + m_remark;
		}
	};

	str_conv_ex get_last_error(in<std::string> remark) {
		DWORD error_code = GetLastError();
		LPVOID lpMsgBuf = nullptr;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			error_code,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);
		std::string msg = "Error Code: ";
		msg += std::to_string(error_code);
		msg += " - ";
		msg += to_string((LPCTSTR)lpMsgBuf);
		msg += " - Remark: ";
		msg += remark;
		LocalFree(lpMsgBuf);
		return str_conv_ex(msg, remark);
	}
	std::string to_string(in<std::u8string> u8str) {
		return std::string(u8str.begin(), u8str.end());
	}
	std::string to_string(in<std::wstring> wstr) {
		const int ResultSize = WideCharToMultiByte(CP_UTF8, NULL, wstr.c_str(), static_cast<int>(wstr.size()), nullptr, NULL, nullptr, nullptr) + 1;
		auto buffer = std::make_unique<char[]>(ResultSize);
		int result = WideCharToMultiByte(CP_UTF8, NULL, wstr.c_str(), static_cast<int>(wstr.size()), buffer.get(), ResultSize, nullptr, nullptr);
		if (result == 0) {
			throw get_last_error("wstr to str");
		}
		return std::string(buffer.get());
	}
	std::wstring to_wstring(in<std::string> str) {
		const int ResultSize = MultiByteToWideChar(CP_UTF8, NULL, str.c_str(), static_cast<int>(str.size()), nullptr, NULL) + 1;
		auto buffer = std::make_unique<wchar_t[]>(ResultSize);
		int result = MultiByteToWideChar(CP_UTF8, NULL, str.c_str(), static_cast<int>(str.size()), buffer.get(), ResultSize);
		if (result == 0) {
			throw get_last_error("str to wstr");
		}
		return std::wstring(buffer.get());
	}
}