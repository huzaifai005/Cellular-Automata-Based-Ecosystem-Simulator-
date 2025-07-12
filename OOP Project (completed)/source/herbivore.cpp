// herbivore.cpp
#include "../headers/herbivore.hpp"
#include "../headers/grid.hpp"
#include "../headers/monthlyStats.hpp"
#include "../headers/plants.hpp" // For type checking when eating
#include "../headers/utils.hpp" // For getRandomInt

// Herbivore constructor.
Herbivore::Herbivore(int r_coord, int c_coord, Gender gender_val)
    : Animal(r_coord, c_coord, EntityType::HERBIVORE, 'H', 'h', gender_val, 
             70, 120, 5, 10, 3, 2, 40, 3, 1.0) {}

// Gets the species name ("Herbivore").
std::string Herbivore::getSpeciesName() const { return "Herbivore"; }

// Herbivore's attempt to eat.
bool Herbivore::attemptEat(Grid &grid, MonthlyStats &stats, Season currentSeason)
{
    if (!isAlive() || getCurrentEnergy() >= getMaximumEnergy() * 0.95)
        return false;
    
    int actualEatingRadius = getSightRange(); // Use base sight range for eating
    if (currentSeason == Season::WINTER)
        actualEatingRadius = std::max(1, getSightRange() - 2);
    else if (currentSeason == Season::AUTUMN)
        actualEatingRadius = std::max(1, getSightRange() - 1);
    else if (currentSeason == Season::SUMMER) // More food availability or easier foraging
        actualEatingRadius = getSightRange() + 2;


    for (int radius_scan = 1; radius_scan <= actualEatingRadius; ++radius_scan)
    {
        for (int dr_eat = -radius_scan; dr_eat <= radius_scan; ++dr_eat)
        {
            for (int dc_eat = -radius_scan; dc_eat <= radius_scan; ++dc_eat)
            {
                if (std::abs(dr_eat) == radius_scan || std::abs(dc_eat) == radius_scan) 
                {
                    int nr_eat = getR() + dr_eat;
                    int nc_eat = getC() + dc_eat;
                    if (grid.isValid(nr_eat, nc_eat))
                    { 
                        auto plantEntity = grid.getEntity(nr_eat, nc_eat);
                        if (plantEntity && plantEntity->getType() == EntityType::PLANT && plantEntity->isAlive())
                        {
                            setCurrentEnergy(std::min(getMaximumEnergy(), getCurrentEnergy() + 35));
                            plantEntity->kill();
                            stats.incrementPlantsEaten();
                            stats.addMonthlyEvent("Herbivore at (" + std::to_string(getR()) + "," + std::to_string(getC()) + ") ate plant at (" + std::to_string(nr_eat) + "," + std::to_string(nc_eat) + ")");
                            grid.removeEntity(plantEntity, stats);
                            // mealsMissedTurns = 0; // This is managed in Animal::update based on eat success
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

// Herbivore's movement logic.
void Herbivore::move(Grid &grid, MonthlyStats &stats, Season currentSeason)
{
    if (!isAlive()) return;
    
    int energyBeforeMove = getCurrentEnergy();

    if (energyBeforeMove <= 0) { // Check before deducting potential move cost by other means
        if (isAlive()) die(stats);
        return;
    }

    auto carnivoresNearby = grid.findNearbyEntities(getR(), getC(), EntityType::CARNIVORE, getSightRange() + (currentSeason == Season::SUMMER ? 1:0) - (currentSeason == Season::WINTER ? 1:0) );
    if (!carnivoresNearby.empty())
    {
        double avgCarnR = 0, avgCarnC = 0;
        for (const auto &c_entity : carnivoresNearby)
        {
            avgCarnR += c_entity->getR();
            avgCarnC += c_entity->getC();
        }
        avgCarnR /= carnivoresNearby.size();
        avgCarnC /= carnivoresNearby.size();
        int bestR_flee = getR(), bestC_flee = getC();
        double maxDistSq_flee = -1;       
        for (int dr_move = -1; dr_move <= 1; ++dr_move)
            for (int dc_move = -1; dc_move <= 1; ++dc_move)
            {
                if (dr_move == 0 && dc_move == 0) continue;
                int nr_move = getR() + dr_move, nc_move = getC() + dc_move;
                if (grid.isEmpty(nr_move, nc_move))
                {
                    double dsq_flee = (nr_move - avgCarnR) * (nr_move - avgCarnR) + (nc_move - avgCarnC) * (nc_move - avgCarnC);
                    if (dsq_flee > maxDistSq_flee)
                    {
                        maxDistSq_flee = dsq_flee;
                        bestR_flee = nr_move;
                        bestC_flee = nc_move;
                    }
                }
            }
        if (bestR_flee != getR() || bestC_flee != getC())
        {
            grid.moveEntity(grid.getEntity(getR(), getC()), bestR_flee, bestC_flee);
            setCurrentEnergy(getCurrentEnergy() - getMovementCostBase()); 
            return;
        }
    }

    if (getCurrentEnergy() < getMaximumEnergy() * 0.9) 
    {
        auto plantsNearby = grid.findNearbyEntities(getR(), getC(), EntityType::PLANT, getSightRange());
        if (!plantsNearby.empty())
        {
            std::shared_ptr<Entity> closest_plant = nullptr;
            double minDistSq_plant = 1e9; 
            for (const auto &p_entity : plantsNearby)
            {
                double dsq_plant = (p_entity->getR() - getR()) * (p_entity->getR() - getR()) + (p_entity->getC() - getC()) * (p_entity->getC() - getC());
                if (dsq_plant < minDistSq_plant)
                {
                    minDistSq_plant = dsq_plant;
                    closest_plant = p_entity;
                }
            }
            if (closest_plant)
            {
                int dr_move_plant = (closest_plant->getR() > getR()) ? 1 : ((closest_plant->getR() < getR()) ? -1 : 0);
                int dc_move_plant = (closest_plant->getC() > getC()) ? 1 : ((closest_plant->getC() < getC()) ? -1 : 0);
                int nr_move_plant = getR() + dr_move_plant, nc_move_plant = getC() + dc_move_plant;
                if (grid.isValid(nr_move_plant, nc_move_plant) && 
                    (grid.isEmpty(nr_move_plant, nc_move_plant) || 
                     (grid.getEntity(nr_move_plant, nc_move_plant) && grid.getEntity(nr_move_plant, nc_move_plant)->getType() == EntityType::PLANT)))
                {
                    grid.moveEntity(grid.getEntity(getR(), getC()), nr_move_plant, nc_move_plant);
                    setCurrentEnergy(getCurrentEnergy() - getMovementCostBase());
                    return;
                }
            }
        }
    }
    auto emptyC_rand = grid.getAdjacentEmptyCells(getR(), getC());
    if (!emptyC_rand.empty())
    {
        int ch_rand = 0; 
        if (emptyC_rand.size() > 1) ch_rand = getRandomInt(0, emptyC_rand.size() - 1);
        else if (emptyC_rand.empty()) return; // Should not be reachable due to !emptyC_rand.empty()

        grid.moveEntity(grid.getEntity(getR(), getC()), emptyC_rand[ch_rand].first, emptyC_rand[ch_rand].second);
        setCurrentEnergy(getCurrentEnergy() - getMovementCostBase());
    }
}

// Herbivore's attempt to reproduce.
void Herbivore::attemptReproduce(Grid &grid, MonthlyStats &stats, const std::vector<std::shared_ptr<Animal>> &potentialMates, Season currentSeason)
{
    if (!isAlive() || !canReproduceInternal()) return;
    double repMul = 1.0;
    if (currentSeason == Season::WINTER) repMul = 0.2;
    else if (currentSeason == Season::AUTUMN) repMul = 0.5;
    else if (currentSeason == Season::SUMMER) repMul = 1.5; 
    if (getRandomDouble(0.0, 1.0) > repMul) return;

    for (const auto &mate_base : potentialMates)
    {
        if (!mate_base->isAlive() || mate_base.get() == this) continue; // Compare raw pointers
        if (mate_base->getType() != EntityType::HERBIVORE || mate_base->getGender() != Gender::MALE) continue;
        
        auto mate = std::static_pointer_cast<Herbivore>(mate_base); // Downcast
        if (mate->getCurrentAge() >= mate->getMinimumBreedingAge() && std::abs(getR() - mate->getR()) <= 1 && std::abs(getC() - mate->getC()) <= 1)
        {
            setCurrentlyPregnant(true);
            // currentGestationProgress is reset in Animal::update after birth
            setCurrentEnergy(getCurrentEnergy() - getEnergyRequiredToReproduce() / 2);
            if(mate->isAlive()) mate->setCurrentEnergy(mate->getCurrentEnergy() - mate->getEnergyRequiredToReproduce() / 4);
            stats.addMonthlyEvent("Herbivore at (" + std::to_string(getR()) + "," + std::to_string(getC()) + ") mated.");
            return;
        }
    }
}

// Herbivore's logic for giving birth.
void Herbivore::giveBirth(Grid &grid, MonthlyStats &stats)
{
    if (!isAlive()) return; // Should be caught by Animal::update, but good to have
    std::vector<std::pair<int, int>> birthLocs = grid.getAdjacentEmptyCells(getR(), getC());
    if (birthLocs.empty()) { 
        for (int dr_scan = -getSightRange(); dr_scan <= getSightRange(); ++dr_scan) // Use getSightRange()
            for (int dc_scan = -getSightRange(); dc_scan <= getSightRange(); ++dc_scan) {
                if (dr_scan==0 && dc_scan==0) continue;
                if (grid.isEmpty(getR() + dr_scan, getC() + dc_scan))
                    birthLocs.push_back({getR() + dr_scan, getC() + dc_scan});
            }
    }
    if (birthLocs.empty()) return;

    int numOff = getRandomInt(1, 3);
    for (int i = 0; i < numOff && !birthLocs.empty(); ++i) 
    {
        int idx_birth = 0; 
        if(birthLocs.size() > 1) idx_birth = getRandomInt(0, birthLocs.size() - 1);
        else if (birthLocs.empty()) break;

        auto pos_birth = birthLocs[idx_birth]; 
        birthLocs.erase(birthLocs.begin() + idx_birth);
        Gender g_birth = (getRandomInt(0, 1) == 0) ? Gender::MALE : Gender::FEMALE; 
        auto newH_birth = std::make_shared<Herbivore>(pos_birth.first, pos_birth.second, g_birth); 
        if (grid.addEntity(newH_birth))
        {
            stats.incrementHerbivoresSpawned();
            stats.addMonthlyEvent("Herbivore born at (" + std::to_string(pos_birth.first) + "," + std::to_string(pos_birth.second) + ")");
        }
    }
}