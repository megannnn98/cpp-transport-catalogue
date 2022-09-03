#include <iostream>
#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

int main()
{
    const tc::io::InputReader ir{std::cin};
    const tc::io::StatReader sr{std::cout};
    const tc::TransportCatalogue tc{tc::io::Load(ir)};

    tc::io::ProcessRequest(tc, ir, sr);

    return 0;
}
