#include <string>
#include <vector>

#include "Logger.hpp"
#include "Parser.hpp"
#include "Timer.hpp"

//int main(int argc, char* argv[]) {
//    if (argc < 2) {
//        logger.error() << "Expected at least one argument";
//        return 1;
//    }
//    std::string funcStr = argv[1];
//    std::vector<float> args;
//    for (int i = 2; i < argc; i++) {
//        args.push_back(std::stof(std::string(argv[i])));
//    }
//    if (args.size() == 0) {
//        auto func = script::parse<>(funcStr);
//        logger.info() << func();
//    }
//    else if (args.size() == 1) {
//        auto func = script::parse<float>(funcStr);
//        logger.info() << func(args[0]);
//    }
//    else if (args.size() == 2) {
//        auto func = script::parse<float, float>(funcStr);
//        logger.info() << func(args[0], args[1]);
//    }
//    else if (args.size() == 3) {
//        auto func = script::parse<float, float, float>(funcStr);
//        logger.info() << func(args[0], args[1], args[2]);
//    }
//    else {
//        logger.error() << "Can only provide functions with a maximum of 3 arguments";
//        return 1;
//    }
//    return 0;
//}

using namespace std::string_view_literals;

constexpr auto NUM = 4;
constexpr auto TIMES = 1000000;

int main() {
    float data[NUM] = { 0 };
    auto func = script::parse<float, float>("(a){a*1.1}"sv);
    Timer timer;
    
    timer.start();
    for (size_t i = 0; i < TIMES; i++) {
        data[(i + 1) % NUM] = func(data[i % NUM]);
    }
    timer.stop();
    logger.info() << "Script: " << timer.elapsed().get() << "s";
    logger.trace() << data[0];

    timer.start();
    for (size_t i = 0; i < TIMES; i++) {
        data[(i + 1) % NUM] = data[i % NUM] * 1.1f;
    }
    timer.stop();
    logger.info() << "Raw: " << timer.elapsed().get() << "s";
    logger.trace() << data[0];

    return 0;
}
