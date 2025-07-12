// animal.cpp
#include "../headers/animal.hpp"
#include "../headers/grid.hpp"         // For Grid class (full definition needed for operations)
#include "../headers/monthlyStats.hpp" // For MonthlyStats struct (full definition needed)
#include "../headers/utils.hpp"        // For getRandomInt, getRandomDouble

#include "../headers/herbivore.hpp"  
#include "../headers/carnivore.hpp"
// Animal constructor.
Animal::Animal(int r_coord, int c_coord, EntityType type_val, char maleSymbol, char femaleSymbol, Gender gender_val,
               int maxAge_val, int maxEnergy_val, int visionRange_val, int moveCost_val,
               int gestationPeriod_val, int minBreedingAge_val, int energyToReproduce_val,
               int maxTurnsWithoutFood_val, double size_val)
    : Entity(r_coord, c_coord, type_val, (gender_val == Gender::MALE ? maleSymbol : femaleSymbol)),
      animalGender(gender_val), currentAge(0), maximumAge(maxAge_val), 
      currentEnergy(maxEnergy_val / 2 + getRandomInt(0, maxEnergy_val / 4)),
      maximumEnergy(maxEnergy_val), sightRange(visionRange_val), movementCostBase(moveCost_val),
      cooldownForReproduction(1), currentCooldownForReproduction(0), currentlyPregnant(false),
      periodOfGestation(gestationPeriod_val), currentGestationProgress(0), minimumBreedingAge(minBreedingAge_val),
      energyRequiredToReproduce(energyToReproduce_val), mealsMissedTurns(0),
      maxTurnsWithoutFoodAllowed(maxTurnsWithoutFood_val), animalSize(size_val) {}

// Gets the gender of the animal.
Gender Animal::getGender() const { return animalGender; } // Call specific getter

// Handles the death of an animal.
void Animal::die(MonthlyStats &stats, bool eaten)
{
    if (!isAlive()) // Use Entity's isAlive()
        return;
    kill(); // Use Entity's kill()
    if (!eaten)
    {
        if (getType() == EntityType::HERBIVORE) // Use Entity's getType()
            stats.incrementHerbivoresDiedNatural();
        else if (getType() == EntityType::CARNIVORE)
            stats.incrementCarnivoresDiedNatural();
    }
}

// Base update logic common to all animals.
void Animal::baseUpdate(MonthlyStats &stats, Season currentSeason)
{
    if (!isAlive())
        return;
    currentAge++;
    mealsMissedTurns++;
    int actualMoveCost = movementCostBase;
    if (currentSeason == Season::WINTER)
        actualMoveCost += 5;
    else if (currentSeason == Season::AUTUMN)
        actualMoveCost += 2;
    else if (currentSeason == Season::SUMMER)
        actualMoveCost = std::max(1, actualMoveCost - 2);

    currentEnergy -= (1 + actualMoveCost / 2);

    if (currentCooldownForReproduction > 0)
        currentCooldownForReproduction--;
    if (currentlyPregnant)
    {
        currentGestationProgress++;
        currentEnergy -= 3;
    }
    if (currentEnergy <= 0 || currentAge > maximumAge || mealsMissedTurns > maxTurnsWithoutFoodAllowed)
        die(stats);
}

// Checks if the animal can currently reproduce.
bool Animal::canReproduceInternal() const
{
    return animalGender == Gender::FEMALE && !currentlyPregnant && currentAge >= minimumBreedingAge && 
           currentEnergy >= energyRequiredToReproduce && currentCooldownForReproduction == 0;
}

