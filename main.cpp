#include <cassert>
#include <chrono>
#include <sstream>
#include <string_view>
#include <iostream>

#include "json_reader.h"
#include "request_handler.h"
#include "map_renderer.h"

using namespace json;

int main() {

    json::JsonReader jr{std::cin};
    auto tc{jr.MakeCatalogue()};
    auto rs{jr.MakeRenderSettings()};

    tc::RequestHandler handler{tc, rs};

    jr.Print(handler, std::cout);
}
