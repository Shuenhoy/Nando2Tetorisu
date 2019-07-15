#ifndef OBSERVER_HPP_
#define OBSERVER_HPP_
#include <any>
#include <functional>
#include <string>

namespace Nando2Tetorisu::common
{
class Observer
{
public:
    uint32_t publish(std::string message, std::any value);
    void subscribe(std::string message, std::function<void(std::any)>);
    void unsubscribe(uint32_t id);
};
} // namespace Nando2Tetorisu::common

#endif