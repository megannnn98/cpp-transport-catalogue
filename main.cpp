#include <iostream>
#include <cassert>
#include <sstream>
#include "geo.h"
#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"
#include "log_duration.h"

int main()
{
    auto& input = std::cin;

    const io::InputReader ir{input};
    const io::StatReader sr{std::cout};
    const TransportCatalogue tc{io::Load(ir)};

    {
        LOG_DURATION("123");
        io::ProcessRequest(tc, ir, sr);
    }

    return 0;
}
