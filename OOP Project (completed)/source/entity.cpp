// entity.cpp
#include "../headers/entity.hpp"
// No other includes needed here as Grid and MonthlyStats are forward declared for pointer/reference use in header.

// Entity constructor.
Entity::Entity(int r_val, int c_val, EntityType type_val, char symbol_val)
    : r_coord(r_val), c_coord(c_val), entityType(type_val), displaySymbol(symbol_val), is_alive(true) {}

// Getters
// Gets the row coordinate of the entity.
int Entity::getR() const { return r_coord; }
// Gets the column coordinate of the entity.
int Entity::getC() const { return c_coord; }
// Gets the type of the entity.
EntityType Entity::getType() const { return entityType; }
// Gets the display symbol for the entity.
char Entity::getSymbol() const { return displaySymbol; }
// Checks if the entity is alive.
bool Entity::isAlive() const { return is_alive; }

// Setters/Modifiers
// Sets the row coordinate of the entity.
void Entity::setR(int r_val) { r_coord = r_val; }
// Sets the column coordinate of the entity.
void Entity::setC(int c_val) { c_coord = c_val; }
// Marks the entity as not alive.
void Entity::kill() { is_alive = false; }

// Gets the gender of the entity (default for non-animals).
Gender Entity::getGender() const { return Gender::NONE; }

// Destructor is defaulted in header.
// update is pure virtual.
// getSpeciesName is pure virtual.