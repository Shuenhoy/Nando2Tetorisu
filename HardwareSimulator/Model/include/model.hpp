#ifndef MODEL_HPP_
#define MODEL_HPP_

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <memory>
#include <regex>
#include "builder.hpp"
#include "chip.hpp"

namespace Nando2Tetorisu::HardwareSimulator
{
using pin_map = std::unordered_map<std::string, uint32_t>;

struct path
{
    path() = default;
    path(std::string path)
    {
        std::size_t botDirPos = path.find_last_of("/\\");
        parent = path.substr(0, botDirPos);
        filename = path.substr(botDirPos, path.length());
    }
    std::string parent, filename;
};
class Model
{
public:
    void load_hdl(std::string path);
    void load_script(std::string path); // TODO
    void update_hdl_source(std::string new_source);
    void open_output(std::string path);  // TODO
    void open_compare(std::string path); // TODO
    void save();
    bool next_script(); // TODO
    void reload();
    void set_input_pin(std::string pin, uint32_t value);
    uint32_t get_output_pin(std::string pin);
    void set_input_pin(int pin, uint32_t value);
    uint32_t get_output_pin(int pin);
    uint32_t get_input_pin(int pin);
    uint32_t get_internal_pin(int pin);
    std::string get_hdl_source()
    {
        return hdl_source;
    }

    void eval();
    const Prototype &get_prototype();

    const ChipBluePrint &get_blueprint();
    const std::string &get_top_name()
    {
        return top_name;
    }

private:
    GraphNodeBuilder builder;
    path hdl_path;
    std::unique_ptr<execution::GraphNode> top;
    std::string top_name;

    std::string hdl_source;
};
} // namespace Nando2Tetorisu::HardwareSimulator

#endif