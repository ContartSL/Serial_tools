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

    auto button = Container::Vertical({
        Button("Serial", [&]{}, Style()),
        Button("tero", [&]{}, Style()),
    });


    auto fullscreen_component = Renderer(button, [&]{
        int terminal_width = screen.dimx();
        int terminal_height = screen.dimy();

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

    auto top_info = hbox(Elements{text(" serial tools ")});

        auto content = vbox(Elements{
            top_info,
            separator(),
            hbox(Elements{
                button->Render(),
                text("内容区域") | size(WIDTH, EQUAL, terminal_width)
            }),
            separator(),
            // 如需重复渲染按钮可取消注释，通常不需要
            // button->Render(),
        }) | border;

        return content;
    });

    screen.Loop(fullscreen_component);
    // auto fullscreen_component = Renderer()
}

