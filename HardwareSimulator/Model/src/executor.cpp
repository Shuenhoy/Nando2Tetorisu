#include <executor.hpp>

namespace Nando2Tetorisu::HardwareSimulator
{
namespace execution
{
void Nand::set_input(int pos, int offset, uint32_t value) { in[pos] = value; }
uint32_t Nand::get_output(int pos, int offset) { return out; }
bool Nand::eval()
{
    uint32_t nout = 1 - (in[0] & in[1]);
    if (nout != out)
    {
        out = nout;
        return true;
    }
    else
    {
        return false;
    }
}
} // namespace execution
} // namespace Nando2Tetorisu::HardwareSimulator