// Overridden update logic for animals.
void Animal::update(Grid &grid, MonthlyStats &stats, Season currentSeason)
{
    if (!isAlive())
        return;
    baseUpdate(stats, currentSeason);
    if (!isAlive())
        return;
    
    if (attemptEat(grid, stats, currentSeason))
    { 
        mealsMissedTurns = 0; // Reset if ate
    }
    if (!isAlive())
        return;

    if (currentlyPregnant && currentGestationProgress >= periodOfGestation)
    {
        giveBirth(grid, stats);
        currentlyPregnant = false;
        currentGestationProgress = 0;
        currentCooldownForReproduction = cooldownForReproduction; // Use base cooldown value
        currentEnergy -= maximumEnergy / 3;
        if (currentEnergy <= 0 && isAlive())
            die(stats);
    }
    if (!isAlive())
        return;

    // Mate finding logic for females who can reproduce.
    if (animalGender == Gender::FEMALE && !currentlyPregnant && currentCooldownForReproduction == 0 && 
        currentAge >= minimumBreedingAge && currentEnergy >= energyRequiredToReproduce)
    {
        std::vector<std::shared_ptr<Animal>> mates_found;
        const auto& all_herbivores = grid.getHerbivores(); // Assuming Grid has getters for these lists
        const auto& all_carnivores = grid.getCarnivores();

        if (this->getType() == EntityType::HERBIVORE)
        {
            for (const auto &herb_mate : all_herbivores)
            {
                // Check if herb_mate is a valid Animal pointer and alive
                if (herb_mate && herb_mate->isAlive() && herb_mate.get() != this && 
                    herb_mate->getGender() == Gender::MALE && herb_mate->getCurrentAge() >= herb_mate->getMinimumBreedingAge())
                {
                    if (std::abs(this->getR() - herb_mate->getR()) <= 2 && std::abs(this->getC() - herb_mate->getC()) <= 2)
                        mates_found.push_back(herb_mate);
                }
            }
        }
        else if (this->getType() == EntityType::CARNIVORE)
        {
            for (const auto &carn_mate : all_carnivores)
            {
                 if (carn_mate && carn_mate->isAlive() && carn_mate.get() != this && 
                    carn_mate->getGender() == Gender::MALE && carn_mate->getCurrentAge() >= carn_mate->getMinimumBreedingAge())
                {
                    if (std::abs(this->getR() - carn_mate->getR()) <= 2 && std::abs(this->getC() - carn_mate->getC()) <= 2)
                        mates_found.push_back(carn_mate);
                }
            }
        }
        if (!mates_found.empty())
            attemptReproduce(grid, stats, mates_found, currentSeason);
    }

    if (!isAlive())
        return;
    
    move(grid, stats, currentSeason); // move() will deduct its own energy cost
    
    if (currentEnergy <= 0 && isAlive()) // Final check after all actions
        die(stats);
}


// Getters
Gender Animal::getAnimalGender() const { return animalGender; }
int Animal::getCurrentAge() const { return currentAge; }
int Animal::getMaximumAge() const { return maximumAge; }
int Animal::getCurrentEnergy() const { return currentEnergy; }
int Animal::getMaximumEnergy() const { return maximumEnergy; }
int Animal::getSightRange() const { return sightRange; }
int Animal::getMovementCostBase() const { return movementCostBase; }
int Animal::getCooldownForReproduction() const { return cooldownForReproduction; }
int Animal::getCurrentCooldownForReproduction() const { return currentCooldownForReproduction; }
bool Animal::isCurrentlyPregnant() const { return currentlyPregnant; }
int Animal::getPeriodOfGestation() const { return periodOfGestation; }
int Animal::getCurrentGestationProgress() const { return currentGestationProgress; }
int Animal::getMinimumBreedingAge() const { return minimumBreedingAge; }
int Animal::getEnergyRequiredToReproduce() const { return energyRequiredToReproduce; }
int Animal::getMealsMissedTurns() const { return mealsMissedTurns; }
int Animal::getMaxTurnsWithoutFoodAllowed() const { return maxTurnsWithoutFoodAllowed; }
double Animal::getAnimalSize() const { return animalSize; }

// Setters/Modifiers
void Animal::setCurrentEnergy(int energy_val) { 
    currentEnergy = std::max(0, std::min(energy_val, maximumEnergy)); 
}
void Animal::setCurrentlyPregnant(bool is_pregnant_val) { currentlyPregnant = is_pregnant_val; }