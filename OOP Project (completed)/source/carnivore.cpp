// carnivore.cpp
#include "../headers/carnivore.hpp"
#include "../headers/grid.hpp"
#include "../headers/monthlyStats.hpp"
#include "../headers/herbivore.hpp" // For casting and type checking
#include "../headers/utils.hpp"     // For getRandomInt

// Carnivore constructor.
Carnivore::Carnivore(int r_coord, int c_coord, Gender gender_val)
    : Animal(r_coord, c_coord, EntityType::CARNIVORE, 'C', 'c', gender_val,
             100, 120, 6, 15, 4, 5, 50, 2, 1.5) {}

// Gets the species name ("Carnivore").
std::string Carnivore::getSpeciesName() const { return "Carnivore"; }

// Carnivore's attempt to eat.
bool Carnivore::attemptEat(Grid &grid, MonthlyStats &stats, Season currentSeason)
{
    if (!isAlive() || getCurrentEnergy() >= getMaximumEnergy() * 0.9) return false;
    
    int eatRad = getSightRange(); // Use Animal's getter
    if (currentSeason == Season::WINTER) eatRad = std::max(1, getSightRange() - 2);
    else if (currentSeason == Season::AUTUMN) eatRad = std::max(1, getSightRange() - 1);
    else if (currentSeason == Season::SUMMER) eatRad = getSightRange() + 1;
    
    for (int radius_scan = 1; radius_scan <= eatRad; ++radius_scan)
        for (int dr_scan = -radius_scan; dr_scan <= radius_scan; ++dr_scan)
            for (int dc_scan = -radius_scan; dc_scan <= radius_scan; ++dc_scan)
            {
                if (std::abs(dr_scan) == radius_scan || std::abs(dc_scan) == radius_scan) 
                {
                    int nr_scan = getR() + dr_scan, nc_scan = getC() + dc_scan; // Use Animal's getters
                    if (grid.isValid(nr_scan, nc_scan))
                    {
                        auto target = grid.getEntity(nr_scan, nc_scan);
                        if (target && target->getType() == EntityType::HERBIVORE && target->isAlive())
                        {
                            auto herb = std::static_pointer_cast<Herbivore>(target);
                            setCurrentEnergy(std::min(getMaximumEnergy(), getCurrentEnergy() + 45)); // Use Animal's setters/getters
                            herb->die(stats, true); // Call die on the herbivore
                            stats.incrementHerbivoresEaten();
                            stats.addMonthlyEvent("Carnivore at (" + std::to_string(getR()) + "," + std::to_string(getC()) + ") ate herbivore at (" + std::to_string(nr_scan) + "," + std::to_string(nc_scan) + ")");
                            grid.removeEntity(herb, stats);
                            // mealsMissedTurns = 0; // This is managed in Animal::update based on eat success
                            return true;
                        }
                    }
                }
            }
    return false;
}

// Carnivore's movement logic.
void Carnivore::move(Grid &grid, MonthlyStats &stats, Season currentSeason)
{
    if (!isAlive()) return;
    
    int energyBeforeMove = getCurrentEnergy();

    if (energyBeforeMove <= 0) { 
        if (isAlive()) die(stats); 
        return;
    }

    if (getCurrentEnergy() < getMaximumEnergy() * 0.85) 
    {
        auto herbsNearby = grid.findNearbyEntities(getR(), getC(), EntityType::HERBIVORE, getSightRange());
        if (!herbsNearby.empty())
        {
            std::shared_ptr<Entity> closest_herb = nullptr; 
            double minD_sq_herb = 1e9; 
            for (const auto &h_entity : herbsNearby)
            {
                double d_sq_herb = (h_entity->getR() - getR()) * (h_entity->getR() - getR()) + (h_entity->getC() - getC()) * (h_entity->getC() - getC());
                if (d_sq_herb < minD_sq_herb)
                {
                    minD_sq_herb = d_sq_herb;
                    closest_herb = h_entity;
                }
            }
            if (closest_herb)
            {
                int dr_move_herb = (closest_herb->getR() > getR()) ? 1 : ((closest_herb->getR() < getR()) ? -1 : 0); 
                int dc_move_herb = (closest_herb->getC() > getC()) ? 1 : ((closest_herb->getC() < getC()) ? -1 : 0); 
                int nr_move_herb = getR() + dr_move_herb, nc_move_herb = getC() + dc_move_herb; 
                if (grid.isValid(nr_move_herb, nc_move_herb) && 
                    (grid.isEmpty(nr_move_herb, nc_move_herb) || 
                     (grid.getEntity(nr_move_herb, nc_move_herb) && grid.getEntity(nr_move_herb, nc_move_herb)->getType() == EntityType::HERBIVORE)))
                {
                    grid.moveEntity(grid.getEntity(getR(), getC()), nr_move_herb, nc_move_herb);
                    setCurrentEnergy(getCurrentEnergy() - getMovementCostBase()); 
                    return;
                }
            }
        }
    }
    auto emptyC_rand_carn = grid.getAdjacentEmptyCells(getR(), getC()); 
    if (!emptyC_rand_carn.empty())
    {
        int ch_rand_carn = 0; 
        if(emptyC_rand_carn.size() > 1) ch_rand_carn = getRandomInt(0, emptyC_rand_carn.size() - 1);
        else if (emptyC_rand_carn.empty()) return;

        grid.moveEntity(grid.getEntity(getR(), getC()), emptyC_rand_carn[ch_rand_carn].first, emptyC_rand_carn[ch_rand_carn].second);
        setCurrentEnergy(getCurrentEnergy() - getMovementCostBase());
    }
}

