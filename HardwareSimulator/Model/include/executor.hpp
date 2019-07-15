#ifndef MODEL_EXECUTOR_HPP
#define MODEL_EXECUTOR_HPP
#include <algorithm>
#include <array>
#include <cinttypes>
#include <map>
#include <memory>
#include <stack>
#include <unordered_map>
#include <vector>
namespace Nando2Tetorisu::HardwareSimulator
{
namespace execution
{
class Executor
{
public:
    virtual void set_input(int pos, int offset, uint32_t value) = 0;
    virtual uint32_t get_output(int pos, int offset) = 0;
    virtual bool eval() = 0;
    virtual int output_number() = 0;
    virtual int output_width(int pos) = 0;
    virtual int input_number() = 0;
    virtual int input_width(int pos) = 0;
    virtual std::unique_ptr<Executor> clone() = 0;
};

class Nand : public Executor
{
private:
    std::array<uint32_t, 2> in;
    uint32_t out;

public:
    Nand()
    {
        in[0] = in[1] = 0;
        eval();
    }

public:
    virtual void set_input(int pos, int offset, uint32_t value);
    virtual uint32_t get_output(int pos, int offset);
    virtual bool eval();
    virtual int input_number() { return 2; }
    virtual int input_width(int pos) { return 1; }

    virtual int output_number() { return 1; }
    virtual int output_width(int pos) { return 1; }
    virtual std::unique_ptr<Executor> clone()
    {
        return std::make_unique<Nand>();
    }
};

class Pin : public Executor
{
private:
    uint32_t in, old;
    int width;

public:
    Pin(int width) : width(width), in(0) {}
    virtual std::unique_ptr<Executor> clone()
    {
        return std::make_unique<Pin>(width);
    }
    virtual void set_input(int pos, int offset, uint32_t value)
    {
        if (offset < 0)
        {
            in = value;
        }
        else
        {
            in = in & (~(1 << offset)) | ((value & 1) << offset);
        }
    };
    virtual uint32_t get_output(int pos, int offset)
    {
        if (offset < 0)
            return in;
        return (in >> offset) & 1;
    }
    virtual bool eval()
    {
        if (in != old)
        {
            old = in;
            return true;
        }
        return false;
    }
    virtual int output_number() { return 1; }
    virtual int output_width(int pos) { return width; }
    virtual int input_number() { return 1; }
    virtual int input_width(int pos) { return width; }
};
// node id, pin id, offset
using pin_id = std::tuple<int, int, int>;
using node_id = std::pair<int, int>;

class GraphNode : public Executor
{
private:
    std::vector<std::unique_ptr<Executor>> nodes;
    std::map<pin_id, pin_id> &from;
    std::vector<int> &inputs, &outputs, &internals;
    std::vector<int> &topology_node;

public:
    GraphNode(std::vector<std::unique_ptr<Executor>> nodes,
              std::map<pin_id, pin_id> &from, std::vector<int> &inputs,
              std::vector<int> &outputs, std::vector<int> &internals,

              std::vector<int> &topology_node)
        : nodes(std::move(nodes)), from(from), inputs(inputs), outputs(outputs),
          internals(internals), topology_node(topology_node)
    {
    }
    virtual std::unique_ptr<Executor> clone()
    {
        std::vector<std::unique_ptr<Executor>> n;
        std::transform(nodes.begin(), nodes.end(), std::back_inserter(n),
                       [](auto &&x) { return x->clone(); });
        return std::make_unique<GraphNode>(std::move(n), from, inputs, outputs,
                                           internals, topology_node);
    }

    virtual void set_input(int pos, int offset, uint32_t value)
    {
        nodes[inputs[pos]]->set_input(0, offset, value);
    };
    virtual uint32_t get_internal(int pos, int offset)
    {
        return nodes[internals[pos]]->get_output(0, offset);
    }
    virtual uint32_t get_input(int pos, int offset)
    {
        return nodes[inputs[pos]]->get_output(0, offset);
    }
    virtual uint32_t get_output(int pos, int offset)
    {
        return nodes[outputs[pos]]->get_output(0, offset);
    }
    virtual int input_number() { return inputs.size(); }
    virtual int input_width(int pos)
    {
        return nodes[inputs[pos]]->input_width(0);
    }
    virtual int output_number() { return outputs.size(); }
    virtual int output_width(int pos)
    {
        return nodes[outputs[pos]]->output_width(0);
    }
    virtual bool eval()
    {
        for (auto node_id : topology_node)
        {
            auto &&node = nodes[node_id];
            for (int i = 0; i < node->input_number(); i++)
            {
                for (int j = -1; j < node->input_width(i); j++)
                {
                    auto pin_id = std::make_tuple(node_id, i, j);
                    if (from.find(pin_id) != from.end())
                    {
                        auto [x, y, z] = from[pin_id];
                        node->set_input(i, j, nodes[x]->get_output(y, z));
                    }
                }
            }
            node->eval();
        }

        return true;
    }
};
} // namespace execution
} // namespace Nando2Tetorisu::HardwareSimulator
#endif