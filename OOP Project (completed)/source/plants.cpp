// plant.cpp
#include "../headers/plants.hpp"
#include "../headers/grid.hpp"
#include "../headers/monthlyStats.hpp"
#include "../headers/utils.hpp" // For getRandomInt

// Plant constructor.
Plant::Plant(int r_coord, int c_coord)
    : Entity(r_coord, c_coord, EntityType::PLANT, 'P'), 
      baseSpreadChanceValue(35), maximumAge(40), currentAge(0),
      winterDeathChanceRate(20), autumnDeathChanceRate(10) {}

// Gets the species name ("Plant").
std::string Plant::getSpeciesName() const { return "Plant"; }

// Updates the plant's state for the current month.
void Plant::update(Grid &grid, MonthlyStats &stats, Season currentSeason) {
    if (!isAlive())
        return;
    currentAge++;
    if (currentAge > maximumAge) {
        if (isAlive()) {
            stats.incrementPlantsDiedNaturalAge();
            kill();
        }
        return;
    }

    if (currentSeason == Season::WINTER) {
        if (getRandomInt(1, 100) <= winterDeathChanceRate) {
            if (isAlive()) {
                stats.incrementPlantsDiedWeather();
                kill();
            }
            return; 
        }
    } else if (currentSeason == Season::AUTUMN) {
        if (getRandomInt(1, 100) <= autumnDeathChanceRate) {
            if (isAlive()) {
                stats.incrementPlantsDiedWeather();
                kill();
            }
            return;
        }
    }
    if (!isAlive()) return;

    int actualSpreadChance = baseSpreadChanceValue;
    if (currentSeason == Season::WINTER)
        actualSpreadChance /= 2;
    else if (currentSeason == Season::SUMMER)
        actualSpreadChance = static_cast<int>(baseSpreadChanceValue * 2);

    if (getRandomInt(1, 100) <= actualSpreadChance) {
        auto localEmptyCells = grid.getAdjacentEmptyCells(getR(), getC());
        if (!localEmptyCells.empty() && getRandomInt(1,100) <= 75 ) {
            int choice = 0;
            if (localEmptyCells.size() > 1) choice = getRandomInt(0, localEmptyCells.size() - 1);
            else if (localEmptyCells.empty()) return;

            auto newPlant = std::make_shared<Plant>(localEmptyCells[choice].first, localEmptyCells[choice].second);
            if (grid.addEntity(newPlant))
                stats.incrementPlantsSpread();
        }
    }
    
    int newPlantChance = 0;
    if (currentSeason == Season::SPRING) {
        newPlantChance = 75;
    } else if (currentSeason == Season::SUMMER) {
        newPlantChance = 100;
    }

    if (newPlantChance > 0) {
        for (int i = 0; i < 2; ++i) { 
            if (getRandomInt(1, 100) <= newPlantChance) {
                int newR_plant, newC_plant;
                int attempts = 0;
                do {
                    newR_plant = getRandomInt(0, GRID_HEIGHT - 1);
                    newC_plant = getRandomInt(0, GRID_WIDTH - 1);
                    attempts++;
                    if (attempts > GRID_WIDTH * GRID_HEIGHT * 2) break; 
                } while (!grid.isEmpty(newR_plant, newC_plant));

                if (grid.isEmpty(newR_plant, newC_plant)) {
                    auto newPlant = std::make_shared<Plant>(newR_plant, newC_plant);
                    if (grid.addEntity(newPlant))
                        stats.incrementPlantsSpread();
                }
            }
        }
    }
}

// Getters
// Gets the base spread chance of the plant.
int Plant::getBaseSpreadChance() const { return baseSpreadChanceValue; }
// Gets the maximum age of the plant.
int Plant::getMaxAgePlant() const { return maximumAge; }
// Gets the current age of the plant.
int Plant::getCurrentAgePlant() const { return currentAge; }