// Carnivore's attempt to reproduce.
void Carnivore::attemptReproduce(Grid &grid, MonthlyStats &stats, const std::vector<std::shared_ptr<Animal>> &potentialMates, Season currentSeason)
{
    if (!isAlive() || !canReproduceInternal()) return;
    double repMul = 1.0;
    if (currentSeason == Season::WINTER) repMul = 0.15;
    else if (currentSeason == Season::AUTUMN) repMul = 0.4;
    else if (currentSeason == Season::SUMMER) repMul = 1.3;
    if (getRandomDouble(0.0, 1.0) > repMul) return;

    for (const auto &mate_base : potentialMates)
    {
        if (!mate_base->isAlive() || mate_base.get() == this) continue;
        if (mate_base->getType() != EntityType::CARNIVORE || mate_base->getGender() != Gender::MALE) continue;
        
        auto mate = std::static_pointer_cast<Carnivore>(mate_base);
        if (mate->getCurrentAge() >= mate->getMinimumBreedingAge() && std::abs(getR() - mate->getR()) <= 1 && std::abs(getC() - mate->getC()) <= 1)
        {
            setCurrentlyPregnant(true);
            // currentGestationProgress is reset in Animal::update after birth
            setCurrentEnergy(getCurrentEnergy() - getEnergyRequiredToReproduce() / 2);
            if(mate->isAlive()) mate->setCurrentEnergy(mate->getCurrentEnergy() - mate->getEnergyRequiredToReproduce() / 4);
            stats.addMonthlyEvent("Carnivore at (" + std::to_string(getR()) + "," + std::to_string(getC()) + ") mated.");
            return;
        }
    }
}

// Carnivore's logic for giving birth.
void Carnivore::giveBirth(Grid &grid, MonthlyStats &stats)
{
    if (!isAlive()) return;
    std::vector<std::pair<int, int>> birthLocs = grid.getAdjacentEmptyCells(getR(), getC());
    if (birthLocs.empty()) { 
         for (int dr_scan = -getSightRange(); dr_scan <= getSightRange(); ++dr_scan)
            for (int dc_scan = -getSightRange(); dc_scan <= getSightRange(); ++dc_scan) {
                if (dr_scan==0 && dc_scan==0) continue;
                if (grid.isEmpty(getR() + dr_scan, getC() + dc_scan))
                    birthLocs.push_back({getR() + dr_scan, getC() + dc_scan});
            }
    }
    if (birthLocs.empty()) return;
    
    int numOff = getRandomInt(1, 2);
    for (int i = 0; i < numOff && !birthLocs.empty(); ++i)
    {
        int idx_birth_carn = 0; 
        if(birthLocs.size() > 1) idx_birth_carn = getRandomInt(0, birthLocs.size() - 1);
        else if (birthLocs.empty()) break;

        auto pos_birth_carn = birthLocs[idx_birth_carn]; 
        birthLocs.erase(birthLocs.begin() + idx_birth_carn);
        Gender g_birth_carn = (getRandomInt(0, 1) == 0) ? Gender::MALE : Gender::FEMALE; 
        auto newC_birth = std::make_shared<Carnivore>(pos_birth_carn.first, pos_birth_carn.second, g_birth_carn); 
        if (grid.addEntity(newC_birth))
        {
            stats.incrementCarnivoresSpawned();
            stats.addMonthlyEvent("Carnivore born at (" + std::to_string(pos_birth_carn.first) + "," + std::to_string(pos_birth_carn.second) + ")");
        }
    }
}