#ifndef MODEL_CHIP_HPP
#define MODEL_CHIP_HPP
#include <vector>
#include <string>
#include <variant>
#include <algorithm>
#include <unordered_map>
namespace Nando2Tetorisu::HardwareSimulator
{
using pin = std::pair<std::string, int>;
using assignment_arg = std::variant<pin, uint32_t>;
using assignment = std::pair<pin, assignment_arg>;
struct ChipPart
{
    std::string name;
    std::vector<assignment> args;
    ChipPart() = default;
    ChipPart(std::string n, decltype(args) a) : name(n), args(std::move(a)) {}
    ChipPart(const ChipPart &rhs)
    {
        name = rhs.name;
        args = rhs.args;
    }
    ChipPart(ChipPart &&rhs) noexcept
    {
        *this = std::move(rhs);
    }
    ChipPart &operator=(const ChipPart &rhs) = default;
    ChipPart &operator=(ChipPart &&rhs) noexcept
    {
        name = std::move(rhs.name);
        args = std::move(rhs.args);
        return *this;
    }
};

struct ChipBluePrint
{
    std::vector<ChipPart> parts;
    std::string name;
    std::vector<pin> inputs, outputs;
    ChipBluePrint() = default;
    ChipBluePrint(std::vector<ChipPart> p, std::string n, std::vector<pin> i, std::vector<pin> o)
        : parts(std::move(p)), name(n), inputs(std::move(i)), outputs(std::move(o))
    {
    }
    int get_index_of_input_pin(std::string pin) const
    {
        return std::distance(inputs.begin(), std::find_if(inputs.begin(), inputs.end(), [&](auto &&x) {
                                 return x.first == pin;
                             }));
    }
    int get_index_of_output_pin(std::string pin) const
    {
        return std::distance(outputs.begin(), std::find_if(outputs.begin(), outputs.end(), [&](auto &&x) {
                                 return x.first == pin;
                             }));
    }
    ChipBluePrint(ChipBluePrint &&rhs) noexcept
    {
        *this = std::move(rhs);
    }
    ChipBluePrint(const ChipBluePrint &rhs)
    {
        parts = rhs.parts;
        name = rhs.name;
        inputs = rhs.inputs;
        outputs = rhs.outputs;
    }
    ChipBluePrint &operator=(const ChipBluePrint &rhs) = default;
    ChipBluePrint &operator=(ChipBluePrint &&rhs) noexcept
    {
        parts = std::move(rhs.parts);
        name = std::move(rhs.name);
        inputs = std::move(rhs.inputs);
        outputs = std::move(rhs.outputs);
        return *this;
    }
};

ChipBluePrint parse_hdl(std::string input);

} // namespace Nando2Tetorisu::HardwareSimulator
#endif