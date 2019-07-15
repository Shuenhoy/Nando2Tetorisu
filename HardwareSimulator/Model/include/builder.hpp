#include "chip.hpp"
#include "executor.hpp"
#include <fstream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

namespace Nando2Tetorisu::HardwareSimulator
{
namespace
{
using execution::Executor;
using execution::GraphNode;
using execution::pin_id;
} // namespace
struct Prototype
{
    std::map<pin_id, pin_id> from;
    std::vector<int> inputs, outputs, internals;
    std::vector<int> topology_node;
    std::vector<std::unique_ptr<Executor>> nodes;
    std::vector<pin> inputs_name, outputs_name, internal_name;

    Prototype() = default;
    Prototype(std::map<pin_id, pin_id> &&from, std::vector<int> &&inputs,
              std::vector<int> &&outputs, std::vector<int> &&internals,
              std::vector<std::unique_ptr<Executor>> &&nodes,
              std::vector<pin> &&inputs_name, std::vector<pin> &&outputs_name,
              std::vector<pin> &&internal_name,
              std::vector<int> &&topology_node)
        : from(std::move(from)), inputs(std::move(inputs)),
          outputs(std::move(outputs)), internals(std::move(internals)),
          nodes(std::move(nodes)), inputs_name(std::move(inputs_name)),
          outputs_name(std::move(outputs_name)),
          internal_name(std::move(internal_name)),
          topology_node(std::move(topology_node)) {}

