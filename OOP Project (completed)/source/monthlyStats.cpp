// monthlyStats.cpp
#include "../headers/monthlyStats.hpp"

// Constructor
MonthlyStats::MonthlyStats() :
    plantsEaten(0), plantsDiedNaturalAge(0), plantsDiedWeather(0), plantsSpread(0), currentPlants(0),
    herbivoresEaten(0), herbivoresDiedNatural(0), herbivoresSpawned(0), currentHerbivores(0),
    carnivoresEaten(0), carnivoresDiedNatural(0), carnivoresSpawned(0), currentCarnivores(0),
    animalsImmigrated(0), animalsEmigrated(0) {}

// Resets all monthly statistical counters.
void MonthlyStats::reset()
{
    plantsEaten = 0;
    plantsDiedNaturalAge = 0;
    plantsDiedWeather = 0;
    plantsSpread = 0;
    // currentPlants is updated explicitly
    herbivoresEaten = 0;
    herbivoresDiedNatural = 0;
    herbivoresSpawned = 0;
    // currentHerbivores is updated explicitly
    carnivoresEaten = 0;
    carnivoresDiedNatural = 0;
    carnivoresSpawned = 0;
    // currentCarnivores is updated explicitly
    animalsImmigrated = 0;
    animalsEmigrated = 0;
    monthlyEvents.clear();
}

// Displays all current monthly statistics.
void MonthlyStats::display() const
{
    std::cout << "\n--- Monthly Statistics for " << currentMonthName << " (" << currentSeasonName << ") ---\n";
    std::cout << "Plants Eaten by Herbivores: " << plantsEaten << "\n";
    std::cout << "Plants Died (Old Age): " << plantsDiedNaturalAge << "\n";
    std::cout << "Plants Died (Weather): " << plantsDiedWeather << "\n";
    std::cout << "New Plants (Spread): " << plantsSpread << "\n\n";
    std::cout << "Herbivores Eaten by Carnivores: " << herbivoresEaten << "\n";
    std::cout << "Herbivores Died (Natural Causes/Starvation): " << herbivoresDiedNatural << "\n";
    std::cout << "New Herbivores (Born): " << herbivoresSpawned << "\n\n";
    std::cout << "Carnivores Died (Natural Causes/Starvation): " << carnivoresDiedNatural << "\n";
    std::cout << "New Carnivores (Born): " << carnivoresSpawned << "\n\n";
    std::cout << "Animals Immigrated: " << animalsImmigrated << "\n";
    std::cout << "Animals Emigrated: " << animalsEmigrated << "\n\n";
    std::cout << "Current Population:\n";
    std::cout << "Plants: " << currentPlants << "\n";
    std::cout << "Herbivores: " << currentHerbivores << "\n";
    std::cout << "Carnivores: " << currentCarnivores << "\n\n";
    std::cout << "Monthly Events:\n";
    if (monthlyEvents.empty())
    {
        std::cout << "No specific events this month.\n";
    }
    else
    {
        for (const auto &event : monthlyEvents)
        {
            std::cout << "- " << event << "\n";
        }
    }
}

// Getters
int MonthlyStats::getPlantsEaten() const { return plantsEaten; }
int MonthlyStats::getPlantsDiedNaturalAge() const { return plantsDiedNaturalAge; }
int MonthlyStats::getPlantsDiedWeather() const { return plantsDiedWeather; }
int MonthlyStats::getPlantsSpread() const { return plantsSpread; }
int MonthlyStats::getCurrentPlants() const { return currentPlants; }
int MonthlyStats::getHerbivoresEaten() const { return herbivoresEaten; }
int MonthlyStats::getHerbivoresDiedNatural() const { return herbivoresDiedNatural; }
int MonthlyStats::getHerbivoresSpawned() const { return herbivoresSpawned; }
int MonthlyStats::getCurrentHerbivores() const { return currentHerbivores; }
int MonthlyStats::getCarnivoresEaten() const { return carnivoresEaten; }
int MonthlyStats::getCarnivoresDiedNatural() const { return carnivoresDiedNatural; }
int MonthlyStats::getCarnivoresSpawned() const { return carnivoresSpawned; }
int MonthlyStats::getCurrentCarnivores() const { return currentCarnivores; }
int MonthlyStats::getAnimalsImmigrated() const { return animalsImmigrated; }
int MonthlyStats::getAnimalsEmigrated() const { return animalsEmigrated; }
std::string MonthlyStats::getCurrentMonthName() const { return currentMonthName; }
std::string MonthlyStats::getCurrentSeasonName() const { return currentSeasonName; }
const std::vector<std::string>& MonthlyStats::getMonthlyEvents() const { return monthlyEvents; }

// Setters / Modifiers
void MonthlyStats::incrementPlantsEaten() { plantsEaten++; }
void MonthlyStats::incrementPlantsDiedNaturalAge() { plantsDiedNaturalAge++; }
void MonthlyStats::incrementPlantsDiedWeather() { plantsDiedWeather++; }
void MonthlyStats::incrementPlantsSpread() { plantsSpread++; }
void MonthlyStats::setCurrentPlants(int count) { currentPlants = count; }
void MonthlyStats::incrementHerbivoresEaten() { herbivoresEaten++; }
void MonthlyStats::incrementHerbivoresDiedNatural() { herbivoresDiedNatural++; }
void MonthlyStats::incrementHerbivoresSpawned() { herbivoresSpawned++; }
void MonthlyStats::setCurrentHerbivores(int count) { currentHerbivores = count; }
void MonthlyStats::incrementCarnivoresEaten() { carnivoresEaten++; }
void MonthlyStats::incrementCarnivoresDiedNatural() { carnivoresDiedNatural++; }
void MonthlyStats::incrementCarnivoresSpawned() { carnivoresSpawned++; }
void MonthlyStats::setCurrentCarnivores(int count) { currentCarnivores = count; }
void MonthlyStats::incrementAnimalsImmigrated() { animalsImmigrated++; }
void MonthlyStats::incrementAnimalsEmigrated() { animalsEmigrated++; }
void MonthlyStats::setCurrentMonthName(const std::string& name) { currentMonthName = name; }
void MonthlyStats::setCurrentSeasonName(const std::string& name) { currentSeasonName = name; }
void MonthlyStats::addMonthlyEvent(const std::string& event) { monthlyEvents.push_back(event); }