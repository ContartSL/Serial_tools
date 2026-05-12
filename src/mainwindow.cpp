#include <memory>
#include <string>

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

ButtonOption Style(){
    auto option = ButtonOption::Animated();
    option.transform = [](const EntryState& s){
        auto element = text(s.label);
        if(s.focused){
            element |= bold;
        }
        return element | center | borderEmpty; 
    };
    return option;
}

void tui_init(){
    auto screen = ScreenInteractive::Fullscreen();

    std::string right_content = "welcome to use SerailTools";

    auto menu = Container::Vertical({
        Button("Serial", [&]{}, Style()),
        Button("Tero", [&]{}, Style()),
    });

    auto fullscreen_component = Renderer(menu, [&]{
        int terminal_width = screen.dimx();

        auto left_panel = menu->Render() | size(WIDTH, EQUAL, 20) | border;

        auto right_panel = vbox({
            text(right_content) | hcenter,
        }) | border | flex;
        
        auto main_layout = hbox({
            left_panel,
            right_panel,
        }) | borderEmpty;

        return main_layout;
    });

    //     auto top_info = hbox({
    //         text(" serial tools "),
    //     });

    //     // auto fax = hbox({
    //     //     button, 
    //     //     text("内容区域") | size(WIDTH, EQUAL, terminal_width),
    //     // });

    //     auto content = vbox({
    //         top_info,
    //         separator(),
    //         hbox({
    //             button, 
    //             text("内容区域") | size(WIDTH, EQUAL, terminal_width),
    //         }),
    //         separator(),
    //         button->Render(),
    //     }) | border;
    //     return content;

    // });

    screen.Loop(fullscreen_component);
    // auto fullscreen_component = Renderer()
}

