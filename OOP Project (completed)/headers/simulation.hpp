// simulation.h
#ifndef SIMULATION_H
#define SIMULATION_H

#include "grid.hpp" // Full definition needed
#include "monthlyStats.hpp" // Full definition needed
#include "constants.hpp" // For Season, MAX_SIMULATION_YEARS, etc.
#include <string>
#include <vector>
#include <limits> // For std::numeric_limits

// Controls the overall simulation flow, time, and initialization.
class Simulation
{
private:
    Grid sim_grid; // Renamed to avoid conflict if any
    MonthlyStats sim_stats; // Renamed
    int totalMonthsDuration; // Renamed
    int currentMonthCounter; // Renamed
    bool carnivoresStarvedPreviousMonth; // Renamed
    bool isNorthernHemisphereSelected; // Renamed
    Season current_Season_sim; // Renamed
    int currentMonthIndexInYear; // Renamed
    std::vector<std::string> month_Names_sim; // Renamed
    std::string simulationEndReason; // Renamed

    // Helper function to get validated integer input.
    int getValidIntInput(const std::string& prompt, int minVal = std::numeric_limits<int>::min(), int maxVal = std::numeric_limits<int>::max());
    // Helper function to get validated hemisphere input.
    char getValidHemisphereInput(const std::string& prompt);

public:
    // Simulation constructor.
    Simulation();
    
    // Determines the current season based on the month and hemisphere.
    void determineSeason();
    // Initializes the simulation with user inputs.
    void initialize();
    // Handles animal migration events.
    void handleMigration();
    // Runs one month of the simulation.
    void runMonth();
    // Starts and manages the simulation loop.
    void start();

    // Getters (add if needed for external access, though most logic is internal to start/runMonth)
    // int getCurrentMonthCounter() const;
    // const Grid& getGrid() const; 
    // const MonthlyStats& getStats() const;
};

#endif // SIMULATION_H