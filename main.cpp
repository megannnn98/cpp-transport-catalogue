#include <iostream>
#include <cassert>
#include <sstream>
#include "geo.h"
#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

using namespace std;


int main()
{
//    std::istringstream input{
//        "10\n"
//        "Stop Tolstopaltsevo: 55.611087, 37.208290\n"
//        "Stop Marushkino: 55.595884, 37.209755\n"
//        "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n"
//        "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka\n"
//        "Stop Rasskazovka: 55.632761, 37.333324\n"
//        "Stop Biryulyovo Zapadnoye: 55.574371, 37.651700\n"
//        "Stop Biryusinka: 55.581065, 37.648390\n"
//        "Stop Universam: 55.587655, 37.645687\n"
//        "Stop Biryulyovo Tovarnaya: 55.592028, 37.653656\n"
//        "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164\n"
//        "3\n"
//        "Bus 256\n"
//        "Bus 750\n"
//        "Bus 751"};

//    std::istringstream input{
//        "3\n"
//        "Stop 97ChPsiaFJ xk: 38.590881, 34.724362\n"
//        "Stop nhfM4: 38.653078, 34.910424\n"
//        "Bus pUFj82Cqn3: 97ChPsiaFJ xk - nhfM4\n"
//        "1\n"
//        "Bus pUFj82Cqn3\n"};
//    std::istringstream input{
//        "3\n"
//        "Stop A: 55.611087, 37.20829\n"
//        "Stop B: 55.595884, 37.209755\n"
//        "Bus B1: A - B\n"
//        "1\n"
//        "Bus B1\n"};


    auto& input = std::cin;

    input::InputReader ir{input};
    input::InputReadParser parser{};
    output::StatReader sr{std::cout};
    TransportCatalogue tc{Load(ir)};

    output::ProcessRequest(tc, ir, parser, sr);

    return 0;
}
