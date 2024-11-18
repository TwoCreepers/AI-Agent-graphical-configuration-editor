# AI-Agent Graphical Configuration Editor  

这是一个名为“AI-Agent”的软件的图形化配置编辑器，不过该软件及其项目尚未公开，所以我无法提供仓库的url。  

## 功能  

- 编辑AI-Agent配置文件的键值，包括：  
  - name -- 名称(字符串)
  - role_system -- AI人设(字符串)
  - awake -- 唤醒文本(字符串数组)
  - listen -- 监听文本(字符串数组)
  - think -- 思考文本(字符串数组)
  - click -- 点击文本(字符串数组)
  - volume -- 音量(整型, 0-100)
  - sizes -- 大小(整型, 10-1000)
  - set_mindb -- 录音阈值(整型)
  - wait_time -- 等待停止录音时间(整型, 单位为: 秒)
  - chat_random -- 聊天增强概率(整数, 0~INT_MAX)
  - v_chat_random -- 主动聊天概率(整数, 0~INT_MAX)
  - gif -- 角色外观(字符串数组选项)
  - tts_model -- 语言模型(字符串数组选项)
  - tts_location -- 语言模型位置(字符串数组选项)
  - mods -- 回答模式(字符串数组选项)
  - control_mijia -- 是否启用统筹控制智能家居(布尔值)

## 平台仅限Windows  

该软件仅限于Windows平台，因为它完全采用win32 API进行开发。  

## 非Windows SDK依赖  

- [nlohmann-json](https://github.com/nlohmann/json)  
- [boost.log](https://github.com/boostorg/log)  
- [boost.stacktrace](https://github.com/boostorg/stacktrace)  

### 通过vcpkg安装依赖  

```
vcpkg install nlohmann-json
vcpkg install boost-log
vcpkg install boost-stacktrace
```