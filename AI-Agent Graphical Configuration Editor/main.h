#pragma once

#include "resource1.h"
#include "utils/libwinapi.hpp"
#include "utils/string_utils.hpp"
#include "utils/template_type_aliases.hpp"
#include <boost/log/attributes.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/stacktrace.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <locale>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>
namespace fs = std::filesystem;
using json = nlohmann::json;

#define STACKTRACE "--stacktrace: \n"\
    <<\
    boost::stacktrace::stacktrace()\
    <<\
    "----------------------\n"

const std::wstring mainClassName(L"AI-Agent-Graphical-Configuration-Editor");
const std::wstring mainWindowName(L"AI-Agent 图形化配置编辑器");

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

[[noreturn]] void unexpected_handler() noexcept {
    try
    {
        const char* file = "./未处理异常_STACKTRACE";
        BOOST_LOG_TRIVIAL(fatal) << "发生未处理异常, 程序即将终止, 未处理异常 STACKTRACE 文件路径: " << file << '\n' << STACKTRACE;
        std::ofstream ofs(file);
        ofs << "发生未处理异常, 程序即将终止" << '\n' << STACKTRACE;
        ofs.close();
        std::wstring msg = L"发生未处理异常, 程序即将终止, 未处理异常 STACKTRACE 文件路径: ";
        msg += utils::to_wstring(file);
        MessageBox(NULL, msg.c_str(), L"错误!",
            MB_ICONEXCLAMATION | MB_OK);
        std::abort(); // 终止程序
    }
    catch (const std::exception&)
    {
        MessageBox(NULL, L"处理\"未处理异常\"发生错误", L"错误!",
            MB_ICONEXCLAMATION | MB_OK);
        std::abort(); // 终止程序
    }
    catch (...)
    {
        MessageBox(NULL, L"处理\"未处理异常\"发生未知错误", L"错误!",
            MB_ICONEXCLAMATION | MB_OK);
        std::abort(); // 终止程序
    }
}


void init_logging() {
    boost::log::add_console_log(
        std::cout,
        boost::log::keywords::format = "[%TimeStamp%] [%ThreadID%] [%Severity%] %Message%"
    );
    boost::log::add_file_log(
        boost::log::keywords::file_name = "AI-Agent 图形化配置编辑器 %N.log",      // 日志文件名模式，%N 表示新的文件在达到一定大小时创建
        boost::log::keywords::rotation_size = 10 * 1024 * 1024, // 日志文件轮转大小，这里设置为 10 MB
        boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0), // 每天轮转
        boost::log::keywords::format = "[%TimeStamp%] [%ThreadID%] [%Severity%] %Message%", // 日志格式
        boost::log::keywords::auto_flush = true
    );
    boost::log::add_common_attributes();
}

std::optional<fs::path>get_open_file_path(HWND hwnd = NULL) {
    OPENFILENAME ofn;       // 公共对话框结构
    wchar_t szFile[260]{ L"setting.json" };       // 缓冲区存放文件名

    // 初始化OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    // 设置缓冲区大小
    //ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"JSON Files\0*.json\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE) {
        fs::path fp = szFile;
        return std::optional<fs::path>(fp);
    }
    else {
        return std::optional<fs::path>{};
    }
}

std::optional<fs::path>get_save_file_path(HWND hwnd = NULL) {
    OPENFILENAME ofn;
    wchar_t szFile[260]{ L"setting.json" };

    // 初始化OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    // 设置缓冲区大小
    //ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"JSON Files\0*.json\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn) == TRUE) {
        fs::path fp = szFile;
        if (!fp.has_extension()) {
            fp += ".json";
        }
        return std::optional<fs::path>(fp);
    }
    else {
        return std::optional<fs::path>{};
    }
}

constexpr auto 每个字符高几像素 = 30;
struct main_buffer {
    const int start_x = 5;
    const int start_y = 10;
    const int width = 620;
    const int height = 25;
    HMENU 临时hmenu = (HMENU)8000;

    //main window
    HINSTANCE main_hinstance;
    HWND main_hwnd;

    json config;            // json config data
    fs::path config_path;   // json config data

    bool 是否需要保存 = false;

