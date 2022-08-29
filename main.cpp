#include <iostream>
#include <cassert>
#include <sstream>
#include "geo.h"
#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

int main()
{
    auto& input = std::cin;

    input::InputReader ir{input};
    input::InputReadParser parser{};
    output::StatReader sr{std::cout};
    TransportCatalogue tc{input::Load(ir)};

    output::ProcessRequest(tc, ir, parser, sr);

    return 0;
}
