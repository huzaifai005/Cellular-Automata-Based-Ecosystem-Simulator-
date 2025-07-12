// monthlyStats.h
#ifndef MONTHLYSTATS_H
#define MONTHLYSTATS_H

#include <string>
#include <vector>
#include <iostream> // For display method
#include <iomanip>  // For display formatting (if any)

// Stores and manages monthly simulation statistics.
struct MonthlyStats
{
private:
    int plantsEaten;
    int plantsDiedNaturalAge;
    int plantsDiedWeather;
    int plantsSpread;
    int currentPlants;
    int herbivoresEaten;
    int herbivoresDiedNatural;
    int herbivoresSpawned;
    int currentHerbivores;
    int carnivoresEaten;
    int carnivoresDiedNatural;
    int carnivoresSpawned;
    int currentCarnivores;
    int animalsImmigrated;
    int animalsEmigrated;
    std::string currentMonthName;
    std::string currentSeasonName;
    std::vector<std::string> monthlyEvents;

public:
    // Constructor
    MonthlyStats();

    // Resets all monthly statistical counters.
    void reset();
    // Displays all current monthly statistics.
    void display() const;

    // Getters
    int getPlantsEaten() const;
    int getPlantsDiedNaturalAge() const;
    int getPlantsDiedWeather() const;
    int getPlantsSpread() const;
    int getCurrentPlants() const;
    int getHerbivoresEaten() const;
    int getHerbivoresDiedNatural() const;
    int getHerbivoresSpawned() const;
    int getCurrentHerbivores() const;
    int getCarnivoresEaten() const;
    int getCarnivoresDiedNatural() const;
    int getCarnivoresSpawned() const;
    int getCurrentCarnivores() const;
    int getAnimalsImmigrated() const;
    int getAnimalsEmigrated() const;
    std::string getCurrentMonthName() const;
    std::string getCurrentSeasonName() const;
    const std::vector<std::string>& getMonthlyEvents() const;

    // Setters / Modifiers
    void incrementPlantsEaten();
    void incrementPlantsDiedNaturalAge();
    void incrementPlantsDiedWeather();
    void incrementPlantsSpread();
    void setCurrentPlants(int count);
    void incrementHerbivoresEaten();
    void incrementHerbivoresDiedNatural();
    void incrementHerbivoresSpawned();
    void setCurrentHerbivores(int count);
    void incrementCarnivoresEaten();
    void incrementCarnivoresDiedNatural();
    void incrementCarnivoresSpawned();
    void setCurrentCarnivores(int count);
    void incrementAnimalsImmigrated();
    void incrementAnimalsEmigrated();
    void setCurrentMonthName(const std::string& name);
    void setCurrentSeasonName(const std::string& name);
    void addMonthlyEvent(const std::string& event);
};

#endif // MONTHLYSTATS_H