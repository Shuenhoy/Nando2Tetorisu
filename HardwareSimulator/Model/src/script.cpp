#include <script.hpp>
#include <sstream>
#include <peglib.h>
namespace Nando2Tetorisu::HardwareSimulator
{

peg::parser create_script_parser()
{
    using namespace peg;

    auto grammar = R"(
        ROOT <- ( _ (COMMENT / COMMAND) _ )*

        ~_ <- [ \t\r\n]*
        COMMAND <- (INIT / EVAL) ';'
        INIT <- LOAD _ ',' _ OUTPUT _ ',' _ COMPARE _ ',' _ OUTPUT_LIST
        LOAD <- 'load' _ FILENAME
        OUTPUT <- 'output-file' _ FILENAME
        COMPARE <- 'compare-to' _ FILENAME
        OUTPUT_LIST <- 'output-list'  ( ' '+  ITEM)+
        ITEM <- ID '%B' INT '.' INT '.' INT
        INT <- [0-9]+

        EVAL <- ( SET _ ',' _ )* 'eval' _ ',' _ 'output'
        SET <- 'set ' ID ' ' LIT

        LIT <- INT / BIN
        BIN <- '%B' [01]+

        FILENAME <- (![;,] .)+
        End                  <-  EndOfLine / EndOfFile
        EndOfLine            <-  '\r\n' / '\n' / '\r'
        EndOfFile            <-  !.
        ID <- [a-zA-Z] [a-zA-Z0-9]*
        ~COMMENT             <-  ('#' / '//') (!End .)* &End
    )";
    parser parser;
    auto ok = parser.load_grammar(grammar);
    parser.log = [](size_t line, size_t col, const std::string &msg) {
        std::stringstream ss;
        ss << line << ":" << col << ": " << msg;
        throw ss.str();
    };
    assert(ok);
    parser["ROOT"] = [](const SemanticValues &sv) {
        std::vector<ScriptCommand> sets;
        std::transform(sv.begin(), sv.end(), std::back_inserter(sets), [](const any &o) {
            return o.get<ScriptCommand>();
        });
        return sets;
    };
    parser["COMMAND"] = [](const SemanticValues &sv) {
        return sv[0].get<ScriptCommand>();
    };
    parser["INIT"] = [](const SemanticValues &sv) {
        std::string load = sv[0].get<std::string>(), output = sv[1].get<std::string>(), compare = sv[2].get<std::string>();
        auto output_lists = sv[3].get<std::vector<std::tuple<std::string, uint32_t, uint32_t, uint32_t>>>();
        return ScriptCommand(sv.line_info().first, ScriptInit(load, output, compare, std::move(output_lists)));
    };
    parser["OUTPUT_LIST"] = [](const SemanticValues &sv) {
        std::vector<std::tuple<std::string, uint32_t, uint32_t, uint32_t>> sets;
        std::transform(sv.begin(), sv.end(), std::back_inserter(sets), [](const any &o) {
            return o.get<std::tuple<std::string, uint32_t, uint32_t, uint32_t>>();
        });
        return sets;
    };
    parser["ITEM"] = [](const SemanticValues &sv) {
        return std::make_tuple(sv[0].get<std::string>(), sv[1].get<uint32_t>(),
                               sv[2].get<uint32_t>(), sv[3].get<uint32_t>());
    };
    parser["SET"] = [](const SemanticValues &sv) {
        return std::make_pair(sv[0].get<std::string>(), sv[1].get<uint32_t>());
    };
    parser["EVAL"] = [](const SemanticValues &sv) {
        std::vector<std::pair<std::string, uint32_t>> sets;
        std::transform(sv.begin(), sv.end(), std::back_inserter(sets), [](const any &o) {
            return o.get<std::pair<std::string, uint32_t>>();
        });
        return ScriptCommand(sv.line_info().first, ScriptEval(std::move(sets)));
    };

    parser["ID"] = [](const SemanticValues &sv) {
        return sv.str();
    };
    parser["INT"] = [](const SemanticValues &sv) {
        uint32_t val = std::stol(sv.str(), nullptr, 10);
        return val;
    };
    parser["BIN"] = [](const SemanticValues &sv) {
        uint32_t val = std::stol(sv.str().substr(2), nullptr, 2);
        return val;
    };
    parser["LIT"] = [](const SemanticValues &sv) {
        return sv[0].get<uint32_t>();
    };

    parser["FILENAME"] = [](const SemanticValues &sv) {
        return sv.str();
    };
    parser["LOAD"] = [](const SemanticValues &sv) {
        return sv[0].get<std::string>();
    };
    parser["OUTPUT"] = [](const SemanticValues &sv) {
        return sv[0].get<std::string>();
    };
    parser["COMPARE"] = [](const SemanticValues &sv) {
        return sv[0].get<std::string>();
    };

    return parser;
}
void ScriptModel::load_script(std::string path)
{
    std::size_t botDirPos = path.find_last_of("/\\");
    parent = path.substr(0, botDirPos);
    filename = path.substr(botDirPos, path.length());
    std::ifstream fin(path);
    if (fin.fail())
    {
        throw std::string("cannot open file: " + path);
    }
    source = std::string((std::istreambuf_iterator<char>(fin)),
                         std::istreambuf_iterator<char>());
    auto parser = create_script_parser();
    parser.enable_packrat_parsing();

    auto ok = parser.parse(source.c_str(), commands, nullptr);
    pc = 0;
    loaded = true;
}
} // namespace Nando2Tetorisu::HardwareSimulator
