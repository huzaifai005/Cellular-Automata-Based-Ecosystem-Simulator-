// entity.h
#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <memory> // For std::shared_ptr (used in derived classes and Grid)
#include "constants.hpp" // For EntityType, Gender, Season
// Forward declaration
struct MonthlyStats; 
class Grid;


// Base class for all entities in the simulation.
class Entity
{
private:
    int r_coord;
    int c_coord;
    EntityType entityType;
    char displaySymbol;
    bool is_alive;

public:
    // Entity constructor.
    Entity(int r_val, int c_val, EntityType type_val, char symbol_val);
    // Virtual destructor for proper cleanup of derived classes.
    virtual ~Entity() = default;

    // Pure virtual function for updating entity state.
    virtual void update(Grid &grid, MonthlyStats &stats, Season currentSeason) = 0;
    
    // Getters
    int getR() const;
    int getC() const;
    EntityType getType() const;
    char getSymbol() const;
    bool isAlive() const;

    // Setters/Modifiers
    void setR(int r_val);
    void setC(int c_val);
    void kill(); // Marks the entity as not alive.
    
    // Gets the gender of the entity (default for non-animals).
    virtual Gender getGender() const;
    // Pure virtual function to get the species name.
    virtual std::string getSpeciesName() const = 0;
};

#endif // ENTITY_H