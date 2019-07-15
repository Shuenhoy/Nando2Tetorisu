#include "../../../common/include/observer.hpp"
#include <any>
#include <iostream>
#include <view_model.hpp>
using namespace Nando2Tetorisu;

template <typename T>
using citer = typename std::vector<T>::const_iterator;
template <typename T>
using cip = typename std::pair<citer<T>, citer<T>>;

using pin = std::pair<std::string, int>;

int main()
{
    common::Observer *observer = new common::Observer;
    HardwareSimulator::ViewModel *view_model = new HardwareSimulator::ViewModel(observer);
    observer->subscribe("set_internal_pins_value", [](std::any v) {
        auto &&[begin, end] = std::any_cast<cip<uint32_t>>(v);
        std::cout << "internal values:";

        for (auto it = begin; it != end; it++)
        {
            std::cout << *it << " ";
        }
        std::cout << std::endl;
    });
    observer->subscribe("clear_script", [](std::any v) {
        std::cout << "clear script" << std::endl;
    });
    observer->subscribe("set_hdl_source", [](std::any v) {
        std::cout << "load hdl source:\n"
                  << std::any_cast<std::string>(v) << std::endl;
    });
    observer->subscribe("set_script_source", [](std::any v) {
        std::cout << "load script source:\n"
                  << std::any_cast<std::string>(v) << std::endl;
    });

    observer->subscribe("set_input_pins_value", [](std::any v) {
        auto &&[begin, end] = std::any_cast<cip<uint32_t>>(v);
        std::cout << "input values:";

        for (auto it = begin; it != end; it++)
        {
            std::cout << *it << " ";
        }
        std::cout << std::endl;
    });
    observer->subscribe("set_output_pins_value", [](std::any v) {
        auto &&[begin, end] = std::any_cast<cip<uint32_t>>(v);
        std::cout << "output values:";

        for (auto it = begin; it != end; it++)
        {
            std::cout << *it << " ";
        }
        std::cout << std::endl;
    });
    observer->subscribe("set_input_pins", [](std::any v) {
        auto &&[begin, end] = std::any_cast<cip<pin>>(v);
        std::cout << "inputs pin:";

        for (auto it = begin; it != end; it++)
        {
            std::cout << it->first << " ";
        }
        std::cout << std::endl;
    });
    observer->subscribe("set_internal_pins", [](std::any v) {
        auto &&[begin, end] = std::any_cast<cip<pin>>(v);
        std::cout << "internal pin:";

        for (auto it = begin; it != end; it++)
        {
            std::cout << it->first << " ";
        }
        std::cout << std::endl;
    });
    observer->subscribe("set_error", [](std::any v) {
        auto err = std::any_cast<std::string>(v);
        std::cout << "ERROR: " << err << std::endl;
    });

    observer->subscribe("set_current_line", [](std::any v) {
        auto err = std::any_cast<int>(v);
        std::cout << "NOW: " << err << std::endl;
    });
    observer->subscribe("set_message", [](std::any v) {
        auto err = std::any_cast<std::string>(v);
        std::cout << "MESSAGE: " << err << std::endl;
    });
    observer->subscribe("set_output_pins", [](std::any v) {
        auto &&[begin, end] = std::any_cast<cip<pin>>(v);
        std::cout << "output pin:";

        for (auto it = begin; it != end; it++)
        {
            std::cout << it->first << " ";
        }
        std::cout << std::endl;
    });

    observer->publish("load_chip", std::string("./viewmodel_test/Not1.hdl"));

    std::cout << "loading not" << std::endl;
    observer->publish("load_chip", std::string("./viewmodel_test/Not.hdl"));
    std::cout << "set 0" << std::endl;
    observer->publish("set_pin", std::make_pair(0, 0u));
    observer->publish("eval", {});
    std::cout << "set 1" << std::endl;

    observer->publish("set_pin", std::make_pair(0, 1u));
    observer->publish("eval", {});

    observer->publish("load_chip", std::string("./viewmodel_test/And.hdl"));
    std::cout << "set 0 0" << std::endl;

    observer->publish("set_pin", std::make_pair(0, 0u));
    observer->publish("set_pin", std::make_pair(1, 0u));
    observer->publish("eval", {});
    std::cout << "set 0 1" << std::endl;

    observer->publish("set_pin", std::make_pair(0, 0u));
    observer->publish("set_pin", std::make_pair(1, 1u));
    observer->publish("eval", {});
    std::cout << "set 1 0" << std::endl;

    observer->publish("set_pin", std::make_pair(0, 1u));
    observer->publish("set_pin", std::make_pair(1, 0u));
    observer->publish("eval", {});
    std::cout << "set 1 1" << std::endl;

    observer->publish("set_pin", std::make_pair(0, 1u));
    observer->publish("set_pin", std::make_pair(1, 1u));
    observer->publish("eval", {});

    observer->publish("load_script", std::string("./viewmodel_test/And.tst"));
    observer->publish("single_step", {});
    observer->publish("single_step", {});
    observer->publish("single_step", {});
    observer->publish("single_step", {});
    observer->publish("single_step", {});
    observer->publish("single_step", {});

    std::cout << "test timer" << std::endl;
    observer->publish("reset", {});
    observer->publish("run", {});
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));

    return 0;
}