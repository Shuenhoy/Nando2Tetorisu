#include <iostream>
#include <chip.hpp>
#include <builder.hpp>
#include <cassert>
#include <executor.hpp>
using namespace Nando2Tetorisu::HardwareSimulator;

int main()
{
    GraphNodeBuilder builder;
    auto x = R"(
CHIP Not {
    IN in;
    OUT out;
PARTS:
    Nand(a=in,b=in,out=out);
}
    )";
    builder.load_blueprint("", x);
    auto y = builder.build("Not");
    y->set_input(0, 0, 1);
    y->eval();
    assert(y->get_output(0, 0) == 0);
    y->set_input(0, 0, 0);
    y->eval();
    assert(y->get_output(0, 0) == 1);
    return 0;
}