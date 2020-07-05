#include "../include/automobile_bits/motorcycle.hpp"

#include <iostream>

namespace autos {

std::shared_ptr<Motorcycle> Motorcycle::create(std::string name) {
    return std::make_shared<Motorcycle>(name);
}

Motorcycle::Motorcycle(std::string name) {
    _name = name;
}

std::string Motorcycle::get_name() const {
    return _name;
}

void Motorcycle::ride(std::string road) const {
    std::cout << "Zoom Zoom on road: " << road << std::endl;
}

EngineType Motorcycle::get_engine_type() const {
    return EngineType::TWO_STROKE;
}

}
