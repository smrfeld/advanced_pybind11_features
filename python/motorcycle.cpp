#include "../cpp/include/automobile_bits/motorcycle.hpp"

#include <pybind11/stl.h>

#include <pybind11/pybind11.h>
namespace py = pybind11;

namespace autos {

class PhotographTrampoline : public Photograph {

public:
    
    using Photograph::Photograph;
    
    bool is_beautiful(std::shared_ptr<Motorcycle> bike) const override {
        PYBIND11_OVERLOAD_PURE(
            bool, /* Return type */
            Photograph,      /* Parent class */
            is_beautiful,          /* Name of function in C++ (must match Python name) */
            bike   /* args */
        );
    }
};

}

void init_motorcycle(py::module &m) {
    
    // Motorcycle
    py::class_<autos::Motorcycle, std::shared_ptr<autos::Motorcycle>>(m, "Motorcycle")
    .def(py::init<std::string>(), py::arg("name"))
    .def_static("create",
         py::overload_cast<std::string>( &autos::Motorcycle::create),
         py::arg("name"))
    .def("get_name",
         py::overload_cast<>( &autos::Motorcycle::get_name, py::const_))
    .def("ride",
         py::overload_cast<std::string>( &autos::Motorcycle::ride, py::const_),
         py::arg("road"))
    .def("get_engine_type", py::overload_cast<>( &autos::Motorcycle::get_engine_type, py::const_ ));
    
    // Photograph
    
    // Does not compile:
    /*
     py::class_<autos::Photograph>(m, "Photograph")
     .def("is_beautiful", py::overload_cast<std::shared_ptr<autos::Motorcycle>>( &autos::Photograph::is_beautiful, py::const_ ), py::arg("bike"));
     */
    // Compiles but error when extending Photograph in python:
    /*
    py::class_<autos::Photograph>(m, "Photograph")
    .def(py::init<>())
    .def("is_beautiful", py::overload_cast<std::shared_ptr<autos::Motorcycle>>( &autos::Photograph::is_beautiful, py::const_ ), py::arg("bike"));
     */
    // Correct way:
    py::class_<autos::Photograph, autos::PhotographTrampoline>(m, "Photograph")
    .def(py::init<>())
    .def("is_beautiful", py::overload_cast<std::shared_ptr<autos::Motorcycle>>( &autos::Photograph::is_beautiful, py::const_ ), py::arg("bike"));
    
    // Engine type
    py::enum_<autos::EngineType>(m, "EngineType")
    .value("TWO_STROKE", autos::EngineType::TWO_STROKE)
    .value("FOUR_STROKE", autos::EngineType::FOUR_STROKE)
    .export_values();
}
