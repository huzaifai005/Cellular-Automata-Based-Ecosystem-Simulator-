#include <iostream>
#include <vector>
#include <memory>
#include <random>
#include <algorithm>
#include <iomanip> // Added this include for std::setw and std::left/right
#include <functional>
#include <string>

// --- Configuration Constants ---
const int GRID_WIDTH = 20;
const int GRID_HEIGHT = 20;

// --- Entity Type & Gender Enums ---
enum class EntityType { EMPTY, PLANT, HERBIVORE, CARNIVORE };
enum class Gender { MALE, FEMALE, NONE }; // NONE for plants

// --- Forward Declarations ---
class Grid;
class Entity;
class Plant;
class Animal;
class Herbivore;
class Carnivore;

// --- Global Random Number Generation ---
std::random_device rd;
std::mt19937 gen(rd());

int getRandomInt(int min, int max) {
    if (min > max) std::swap(min, max);
    if (min == max) return min; 
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}

double getRandomDouble(double min, double max) {
    if (min > max) std::swap(min, max);
    std::uniform_real_distribution<> distrib(min, max);
    return distrib(gen);
}

// --- Statistics Struct ---
struct MonthlyStats {
    int plantsEatenOrDied = 0;
    int plantsSpread = 0;
    int currentPlants = 0;

    int herbivoresEaten = 0;
    int herbivoresDiedNatural = 0;
    int herbivoresSpawned = 0;
    int currentHerbivores = 0;

    int carnivoresEaten = 0;
    int carnivoresDiedNatural = 0;
    int carnivoresSpawned = 0;
    int currentCarnivores = 0;

    std::vector<std::string> monthlyEvents;

    void reset() {
        plantsEatenOrDied = 0;
        plantsSpread = 0;
        herbivoresEaten = 0;
        herbivoresDiedNatural = 0;
        herbivoresSpawned = 0;
        carnivoresEaten = 0;
        carnivoresDiedNatural = 0;
        carnivoresSpawned = 0;
        monthlyEvents.clear();
    }

    void display() const {
        std::cout << "\nMonthly Statistics:\n";
        std::cout << "Plants eaten: " << plantsEatenOrDied << "\n";
        std::cout << "Herbivores eaten: " << herbivoresEaten << "\n";
        std::cout << "Herbivores died: " << herbivoresDiedNatural << "\n";
        std::cout << "Carnivores died: " << carnivoresDiedNatural << "\n";
        std::cout << "New plants: " << plantsSpread << "\n";
        std::cout << "New herbivores: " << herbivoresSpawned << "\n";
        std::cout << "New carnivores: " << carnivoresSpawned << "\n\n";

        std::cout << "Current Population:\n";
        std::cout << "Plants: " << currentPlants << "\n";
        std::cout << "Herbivores: " << currentHerbivores << "\n";
        std::cout << "Carnivores: " << currentCarnivores << "\n\n";

        std::cout << "Monthly Events:\n";
        if (monthlyEvents.empty()) {
            std::cout << "No events this month.\n";
        } else {
            for (const auto& event : monthlyEvents) {
                std::cout << "- " << event << "\n";
            }
        }
    }
};


// --- Entity Base Class ---
class Entity {
public:
    int r, c;
    EntityType type;
    char symbol;
    bool alive;

    Entity(int r_coord, int c_coord, EntityType type, char symbol)
        : r(r_coord), c(c_coord), type(type), symbol(symbol), alive(true) {}

    virtual ~Entity() = default;

    virtual void update(Grid& grid, MonthlyStats& stats) = 0;

    bool isAlive() const { return alive; }
    void kill() { alive = false; }

    virtual Gender getGender() const { return Gender::NONE; }
    virtual std::string getSpeciesName() const = 0;
};

// --- Grid Class ---
class Grid {
public:
    std::vector<std::vector<std::shared_ptr<Entity>>> cells;
    std::vector<std::shared_ptr<Plant>> plants;
    std::vector<std::shared_ptr<Herbivore>> herbivores;
    std::vector<std::shared_ptr<Carnivore>> carnivores;

    Grid() : cells(GRID_HEIGHT, std::vector<std::shared_ptr<Entity>>(GRID_WIDTH, nullptr)) {}

    bool isValid(int r_coord, int c_coord) const {
        return r_coord >= 0 && r_coord < GRID_HEIGHT && c_coord >= 0 && c_coord < GRID_WIDTH;
    }

    bool isEmpty(int r_coord, int c_coord) const {
        return isValid(r_coord, c_coord) && cells[r_coord][c_coord] == nullptr;
    }

