#include <view_model.hpp>
#include <any>

namespace Nando2Tetorisu::HardwareSimulator
{
ViewModel::ViewModel(Observer *o) : observer(o), timer(Timer([this]() {
                                        on_single_step();
                                    }))
{
    model = std::make_unique<Model>();
    script_model = std::make_unique<ScriptModel>(model.get());
    error = false;
    speed = 2;
    o->subscribe("save", [&](std::any v) {
        on_save();
    });

    o->subscribe("load_script", [&](std::any v) {
        on_load_script(std::any_cast<std::string>(v));
    });
    o->subscribe("load_chip", [&](std::any v) {
        on_load_chip(std::any_cast<std::string>(v));
    });
    o->subscribe("single_step", [&](std::any v) {
        on_single_step();
    });
    o->subscribe("run", [&](std::any v) {
        on_run();
    });
    o->subscribe("change_speed", [&](std::any v) {
        on_change_speed(std::any_cast<int>(v));
    });
    o->subscribe("pause", [&](std::any v) {
        on_pause();
    });
    o->subscribe("reset", [&](std::any b) {
        on_reset();
    });
    o->subscribe("edit_chip", [&](std::any v) {
        on_edit_chip(std::any_cast<std::string>(v));
    });
    o->subscribe("reload", [&](std ::any v) {
        on_reload();
    });
    o->subscribe("set_pin", [&](std::any v) {
        auto &&[x, y] = std::any_cast<std::pair<int, uint32_t>>(v);
        on_set_pin(x, y);
    });
    o->subscribe("eval", [&](std::any v) {
        on_eval();
    });
}
void ViewModel::on_save()
{
    model->save();
}
void ViewModel::on_run()
{
    timer.start();
}
void ViewModel::on_pause()
{
    timer.stop();
}
void ViewModel::on_change_speed(int speed)
{
    switch (speed)
    {
    case 0:
        timer.interval = 100;
        break;
    case 1:
        timer.interval = 250;
        break;
    case 2:
        timer.interval = 500;
        break;
    case 3:
        timer.interval = 750;
        break;
    case 4:
        timer.interval = 1000;
        break;
    }
}
void ViewModel::set_input_pin()
{
    auto &&prototype = model->get_prototype();

    std::vector<uint32_t> input_values;
    for (int i = 0; i < prototype.inputs_name.size(); i++)
    {
        input_values.push_back(model->get_input_pin(i));
    }

    observer->publish("set_input_pins_value",
                      std::make_pair(std::cbegin(input_values), std::cend(input_values)));
}
void ViewModel::set_basic()
{
    auto &&prototype = model->get_prototype();

    observer->publish("set_input_pins",
                      std::make_pair(std::cbegin(prototype.inputs_name), std::cend(prototype.inputs_name)));
    observer->publish("set_internal_pins",
                      std::make_pair(std::cbegin(prototype.internal_name), std::cend(prototype.internal_name)));
    observer->publish("set_output_pins",
                      std::make_pair(std::cbegin(prototype.outputs_name), std::cend(prototype.outputs_name)));
    observer->publish("set_top_name", model->get_top_name());
    observer->publish("unset_status", {});
}
void ViewModel::on_reset()
{
    try
    {
        script_model->reset();
        observer->publish("set_current_line", script_model->current_command().first);
    }
    catch (std::string err)
    {
        observer->publish("set_error", err);
        error = true;
    }
}
void ViewModel::on_single_step()
{

    try
    {
        if (script_model->eof())
        {
            observer->publish("set_message", std::string("end of script"));
            on_pause();
        }
        else
        {
            auto &&current = script_model->current_command();
            script_model->exec();

            if (script_model->current_type() == 0)
            {
                observer->publish("set_hdl_source", model->get_hdl_source());
                set_basic();
            }
            else
            {
                set_input_pin();
                set_output_pin();
            }
            script_model->next();
            observer->publish("set_current_line", script_model->current_command().first);
        }
    }
    catch (std::string err)
    {
        observer->publish("set_error", err);
        error = true;
    }
}
void ViewModel::on_load_script(std::string path)
{
    try
    {
        script_model->load_script(path);
        observer->publish("set_script_source", script_model->get_source());
        observer->publish("set_current_line", script_model->current_command().first);
    }
    catch (std::string err)
    {
        observer->publish("set_error", err);
        error = true;
    }
}
void ViewModel::on_load_chip(std::string path)
{

    try
    {
        model->load_hdl(path);
        observer->publish("set_hdl_source", model->get_hdl_source());
        on_reload();
        observer->publish("clear_script", {});
        observer->publish("set_current_line", -1);
    }
    catch (std::string err)
    {
        observer->publish("set_error", err);
        error = true;
    }
}
void ViewModel::on_edit_chip(std::string content)
{

    model->update_hdl_source(content);
}
void ViewModel::on_set_pin(int id, uint32_t value)
{
    if (error)
        return;
    try
    {
        model->set_input_pin(id, value);
    }
    catch (std::string err)
    {
        observer->publish("set_error", err);
        error = true;
    }
}
void ViewModel::set_output_pin()
{
    auto &&prototype = model->get_prototype();

    std::vector<uint32_t> internal_values, outputs_values;
    for (int i = 0; i < prototype.outputs_name.size(); i++)
    {
        outputs_values.push_back(model->get_output_pin(i));
    }

    for (int i = 0; i < prototype.internal_name.size(); i++)
    {
        internal_values.push_back(model->get_internal_pin(i));
    }
    observer->publish("set_internal_pins_value",
                      std::make_pair(std::cbegin(internal_values), std::cend(internal_values)));
    observer->publish("set_output_pins_value",
                      std::make_pair(std::cbegin(outputs_values), std::cend(outputs_values)));
}
void ViewModel::on_eval()
{
    if (error)
        return;
    try
    {
        model->eval();
        set_output_pin();
    }
    catch (std::string err)
    {
        observer->publish("set_error", err);
        error = true;
    }
}
void ViewModel::on_reload()
{
    try
    {
        model->reload();

        auto &&prototype = model->get_prototype();

        set_basic();
        error = false;
    }
    catch (std::string err)
    {
        observer->publish("set_error", err);
        error = true;
    }
}
} // namespace Nando2Tetorisu::HardwareSimulator