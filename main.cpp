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

    const io::InputReader ir{input};
    const io::StatReader sr{std::cout};
    TransportCatalogue tc{io::Load(ir)};

    io::ProcessRequest(tc, ir, sr);

    return 0;
}