    std::shared_ptr<Entity> getEntity(int r_coord, int c_coord) const {
        if (isValid(r_coord, c_coord)) return cells[r_coord][c_coord];
        return nullptr;
    }

    bool addEntity(const std::shared_ptr<Entity>& entity) {
        if (!entity) return false;

        //check if grid already full    
        if (plants.size() + herbivores.size() + carnivores.size() >= (size_t)GRID_WIDTH * GRID_HEIGHT) {
            return false; // Cannot add entity if grid is full
        }
        if (isValid(entity->r, entity->c) && isEmpty(entity->r, entity->c)) {
            cells[entity->r][entity->c] = entity;
            if (entity->type == EntityType::PLANT) {
                plants.push_back(std::static_pointer_cast<Plant>(entity));
            } else if (entity->type == EntityType::HERBIVORE) {
                herbivores.push_back(std::static_pointer_cast<Herbivore>(entity));
            } else if (entity->type == EntityType::CARNIVORE) {
                carnivores.push_back(std::static_pointer_cast<Carnivore>(entity));
            }
            return true;
        }
        return false;
    }

    void removeEntity(const std::shared_ptr<Entity>& entity, MonthlyStats& stats) {
        if (!entity || !isValid(entity->r, entity->c)) return;

        if (cells[entity->r][entity->c] == entity) {
            cells[entity->r][entity->c] = nullptr;
        }
        entity->kill();

        if (entity->type == EntityType::PLANT) {
            plants.erase(std::remove(plants.begin(), plants.end(), entity), plants.end());
        } else if (entity->type == EntityType::HERBIVORE) {
            herbivores.erase(std::remove(herbivores.begin(), herbivores.end(), entity), herbivores.end());
        } else if (entity->type == EntityType::CARNIVORE) {
            carnivores.erase(std::remove(carnivores.begin(), carnivores.end(), entity), carnivores.end());
        }
    }

    void moveEntity(const std::shared_ptr<Entity>& entity, int newR, int newC) {
        if (!entity) {
            return;
        }

        if (isValid(entity->r, entity->c) && cells[entity->r][entity->c] == entity) {
            cells[entity->r][entity->c] = nullptr;
        }

        entity->r = newR;
        entity->c = newC;

        if (isValid(newR, newC)) {
            cells[newR][newC] = entity;
        } else {
            entity->kill();
        }
    }

    // START OF UPDATED DISPLAY FUNCTION
    void display() const {

        std::cout << std::setw(5) << " "; 

        for (int j = 0; j < GRID_WIDTH; ++j) {

            std::cout << std::setw(2) << std::left << j;
        }
        std::cout << std::endl;


        std::cout << std::setw(5) << " ";
        for (int j = 0; j < GRID_WIDTH; ++j) {
            std::cout << "--"; 
        }
        std::cout << std::endl;

        // Grid rows
        for (int i = 0; i < GRID_HEIGHT; ++i) {

            std::cout << std::setw(2) << std::right << i << " | ";
            
            // Print grid cells
            for (int j = 0; j < GRID_WIDTH; ++j) {
                char displaySymbol = '*';
                if (cells[i][j]) {
                    if (cells[i][j]->type == EntityType::PLANT)
                        displaySymbol = 'p';
                    else if (cells[i][j]->type == EntityType::HERBIVORE) {
                        if (cells[i][j]->getGender() == Gender::MALE)
                            displaySymbol = 'H';
                        else
                            displaySymbol = 'h';
                    }
                    else if (cells[i][j]->type == EntityType::CARNIVORE) {
                        if (cells[i][j]->getGender() == Gender::MALE)
                            displaySymbol = 'C';
                        else
                            displaySymbol = 'c';
                    }
                }

                std::cout << displaySymbol << " "; 
            }
            std::cout << std::endl;
        }
    }
    // END OF UPDATED DISPLAY FUNCTION

    std::vector<std::pair<int, int>> getAdjacentEmptyCells(int r_coord, int c_coord) const {
        std::vector<std::pair<int, int>> emptyCells_vec; // Changed name to avoid conflict if any
        int dr[] = {-1, -1, -1, 0, 0, 1, 1, 1};
        int dc[] = {-1, 0, 1, -1, 1, -1, 0, 1};
        for (int i = 0; i < 8; ++i) {
            int nr = r_coord + dr[i];
            int nc = c_coord + dc[i];
            if (isEmpty(nr, nc)) {
                emptyCells_vec.push_back({nr, nc});
            }
        }
        return emptyCells_vec;
    }

