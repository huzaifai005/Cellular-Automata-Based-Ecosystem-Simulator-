// grid.h
#ifndef GRID_H
#define GRID_H

#include <vector>
#include <algorithm>
#include <memory> // For std::shared_ptr
#include <string> // For display/events (indirectly via MonthlyStats)
#include <iomanip> // For display formatting
#include "constants.hpp" // For GRID_WIDTH, GRID_HEIGHT, EntityType, etc.

// Forward declarations
class Entity;
class Plant;
class Herbivore;
class Carnivore;
class Animal; // Animal needed for addMigratingAnimal
struct MonthlyStats;

// Manages the simulation grid and entities within it.
class Grid
{
private:
    std::vector<std::vector<std::shared_ptr<Entity>>> cells_grid; // Renamed to avoid conflict
    std::vector<std::shared_ptr<Plant>> plants_list; // Renamed
    std::vector<std::shared_ptr<Herbivore>> herbivores_list; // Renamed
    std::vector<std::shared_ptr<Carnivore>> carnivores_list; // Renamed

public:
    // Grid constructor.
    Grid();
    
    // Checks if given coordinates are within grid boundaries.
    bool isValid(int r_coord, int c_coord) const;
    // Checks if a cell at given coordinates is empty.
    bool isEmpty(int r_coord, int c_coord) const;
    // Retrieves the entity at given coordinates.
    std::shared_ptr<Entity> getEntity(int r_coord, int c_coord) const;
    
    // Adds an entity to the grid.
    bool addEntity(const std::shared_ptr<Entity> &entity);
    // Adds a migrating animal to a random empty cell.
    bool addMigratingAnimal(const std::shared_ptr<Animal> &animal_ptr);
    // Removes an entity from the grid and lists.
    void removeEntity(const std::shared_ptr<Entity> &entity_ptr, MonthlyStats &stats);
    // Moves an entity from its current position to new coordinates.
    void moveEntity(const std::shared_ptr<Entity> &entity_ptr, int newR, int newC);
    
    // Displays the current state of the grid.
    void display() const;
    // Gets a list of empty cells adjacent to given coordinates.
    std::vector<std::pair<int, int>> getAdjacentEmptyCells(int r_coord, int c_coord) const;
    // Finds entities of a specific type within a given range of coordinates.
    std::vector<std::shared_ptr<Entity>> findNearbyEntities(int r_coord, int c_coord, EntityType targetType, int range_val) const;

    // Getters for entity lists (const reference to avoid modification)
    const std::vector<std::shared_ptr<Plant>>& getPlants() const;
    const std::vector<std::shared_ptr<Herbivore>>& getHerbivores() const;
    const std::vector<std::shared_ptr<Carnivore>>& getCarnivores() const;
};

#endif // GRID_H