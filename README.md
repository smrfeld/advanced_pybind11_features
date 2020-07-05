# Advanced pybind11 features

[You can find the corresponding Medium article here.](https://medium.com/practical-coding/three-advanced-pybind11-features-for-wrapping-c-code-into-python-6dbcac169b93)

This repo details three advanced pybind11 features:
* Shared pointers.
* Enum.
* Abstract base classes (ABC) and pure virtual methods.

The starting point for this project is a previous project found [here](https://github.com/smrfeld/cmake_cpp_pybind11_tutorial).

<img src="cover.jpg" alt="drawing" width="400"/>

[Image source](https://commons.wikimedia.org/wiki/File:Sr4002012.jpg).

## Requirements

Obviously, you will need `pybind11`. On Mac:
```
brew install pybind11
```
will do it.

## Setup with CMake

We will start with the `CMake` based setup from a [previous introduction found here](https://github.com/smrfeld/cmake_cpp_pybind11_tutorial). 
Are you gonna check it out? 
Of course not.
No time for that!
Here is the setup:

The directory structure is as follows:
```
cpp/CMakeLists.txt
cpp/include/automobile
cpp/include/automobile_bits/motorcycle.hpp
cpp/src/motorcycle.cpp
python/automobile.cpp
python/automobile.hpp
CMakeLists.txt
```
The idea here is:
1. The inner `cpp` folder contains a `C++` project for a library. It can be built using the `CMakeLists.txt` as follows:
    ```
    cd cpp
    mkdir build
    cd build
    cmake ..
    make
    make install
    ```
2. The outer folder contains the wrapping code in the `python` library, and a second `CMakeLists.txt` for building the `python` library as follows:
    ```
    mkdir build
    cd build
    cmake .. -DPYTHON_LIBRARY_DIR="/path/to/site-packages" -DPYTHON_EXECUTABLE="/path/to/executable/python3"
    make
    make install
    ```
    My paths are:
    ```
    DPYTHON_LIBRARY_DIR="/Users/USERNAME/opt/anaconda3/lib/python3.7/site-packages"
    DPYTHON_EXECUTABLE="/Users/USERNAME/opt/anaconda3/bin/python3"
    ```

I won't review all the files here - you can find them in this repo.

## Shared pointers

`C++` standard 11 introduced shared and unique pointers which do not require manual memory cleanup.
This is highly parallel to Python, where garbage collection is automatic. 
Wrapping shared pointers into Python is therefore only natural.

We will add a static constructor method that returns a `shared_ptr` to a `Motorcycle`. Add to `cpp/include/automobile_bits/motorcycle.hpp` above the constructor:
```
/// Shared pointer constructor
static std::shared_ptr<Motorcycle> create(std::string name);
```
and the implementation in `cpp/src/motorcycle.cpp`:
```
std::shared_ptr<Motorcycle> Motorcycle::create(std::string name) {
    return std::make_shared<Motorcycle>(name);
}
```

There are two parts now: (1) we must allow a `shared_ptr<Motorcycle>` to be accessible by Python, and (2) we need to expose the `create` method.

For the first part, we will modify the glue code in `python/motorcycle.cpp`:
```
// Old:
// py::class_<autos::Motorcycle>(m, "Motorcycle")
// New:
py::class_<autos::Motorcycle, std::shared_ptr<autos::Motorcycle>>(m, "Motorcycle")
```
For the second part, to wrap the static method, we will also add:
```
.def_static("create",
        py::overload_cast<std::string>( &autos::Motorcycle::create),
        py::arg("name"))
```
Notice that we used `def_static` instead of `def` for a static method.

Build and install the library as before. The test `python` code:
```
import automobile
bike = automobile.Motorcycle.create("yamaha")
bike.ride("mullholland")
```
works as expected with output:
```
Zoom Zoom on road: mullholland
```

## Enum

Enum are great for setting flags or options in a more verbose way than simply `true/false` or `1/2/3/4...`. They are supported in both `Python` and `C++`.

Let's create an enum in `C++`. In the header `cpp/include/automobile_bits/motorcycle.hpp` add:
```
enum EngineType {
    TWO_STROKE = 0,
    FOUR_STROKE = 1
};
```
above the `Motorcycle` class, as well as the public method of the `Motorcycle` class:
```
/// Get engine type
/// @return Engine type
EngineType get_engine_type() const;
```
and it's implementation in `cpp/src/motorcycle.cpp`:
```
EngineType Motorcycle::get_engine_type() const {
    return EngineType::TWO_STROKE;
}
```

Add the following glue code in `python/motorcycle.cpp` **below** the `Motorcycle` wrapper:
```
py::enum_<autos::EngineType>(m, "EngineType")
.value("TWO_STROKE", autos::EngineType::TWO_STROKE)
.value("FOUR_STROKE", autos::EngineType::FOUR_STROKE)
.export_values();
```
and expose the method in the `Motorcycle` class:
```
.def("get_engine_type", py::overload_cast<>( &autos::Motorcycle::get_engine_type, py::const_ ))
```

Finally, the proof in Python is:
```
import automobile
bike = automobile.Motorcycle("yamaha")
bike.get_engine_type()
```
returns
```
EngineType.TWO_STROKE
```

## Abstract base classes

Python also has an `abc` module that is entirely underused. 
The basic principles of abstract base classes will translate nicely from `C++` to `Python`, but as we shall see some behavior is missing.

Add to the header `cpp/include/automobile_bits/motorcycle.hpp`:
```
class Photograph {
    
public:
    
    /// Constructor/destructor
    virtual ~Photograph() {};
    
    /// Pure virtual method
    /// @param bike Bike
    /// @return true if beautiful
    virtual bool is_beautiful(std::shared_ptr<Motorcycle> bike) const = 0;
};
```
and of course, no implementation for `is_beautiful` (although you **could** have one!).

What happens when we try to add the glue code in `python/motorcycle.cpp`? If we try:
```
py::class_<autos::Photograph>(m, "Photograph")
.def(py::init<>())
.def("is_beautiful", py::overload_cast<std::shared_ptr<autos::Motorcycle>>( &autos::Photograph::is_beautiful, py::const_ ), py::arg("bike"));
```
we'll get the error
```
Allocating an object of abstract class type 'autos::Photograph'
```
Uh oh! It looks like it is unhappy with the constructor. Of course, we could simply eliminate the constructor:
```
py::class_<autos::Photograph>(m, "Photograph")
.def("is_beautiful", py::overload_cast<std::shared_ptr<autos::Motorcycle>>( &autos::Photograph::is_beautiful, py::const_ ), py::arg("bike"));
```
This compiles - but now consider the following example in Python:
```
import automobile

class YamahaPhoto(automobile.Photograph):
    
    def __init__(self):
        super().__init__()
        
    def is_beautiful(self, bike):
        return True

bike = automobile.Motorcycle.create("yamaha")

photo = YamahaPhoto()
print(photo.is_beautiful(bike))
```
This gives the error:
```
TypeError: YamahaPhoto: No constructor defined!
```
because of course, we deleted the constructor! So abstract base classes are no longer extensible.

The solution is to define what `pybind11` refers to as a "trampoline" class. In `python/motorcycle.cpp`, define the trampoline at the top:
```
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
```
and change the glue code to:
```
py::class_<autos::Photograph, autos::PhotographTrampoline>(m, "Photograph")
.def(py::init<>())
.def("is_beautiful", py::overload_cast<std::shared_ptr<autos::Motorcycle>>( &autos::Photograph::is_beautiful, py::const_ ), py::arg("bike"));
```
Notice here the order in `py::class_<autos::Photograph, autos::PhotographTrampoline>` -  first the parent class (the ABC), then the trampoline.
Everywhere else, we use just the name of the ABC, i.e. `Photograph::is_beautiful`, not `PhotographTrampoline::is_beautiful`.
Now we could also add the constructor without an error.

The python example will now run and produce a resounding `True`.

A limitation here is that the `Photograh` class in `Python` is no longer an abstract base class. That means, we can actually run the following:
```
import automobile

class YamahaPhoto(automobile.Photograph):
    
    def __init__(self):
        super().__init__()

bike = automobile.Motorcycle.create("yamaha")

photo = YamahaPhoto()
```
which will construct a `YamahaPhoto` object, despite the fact that we did not implement the `is_beautiful` method.
This is unfortunate, as it breaks some of the design principles enforced in `C++`. At the moment, it seems we just cannot have everything - but maybe one day!

## Final thoughts

That's three advanced features of `pybind11` - some things are not so obvious, but it seems just about everything is possible!

