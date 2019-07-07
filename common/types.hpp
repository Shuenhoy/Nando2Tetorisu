#ifndef TYPES_HPP
#define TYPES_HPP

namespace Nando2Tetorisu::common
{
enum class AnimateType
{
    ProgramFlow,
    ProgramFlowAndDataFlow,
    NoAnimation
};

enum class FormatType
{
    Decimal,
    Hexa,
    Binary
};

enum class ViewType
{
    Screen,
    Output,
    Compare,
    Script
};
} // namespace Nando2Tetorisu::common

#endif