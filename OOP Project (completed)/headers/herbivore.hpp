// herbivore.h
#ifndef HERBIVORE_H
#define HERBIVORE_H

#include "animal.hpp"

// Forward declarations
class Grid;
struct MonthlyStats;

// Represents herbivore entities in the simulation.
class Herbivore : public Animal
{
public:
    // Herbivore constructor.
    Herbivore(int r_coord, int c_coord, Gender gender_val);
    
    // Gets the species name ("Herbivore").
    std::string getSpeciesName() const override;
    // Herbivore's attempt to eat.
    bool attemptEat(Grid &grid, MonthlyStats &stats, Season currentSeason) override;
    // Herbivore's movement logic.
    void move(Grid &grid, MonthlyStats &stats, Season currentSeason) override;
    // Herbivore's attempt to reproduce.
    void attemptReproduce(Grid &grid, MonthlyStats &stats, const std::vector<std::shared_ptr<Animal>> &potentialMates, Season currentSeason) override;
    // Herbivore's logic for giving birth.
    void giveBirth(Grid &grid, MonthlyStats &stats) override;
};

#endif // HERBIVORE_H