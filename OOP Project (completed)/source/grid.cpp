// grid.cpp
#include "../headers/grid.hpp"
#include "../headers/entity.hpp"
#include "../headers/plants.hpp"
#include "../headers/herbivore.hpp"
#include "../headers/carnivore.hpp"
#include "../headers/animal.hpp"
#include "../headers/utils.hpp"
#include "../headers/monthlyStats.hpp"

// Grid constructor.
Grid::Grid() : cells_grid(GRID_HEIGHT, std::vector<std::shared_ptr<Entity>>(GRID_WIDTH, nullptr)) {}

// Checks if given coordinates are within grid boundaries.
bool Grid::isValid(int r_coord, int c_coord) const
{
    return r_coord >= 0 && r_coord < GRID_HEIGHT && c_coord >= 0 && c_coord < GRID_WIDTH;
}

// Checks if a cell at given coordinates is empty.
bool Grid::isEmpty(int r_coord, int c_coord) const
{
    return isValid(r_coord, c_coord) && cells_grid[r_coord][c_coord] == nullptr;
}

// Retrieves the entity at given coordinates.
std::shared_ptr<Entity> Grid::getEntity(int r_coord, int c_coord) const
{
    if (isValid(r_coord, c_coord))
        return cells_grid[r_coord][c_coord];
    return nullptr;
}

// Adds an entity to the grid.
bool Grid::addEntity(const std::shared_ptr<Entity> &entity)
{
    if (!entity)
        return false;
    // Check against MAX_POPULATION using the sum of current list sizes
    if (plants_list.size() + herbivores_list.size() + carnivores_list.size() >= (size_t)MAX_POPULATION) 
        return false; 
    
    if (isValid(entity->getR(), entity->getC()) && isEmpty(entity->getR(), entity->getC()))
    {
        cells_grid[entity->getR()][entity->getC()] = entity;
        if (entity->getType() == EntityType::PLANT)
            plants_list.push_back(std::static_pointer_cast<Plant>(entity));
        else if (entity->getType() == EntityType::HERBIVORE)
            herbivores_list.push_back(std::static_pointer_cast<Herbivore>(entity));
        else if (entity->getType() == EntityType::CARNIVORE)
            carnivores_list.push_back(std::static_pointer_cast<Carnivore>(entity));
        return true;
    }
    return false;
}

// Adds a migrating animal to a random empty cell.
bool Grid::addMigratingAnimal(const std::shared_ptr<Animal> &animal_ptr)
{
    if (plants_list.size() + herbivores_list.size() + carnivores_list.size() >= (size_t)MAX_POPULATION)
        return false;
    for (int attempts = 0; attempts < GRID_WIDTH * GRID_HEIGHT; ++attempts)
    {
        int r_coord = getRandomInt(0, GRID_HEIGHT - 1);
        int c_coord = getRandomInt(0, GRID_WIDTH - 1);
        if (isEmpty(r_coord, c_coord))
        {
            animal_ptr->setR(r_coord); // Use setter
            animal_ptr->setC(c_coord); // Use setter
            return addEntity(animal_ptr); 
        }
    }
    return false; 
}

// Removes an entity from the grid and lists.
void Grid::removeEntity(const std::shared_ptr<Entity> &entity_ptr, MonthlyStats &stats)
{
    if (!entity_ptr || !isValid(entity_ptr->getR(), entity_ptr->getC()))
        return;
    
    if (cells_grid[entity_ptr->getR()][entity_ptr->getC()] == entity_ptr)
        cells_grid[entity_ptr->getR()][entity_ptr->getC()] = nullptr;
    
    entity_ptr->kill(); 
    
    if (entity_ptr->getType() == EntityType::PLANT)
    {
        plants_list.erase(std::remove_if(plants_list.begin(), plants_list.end(),
                                      [&](const auto &p)
                                      { return !p->isAlive() || p == entity_ptr; }),
                         plants_list.end());
    }
    else if (entity_ptr->getType() == EntityType::HERBIVORE)
    {
        herbivores_list.erase(std::remove_if(herbivores_list.begin(), herbivores_list.end(),
                                          [&](const auto &h)
                                          { return !h->isAlive() || h == entity_ptr; }),
                         herbivores_list.end());
    }
    else if (entity_ptr->getType() == EntityType::CARNIVORE)
    {
        carnivores_list.erase(std::remove_if(carnivores_list.begin(), carnivores_list.end(),
                                          [&](const auto &c_ptr_lambda) 
                                          { return !c_ptr_lambda->isAlive() || c_ptr_lambda == entity_ptr; }),
                         carnivores_list.end());
    }
}