    // 字符串
    utils::myedit 名称;
    utils::myedit AI人设;
    // 字符串数组
    utils::myedit 唤醒;
    utils::myedit 检测声音;
    utils::myedit 思考;
    utils::myedit 点击;
    // 数字
    utils::myedit 音量;
    utils::myedit 大小;
    utils::myedit 录音阈值;
    utils::myedit 停止录音时间;
    utils::myedit 聊天增强;
    utils::myedit 主动聊天;
    // 组合框
    utils::mycombobox<fs::path> 外观;
    utils::mycombobox<std::string> 模型;
    utils::mycombobox<std::string> 语言模型位置;
    utils::mycombobox<std::string> 回答模式;
    // 布尔
    utils::mycombobox<bool> 统筹控制智能家居;
    main_buffer(HWND hwnd, HINSTANCE hinstance) :
        main_hwnd(hwnd), main_hinstance(hinstance),
        // 字符串
        名称{ start_x, start_y + 0 * 每个字符高几像素, width, height, L"名称", hwnd, 临时hmenu++ },
        AI人设{ start_x, start_y + 1 * 每个字符高几像素, width, height, L"AI人设", hwnd, 临时hmenu++ },
        // 字符串数组
        唤醒{ start_x, start_y + 2 * 每个字符高几像素, width, height, L"唤醒文本", hwnd, 临时hmenu++ },
        检测声音{ start_x, start_y + 3 * 每个字符高几像素, width, height, L"监听文本", hwnd, 临时hmenu++ },
        思考{ start_x, start_y + 4 * 每个字符高几像素, width, height, L"思考文本", hwnd, 临时hmenu++ },
        点击{ start_x, start_y + 5 * 每个字符高几像素, width, height, L"点击文本", hwnd, 临时hmenu++ },
        // 数字
        音量{ start_x, start_y + 6 * 每个字符高几像素, width, height, L"音量", hwnd, 临时hmenu++, 
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL  | ES_NUMBER },
        大小{ start_x, start_y + 7 * 每个字符高几像素, width, height, L"大小", hwnd, 临时hmenu++, 
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER },
        录音阈值{ start_x, start_y + 8 * 每个字符高几像素, width, height, L"录音阈值", hwnd, 临时hmenu++, 
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER },
        /*y重新计数*/
        停止录音时间{ start_x * 2 + width * 1, start_y + 0 * 每个字符高几像素, width, height, L"等待停止录音时间", hwnd, 临时hmenu++, 
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER },
        聊天增强{ start_x * 2 + width * 1, start_y + 1 * 每个字符高几像素, width, height, L"聊天增强概率", hwnd, 临时hmenu++, 
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER },
        主动聊天{ start_x * 2 + width * 1, start_y + 2 * 每个字符高几像素, width, height, L"主动聊天概率", hwnd, 临时hmenu++, 
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER },
        // 组合框
        外观{ start_x * 2 + width * 1, start_y + 3 * 每个字符高几像素, width, height, L"角色外观", hwnd, 临时hmenu++,
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_SORT },
        模型{ start_x * 2 + width * 1, start_y + 4 * 每个字符高几像素, width, height, L"语言模型", hwnd, 临时hmenu++,
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_SORT },
        语言模型位置{ start_x * 2 + width * 1, start_y + 5 * 每个字符高几像素, width, height, L"语言模型位置", hwnd, 临时hmenu++ },
        回答模式{ start_x * 2 + width * 1, start_y + 6 * 每个字符高几像素, width, height, L"回答模式", hwnd, 临时hmenu++ },
        // 布尔
        统筹控制智能家居{ start_x * 2 + width * 1, start_y + 7 * 每个字符高几像素, width, height, L"是否启用统筹控制智能家居", hwnd, 临时hmenu++ }
    {
        constexpr auto 与提示间隔 = 500;
        CreateWindow(L"edit", 
            L"0. 在保存前请备份, 数据无价, 谨慎操作\n",
            WS_CHILD | WS_VISIBLE | ES_READONLY | WS_BORDER | ES_LEFT,
            start_x, start_y + 与提示间隔 + 0 * height, width * 2, height,
            hwnd, nullptr, GetModuleHandle(NULL), nullptr);
        CreateWindow(L"edit",
            L"1. 打开文件后短时间卡顿/未响应是正常的因为需要检索\"外观包\"(又称\"皮肤\")\n",
            WS_CHILD | WS_VISIBLE | ES_READONLY | WS_BORDER | ES_LEFT,
            start_x, start_y + 与提示间隔 + 1 * height, width * 2, height,
            hwnd, nullptr, GetModuleHandle(NULL), nullptr);
        CreateWindow(L"edit",
            L"2. 看不懂的选项不要改\n",
            WS_CHILD | WS_VISIBLE | ES_READONLY | WS_BORDER | ES_LEFT,
            start_x, start_y + 与提示间隔 + 2 * height, width * 2, height,
            hwnd, nullptr, GetModuleHandle(NULL), nullptr);
        CreateWindow(L"edit",
            L"3. 提交问题请将log文件一起扔给我\n",
            WS_CHILD | WS_VISIBLE | ES_READONLY | WS_BORDER | ES_LEFT,
            start_x, start_y + 与提示间隔 + 3 * height, width * 2, height,
            hwnd, nullptr, GetModuleHandle(NULL), nullptr);


        utils::myfont font{};
        
        // 我努力过了
        font.nHeight = 20;
        font.lpszFace = TEXT("微软雅黑");

        /*set font*/
        // 字符串
        名称.set_font(font);
        AI人设.set_font(font);
        // 字符串数组
        唤醒.set_font(font);
        检测声音.set_font(font);
        思考.set_font(font);
        点击.set_font(font);
        // 数字
        音量.set_font(font);
        大小.set_font(font);
        录音阈值.set_font(font);
        停止录音时间.set_font(font);
        聊天增强.set_font(font);
        主动聊天.set_font(font);
        // 组合框
        外观.set_font(font);
        模型.set_font(font);

        语言模型位置.set_font(font);
        语言模型位置.add(L"服务器", "server");
        语言模型位置.add(L"本地", "local");

        回答模式.set_font(font);
        回答模式.add(L"AI总是回答", "free");
        回答模式.add(L"呼唤AI后提问", "call");
        // 布尔
        统筹控制智能家居.set_font(font);
        统筹控制智能家居.add(L"启用", true);
        统筹控制智能家居.add(L"禁用", false);
    }
    ~main_buffer() {

    }
    void save(utils::in<fs::path> fp) {
        BOOST_LOG_TRIVIAL(trace) << "开始处理保存文件\n";

        BOOST_LOG_TRIVIAL(trace) << "正在从窗口中更新config对象\n";
        config["name"] = utils::to_string(名称.get());
        config["role_system"] = utils::to_string(AI人设.get());

        config["awake"] = json::parse(utils::to_string(唤醒.get()));
        config["listen"] = json::parse(utils::to_string(检测声音.get()));
        config["think"] = json::parse(utils::to_string(思考.get()));
        config["click"] = json::parse(utils::to_string(点击.get()));

        config["volume"] = utils::stoll(音量.get()) / 100;
        config["sizes"] = utils::stoll(大小.get());
        config["set_mindb"] = utils::stoll(录音阈值.get());
        config["wait_time"] = utils::stoll(停止录音时间.get());
        config["chat_random"] = utils::stoll(聊天增强.get());
        config["v_chat_random"] = utils::stoll(主动聊天.get());

        config["control_mijia"] = 统筹控制智能家居.get();
        config["mods"] = 回答模式.get();
        config["tts_location"] = 语言模型位置.get();

        if (!模型.empty())
        {
            config["tts_model"] = 模型.get();
        }

        if (!外观.empty())
        {
            config["gif"] = 外观.get().string();
        }

        BOOST_LOG_TRIVIAL(trace) << "保存到: " << fp.string() << '\n';
        std::ofstream ofs(fp);
        ofs << config.dump(4);

        BOOST_LOG_TRIVIAL(trace) << "更新保存内部状态\n";
        是否需要保存 = false;
        BOOST_LOG_TRIVIAL(trace) << "去除窗口标题后的'*'\n";
        SetWindowText(main_hwnd, mainWindowName.c_str());
    }
    void save_file() {
        save(config_path);
    }
    void save_as_file() {
        const auto fp = get_save_file_path(main_hwnd);
        if (!fp.has_value())
        {
            BOOST_LOG_TRIVIAL(warning) << "无法获取保存文件路径\n" << STACKTRACE;
            return;
        }
        save(fp.value());
    }
    void open_file(utils::in<fs::path> file_path) {
        BOOST_LOG_TRIVIAL(trace) << "开始处理打开文件\n";
        config_path = file_path;
        if (!fs::exists(file_path))
        {
            BOOST_LOG_TRIVIAL(error) << "无法读取主配置文件, 该文件不存在: " << file_path.string() << '\n' << STACKTRACE;
            return;
        }

        BOOST_LOG_TRIVIAL(trace) << "清空外观列表\n";
        外观.clear();
        BOOST_LOG_TRIVIAL(trace) << "清空模型列表\n";
        模型.clear();

        BOOST_LOG_TRIVIAL(trace) << "遍历配置文件目录查找外观文件夹, 并添加进外观列表中\n";
        for (const auto& i : 搜索外观文件夹(file_path.parent_path()))
        {
            外观.add(i.filename().wstring(), i);
        }
        const fs::path TTS配置 = file_path.parent_path() / "tools" / "GPT-SoVITS-Inference" / "trained" / "character_info.json";

        BOOST_LOG_TRIVIAL(trace) << "加载TTS配置, 并添加进模型列表中\n";
        if (!fs::exists(TTS配置))
        {
            BOOST_LOG_TRIVIAL(warning) << "无法读取TTS配置, 该文件夹不存在: " << TTS配置.string() << '\n' << STACKTRACE;
        }
        else {
            json TTS配置json;
            std::ifstream ifs(TTS配置);
            if (!ifs.is_open()) {
                BOOST_LOG_TRIVIAL(error) << "无法读取TTS配置, 文件流打开失败\n" << STACKTRACE;
            }
            else {
                ifs >> TTS配置json;
                auto& characters_and_emotions = TTS配置json.at("characters_and_emotions");
                for (auto it = characters_and_emotions.begin(); it != characters_and_emotions.end(); ++it) {
                    const auto& key = it.key();
                    模型.add(utils::to_wstring(key), key);
                }
            }
        }

        BOOST_LOG_TRIVIAL(trace) << "加载主配置文件, 并设置其他控件内容\n";
        {
            std::ifstream ifs(file_path);
            if (!ifs.is_open()) {
                BOOST_LOG_TRIVIAL(error) << "无法读取配置, 文件流打开失败\n" << STACKTRACE;
                return;
            }
            ifs >> config;
            名称.set(utils::to_wstring(config.at("name").get<std::string>()));
            AI人设.set(utils::to_wstring(config.at("role_system").get<std::string>()));

            唤醒.set(utils::to_wstring(config.at("awake").dump()));
            检测声音.set(utils::to_wstring(config.at("listen").dump()));
            思考.set(utils::to_wstring(config.at("think").dump()));
            点击.set(utils::to_wstring(config.at("click").dump()));

            音量.set(std::to_wstring(config.at("volume").get<float>() * 100));
            大小.set(std::to_wstring(config.at("sizes").get<int>()));
            录音阈值.set(std::to_wstring(config.at("set_mindb").get<int>()));
            停止录音时间.set(std::to_wstring(config.at("wait_time").get<int>()));
            聊天增强.set(std::to_wstring(config.at("chat_random").get<int>()));
            主动聊天.set(std::to_wstring(config.at("v_chat_random").get<int>()));

            if (config.at("control_mijia").get<bool>()) {
                统筹控制智能家居.set(L"启用");
            } else {
                统筹控制智能家居.set(L"禁用");
            }

            if (config.at("mods").get<std::string>() == "free") {
                回答模式.set(L"AI总是回答");
            } else if (config.at("mods").get<std::string>() == "call") {
                回答模式.set(L"呼唤AI后提问");
            }

            if (config.at("tts_location").get<std::string>() == "server") {
                语言模型位置.set(L"服务器");
            } else if (config.at("tts_location").get<std::string>() == "local") {
                语言模型位置.set(L"本地");
            }

            if (!模型.empty())
            {
                模型.set(utils::to_wstring(config.at("tts_model").get<std::string>()));
            }

            if (!外观.empty())
            {
                外观.set(fs::path(config.at("gif").get<std::string>()).filename().wstring());
            }
        }
    }
    void update() {

    }
    std::vector<fs::path> 搜索外观文件夹(utils::in<fs::path> path)
    {
        std::vector<fs::path> data{};
        std::vector<fs::path> out{};
        for (const auto& entry : fs::directory_iterator(path)) {
            // 检查当前条目是否为一个目录
            if (entry.is_directory()) {
                data.emplace_back(entry.path());
            }
        }
        while (!data.empty()) {
            const auto i = data.back();
            data.pop_back();

            bool 存在normal = false;
            bool 存在other = false;
            for (const auto& e : fs::directory_iterator(i))
            {
                if (e.is_directory()) {
                    if (e.path().filename() == "normal") {
                        存在normal = true;
                        if (存在other)
                        {
                            break;
                        }
                    }
                    if (e.path().filename() == "other") {
                        存在other = true;
                        if (存在normal)
                        {
                            break;
                        }
                    }
                    data.emplace_back(e.path());
                }
            }

            if (存在normal && 存在other)
            {
                out.emplace_back(i);
            }

        }
        return out;
    }
    void 内容被修改() {
        是否需要保存 = true;
        auto 新标题 = mainWindowName + L"*";
        SetWindowText(main_hwnd, 新标题.c_str());
    }
};