    Prototype(Prototype &&) = default;
    Prototype(const Prototype &) = default;
    Prototype &operator=(const Prototype &) = default;
    Prototype &operator=(Prototype &&) = default;
};
class GraphNodeBuilder
{
    std::unordered_map<std::string, ChipBluePrint> blueprints;
    std::unordered_map<std::string, Prototype> prototypes;

public:
    const ChipBluePrint &get_blueprint(std::string name)
    {
        return blueprints[name];
    }
    const Prototype &get_prototype(std::string name)
    {
        return prototypes[name];
    }

public:
    GraphNodeBuilder()
    {
        blueprints["Nand"] = ChipBluePrint(
            {}, "Nand", {pin("a", 1), pin("b", 1)}, {pin("out", 1)});
    }
    void clear()
    {
        blueprints.clear();
        prototypes.clear();
        blueprints["Nand"] = ChipBluePrint(
            {}, "Nand", {pin("a", 1), pin("b", 1)}, {pin("out", 1)});
    }
    std::string load_blueprint_from_file(std::string dir, std::string name)
    {
        std::ifstream fin(dir + "/" + name + ".hdl");
        if (fin.fail())
        {
            throw std::string("cannot open file: " + dir + "/" + name + ".hdl");
        }
        return load_blueprint(dir,
                              std::string((std::istreambuf_iterator<char>(fin)),
                                          std::istreambuf_iterator<char>()));
    }
    std::string load_blueprint(std::string dir, std::string src)
    {

        auto bp = parse_hdl(src);
        std::unordered_map<std::string, size_t> pin_to_id;

        std::vector<std::unique_ptr<Executor>> nodes;
        std::vector<int> inputs, outputs, internals;
        std::vector<pin> internal_name;
        std::map<pin_id, pin_id> from;
        std::vector<int> topology_node;
        std::unordered_map<int, std::unordered_map<int, bool>> supply;

        for (auto &&[name, width] : bp.inputs)
        {
            pin_to_id[name] = nodes.size();
            nodes.push_back(std::make_unique<execution::Pin>(width));
            inputs.push_back(pin_to_id[name]);
        }
        for (auto &&[name, width] : bp.outputs)
        {
            pin_to_id[name] = nodes.size();
            nodes.push_back(std::make_unique<execution::Pin>(width));
            outputs.push_back(pin_to_id[name]);
        }
        for (auto &&part : bp.parts)
        {
            if (blueprints.find(part.name) == blueprints.end())
            {
                load_blueprint_from_file(dir, part.name);
            }
            auto &&bp_part = blueprints[part.name];
            int bp_part_id = nodes.size();
            nodes.push_back(build(part.name));

            for (auto &&assignment : part.args)
            {
                switch (assignment.second.index())
                {
                case 0:
                {
                    auto &&[in_pin, in_pin_w] = assignment.first;
                    auto &&[out_pin, out_pin_w] =
                        std::get<0>(assignment.second);
                    if (pin_to_id.find(out_pin) == pin_to_id.end())
                    {
                        pin_to_id[out_pin] = nodes.size();
                        auto iter = std::find_if(
                            internal_name.begin(), internal_name.end(),
                            [&](auto &&x) { return x.first == out_pin; });
                        if (iter == internal_name.end())
                        {
                            internal_name.push_back(pin(out_pin, 1));
                            internals.push_back(pin_to_id[out_pin]);
                        }
                        nodes.push_back(std::make_unique<execution::Pin>(1));
                    }
                    auto iter = std::find_if(
                        bp_part.inputs.begin(), bp_part.inputs.end(),
                        [&](auto &&o) { return o.first == in_pin; });
                    if (iter != bp_part.inputs.end())
                    {

                        auto in_pin_id =
                            pin_id(bp_part_id,
                                   std::distance(bp_part.inputs.begin(), iter),
                                   in_pin_w);
                        auto op = pin_to_id[out_pin];
                        auto out_pin_id = pin_id(op, 0, out_pin_w);
                        from[in_pin_id] = out_pin_id;
                        if (supply.find(op) == supply.end())
                        {
                            supply[op] = {};
                        }
                        supply[op][bp_part_id] = true;
                    }
                    else
                    {
                        iter = std::find_if(
                            bp_part.outputs.begin(), bp_part.outputs.end(),
                            [&](auto &&o) { return o.first == in_pin; });
                        if (iter != bp_part.outputs.end())
                        {
                            auto ip = bp_part_id;
                            auto in_pin_id = pin_id(
                                ip,
                                std::distance(bp_part.outputs.begin(), iter),
                                in_pin_w);
                            auto op = pin_to_id[out_pin];
                            auto out_pin_id = pin_id(op, 0, out_pin_w);
                            from[out_pin_id] = in_pin_id;
                            if (supply.find(op) == supply.end())
                            {
                                supply[op] = {};
                            }
                            supply[ip][op] = true;
                        }
                        else
                        {
                            throw std::string("no such pin <") + in_pin + "> in chip <" + part.name + ">";
                        }
                    }
                }
                break;
                case 1: // TODO
                    break;
                }
            }
        }
        std::vector<int> indegree(nodes.size());
        for (auto &&[u, x] : supply)
        {
            for (auto &&[v, t] : x)
            {
                indegree[v]++;
            }
        }
        std::queue<int> Q;

        for (auto x : indegree)
        {
            if (x == 0)
            {
                Q.push(x);
            }
        }
        while (!Q.empty())
        {
            int x = Q.front();
            Q.pop();
            if (std::find(inputs.begin(), inputs.end(), x) == inputs.end())
                topology_node.push_back(x);
            for (auto &&[v, t] : supply[x])
            {
                indegree[v]--;
                if (indegree[v] == 0)
                    Q.push(v);
            }
        }
        if (topology_node.size() < indegree.size() - inputs.size())
        {
            throw std::string("loops exists in chip <") + bp.name + ">";
        }
        auto namae = bp.name;

        prototypes[namae] =
            Prototype(std::move(from), std::move(inputs), std::move(outputs),
                      std::move(internals), std::move(nodes),
                      std::vector(bp.inputs), std::vector(bp.outputs),
                      std::move(internal_name), std::move(topology_node));
        blueprints[namae] = std::move(bp);

        return namae;
    }

    std::unique_ptr<Executor> build(std::string name)
    {
        if (name == "Nand")
        {
            return std::make_unique<execution::Nand>();
        }
        else
        {
            auto &&prototype = prototypes[name];
            std::vector<std::unique_ptr<Executor>> nodes;
            std::transform(prototype.nodes.begin(), prototype.nodes.end(),
                           std::back_inserter(nodes),
                           [](auto &&x) { return x->clone(); });
            return std::make_unique<GraphNode>(
                std::move(nodes), prototype.from, prototype.inputs,
                prototype.outputs, prototype.internals,
                prototype.topology_node);
        }
    }
}; // namespace Nando2Tetorisu::HardwareSimulator
} // namespace Nando2Tetorisu::HardwareSimulator