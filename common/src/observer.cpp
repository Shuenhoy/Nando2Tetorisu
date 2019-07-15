#include <observer.hpp>

namespace Nando2Tetorisu::common
{
void Observer::publish(std::string message, std::any value)
{
    auto &&c = callbacks.find(message);
    if (c != callbacks.end())
    {
        for (auto &&vv : c->second)
        {
            vv(value);
        }
    }
}
void Observer::subscribe(std::string message, action callback)
{
    auto &&c = callbacks.find(message);
    if (c != callbacks.end())
    {
        c->second.push_back(callback);
    }
    else
    {
        callbacks[message] = {callback};
    }
}
} // namespace Nando2Tetorisu::common