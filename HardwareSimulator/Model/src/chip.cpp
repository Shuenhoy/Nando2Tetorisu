#include <peglib.h>
#include <string>
#include <cassert>
#include <chip.hpp>
#include <variant>
#include <algorithm>
#include <utility>
#include <sstream>

namespace Nando2Tetorisu::HardwareSimulator
{
peg::parser create_chip_parser()
{
    using namespace peg;
    auto grammar = R"(
ROOT <- _ CHIP _
        CHIP <- 'CHIP' _ ID _ '{' _ IN _ OUT _ PARTS _ '}'
        IN <- 'IN' _ PINDECL _ ( _ ',' _  PINDECL _ )* _ ';'
        OUT <- 'OUT' _ PINDECL _ ( _ ',' _  PINDECL _ )* _ ';'
        PARTS <- 'PARTS:' _ ( _ PART _ )*
        PART <- ID '(' _ ARG _ ( _ ',' _ ARG _ )* ')' ';'
        ARG <- (PIN _ '=' _ PIN) / (PIN _ '=' _ INT)
        PIN <- ID BUS?
        PINDECL <- ID BUS?
        BUS <- '[' INT ']'
        INT <- [0-9]+
        ID <- [a-zA-Z] [a-zA-Z0-9]*
        ~COMMENT <- '/*' (!'*/' .)* '*/'
        ~_ <- [ \t\n]* COMMENT* [ \t\n]*)";

    parser parser;
    auto ok = parser.load_grammar(grammar);
    parser.log = [](size_t line, size_t col, const std::string &msg) {
        std::stringstream ss;
        ss << line << ":" << col << ": " << msg;
        throw ss.str();
    };
    assert(ok);
    parser["ID"] = [](const SemanticValues &sv) {
        return sv.str();
    };
    parser["INT"] = [](const SemanticValues &sv) {
        uint32_t val = std::stol(sv.str(), nullptr, 10);
        return val;
    };
    parser["BUS"] = [](const SemanticValues &sv) {
        uint32_t val = sv[0].get<uint32_t>();
        return val;
    };
    parser["PIN"] = [](const SemanticValues &sv) {
        pin val = pin{sv[0].get<std::string>(), sv.size() < 2 ? -1 : sv[1].get<uint32_t>()};
        return val;
    };
    parser["PINDECL"] = [](const SemanticValues &sv) {
        pin val = pin{sv[0].get<std::string>(), sv.size() < 2 ? 0 : sv[1].get<uint32_t>()};
        return val;
    };
    parser["ARG"] = [](const SemanticValues &sv) {
        return assignment{
            sv[0].get<pin>(), sv.choice() == 0
                                  ? assignment_arg{sv[1].get<pin>()}
                                  : assignment_arg{sv[1].get<uint32_t>()}};
    };
    parser["PART"] = [](const SemanticValues &sv) {
        std::string name = sv[0].get<std::string>();
        std::vector<assignment> args;
        std::transform(std::next(sv.begin(), 1), sv.end(), std::back_inserter(args), [](const any &a) {
            return a.get<assignment>();
        });
        return ChipPart(name, std::move(args));
    };
    parser["PARTS"] = [](const SemanticValues &sv) {
        std::vector<ChipPart> parts;
        std::transform(sv.begin(), sv.end(), std::back_inserter(parts), [](const any &a) {
            return std::move(a.get<ChipPart>());
        });
        return parts;
    };
    parser["IN"] = [](const SemanticValues &sv) {
        std::vector<pin> ins;
        std::transform(sv.begin(), sv.end(), std::back_inserter(ins), [](const any &a) {
            return a.get<pin>();
        });
        return ins;
    };
    parser["OUT"] = [](const SemanticValues &sv) {
        std::vector<pin> outs;
        std::transform(sv.begin(), sv.end(), std::back_inserter(outs), [](const any &a) {
            return a.get<pin>();
        });
        return outs;
    };
    parser["CHIP"] = [](const SemanticValues &sv) {
        std::string name = sv[0].get<std::string>();
        std::vector<pin> ins = std::move(sv[1].get<std::vector<pin>>()),
                         outs = std::move(sv[2].get<std::vector<pin>>());
        std::vector<ChipPart> parts = std::move(sv[3].get<std::vector<ChipPart>>());
        return ChipBluePrint(parts, name, ins, outs);
    };
    parser["ROOT"] = [](const SemanticValues &sv) {
        return sv[0].get<ChipBluePrint>();
    };
    return parser;
}

ChipBluePrint
parse_hdl(std::string input)
{
    auto parser = create_chip_parser();
    parser.enable_packrat_parsing();

    ChipBluePrint chip;
    auto ok = parser.parse(input.c_str(), chip, nullptr);
    assert(ok);
    return chip;
}
} // namespace Nando2Tetorisu::HardwareSimulator
