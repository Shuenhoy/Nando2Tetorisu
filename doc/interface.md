example:
```c++
// in view
observer.publish("load_chip", std::make_any("C:\\a.hdl"));
// in viewmodel
observer.subscribe("load_chip", [](auto &&p){
    auto path = std::any_cast<std::string>(p);
    // ...
});


// in viewmodel
observer.publish("set_pin_value", std::make_tuple("input1", 123));
// in view
```
## Command


* `load_chip(string path)`
* `load_script(string path)`
* `edit_hdl(string new_content)`
* `change_animate(AnimateType type)`
* `change_view(ViewType type)`
* `change_speed(int speed)`
* `save_content()`
* `save_script()`
* `single_step()`
* `run()`
* `pause()`
* `reset()`
* `change_input_pin(string pin_name, uint32_t value)`
* `eval()`

## Notify
```c++
using iter=unorded_set<string>::iterator;
```

* `set_input_pins(iter begin, iter end)`
* `set_internal_pins(iter begin, iter end)`
* `set_output_pins(iter begin, iter end)`
* `set_pin_value(string pin, uint32_t value)`
* `set_pin_highlight(string pin)`
* `unset_pin_hightlight()`
* `set_script_highlight(int position)`
* `set_hdl(string content)`
* `set_script(string content)`
* `set_error_message(string message_info)`