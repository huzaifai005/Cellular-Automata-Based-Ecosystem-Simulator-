// carnivore.h
#ifndef CARNIVORE_H
#define CARNIVORE_H

#include "animal.hpp"

// Forward declarations
class Grid;
struct MonthlyStats;
class Herbivore; // For casting in attemptEat

// Represents carnivore entities in the simulation.
class Carnivore : public Animal
{
public:
    // Carnivore constructor.
    Carnivore(int r_coord, int c_coord, Gender gender_val);

    // Gets the species name ("Carnivore").
    std::string getSpeciesName() const override;
    // Carnivore's attempt to eat.
    bool attemptEat(Grid &grid, MonthlyStats &stats, Season currentSeason) override;
    // Carnivore's movement logic.
    void move(Grid &grid, MonthlyStats &stats, Season currentSeason) override;
    // Carnivore's attempt to reproduce.
    void attemptReproduce(Grid &grid, MonthlyStats &stats, const std::vector<std::shared_ptr<Animal>> &potentialMates, Season currentSeason) override;
    // Carnivore's logic for giving birth.
    void giveBirth(Grid &grid, MonthlyStats &stats) override;
};

#endif // CARNIVORE_H