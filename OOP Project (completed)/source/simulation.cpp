// simulation.cpp
#include "../headers/simulation.hpp"
#include "../headers/plants.hpp"
#include "../headers/herbivore.hpp"
#include "../headers/carnivore.hpp"
#include "../headers/utils.hpp"
#include "../headers/grid.hpp"

#include <iostream>     // For std::cout, std::cin
#include <algorithm>    // For std::min, std::remove_if
#include <cctype>        // For toupper

// Simulation constructor.
Simulation::Simulation() 
    : totalMonthsDuration(0), currentMonthCounter(0), carnivoresStarvedPreviousMonth(false), 
      isNorthernHemisphereSelected(true), current_Season_sim(Season::NONE), currentMonthIndexInYear(0),
      month_Names_sim{"January", "February", "March", "April", "May", "June", 
                      "July", "August", "September", "October", "November", "December"},
      simulationEndReason("") {}

// Helper function to get validated integer input.
int Simulation::getValidIntInput(const std::string& prompt, int minVal, int maxVal) {
    int value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.good() && value >= minVal && value <= maxVal) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
            return value;
        }
        std::cout << "Invalid input. Please enter an integer between " << minVal << " and " << maxVal << ".\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// Helper function to get validated hemisphere input.
char Simulation::getValidHemisphereInput(const std::string& prompt) {
    char hemiC;
    while (true) {
        std::cout << prompt;
        std::cin >> hemiC;
        hemiC = toupper(hemiC);
        if (std::cin.good() && (hemiC == 'N' || hemiC == 'S')) { // Added std::cin.good() check
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
            return hemiC;
        }
        std::cout << "Invalid input. Please enter 'N' for Northern or 'S' for Southern.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// Determines the current season based on the month and hemisphere.
void Simulation::determineSeason()
{
    currentMonthIndexInYear = (currentMonthCounter) % 12; 
    if (isNorthernHemisphereSelected)
    {
        if (currentMonthIndexInYear >= 2 && currentMonthIndexInYear <= 4) current_Season_sim = Season::SPRING;
        else if (currentMonthIndexInYear >= 5 && currentMonthIndexInYear <= 7) current_Season_sim = Season::SUMMER;
        else if (currentMonthIndexInYear >= 8 && currentMonthIndexInYear <= 10) current_Season_sim = Season::AUTUMN;
        else current_Season_sim = Season::WINTER;
    }
    else 
    {
        if (currentMonthIndexInYear >= 8 && currentMonthIndexInYear <= 10) current_Season_sim = Season::SPRING;
        else if (currentMonthIndexInYear >= 11 || currentMonthIndexInYear <= 1) current_Season_sim = Season::SUMMER;
        else if (currentMonthIndexInYear >= 2 && currentMonthIndexInYear <= 4) current_Season_sim = Season::AUTUMN;
        else current_Season_sim = Season::WINTER;
    }
    sim_stats.setCurrentMonthName(month_Names_sim[currentMonthIndexInYear]);
    sim_stats.setCurrentSeasonName(getSeasonName(current_Season_sim));
}

// Initializes the simulation with user inputs.
void Simulation::initialize()
{
    std::cout << "Welcome to the Cellular Automata Ecosystem Simulation!\n";
    std::cout << "This simulation attempts to model a natural environment with plants, herbivores, and carnivores.\n";
    std::cout << "It is intended for research and educational purposes to observe population dynamics.\n\n";
    std::cout << "You will be asked to input specifications for the simulation.\n";
    std::cout << "The simulation will then output a " << GRID_WIDTH * GRID_HEIGHT << "-cell grid (representing a "
              << GRID_WIDTH * GRID_HEIGHT << " sq km map) each month.\n";
    std::cout << "Monthly statistics and notable events will also be reported.\n\n";
    std::cout << "Simulation Rules:\n";
    std::cout << "- The total population of plants, herbivores, and carnivores cannot exceed " << MAX_POPULATION << ".\n";
    std::cout << "- Maximum simulation duration is " << MAX_SIMULATION_YEARS << " years.\n\n";
    std::cout << "Grid Key:\n";
    std::cout << "  P: Plant\n";
    std::cout << "  H: Male Herbivore\n";
    std::cout << "  h: Female Herbivore\n";
    std::cout << "  C: Male Carnivore\n";
    std::cout << "  c: Female Carnivore\n";
    std::cout << "  *: Empty Cell\n\n";

    char hemiC = getValidHemisphereInput("Choose Hemisphere (N for Northern, S for Southern): ");
    isNorthernHemisphereSelected = (hemiC == 'N');

    int years = getValidIntInput("Enter number of years for simulation (1-" + std::to_string(MAX_SIMULATION_YEARS) + "): ", 1, MAX_SIMULATION_YEARS);
    totalMonthsDuration = years * 12;

    std::cout << "\nEnter initial populations:\n";
    int numP = getValidIntInput("Number of Plants (Ideal: 230-280): ", 0, MAX_POPULATION);
    int numH = getValidIntInput("Number of Herbivores (Ideal: 90-140): ", 0, MAX_POPULATION);
    int numC = getValidIntInput("Number of Carnivores (Ideal: 5-20): ", 0, MAX_POPULATION);
    
    if (numP + numH + numC > MAX_POPULATION) {
        std::cout << "Error: Total initial population (" << (numP + numH + numC)
                  << ") exceeds the maximum allowed (" << MAX_POPULATION << "). Exiting.\n";
        exit(1); 
    }

    auto place = [&](EntityType type_val, int count) {
        for (int i = 0; i < count; ++i) {
            int r_coord, c_coord, attempts = 0;
            do {
                r_coord = getRandomInt(0, GRID_HEIGHT - 1);
                c_coord = getRandomInt(0, GRID_WIDTH - 1);
                attempts++;
            } while (!sim_grid.isEmpty(r_coord, c_coord) && attempts < GRID_WIDTH * GRID_HEIGHT * 2);
            
            if (sim_grid.isEmpty(r_coord, c_coord)) {
                std::shared_ptr<Entity> newEntity = nullptr;
                Gender g = (getRandomInt(0, 1) == 0) ? Gender::MALE : Gender::FEMALE;
                if (type_val == EntityType::PLANT) newEntity = std::make_shared<Plant>(r_coord, c_coord);
                else if (type_val == EntityType::HERBIVORE) newEntity = std::make_shared<Herbivore>(r_coord, c_coord, g);
                else if (type_val == EntityType::CARNIVORE) newEntity = std::make_shared<Carnivore>(r_coord, c_coord, g);
                
                if (newEntity) sim_grid.addEntity(newEntity);
            } else {
                std::cout << "Warning: Could not place all initial entities due to lack of space.\n";
                break;
            }
        }
    };
    place(EntityType::PLANT, numP);
    place(EntityType::HERBIVORE, numH);
    place(EntityType::CARNIVORE, numC);
}

// Handles animal migration events.
void Simulation::handleMigration()
{
    if (current_Season_sim == Season::SPRING)
    {
        int immigrateCount = getRandomInt(1, 3), actualImmigrated = 0;
        for (int i = 0; i < immigrateCount; ++i)
        {
            std::shared_ptr<Animal> newAnimal = nullptr;
            EntityType type_val = (getRandomInt(0, 1) == 0) ? EntityType::HERBIVORE : EntityType::CARNIVORE;
            Gender g = (getRandomInt(0, 1) == 0) ? Gender::MALE : Gender::FEMALE;
            if (type_val == EntityType::HERBIVORE)
                newAnimal = std::make_shared<Herbivore>(0, 0, g); 
            else
                newAnimal = std::make_shared<Carnivore>(0, 0, g); 

            if (sim_grid.addMigratingAnimal(newAnimal))
            {
                actualImmigrated++;
                sim_stats.addMonthlyEvent(newAnimal->getSpeciesName() + " immigrated to (" + std::to_string(newAnimal->getR()) + "," + std::to_string(newAnimal->getC()) + ").");
            }
        }
        if(actualImmigrated > 0) sim_stats.incrementAnimalsImmigrated(); // Only increment if any actually immigrated
    }
    else if (current_Season_sim == Season::AUTUMN)
    {
        int emigrateHerbivores = 0, emigrateCarnivores = 0, actualEmigrated = 0;
        if (sim_grid.getHerbivores().size() > 2) emigrateHerbivores = getRandomInt(0, std::min((int)sim_grid.getHerbivores().size() / 4, 2));
        if (sim_grid.getCarnivores().size() > 1) emigrateCarnivores = getRandomInt(0, std::min((int)sim_grid.getCarnivores().size() / 5, 1));
        
        std::vector<std::shared_ptr<Herbivore>> temp_herbivores_to_emigrate; // To avoid modifying list while iterating conceptually
        for(int i=0; i< emigrateHerbivores && i < sim_grid.getHerbivores().size(); ++i) { // Ensure we don't go out of bounds
             int randIdx = getRandomInt(0, sim_grid.getHerbivores().size() - 1);
             temp_herbivores_to_emigrate.push_back(sim_grid.getHerbivores()[randIdx]);
        }
        for(const auto& herbivoreToRemove : temp_herbivores_to_emigrate) {
            if(herbivoreToRemove->isAlive()){ // Double check if still alive
                sim_stats.addMonthlyEvent("Herbivore at (" + std::to_string(herbivoreToRemove->getR()) + "," + std::to_string(herbivoreToRemove->getC()) + ") emigrated.");
                sim_grid.removeEntity(herbivoreToRemove, sim_stats);
                actualEmigrated++;
            }
        }


        std::vector<std::shared_ptr<Carnivore>> temp_carnivores_to_emigrate;
        for(int i=0; i< emigrateCarnivores && i < sim_grid.getCarnivores().size(); ++i) {
             int randIdx = getRandomInt(0, sim_grid.getCarnivores().size() - 1);
             temp_carnivores_to_emigrate.push_back(sim_grid.getCarnivores()[randIdx]);
        }
        for(const auto& carnivoreToRemove : temp_carnivores_to_emigrate) {
            if(carnivoreToRemove->isAlive()){
                sim_stats.addMonthlyEvent("Carnivore at (" + std::to_string(carnivoreToRemove->getR()) + "," + std::to_string(carnivoreToRemove->getC()) + ") emigrated.");
                sim_grid.removeEntity(carnivoreToRemove, sim_stats);
                actualEmigrated++;
            }
        }
        if(actualEmigrated > 0) sim_stats.incrementAnimalsEmigrated(); // Corrected to actualEmigrated and incrementAnimalsEmigrated
    }
}

// Runs one month of the simulation.
void Simulation::runMonth()
{
    currentMonthCounter++; 
    sim_stats.reset();
    determineSeason(); 
    std::cout << "\n--- Month: " << sim_stats.getCurrentMonthName() << " " << (currentMonthCounter -1 ) / 12 + 1 << " (Season: " << sim_stats.getCurrentSeasonName() << ") ---\n";

    auto plants_copy = sim_grid.getPlants(); // Use getter
    auto herbivores_copy = sim_grid.getHerbivores(); // Use getter
    auto carnivores_copy = sim_grid.getCarnivores(); // Use getter

    for (const auto &c_ptr : carnivores_copy)
        if (c_ptr->isAlive()) c_ptr->update(sim_grid, sim_stats, current_Season_sim);
    for (const auto &h_ptr : herbivores_copy)
        if (h_ptr->isAlive()) h_ptr->update(sim_grid, sim_stats, current_Season_sim);
    for (const auto &p_ptr : plants_copy)
        if (p_ptr->isAlive()) p_ptr->update(sim_grid, sim_stats, current_Season_sim);
    
    handleMigration();

    // Re-fetch live lists from grid for cleanup, as update calls might have changed them
    // The Grid's removeEntity already handles list removal, but this ensures consistency.
    // For cleanupDead lambda, it should operate on the Grid's actual lists, not copies.
    // However, the current grid.removeEntity and animal->die flow should handle marking dead.
    // The main lists in grid (plants_list, etc.) are modified by removeEntity.
    // The cleanupDead lambda was intended for the main lists, but it's tricky with shared_ptr if not careful.
    // The current mechanism:
    // 1. Animal::die() sets alive=false.
    // 2. Grid::removeEntity() (called when eaten) removes from list AND grid.cells.
    // 3. If died naturally, alive=false. The cleanup in Simulation::runMonth (below) will remove from lists and grid.cells.

    // Create temporary lists of shared_ptrs to iterate for cleanup
    // This avoids iterator invalidation issues with the grid's main lists
    std::vector<std::shared_ptr<Entity>> all_entities_for_cleanup;
    for(const auto& p : sim_grid.getPlants()) all_entities_for_cleanup.push_back(p);
    for(const auto& h : sim_grid.getHerbivores()) all_entities_for_cleanup.push_back(h);
    for(const auto& c : sim_grid.getCarnivores()) all_entities_for_cleanup.push_back(c);

    for(const auto& e_ptr : all_entities_for_cleanup) {
        if (!e_ptr->isAlive()) {
            // If entity died and was not already removed (e.g. by being eaten)
            // ensure it's cleared from the grid cells map and then from specialized lists by removeEntity.
            // removeEntity handles the removal from both grid.cells and specialized lists.
            // Calling it again is safe if it's already removed.
            sim_grid.removeEntity(e_ptr, sim_stats);
        }
    }


    sim_stats.setCurrentPlants(sim_grid.getPlants().size());
    sim_stats.setCurrentHerbivores(sim_grid.getHerbivores().size());
    sim_stats.setCurrentCarnivores(sim_grid.getCarnivores().size());

    sim_grid.display();
    sim_stats.display();

    bool allAnimalsDead = sim_grid.getHerbivores().empty() && sim_grid.getCarnivores().empty();
    bool plantsGone = sim_grid.getPlants().empty() && (sim_stats.getCurrentHerbivores() > 0 || sim_stats.getCurrentCarnivores() > 0) ;
    bool herbivoresGone = sim_grid.getHerbivores().empty() && sim_stats.getCurrentCarnivores() > 0;

    if (currentMonthCounter > 1) { 
        if (allAnimalsDead) {
            simulationEndReason = "All animals have died.";
            currentMonthCounter = totalMonthsDuration + 1; 
        } else if (plantsGone && allAnimalsDead) { // If plants are gone AND animals are gone
             simulationEndReason = "All plants and animals have died.";
             currentMonthCounter = totalMonthsDuration + 1;
        }
         else if (plantsGone) {
            simulationEndReason = "All plants have died. Remaining animals will likely starve.";
            currentMonthCounter = totalMonthsDuration + 1; 
        } else if (herbivoresGone) {
            simulationEndReason = "All herbivores have died. Carnivores will starve.";
            currentMonthCounter = totalMonthsDuration + 1; 
        }
    }

    if (currentMonthCounter <= totalMonthsDuration) { 
        std::cout << "Press Enter to continue to the next month (or Q to quit)...";
        char q_input_char = std::cin.get();
        if (std::cin.eof()){ // Handle EOF / Ctrl+D
             simulationEndReason = "User requested exit (EOF).";
             currentMonthCounter = totalMonthsDuration + 1;
        } else if (toupper(q_input_char) == 'Q')
        {
            simulationEndReason = "User quit the simulation.";
            currentMonthCounter = totalMonthsDuration + 1; 
            if (q_input_char != '\n') 
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        else if (q_input_char != '\n') 
             std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// Starts and manages the simulation loop.
void Simulation::start()
{
    initialize();      
    determineSeason();  
    sim_stats.setCurrentMonthName("Initial Setup"); 
    sim_stats.setCurrentSeasonName(getSeasonName(current_Season_sim));
    sim_stats.setCurrentPlants(sim_grid.getPlants().size());
    sim_stats.setCurrentHerbivores(sim_grid.getHerbivores().size());
    sim_stats.setCurrentCarnivores(sim_grid.getCarnivores().size());
    
    sim_grid.display();
    std::cout << "\n--- Initial State ---\nSeason: " << sim_stats.getCurrentSeasonName()
              << "\nInitial Plants: " << sim_stats.getCurrentPlants()
              << "\nInitial Herbivores: " << sim_stats.getCurrentHerbivores()
              << "\nInitial Carnivores: " << sim_stats.getCurrentCarnivores()
              << "\nPress Enter to start simulation...";
    
    char start_char = std::cin.get();
    if(std::cin.eof()){
        std::cout << "\nEOF detected. Exiting simulation setup." << std::endl;
        return;
    }
    if (start_char != '\n') { // Clear buffer if user typed something then enter
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }


    while (currentMonthCounter < totalMonthsDuration)
    {
        runMonth(); 
    }

    int finalMonthCount = std::min(currentMonthCounter -1, totalMonthsDuration); 
    if (finalMonthCount < 0 && currentMonthCounter == 0) finalMonthCount = 0; // If user quits at initial prompt
    else if (finalMonthCount < 0) finalMonthCount = 0;


    if (!simulationEndReason.empty()) {
        std::cout << "\nSimulation ended after " << finalMonthCount << " completed months.\n";
        std::cout << "Reason: " << simulationEndReason << "\n";
    } else {
        std::cout << "\nSimulation for " << totalMonthsDuration / 12 << " years (" << totalMonthsDuration << " months) finished.\n";
    }
}