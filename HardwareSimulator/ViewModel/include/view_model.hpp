#ifndef VIEW_MODEL_HPP_
#define VIEW_MODEL_HPP_

#include <string>
#include <memory>
#include <chrono>
#include <thread>
#include "../../Model/include/model.hpp"
#include "../../types.hpp"
#include "../../../common/include/observer.hpp"
#include "../../Model/include/script.hpp"

namespace Nando2Tetorisu::HardwareSimulator
{

using Nando2Tetorisu::common::Observer;

class Timer
{
    bool is_clear;

    std::function<void()> uu;

public:
    template <typename U>
    Timer(U u) : uu(u),
                 interval(500) { is_clear = true; }
    int interval;

    void stop()
    {
        is_clear = true;
    }
    void start()
    {
        is_clear = false;
        std::thread thread = std::thread([=]() {
            while (true)
            {
                if (is_clear)
                    return;
                std::this_thread::sleep_for(std::chrono::milliseconds(interval));
                if (is_clear)
                    return;
                uu();
            }
        });
        thread.detach();
    }
};

class ViewModel
{
public:
    ViewModel(Observer *observer);

private:
    void on_load_chip(std::string path);
    void on_edit_chip(std::string content);
    void on_reload();
    void on_set_pin(int id, uint32_t value);
    void on_eval();
    void on_load_script(std::string path);    // TODO
    void on_change_animate(AnimateType type); // TODO
    void on_change_format(FormatType type);   // TODO
    void on_change_view(ViewType type);       // TODO
    void on_change_speed(int speed);
    void on_single_step();
    void on_run();
    void on_pause();
    void on_reset();
    void on_save();
    void set_output_pin();
    void set_input_pin();
    void set_basic();

private:
    Timer timer;
    std::unique_ptr<Model> model;
    std::unique_ptr<ScriptModel> script_model;
    std::vector<pin>
        inputs, outputs, internals;
    Observer *const observer;
    bool error;
    int speed;
};

} // namespace Nando2Tetorisu::HardwareSimulator

#endif