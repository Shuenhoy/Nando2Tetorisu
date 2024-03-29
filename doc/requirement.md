# 总体目标
实现HDL语言中组合逻辑部分的模拟与仿真，支持显示变量、单步仿真等功能。

# 系统需求
Windows 7/10 64位。

# 功能分析
0. **界面**
    - 包括菜单栏、代码编辑区、调试区、设计图区、输出区。
    - 用户通过菜单栏进行新建、读取、保存HDL文件或仿真文件的操作等，还有调试、仿真等功能
    - 代码编辑区显示HDL文件代码。
    - 调试区显示跟踪信号的值，并可跟踪当前执行的仿真语句。
    - 设计图区用于显示HDL文件生成的设计图（只读）。
    - 输出区显示软件在编译时遇到的问题，如代码文件的警告、错误，仿真是否正确执行的信息等。

1. **编辑**
    - 在软件内实现代码的实时编辑与显示。

2. **代码分析**
   - 用户选择“生成代码”以后对代码进行初步的编译，并对语法和语义问题提出错误或警告。
   - 支持组合逻辑模块。

3. **软件内调试与运行**
   - 在逻辑无误的情况下，程序可根据HDL文件生成门级的设计图。
   - 在逻辑无误的情况下，用户可在HDL文件的基础上提供输入信号，由程序直接返回输出信号。
   - 在逻辑无误的情况下，用户可提供仿真文件，软件对仿真文件的正确性进行检查并提出警告与错误。
   - 在仿真文件也无误的情况下，程序可尝试根据组合逻辑与仿真文件进行调试并输出变量属性。也支持仿真文件的单步仿真。
   - 在单步仿真的时候，程序会通过简单的动画展现当前语句修改了哪些信号的值。