// animal.h
#ifndef ANIMAL_H
#define ANIMAL_H

#include "entity.hpp"
#include <vector>  // For potentialMates vector
#include <memory>  // For std::shared_ptr
#include <string>  // For species name in events
#include <cmath>   // For std::abs

// Forward declarations
class Grid;
struct MonthlyStats;

// Base class for animal entities (Herbivores, Carnivores).
class Animal : public Entity
{
private:
    Gender animalGender;
    int currentAge;
    int maximumAge;
    int currentEnergy;
    int maximumEnergy;
    int sightRange;
    int movementCostBase;
    int cooldownForReproduction;
    int currentCooldownForReproduction;
    bool currentlyPregnant;
    int periodOfGestation;
    int currentGestationProgress;
    int minimumBreedingAge;
    int energyRequiredToReproduce;
    int mealsMissedTurns;
    int maxTurnsWithoutFoodAllowed;
    double animalSize;

public:
    // Animal constructor.
    Animal(int r_coord, int c_coord, EntityType type_val, char maleSymbol, char femaleSymbol, Gender gender_val,
           int maxAge_val, int maxEnergy_val, int visionRange_val, int moveCost_val,
           int gestationPeriod_val, int minBreedingAge_val, int energyToReproduce_val,
           int maxTurnsWithoutFood_val, double size_val);
    
    // Gets the gender of the animal.
    Gender getGender() const override;
    // Handles the death of an animal.
    virtual void die(MonthlyStats &stats, bool eaten = false);
    // Base update logic common to all animals.
    virtual void baseUpdate(MonthlyStats &stats, Season currentSeason);
    // Overridden update logic for animals.
    void update(Grid &grid, MonthlyStats &stats, Season currentSeason) override;

    // Pure virtual function for attempting reproduction.
    virtual void attemptReproduce(Grid &grid, MonthlyStats &stats, const std::vector<std::shared_ptr<Animal>> &potentialMates, Season currentSeason) = 0;
    // Pure virtual function for giving birth.
    virtual void giveBirth(Grid &grid, MonthlyStats &stats) = 0;
    // Pure virtual function for attempting to eat.
    virtual bool attemptEat(Grid &grid, MonthlyStats &stats, Season currentSeason) = 0;
    // Pure virtual function for movement logic.
    virtual void move(Grid &grid, MonthlyStats &stats, Season currentSeason) = 0;
    
    // Checks if the animal can currently reproduce.
    bool canReproduceInternal() const;

    // Getters
    Gender getAnimalGender() const; // Specific getter for animalGender to avoid clash with Entity::getGender
    int getCurrentAge() const;
    int getMaximumAge() const;
    int getCurrentEnergy() const;
    int getMaximumEnergy() const;
    int getSightRange() const;
    int getMovementCostBase() const;
    int getCooldownForReproduction() const; // Base cooldown value
    int getCurrentCooldownForReproduction() const;
    bool isCurrentlyPregnant() const;
    int getPeriodOfGestation() const;
    int getCurrentGestationProgress() const;
    int getMinimumBreedingAge() const;
    int getEnergyRequiredToReproduce() const;
    int getMealsMissedTurns() const;
    int getMaxTurnsWithoutFoodAllowed() const;
    double getAnimalSize() const;

    // Setters/Modifiers (examples, add more if needed by external logic not part of update flow)
    void setCurrentEnergy(int energy_val);
    void setCurrentlyPregnant(bool is_pregnant_val);
    // Note: Most other variables are managed internally by update/baseUpdate cycles.
    // For example, age is incremented internally.
};

#endif // ANIMAL_H