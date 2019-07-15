#ifndef MODEL_SCRIPT_HPP
#define MODEL_SCRIPT_HPP
#include <variant>
#include <algorithm>
#include <tuple>
#include <vector>
#include <fstream>
#include "model.hpp"
namespace Nando2Tetorisu::HardwareSimulator
{
struct ScriptInit
{
    std::string open, output, compare;
    std::vector<std::tuple<std::string, uint32_t, uint32_t, uint32_t>> output_lists;
    ScriptInit() = default;
    ScriptInit(const ScriptInit &) = default;
    ScriptInit(ScriptInit &&) = default;
    ScriptInit(std::string open, std::string output, std::string compare, decltype(output_lists) output_lists)
        : open(open), output(output), compare(compare), output_lists(std::move(output_lists)) {}
    ScriptInit &operator=(const ScriptInit &) = default;
};
struct ScriptEval
{
    std::vector<std::pair<std::string, uint32_t>> set_lists;
    ScriptEval() = default;
    ScriptEval(const ScriptEval &) = default;
    ScriptEval(ScriptEval &&) = default;
    ScriptEval(decltype(set_lists) set_lists) : set_lists(set_lists) {}
    ScriptEval &operator=(const ScriptEval &) = default;
};
using ScriptCommand = std::pair<int, std::variant<ScriptInit, ScriptEval>>;
class ScriptModel
{
    Model *model;
    std::vector<ScriptCommand> commands;
    bool loaded;
    int pc;
    std::string source, parent, filename;

public:
    ScriptModel(Model *model) : model(model) {}

    void load_script(std::string path);
    const std::string &get_source() { return source; }
    void next()
    {
        pc++;
    }
    void exec()
    {
        if (pc < commands.size())
        {
            auto &&current = current_command();
            if (current.second.index() == 0)
            {
                ScriptInit init = std::get<ScriptInit>(current.second);
                model->load_hdl(parent + "/" + init.open);
                model->reload();
            }
            else
            {
                ScriptEval eval = std::get<ScriptEval>(current.second);
                for (auto &&set : eval.set_lists)
                {
                    model->set_input_pin(set.first, set.second);
                }
                model->eval();
            }
        }
    }
    bool eof()
    {
        return pc >= commands.size();
    }
    int current_type()
    {
        return current_command().second.index();
    }
    void reset()
    {
        pc = 0;
    }
    const ScriptCommand &current_command()
    {
        return commands[pc];
    }
};
} // namespace Nando2Tetorisu::HardwareSimulator
#endif