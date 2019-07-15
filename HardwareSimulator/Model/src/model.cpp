#include <cassert>
#include <string>
#include <model.hpp>
#include <fstream>
#include <chip.hpp>
#include <executor.hpp>

namespace Nando2Tetorisu::HardwareSimulator
{
void Model::set_input_pin(std::string pin, uint32_t value)
{
    top->set_input(get_blueprint().get_index_of_input_pin(pin), -1, value);
}
void Model::set_input_pin(int pin, uint32_t value)
{
    top->set_input(pin, -1, value);
}
void Model::eval()
{
    top->eval();
}
uint32_t Model::get_output_pin(int pin)
{
    return top->get_output(pin, -1);
}
uint32_t Model::get_input_pin(int pin)
{
    return top->get_input(pin, -1);
}

uint32_t Model::get_internal_pin(int pin)
{
    return top->get_output(pin, -1);
}
uint32_t Model::get_output_pin(std::string pin)
{
    return top->get_output(get_blueprint().get_index_of_output_pin(pin), -1);
}
void Model::save()
{
    std::fstream fout(hdl_path.parent + '/' + hdl_path.filename);
    if (fout.fail())
    {
        throw std::string("cannot open file: " + hdl_path.parent + '/' + hdl_path.filename);
    }
    fout << hdl_source;
}
void Model::load_hdl(std::string path)
{
    hdl_path = path;
    std::ifstream fin(path);
    if (fin.fail())
    {
        throw std::string("cannot open file: " + path);
    }
    hdl_source = std::string((std::istreambuf_iterator<char>(fin)),
                             std::istreambuf_iterator<char>());
    builder.load_blueprint(hdl_path.parent, hdl_source);
}
void Model::update_hdl_source(std::string new_source)
{
    hdl_source = new_source;
}
void Model::reload()
{
    builder.clear();
    top_name = builder.load_blueprint(hdl_path.parent, hdl_source);
    top = std::move(std::unique_ptr<execution::GraphNode>(static_cast<execution::GraphNode *>(builder.build(top_name).release())));
}
const ChipBluePrint &Model::get_blueprint()
{
    return builder.get_blueprint(top_name);
}
const Prototype &Model::get_prototype()
{
    return builder.get_prototype(top_name);
}
} // namespace Nando2Tetorisu::HardwareSimulator