    std::vector<std::shared_ptr<Entity>> findNearbyEntities(int r_coord, int c_coord, EntityType targetType, int range) const {
        std::vector<std::shared_ptr<Entity>> found;
        for (int dr_scan = -range; dr_scan <= range; ++dr_scan) {
            for (int dc_scan = -range; dc_scan <= range; ++dc_scan) {
                if (dr_scan == 0 && dc_scan == 0) continue;
                int nr = r_coord + dr_scan;
                int nc = c_coord + dc_scan;
                if (isValid(nr, nc) && cells[nr][nc] && cells[nr][nc]->type == targetType && cells[nr][nc]->isAlive()) {
                    found.push_back(cells[nr][nc]);
                }
            }
        }
        return found;
    }
};

// --- Plant Class ---
class Plant : public Entity {
public:
    int spreadChance;
    int maxAge;
    int age;

    Plant(int r_coord, int c_coord)
        : Entity(r_coord, c_coord, EntityType::PLANT, 'p'), spreadChance(25), maxAge(40), age(0) {} // Increased spreadChance, maxAge

      std::string getSpeciesName() const override {
        return "Plant";
    }

    void update(Grid& grid, MonthlyStats& stats) override {
        if (!alive) return;
        age++;
        if (age > maxAge) {
            if (alive) {
                stats.plantsEatenOrDied++;
                kill();
            }
            return;
        }

        if (getRandomInt(1, 100) <= spreadChance) {
        auto localEmptyCells = grid.getAdjacentEmptyCells(r, c);
        for (const auto& newPos : localEmptyCells) {  // Try all empty cells
                if (getRandomInt(1, 100) <= 50) {  // 50% chance per cell
                auto newPlant = std::make_shared<Plant>(newPos.first, newPos.second);
                if (grid.addEntity(newPlant)) {
                    stats.plantsSpread++;
                    }
                } 
            }
        }
    }

};

// --- Animal Base Class ---
class Animal : public Entity {
public:
    Gender gender;
    int age;
    int maxAge;
    int energy;
    int maxEnergy;
    int visionRange;
    int moveCost;
    int reproductionCooldown;
    int currentReproductionCooldown;
    bool isPregnant;
    int gestationPeriod;
    int currentGestationTime;
    int minBreedingAge;
    int energyToReproduce;
    int turnsSinceLastMeal;
    int maxTurnsWithoutFood;
    double size; //added size for realistic simulation

    Animal(int r_coord, int c_coord, EntityType type, char maleSymbol, char femaleSymbol, Gender gender_val, // Renamed parameter
           int maxAge_val, int maxEnergy_val, int visionRange_val, int moveCost_val,
           int gestationPeriod_val, int minBreedingAge_val, int energyToReproduce_val, int maxTurnsWithoutFood_val, double size_val)
        : Entity(r_coord, c_coord, type, (gender_val == Gender::MALE ? maleSymbol : femaleSymbol)), // Used renamed parameter
          gender(gender_val), age(0), maxAge(maxAge_val), energy(maxEnergy_val / 2 + getRandomInt(0, maxEnergy_val / 4)), maxEnergy(maxEnergy_val),
          visionRange(visionRange_val), moveCost(moveCost_val), reproductionCooldown(1), currentReproductionCooldown(0), // Reduced cooldown
          isPregnant(false), gestationPeriod(gestationPeriod_val), currentGestationTime(0),
          minBreedingAge(minBreedingAge_val), energyToReproduce(energyToReproduce_val),
          turnsSinceLastMeal(0), maxTurnsWithoutFood(maxTurnsWithoutFood_val), size(size_val) {}

    Gender getGender() const override { return gender; }

    virtual void die(MonthlyStats& stats, bool eaten = false) {
        if (!alive) return;
        kill();
        if (!eaten) {
            if (type == EntityType::HERBIVORE) stats.herbivoresDiedNatural++;
            else if (type == EntityType::CARNIVORE) stats.carnivoresDiedNatural++;
        }
    }

    virtual void baseUpdate(MonthlyStats& stats) {
        if (!alive) return;

        age++;
        energy -= 1 + moveCost/2; // Increased energy consumption
        turnsSinceLastMeal++;

        if (currentReproductionCooldown > 0) {
            currentReproductionCooldown--;
        }

        if (isPregnant) {
            currentGestationTime++;
            energy -= 3; // Increased energy cost of pregnancy
        }

        if (energy <= 0 || age > maxAge || turnsSinceLastMeal > maxTurnsWithoutFood) {
            die(stats); // die() will set alive to false
        }
    }

