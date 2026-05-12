// // #include <ftxui/dom/elements.hpp>
// // #include <ftxui/screen/screen.hpp>
// // #include <ftxui/component/component.hpp>
// // #include "iostream"

// // std::string phone_number;
// // // Component input = Input(&phone_number, "phonr number");


// // int main(){
// //     using namespace ftxui;

// //     Element document = hbox({
// //         text("left") | border,
// //         text("middle") | border | flex,
// //         text("right") | border,
// //     });

// //     auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
// //     Render(screen, document);
// //     screen.Print();

// // }

// #include <memory>
// #include <string>

// #include "ftxui/component/captured_mouse.hpp"
// #include "ftxui/component/component.hpp"
// #include "ftxui/component/component_base.hpp"
// #include "ftxui/component/screen_interactive.hpp"
// #include "ftxui/dom/elements.hpp"

// using namespace ftxui;
// /*
//     这是一个辅助函数, 用于创建居于自定义样式的按钮
//     样式由一个 lambda 函数定义, 该函数接受一个 entryState 并且返回一个 "element"
//     我们使用 'center' 来使文本在按钮内居中, 然后使用 'border' 来在按钮周围加上边框, 最后使用 'flex'来使按钮填充
// */

// ButtonOption style(){
//     auto option = ButtonOption::Animated();
//     option.transform = [](const EntryState& s){
//         auto element = text(s.label);
//         if(s.focused){
//             element |= bold;
//         }
//         return element | center | borderEmpty | flex;
//     };
//     return option;
// }

// int main(){
//     int value = 50;

//     auto btn_dec_01 = Button("-1", [&]{value -= 1;}, style());
//     auto btn_inc_01 = Button("+1", [&]{value += 1;}, style());
//     auto btn_dev_10 = Button("-10", [&]{value -= 10;}, style());
//     auto btn_inc_10 = Button("+10", [&]{value += 10;}, style());
//     auto button = Container::Horizontal({btn_dec_01, btn_inc_01, btn_dev_10, btn_inc_10});

//     // 组件树, 这定义了如何使用键盘导航
//     // 选定 'row' 共享以获得网格布局

//     // 修改渲染方式
//     auto component = Renderer(button, [&]{
//         return vbox({
//             text("value = " + std::to_string(value)),
//             separator(),
//             button->Render() | flex,
//         }) | 
//         flex | border;
//     });

//     auto screen = ScreenInteractive::FitComponent();
//     screen.Loop(component);
//     return 0;
// }


#include "mainwindow.hpp"

int main(){
    tui_init();
    return 0;
}
