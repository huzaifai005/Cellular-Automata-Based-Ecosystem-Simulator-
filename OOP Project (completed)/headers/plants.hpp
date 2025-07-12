// plant.h
#ifndef PLANT_H
#define PLANT_H

#include "entity.hpp"

// Forward declarations
class Grid;
struct MonthlyStats;

// Represents plant entities in the simulation.
class Plant : public Entity
{
private:
    int baseSpreadChanceValue;
    int maximumAge;
    int currentAge;
    const int winterDeathChanceRate; 
    const int autumnDeathChanceRate; 

public:
    // Plant constructor.
    Plant(int r_coord, int c_coord);
    
    // Gets the species name ("Plant").
    std::string getSpeciesName() const override;
    // Updates the plant's state for the current month.
    void update(Grid &grid, MonthlyStats &stats, Season currentSeason) override;

    // Getters
    int getBaseSpreadChance() const;
    int getMaxAgePlant() const; // Renamed to avoid conflict if Animal also had getMaxAge
    int getCurrentAgePlant() const; // Renamed for clarity

    // Setters (if any are needed externally, most are internal)
    // void setBaseSpreadChance(int chance);
};

#endif // PLANT_H