    virtual void attemptReproduce(Grid& grid, MonthlyStats& stats, const std::vector<std::shared_ptr<Animal>>& potentialMates) = 0;
    virtual void giveBirth(Grid& grid, MonthlyStats& stats) = 0;
    virtual bool attemptEat(Grid& grid, MonthlyStats& stats) = 0; // Changed return type to bool
    virtual void move(Grid& grid, MonthlyStats& stats) = 0;

    void update(Grid& grid, MonthlyStats& stats) override {
        if (!alive) return;
        baseUpdate(stats);
        if (!alive) return;

        if (attemptEat(grid, stats)) return; // Eat first, and if successful, return.
        if (!alive) return;

        if (isPregnant && currentGestationTime >= gestationPeriod) {
            giveBirth(grid, stats); // This might change 'alive' status if energy drops significantly
            isPregnant = false; // Reset pregnancy state regardless of birth success
            currentGestationTime = 0;
            currentReproductionCooldown = reproductionCooldown; // Apply cooldown
            energy -= maxEnergy / 3; // Increased cost of giving birth
            if (energy <= 0 && alive) die(stats); // Check for death after birth energy cost
        }

        if (!alive) return;

        move(grid, stats); // This might change 'alive' status due to energy cost or moving off-grid
        if (energy <= 0 && alive) die(stats);
    }

    bool canReproduce() const {
        return gender == Gender::FEMALE && !isPregnant && age >= minBreedingAge && energy >= energyToReproduce && currentReproductionCooldown == 0;
    }
};

// --- Herbivore Class ---
class Herbivore : public Animal {
public:
    Herbivore(int r_coord, int c_coord, Gender gender_val)
        : Animal(r_coord, c_coord, EntityType::HERBIVORE, 'H', 'h', gender_val,
                 70, 120, 5, 10, 3, 2, 40, 2, 1.0) {} 
// Increased maxAge, maxEnergy, visionRange, reduced moveCost, increased energyToReproduce, reduced maxTurnsWithoutFood, added size
    std::string getSpeciesName() const override {
        return "Herbivore";
    }

