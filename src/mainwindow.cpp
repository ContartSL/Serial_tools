#include <memory>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <chrono>

#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/dom/elements.hpp>
#include "tty_device/mypthread.h"
#include "tty_device/serial.h"

using namespace ftxui;

static serial_t *g_serial = nullptr;
static std::mutex g_rx_mutex;
static std::string g_rx_buffer;
static std::atomic<bool> g_serial_open_flag{false};
static std::thread g_rx_thread;

void serial_receive_thread() {
    char buf[256];
    while (g_serial_open_flag.load() && g_serial) {
        int n = g_serial->read(g_serial, buf, sizeof(buf));
        if (n > 0) {
            std::lock_guard<std::mutex> lock(g_rx_mutex);
            g_rx_buffer.append(buf, n);
            if (g_rx_buffer.size() > 10000) {
                g_rx_buffer.erase(0, g_rx_buffer.size() - 5000);
            }
        } else if (n == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } else {
            break;
        }
    }
}

ButtonOption Style() {
    auto option = ButtonOption::Animated();
    option.transform = [](const EntryState& s) {
        auto element = text(s.label);
        if (s.focused) {
            element |= bold;
        }
        return element | center | borderEmpty;
    };
    return option;
}

enum Page {
    PAGE_HOME = 0,
    PAGE_SERIAL_CONFIG,
    PAGE_SERIAL_IO
};

void tui_init() {
    auto screen = ScreenInteractive::Fullscreen();

    int current_page = PAGE_HOME;

    // 串口配置相关变量
    std::string serial_path = "/dev/ttyUSB0";
    int baudrate = 115200;
    int data_bits = 8;
    int stop_bits = 1;
    char parity = 'N';
    std::string config_status = "未配置";

    // 串口收发相关变量
    std::string send_input;
    std::string last_error;

    // 左侧菜单
    auto menu = Container::Vertical({
        Button("主界面", [&] {
            current_page = PAGE_HOME;
        }, Style()),
        Button("串口界面", [&] {
            current_page = PAGE_SERIAL_CONFIG;
        }, Style()),
    });

    // ---------- 配置界面组件 ----------
    std::vector<std::string> baud_entries = {"9600", "19200", "38400", "57600", "115200"};
    int baud_selected = 4;
    auto baud_radio = Radiobox(&baud_entries, &baud_selected);

    std::vector<std::string> data_entries = {"5", "6", "7", "8"};
    int data_selected = 3;
    auto data_radio = Radiobox(&data_entries, &data_selected);

    std::vector<std::string> stop_entries = {"1", "2"};
    int stop_selected = 0;
    auto stop_radio = Radiobox(&stop_entries, &stop_selected);

    std::vector<std::string> parity_entries = {"N", "E", "O"};
    int parity_selected = 0;
    auto parity_radio = Radiobox(&parity_entries, &parity_selected);

    Component path_input = Input(&serial_path, "串口设备路径");

    Component open_button = Button("打开串口", [&] {
        // 收集配置
        baudrate = std::stoi(baud_entries[baud_selected]);
        data_bits = std::stoi(data_entries[data_selected]);
        stop_bits = std::stoi(stop_entries[stop_selected]);
        if (parity_selected == 0) parity = 'N';
        else if (parity_selected == 1) parity = 'E';
        else parity = 'O';

        // 关闭已有串口
        if (g_serial) {
            g_serial_open_flag = false;
            if (g_rx_thread.joinable()) g_rx_thread.join();
            serial_destroy(g_serial);
            g_serial = nullptr;
        }

        g_serial = serial_create();
        if (!g_serial) {
            config_status = "创建串口对象失败";
            return;
        }

        serial_config_t cfg;
        cfg.path = serial_path.c_str();
        cfg.baudrate = baudrate;
        cfg.data_bits = data_bits;
        cfg.stop_bits = stop_bits;
        cfg.parity = parity;

        if (serial_configure(g_serial, &cfg) == 0) {
            config_status = "串口打开成功！切换到通信界面";
            g_serial_open_flag = true;
            g_rx_buffer.clear();
            g_rx_thread = std::thread(serial_receive_thread);
            current_page = PAGE_SERIAL_IO;
        } else {
            config_status = "打开串口失败，请检查路径和权限";
            serial_destroy(g_serial);
            g_serial = nullptr;
        }
    }, Style());

    auto config_container = Container::Vertical({
        path_input,
        baud_radio,
        data_radio,
        stop_radio,
        parity_radio,
        open_button,
    });

    auto config_renderer = Renderer(config_container, [&] {
        return vbox({
            text("串口配置") | bold | hcenter,
            separator(),
            hbox({text("设备路径: "), path_input->Render()}) | flex,
            separator(),
            text("波特率:"),
            baud_radio->Render(),
            text("数据位:"),
            data_radio->Render(),
            text("停止位:"),
            stop_radio->Render(),
            text("校验位:"),
            parity_radio->Render(),
            separator(),
            open_button->Render() | center,
            text(config_status) | color(Color::GrayLight),
        }) | border;
    });

    // ---------- 收发界面组件 ----------
    Component send_input_component = Input(&send_input, "输入要发送的数据");
    Component send_button = Button("发送", [&] {
        if (g_serial && g_serial_open_flag) {
            int n = g_serial->write(g_serial, send_input.c_str(), send_input.size());
            if (n < 0) {
                last_error = "发送失败";
            } else {
                send_input.clear();
                last_error.clear();
            }
        } else {
            last_error = "串口未打开";
        }
    }, Style());

    Component back_button = Button("返回配置", [&] {
        current_page = PAGE_SERIAL_CONFIG;
    }, Style());

    auto io_container = Container::Vertical({
        send_input_component,
        send_button,
        back_button,
    });

    auto io_renderer = Renderer(io_container, [&] {
        std::string rx_display;
        {
            std::lock_guard<std::mutex> lock(g_rx_mutex);
            rx_display = g_rx_buffer;
        }

        auto rx_element = vbox({
            text("接收区:") | bold,
            separator(),
            paragraph(rx_display) | size(HEIGHT, GREATER_THAN, 10) | border,
        }) | flex;

        auto tx_element = vbox({
            text("发送区:") | bold,
            send_input_component->Render(),
            hbox({
                send_button->Render(),
                filler(),
                back_button->Render(),
            }),
            text(last_error) | color(Color::Red),
        }) | border;

        return vbox({
            rx_element,
            separator(),
            tx_element,
        }) | border;
    });

    // ---------- 主页界面 ----------
    auto home_renderer = Renderer([&] {
        return vbox({
            text("串口调试助手 v1.0") | bold | hcenter,
            separator(),
            paragraph(" 欢迎您的使用\n\n左侧菜单选择「串口界面」开始配置 ") | center,
        }) | border;
    });

    // 动态右侧组件
    Component right_component;
    auto update_right_component = [&]() {
        if (current_page == PAGE_HOME) {
            right_component = home_renderer;
        } else if (current_page == PAGE_SERIAL_CONFIG) {
            right_component = config_renderer;
        } else {
            right_component = io_renderer;
        }
    };
    update_right_component();

    // 整体布局
    auto main_layout = Renderer(menu, [&] {
        auto left_panel = menu->Render() | size(WIDTH, EQUAL, 20) | border;
        update_right_component();
        auto right_panel = right_component->Render() | flex | border;
        return hbox({left_panel, right_panel}) | borderEmpty;
    });

    screen.Loop(main_layout);

    // 清理资源
    if (g_serial) {
        g_serial_open_flag = false;
        if (g_rx_thread.joinable()) g_rx_thread.join();
        serial_destroy(g_serial);
    }
}