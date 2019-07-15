#ifndef OBSERVER_HPP_
#define OBSERVER_HPP_
#include <any>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

namespace Nando2Tetorisu::common
{
using action = std::function<void(std::any)>;
class Observer
{
public:
    void publish(std::string message, std::any value);
    void subscribe(std::string message, action callback);
    void unsubscribe(uint32_t id);

private:
    std::unordered_map<std::string, std::vector<action>> callbacks;
};
} // namespace Nando2Tetorisu::common

#endif