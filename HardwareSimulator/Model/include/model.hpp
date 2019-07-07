#ifndef MODEL_HPP_
#define MODEL_HPP_

#include <string>
#include <unordered_map>
#include <unordered_set>

namespace Nando2Tetorisu::Model
{
using pin_map = std::unordered_map<std::string, uint32_t>;
class Model
{
public:
    void load_hdl(std::string source);
    void load_script(std::string source);
    void open_output(std::string path);
    void open_compare(std::string path);

    bool next_script();
    void set_input_pin(std::string pin, uint32_t value);

    std::pair<pin_map::iterator, pin_map::iterator> input_iter();
    std::pair<pin_map::iterator, pin_map::iterator> internal_iter();
    std::pair<pin_map::iterator, pin_map::iterator> output_iter();

private:
    std::unordered_map<std::string, uint32_t>
        pins_value;
    std::unordered_set<std::string> input_pins, output_pins, internal_pins;
};
} // namespace Nando2Tetorisu::Model

#endif