    bool attemptEat(Grid& grid, MonthlyStats& stats) override {
    if (!alive || energy >= maxEnergy * 0.95) return false;

    // Scan in expanding concentric rings up to 5 cells away
    for (int radius = 1; radius <= 7; ++radius) {
        // Check all positions at current radius
        for (int dr = -radius; dr <= radius; ++dr) {
            for (int dc = -radius; dc <= radius; ++dc) {
                // Only check perimeter of the square (not inner cells)
                if (abs(dr) == radius || abs(dc) == radius) {
                    int nr = r + dr;
                    int nc = c + dc;
                    
                    if (grid.isValid(nr, nc)) {
                        auto plantEntity = grid.getEntity(nr, nc);
                        if (plantEntity && plantEntity->type == EntityType::PLANT && plantEntity->isAlive()) {
                            // Energy gain scales inversely with distance (closer = more energy)
                            int energyGain = 35;
                            energy = std::min(maxEnergy, energy + energyGain);
                            
                            plantEntity->kill();
                            stats.plantsEatenOrDied++;
                            stats.monthlyEvents.push_back(
                                "Herbivore ate plant at (" + 
                                std::to_string(nr) + "," + 
                                std::to_string(nc) + ") from " +
                                std::to_string(radius) + " cells away"
                            );
                            grid.removeEntity(plantEntity, stats);
                            turnsSinceLastMeal = 0;
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

    void move(Grid& grid, MonthlyStats& stats) override {
        if (!alive) return;
        energy -= moveCost;
        if (energy <= 0) {
            if (alive) die(stats);
            return;
        }

        // 1. Flee from carnivores
        auto carnivoresNearby = grid.findNearbyEntities(r, c, EntityType::CARNIVORE, visionRange); // Increased vision
        if (!carnivoresNearby.empty()) {

        energy -= 5;
        if (energy <= 0) {
            if (alive) die(stats);
            return;
        }

            double avgCarnR = 0, avgCarnC = 0;
            for (const auto& carn : carnivoresNearby) {
                avgCarnR += carn->r;
                avgCarnC += carn->c;
            }
            avgCarnR /= carnivoresNearby.size();
            avgCarnC /= carnivoresNearby.size();

            int bestR = r, bestC = c;
            double maxDistSq = -1;

            for (int dr_move = -1; dr_move <= 1; ++dr_move) {
                for (int dc_move = -1; dc_move <= 1; ++dc_move) {
                    if (dr_move == 0 && dc_move == 0) continue;
                    int nr = r + dr_move;
                    int nc = c + dc_move;
                    if (grid.isEmpty(nr, nc)) {
                        double distSqToCarn = (nr - avgCarnR) * (nr - avgCarnR) + (nc - avgCarnC) * (nc - avgCarnC);
                        if (distSqToCarn > maxDistSq) {
                            maxDistSq = distSqToCarn;
                            bestR = nr;
                            bestC = nc;
                        }
                    }
                }
            }
            if (bestR != r || bestC != c) {
                grid.moveEntity(grid.getEntity(r, c), bestR, bestC);
                return;
            }
        }
                
                energy -= moveCost;
                if (energy <= 0) {
                    if (alive) die(stats);
                    return;
                }

        // 2. Move towards plants if hungry
        if (energy < maxEnergy * 0.9) {
            auto plantsNearby = grid.findNearbyEntities(r, c, EntityType::PLANT, visionRange);
            if (!plantsNearby.empty()) {

                std::shared_ptr<Entity> closestPlant = nullptr;
                double minDistSq = 1e9;

                for (const auto& plant : plantsNearby) {
                    double distSq = (plant->r - r) * (plant->r - r) + (plant->c - c) * (plant->c - c);
                    if (distSq < minDistSq) {
                        minDistSq = distSq;
                        closestPlant = plant;
                    }
                }

                if (closestPlant) {
                    int dr_move = (closestPlant->r > r) ? 1 : ((closestPlant->r < r) ? -1 : 0); // Move one step towards target
                    int dc_move = (closestPlant->c > c) ? 1 : ((closestPlant->c < c) ? -1 : 0); // Move one step towards target
                    int nr = r + dr_move;
                    int nc = c + dc_move;

                    if (grid.isValid(nr, nc) && (grid.isEmpty(nr, nc) || (grid.getEntity(nr, nc) && grid.getEntity(nr, nc)->type == EntityType::PLANT))) {
                        grid.moveEntity(grid.getEntity(r, c), nr, nc);
                        return;
                    }
                }
            }
        }

    else {    // 3. Random move
        auto localEmptyCells = grid.getAdjacentEmptyCells(r, c);
        if (!localEmptyCells.empty()) {
            int choice = getRandomInt(0, localEmptyCells.size() - 1);
            grid.moveEntity(grid.getEntity(r, c), localEmptyCells[choice].first, localEmptyCells[choice].second);
        }
    }
    }

    void attemptReproduce(Grid& grid, MonthlyStats& stats, const std::vector<std::shared_ptr<Animal>>& potentialMates) override {
        if (!alive || !canReproduce()) return;

        for (const auto& mate_entity_base : potentialMates) {
            if (!mate_entity_base->isAlive() || mate_entity_base.get() == this) continue;

            if (mate_entity_base->type != EntityType::HERBIVORE || mate_entity_base->getGender() != Gender::MALE) continue;
            auto potential_mate = std::static_pointer_cast<Herbivore>(mate_entity_base);

            if (potential_mate->age >= potential_mate->minBreedingAge) {
                
                     isPregnant = true;
                     currentGestationTime = 0;
                     energy -= energyToReproduce / 2;
                     if (potential_mate->isAlive()) potential_mate->energy -= potential_mate->energyToReproduce / 4; // Corrected to use potential_mate
                     return;
            }
        }
    }

void giveBirth(Grid& grid, MonthlyStats& stats) override {
    
    if (!alive) return;

    // Get ALL empty cells in the grid
    std::vector<std::pair<int, int>> allEmptyCells;
    for (int i = 0; i < GRID_HEIGHT; ++i) {
        for (int j = 0; j < GRID_WIDTH; ++j) {
            if (grid.isEmpty(i, j)) {
                allEmptyCells.emplace_back(i, j);
            }
        }
    }

    if (allEmptyCells.empty()) return;

    int offspringCount = getRandomInt(1, 3);
    for (int i = 0; i < offspringCount && !allEmptyCells.empty(); ++i) {
        int choice_idx = (allEmptyCells.size() > 1) ? getRandomInt(0, allEmptyCells.size() - 1) : 0;
        
        auto birthPos = allEmptyCells[choice_idx];
        allEmptyCells.erase(allEmptyCells.begin() + choice_idx);

        Gender childGender = (getRandomInt(0, 1) == 0) ? Gender::MALE : Gender::FEMALE;
        auto newHerbivore = std::make_shared<Herbivore>(birthPos.first, birthPos.second, childGender);
        if (grid.addEntity(newHerbivore)) {
            stats.herbivoresSpawned++;
        }
    }
}

};

// --- Carnivore Class ---
class Carnivore : public Animal {
public:
    Carnivore(int r_coord, int c_coord, Gender gender_val)
        : Animal(r_coord, c_coord, EntityType::CARNIVORE, 'C', 'c', gender_val,
                 100, 120, 6, 15, 4, 5, 50, 2, 1.5) {} // Increased values 
    std::string getSpeciesName() const override {
        return "Carnivore";
    }

bool attemptEat(Grid& grid, MonthlyStats& stats) override {
    if (!alive || energy >= maxEnergy * 0.9) return false;

    // Scan in expanding concentric rings up to 5 cells away
    for (int radius = 1; radius <= 4; ++radius) {
        // Check all positions at current radius
        for (int dr = -radius; dr <= radius; ++dr) {
            for (int dc = -radius; dc <= radius; ++dc) {
                // Only check perimeter of the square
                if (abs(dr) == radius || abs(dc) == radius) {
                    int nr = r + dr;
                    int nc = c + dc;
                    
                    if (grid.isValid(nr, nc)) {
                        auto targetEntity = grid.getEntity(nr, nc);
                        if (targetEntity && targetEntity->type == EntityType::HERBIVORE && targetEntity->isAlive()) {
                            // Energy gain scales with distance (closer kills = more energy)
                            int energyGain = 25;
                            energy = std::min(maxEnergy, energy + energyGain);
                            
                            auto herbivore = std::static_pointer_cast<Herbivore>(targetEntity);
                            herbivore->die(stats, true);
                            stats.herbivoresEaten++;
                            stats.monthlyEvents.push_back(
                                "Carnivore ate herbivore at (" + 
                                std::to_string(nr) + "," + 
                                std::to_string(nc) + ") from " +
                                std::to_string(radius) + " cells away"
                            );
                            grid.removeEntity(herbivore, stats);
                            turnsSinceLastMeal = 0;
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

   void move(Grid& grid, MonthlyStats& stats) override {
    if (!alive) return;
    energy -= moveCost;  
    if (energy <= 0) {
        if (alive) die(stats);
        return;
    }

    // 2. Move towards herbivores if hungry
    if (energy < maxEnergy * 0.85) {
        auto herbivoresNearby = grid.findNearbyEntities(r, c, EntityType::HERBIVORE, visionRange);
        if (!herbivoresNearby.empty()) {
            std::shared_ptr<Entity> closestHerbivore = nullptr;
            double minDistSq = 1e9;

            // Find closest herbivore
            for (const auto& herb : herbivoresNearby) {
                double distSq = (herb->r - r) * (herb->r - r) + (herb->c - c) * (herb->c - c);
                if (distSq < minDistSq) {
                    minDistSq = distSq;
                    closestHerbivore = herb;
                }
            }

            // Move toward closest herbivore
            if (closestHerbivore) {
                int dr_move = (closestHerbivore->r > r) ? 1 : ((closestHerbivore->r < r) ? -1 : 0); // Move one step towards target
                int dc_move = (closestHerbivore->c > c) ? 1 : ((closestHerbivore->c < c) ? -1 : 0); // Move one step towards target
                int nr = r + dr_move;
                int nc = c + dc_move;

                // Can move to empty cells or cells containing herbivores
                if (grid.isValid(nr, nc) && (grid.isEmpty(nr, nc) || (grid.getEntity(nr, nc) && grid.getEntity(nr, nc)->type == EntityType::HERBIVORE))) {
                    grid.moveEntity(grid.getEntity(r, c), nr, nc);
                    return;
                }
            }
        }
    }

    // 3. Random move if no herbivores found
    else {auto localEmptyCells = grid.getAdjacentEmptyCells(r, c);
    if (!localEmptyCells.empty()) {
        int choice = getRandomInt(0, localEmptyCells.size() - 1);
        grid.moveEntity(grid.getEntity(r, c), 
                         localEmptyCells[choice].first, 
                         localEmptyCells[choice].second);
        }
    }
}
    void attemptReproduce(Grid& grid, MonthlyStats& stats, const std::vector<std::shared_ptr<Animal>>& potentialMates) override {
        if (!alive || !canReproduce()) return;

        for (const auto& mate_entity_base : potentialMates) {
            if (!mate_entity_base->isAlive() || mate_entity_base.get() == this) continue;

            if (mate_entity_base->type != EntityType::CARNIVORE || mate_entity_base->getGender() != Gender::MALE) continue;
            auto potential_mate = std::static_pointer_cast<Carnivore>(mate_entity_base);


            if (potential_mate->age >= potential_mate->minBreedingAge) {
                if (std::abs(r - potential_mate->r) <= 7 && std::abs(c - potential_mate->c) <= 7) {
                    isPregnant = true;
                    currentGestationTime = 0;
                    energy -= energyToReproduce / 2;
                    if (potential_mate->isAlive()) potential_mate->energy -= potential_mate->energyToReproduce / 4;
                    return;
                }
            }
        }
    }

   void giveBirth(Grid& grid, MonthlyStats& stats) override {
    if (!alive) return;

    // Get ALL empty cells in the entire grid
    std::vector<std::pair<int, int>> allEmptyCells;
    for (int i = 0; i < GRID_HEIGHT; ++i) {
        for (int j = 0; j < GRID_WIDTH; ++j) {
            if (grid.isEmpty(i, j)) {
                allEmptyCells.emplace_back(i, j);
            }
        }
    }

    if (allEmptyCells.empty()) return;

    int offspringCount = getRandomInt(1, 2); // 1-2 offspring
    for (int i = 0; i < offspringCount && !allEmptyCells.empty(); ++i) {
        int choice_idx = (allEmptyCells.size() > 1) ? 
                                getRandomInt(0, allEmptyCells.size() - 1) : 0;
        
        auto birthPos = allEmptyCells[choice_idx];
        allEmptyCells.erase(allEmptyCells.begin() + choice_idx);

        Gender childGender = (getRandomInt(0, 1) == 0) ? Gender::MALE : Gender::FEMALE;
        auto newCarnivore = std::make_shared<Carnivore>(birthPos.first, birthPos.second, childGender);
        if (grid.addEntity(newCarnivore)) {
            stats.carnivoresSpawned++;
        }
    }
}
};


// --- Simulation Class ---
class Simulation {
public:
    Grid grid;
    MonthlyStats stats;
    int totalMonths;
    int currentMonth;
    bool carnivoresStarvedLastMonth; //track starvation

    Simulation() : currentMonth(0), carnivoresStarvedLastMonth(false) {}

    void initialize() {
        int years;
        int numPlants, numHerbivores, numCarnivores;

        std::cout << "Enter number of years for simulation: ";
        std::cin >> years;
        totalMonths = years * 12;

        std::cout << "Enter initial number of plants: ";
        std::cin >> numPlants;
        std::cout << "Enter initial number of herbivores: ";
        std::cin >> numHerbivores;
        std::cout << "Enter initial number of carnivores: ";
        std::cin >> numCarnivores;

        auto placeInitialEntity = [&](EntityType type, int count) {
            for (int i = 0; i < count; ++i) {
                int r_coord, c_coord;
                int attempts = 0;
                do {
                    r_coord = getRandomInt(0, GRID_HEIGHT - 1);
                    c_coord = getRandomInt(0, GRID_WIDTH - 1);
                    attempts++;
                } while (!grid.isEmpty(r_coord, c_coord) && attempts < GRID_WIDTH * GRID_HEIGHT * 2);

                if (grid.isEmpty(r_coord, c_coord)) {
                    std::shared_ptr<Entity> newEntity = nullptr;
                    if (type == EntityType::PLANT) {
                        newEntity = std::make_shared<Plant>(r_coord, c_coord);
                    } else if (type == EntityType::HERBIVORE) {
                        Gender g = (getRandomInt(0, 1) == 0) ? Gender::MALE : Gender::FEMALE;
                        newEntity = std::make_shared<Herbivore>(r_coord, c_coord, g);
                    } else if (type == EntityType::CARNIVORE) {
                        Gender g = (getRandomInt(0, 1) == 0) ? Gender::MALE : Gender::FEMALE;
                        newEntity = std::make_shared<Carnivore>(r_coord, c_coord, g);
                    }
                    if (newEntity) {
                        grid.addEntity(newEntity);
                    }
                } else {
                    std::cout << "Warning: Could not place all initial " << (
                        type == EntityType::PLANT ? "plants." :
                        type == EntityType::HERBIVORE ? "herbivores." : "carnivores."
                        ) << " Due to lack of space." << std::endl;
                    break;
                }
            }
        };

        placeInitialEntity(EntityType::PLANT, numPlants);
        placeInitialEntity(EntityType::HERBIVORE, numHerbivores);
        placeInitialEntity(EntityType::CARNIVORE, numCarnivores);
    }

    void runMonth() {
        currentMonth++;
        std::cout << "\n--- Month: " << currentMonth << " (Year " << (currentMonth - 1) / 12 + 1 << ") ---" << std::endl;
        stats.reset();
        carnivoresStarvedLastMonth = false; //reset

        auto currentPlants = grid.plants;
        auto currentHerbivores = grid.herbivores;
        auto currentCarnivores = grid.carnivores;

        // Handle carnivores eating first
        for (const auto& carnivore_ptr : currentCarnivores) {
            if (carnivore_ptr->isAlive()) {
                carnivore_ptr->update(grid, stats);
            }
        }


        //handle herbivores eating
        for (const auto& herbivore_ptr : currentHerbivores) {
            if (herbivore_ptr->isAlive()) {
                herbivore_ptr->update(grid, stats);
            }
        }

        // Animal reproduction
        for (const auto& herbivore : currentHerbivores) {
            if (herbivore->isAlive()) {
                std::vector<std::shared_ptr<Animal>> potentialMates;
                for (const auto& otherHerbivore : currentHerbivores) {
                    if (otherHerbivore.get() != herbivore.get()) { //prevent self mating
                        potentialMates.push_back(otherHerbivore);
                    }
                }
                herbivore->attemptReproduce(grid, stats, potentialMates);
            }
        }

        for (const auto& carnivore : currentCarnivores) {
            if (carnivore->isAlive()) {
                std::vector<std::shared_ptr<Animal>> potentialMates;
                   for (const auto& otherCarnivore : currentCarnivores) {
                    if (otherCarnivore.get() != carnivore.get()) { //prevent self mating
                        potentialMates.push_back(otherCarnivore);
                    }
                }
                carnivore->attemptReproduce(grid, stats, potentialMates);
            }
        }

        // Plant updates
        for (const auto& plant_ptr : currentPlants) {
            if (plant_ptr->isAlive()) {
                plant_ptr->update(grid, stats);
            }
        }

        auto cleanupDeadEntities = [&](auto& entityList) {
            entityList.erase(
                std::remove_if(entityList.begin(), entityList.end(),
                               [&](const std::shared_ptr<Entity>& e_ptr) {
                                   if (!e_ptr->isAlive()) {
                                       if (grid.isValid(e_ptr->r, e_ptr->c) && grid.cells[e_ptr->r][e_ptr->c] == e_ptr) {
                                           grid.cells[e_ptr->r][e_ptr->c] = nullptr;
                                       }
                                       return true;
                                   }
                                   return false;
                               }),
                entityList.end());
        };

        cleanupDeadEntities(grid.plants);
        cleanupDeadEntities(grid.herbivores);
        cleanupDeadEntities(grid.carnivores);

        stats.currentPlants = grid.plants.size();
        stats.currentHerbivores = grid.herbivores.size();
        stats.currentCarnivores = grid.carnivores.size();

        grid.display();
        stats.display();

        //check for starvation and end simulation
        if (grid.herbivores.empty() && grid.carnivores.empty() && currentMonth > 1) {
            std::cout << "All animals have died. Simulation ends early." << std::endl;
            currentMonth = totalMonths + 1;
        }
        else if (grid.herbivores.empty() && grid.carnivores.size() > 0)
        {
             std::cout << "All herbivores have died. Carnivores will starve." << std::endl;
             currentMonth = totalMonths + 1;
        }
        // else if (grid.carnivores.empty() && grid.herbivores.size() > 0)
        // {
        //     std::cout << "All carnivores have died. Herbivores will overpopulate." << std::endl;
        //     currentMonth = totalMonths + 1;
        // }
        else if (grid.plants.empty() && grid.herbivores.size() > 0)
        {
            std::cout<< "All plants are dead. Herbivores will starve." << std::endl;
            currentMonth = totalMonths + 1;
        }
        std::cout << "Press Enter to continue to the next month...";

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer
        std::cin.get();
    }

    void start() {
        initialize();
        grid.display();

        stats.currentHerbivores = grid.herbivores.size();
        stats.currentCarnivores = grid.carnivores.size();
        std::cout << "Initial Herbivores: " << stats.currentHerbivores << std::endl;
        std::cout << "Initial Carnivores: " << stats.currentCarnivores << std::endl;
        std::cout << "Initial Plants: " << grid.plants.size() << std::endl;


        while (currentMonth < totalMonths) {
            runMonth();
            if ((grid.herbivores.empty() && grid.carnivores.empty() && grid.plants.empty() && currentMonth > 5) ||
                ((grid.herbivores.empty() || grid.plants.empty()) && grid.carnivores.empty() && currentMonth > 12 && (grid.herbivores.size() < 2 || grid.plants.size() < 5))
                ) {
                std::cout << "\nEcosystem collapse or severe decline. Ending simulation." << std::endl;
                break;
            }
        }
        std::cout << "\nSimulation Finished after " << std::min(currentMonth, totalMonths) << " months." << std::endl;
    }
};


int main() {
    Simulation sim;
    sim.start();
    return 0;
}