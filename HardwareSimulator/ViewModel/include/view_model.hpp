#ifndef VIEW_MODEL_HPP_
#define VIEW_MODEL_HPP_

#include <string>
#include "../../Model/include/model.hpp"
#include "../../common/types.hpp"

namespace Nando2Tetorisu::ViewModel
{

using Model::pin_map;

class ViewModel
{

private:
    void on_load_chip(std::string path);
    void on_load_script(std::string path);
    void on_change_animate(common::AnimateType type);
    void on_change_format(common::FormatType type);
    void on_change_view(common::ViewType type);
    void on_change_speed(int speed);
    void on_single_step();
    void on_run();
    void on_reset();

public:
};

} // namespace Nando2Tetorisu::ViewModel

#endif