// Moves an entity from its current position to new coordinates.
void Grid::moveEntity(const std::shared_ptr<Entity> &entity_ptr, int newR, int newC)
{
    if (!entity_ptr)
        return;
    if (isValid(entity_ptr->getR(), entity_ptr->getC()) && cells_grid[entity_ptr->getR()][entity_ptr->getC()] == entity_ptr)
    {
        cells_grid[entity_ptr->getR()][entity_ptr->getC()] = nullptr;
    }
    entity_ptr->setR(newR); // Use setter
    entity_ptr->setC(newC); // Use setter
    
    if (isValid(newR, newC))
        cells_grid[newR][newC] = entity_ptr;
    else
        entity_ptr->kill();
}

// Displays the current state of the grid.
void Grid::display() const
{
    std::cout << std::setw(5) << " "; 
    for (int j = 0; j < GRID_WIDTH; ++j)
        std::cout << std::setw(2) << std::left << j;
    std::cout << std::endl;
    std::cout << std::setw(5) << " ";
    for (int j = 0; j < GRID_WIDTH; ++j)
        std::cout << "--";
    std::cout << std::endl;

    for (int i = 0; i < GRID_HEIGHT; ++i)
    {
        std::cout << std::setw(2) << std::right << i << " | ";
        for (int j = 0; j < GRID_WIDTH; ++j)
        {
            char displaySymbolToPrint = '*'; 
            if (cells_grid[i][j])
            {
                // Use getters to access entity properties
                if (cells_grid[i][j]->getType() == EntityType::PLANT)
                    displaySymbolToPrint = cells_grid[i][j]->getSymbol(); // Or just 'P'
                else if (cells_grid[i][j]->getType() == EntityType::HERBIVORE)
                    displaySymbolToPrint = cells_grid[i][j]->getSymbol(); // Symbol is set based on gender in constructor
                else if (cells_grid[i][j]->getType() == EntityType::CARNIVORE)
                    displaySymbolToPrint = cells_grid[i][j]->getSymbol();
            }
            std::cout << displaySymbolToPrint << " ";
        }
        std::cout << std::endl;
    }
}

// Gets a list of empty cells adjacent to given coordinates.
std::vector<std::pair<int, int>> Grid::getAdjacentEmptyCells(int r_coord, int c_coord) const
{
    std::vector<std::pair<int, int>> emptyCells_vec;
    int dr_arr[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dc_arr[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    for (int i = 0; i < 8; ++i)
    {
        int nr = r_coord + dr_arr[i];
        int nc = c_coord + dc_arr[i];
        if (isEmpty(nr, nc)) // isEmpty uses isValid internally
            emptyCells_vec.push_back({nr, nc});
    }
    return emptyCells_vec;
}

// Finds entities of a specific type within a given range of coordinates.
std::vector<std::shared_ptr<Entity>> Grid::findNearbyEntities(int r_coord, int c_coord, EntityType targetType, int range_val) const
{
    std::vector<std::shared_ptr<Entity>> found_entities; // Renamed to avoid conflict
    for (int dr_scan = -range_val; dr_scan <= range_val; ++dr_scan)
    {
        for (int dc_scan = -range_val; dc_scan <= range_val; ++dc_scan)
        {
            if (dr_scan == 0 && dc_scan == 0)
                continue;
            int nr = r_coord + dr_scan;
            int nc = c_coord + dc_scan;
            // getEntity performs isValid check
            std::shared_ptr<Entity> current_entity = getEntity(nr, nc); 
            if (current_entity && current_entity->getType() == targetType && current_entity->isAlive())
            {
                found_entities.push_back(current_entity);
            }
        }
    }
    return found_entities;
}

// Getters for entity lists
// Returns a const reference to the list of plants.
const std::vector<std::shared_ptr<Plant>>& Grid::getPlants() const { return plants_list; }
// Returns a const reference to the list of herbivores.
const std::vector<std::shared_ptr<Herbivore>>& Grid::getHerbivores() const { return herbivores_list; }
// Returns a const reference to the list of carnivores.
const std::vector<std::shared_ptr<Carnivore>>& Grid::getCarnivores() const { return carnivores_list; }