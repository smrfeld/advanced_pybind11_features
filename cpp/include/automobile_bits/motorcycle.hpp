#include <string>

#ifndef MOTORCYCLE_H
#define MOTORCYCLE_H

namespace autos {

enum EngineType {
    TWO_STROKE = 0,
    FOUR_STROKE = 1
};

class Motorcycle {

private:

    /// Name
    std::string _name;
    
public:
    
    /// Shared pointer constructor
    static std::shared_ptr<Motorcycle> create(std::string name);
    
    /// Constructor
    Motorcycle(std::string name);

    /// Get name
    /// @return Name
    std::string get_name() const;

    /// Ride the bike
    /// @param road Name of the road
    void ride(std::string road) const;
    
    /// Get engine type
    /// @return Engine type
    EngineType get_engine_type() const;
};

class Photograph {
    
public:
    
    /// Constructor/destructor
    virtual ~Photograph() {};
    
    /// Pure virtual method
    /// @param bike Bike
    /// @return true if beautiful
    virtual bool is_beautiful(std::shared_ptr<Motorcycle> bike) const = 0;
};

}

#endif
