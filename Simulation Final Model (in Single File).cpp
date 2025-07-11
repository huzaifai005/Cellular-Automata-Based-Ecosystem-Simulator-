#include <iostream>
#include <vector>
#include <memory>
#include <random>
#include <algorithm>
#include <iomanip>
#include <functional>
#include <string>
#include <map>
#include <cctype>
#include <cmath>  
#include <limits> 

//  Configuration Constants 
const int GRID_WIDTH = 20;
const int GRID_HEIGHT = 20;

//  Enums 
enum class EntityType
{
    EMPTY,
    PLANT,
    HERBIVORE,
    CARNIVORE
};
enum class Gender
{
    MALE,
    FEMALE,
    NONE
};
enum class Season
{
    WINTER,
    SPRING,
    SUMMER,
    AUTUMN,
    NONE
};

//  Forward Declarations 
class Grid;
class Entity;
class Plant;
class Animal;
class Herbivore;
class Carnivore;
class Simulation;
struct MonthlyStats;

//  Global Random Number Generation & Helpers 
std::random_device rd;
std::mt19937 gen(rd());

int getRandomInt(int min, int max)
{
    if (min > max)
        std::swap(min, max);
    if (min == max)
        return min;
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}

double getRandomDouble(double min, double max)
{
    if (min > max)
        std::swap(min, max);
    std::uniform_real_distribution<> distrib(min, max);
    return distrib(gen);
}

std::string getSeasonName(Season season)
{
    switch (season)
    {
    case Season::WINTER:
        return "Winter";
    case Season::SPRING:
        return "Spring";
    case Season::SUMMER:
        return "Summer";
    case Season::AUTUMN:
        return "Autumn";
    default:
        return "Unknown";
    }
}

//  Statistics Struct Definition 
struct MonthlyStats
{
    int plantsEaten = 0;
    int plantsDiedNaturalAge = 0;
    int plantsDiedWeather = 0;
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
    int animalsImmigrated = 0;
    int animalsEmigrated = 0;
    std::string currentMonthName = "";
    std::string currentSeasonName = "";
    std::vector<std::string> monthlyEvents;
    void reset();
    void display() const;
};

//  Entity Class Definition (Declaration + Inline Simple Methods) 
class Entity
{
public:
    int r, c;
    EntityType type;
    char symbol;
    bool alive;

    Entity(int r_coord, int c_coord, EntityType type_val, char symbol_val)
        : r(r_coord), c(c_coord), type(type_val), symbol(symbol_val), alive(true) {}
    virtual ~Entity() = default;
    virtual void update(Grid &grid, MonthlyStats &stats, Season currentSeason) = 0;
    bool isAlive() const { return alive; }
    void kill() { alive = false; }
    virtual Gender getGender() const { return Gender::NONE; }
    virtual std::string getSpeciesName() const = 0;
};

//  Animal Class Declaration 
class Animal : public Entity
{
public:
    Gender gender;
    int age;
    int maxAge;
    int energy;
    int maxEnergy;
    int visionRange;
    int baseMoveCost;
    int reproductionCooldown;
    int currentReproductionCooldown;
    bool isPregnant;
    int gestationPeriod;
    int currentGestationTime;
    int minBreedingAge;
    int energyToReproduce;
    int turnsSinceLastMeal;
    int maxTurnsWithoutFood;
    double size;

    Animal(int r_coord, int c_coord, EntityType type_val, char maleSymbol, char femaleSymbol, Gender gender_val,
           int maxAge_val, int maxEnergy_val, int visionRange_val, int moveCost_val,
           int gestationPeriod_val, int minBreedingAge_val, int energyToReproduce_val,
           int maxTurnsWithoutFood_val, double size_val);

    Gender getGender() const override;
    virtual void die(MonthlyStats &stats, bool eaten = false);
    virtual void baseUpdate(MonthlyStats &stats, Season currentSeason);
    void update(Grid &grid, MonthlyStats &stats, Season currentSeason) override;

    virtual void attemptReproduce(Grid &grid, MonthlyStats &stats, const std::vector<std::shared_ptr<Animal>> &potentialMates, Season currentSeason) = 0;
    virtual void giveBirth(Grid &grid, MonthlyStats &stats) = 0;
    virtual bool attemptEat(Grid &grid, MonthlyStats &stats, Season currentSeason) = 0;
    virtual void move(Grid &grid, MonthlyStats &stats, Season currentSeason) = 0;
    bool canReproduceInternal() const;
};

// Plant Class Declaration
class Plant : public Entity
{
public:
    int baseSpreadChance;
    int maxAge;
    int age;
    const int WINTER_DEATH_CHANCE = 20;
    const int AUTUMN_DEATH_CHANCE = 10;
    Plant(int r_coord, int c_coord);
    std::string getSpeciesName() const override;
    void update(Grid &grid, MonthlyStats &stats, Season currentSeason) override;
};

// Herbivore Class Declaration
class Herbivore : public Animal
{
public:
    Herbivore(int r_coord, int c_coord, Gender gender_val);
    std::string getSpeciesName() const override;
    bool attemptEat(Grid &grid, MonthlyStats &stats, Season currentSeason) override;
    void move(Grid &grid, MonthlyStats &stats, Season currentSeason) override;
    void attemptReproduce(Grid &grid, MonthlyStats &stats, const std::vector<std::shared_ptr<Animal>> &potentialMates, Season currentSeason) override;
    void giveBirth(Grid &grid, MonthlyStats &stats) override;
};

// Carnivore Class Declaration
class Carnivore : public Animal
{
public:
    Carnivore(int r_coord, int c_coord, Gender gender_val);
    std::string getSpeciesName() const override;
    bool attemptEat(Grid &grid, MonthlyStats &stats, Season currentSeason) override;
    void move(Grid &grid, MonthlyStats &stats, Season currentSeason) override;
    void attemptReproduce(Grid &grid, MonthlyStats &stats, const std::vector<std::shared_ptr<Animal>> &potentialMates, Season currentSeason) override;
    void giveBirth(Grid &grid, MonthlyStats &stats) override;
};

// Grid Class Declaration
class Grid
{
public:
    std::vector<std::vector<std::shared_ptr<Entity>>> cells;
    std::vector<std::shared_ptr<Plant>> plants;
    std::vector<std::shared_ptr<Herbivore>> herbivores;
    std::vector<std::shared_ptr<Carnivore>> carnivores;

    Grid(); // Constructor
    bool isValid(int r_coord, int c_coord) const;
    bool isEmpty(int r_coord, int c_coord) const;
    std::shared_ptr<Entity> getEntity(int r_coord, int c_coord) const;
    bool addEntity(const std::shared_ptr<Entity> &entity);
    bool addMigratingAnimal(const std::shared_ptr<Animal> &animal_ptr);
    void removeEntity(const std::shared_ptr<Entity> &entity_ptr, MonthlyStats &stats);
    void moveEntity(const std::shared_ptr<Entity> &entity_ptr, int newR, int newC);
    void display() const;
    std::vector<std::pair<int, int>> getAdjacentEmptyCells(int r_coord, int c_coord) const;
    std::vector<std::shared_ptr<Entity>> findNearbyEntities(int r_coord, int c_coord, EntityType targetType, int range_val) const;
};

// Simulation Class Declaration
class Simulation
{
public:
    Grid grid;
    MonthlyStats stats;
    int totalMonthsToSim;
    int currentMonthSimTime;
    bool carnivoresStarvedLastMonth_sim;
    bool isNorthernHemisphere_sim;
    Season currentSeason_sim;
    int currentMonthOfYear_sim;
    std::vector<std::string> monthNames_sim = {
        "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
    Simulation(); // Constructor
    void determineSeason();
    void initialize();
    void handleMigration();
    void runMonth();
    void start();
};

// Method Definitions for MonthlyStats
void MonthlyStats::reset()
{
    plantsEaten = 0;
    plantsDiedNaturalAge = 0;
    plantsDiedWeather = 0;
    plantsSpread = 0;
    currentPlants = 0;
    herbivoresEaten = 0;
    herbivoresDiedNatural = 0;
    herbivoresSpawned = 0;
    currentHerbivores = 0;
    carnivoresEaten = 0;
    carnivoresDiedNatural = 0;
    carnivoresSpawned = 0;
    currentCarnivores = 0;
    animalsImmigrated = 0;
    animalsEmigrated = 0;
    monthlyEvents.clear();
}
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

// Method Definitions for Grid
Grid::Grid() : cells(GRID_HEIGHT, std::vector<std::shared_ptr<Entity>>(GRID_WIDTH, nullptr)) {}
bool Grid::isValid(int r_coord, int c_coord) const
{
    return r_coord >= 0 && r_coord < GRID_HEIGHT && c_coord >= 0 && c_coord < GRID_WIDTH;
}
bool Grid::isEmpty(int r_coord, int c_coord) const
{
    return isValid(r_coord, c_coord) && cells[r_coord][c_coord] == nullptr;
}
std::shared_ptr<Entity> Grid::getEntity(int r_coord, int c_coord) const
{
    if (isValid(r_coord, c_coord))
        return cells[r_coord][c_coord];
    return nullptr;
}
bool Grid::addEntity(const std::shared_ptr<Entity> &entity)
{
    if (!entity)
        return false;
    if (plants.size() + herbivores.size() + carnivores.size() >= (size_t)GRID_WIDTH * GRID_HEIGHT)
        return false;
    // Entity's members r, c are accessible as Entity is fully declared
    if (isValid(entity->r, entity->c) && isEmpty(entity->r, entity->c))
    {
        cells[entity->r][entity->c] = entity;
        // Types Plant, Herbivore, Carnivore are known from forward declarations for static_pointer_cast
        if (entity->type == EntityType::PLANT)
            plants.push_back(std::static_pointer_cast<Plant>(entity));
        else if (entity->type == EntityType::HERBIVORE)
            herbivores.push_back(std::static_pointer_cast<Herbivore>(entity));
        else if (entity->type == EntityType::CARNIVORE)
            carnivores.push_back(std::static_pointer_cast<Carnivore>(entity));
        return true;
    }
    return false;
}
bool Grid::addMigratingAnimal(const std::shared_ptr<Animal> &animal_ptr)
{
    for (int attempts = 0; attempts < GRID_WIDTH * GRID_HEIGHT; ++attempts)
    {
        int r_coord = getRandomInt(0, GRID_HEIGHT - 1);
        int c_coord = getRandomInt(0, GRID_WIDTH - 1);
        if (isEmpty(r_coord, c_coord))
        {
            animal_ptr->r = r_coord; // Animal members (from Entity) are accessible
            animal_ptr->c = c_coord;
            return addEntity(animal_ptr); // addEntity takes shared_ptr<Entity>
        }
    }
    return false;
}
void Grid::removeEntity(const std::shared_ptr<Entity> &entity_ptr, MonthlyStats &stats)
{
    if (!entity_ptr || !isValid(entity_ptr->r, entity_ptr->c))
        return;
    if (cells[entity_ptr->r][entity_ptr->c] == entity_ptr)
        cells[entity_ptr->r][entity_ptr->c] = nullptr;
    entity_ptr->kill(); // Entity::kill() is accessible
    if (entity_ptr->type == EntityType::PLANT)
    {
        plants.erase(std::remove_if(plants.begin(), plants.end(),
                                    [&](const auto &p)
                                    { return !p->isAlive() || p == entity_ptr; }),
                     plants.end());
    }
    else if (entity_ptr->type == EntityType::HERBIVORE)
    {
        herbivores.erase(std::remove_if(herbivores.begin(), herbivores.end(),
                                        [&](const auto &h)
                                        { return !h->isAlive() || h == entity_ptr; }),
                         herbivores.end());
    }
    else if (entity_ptr->type == EntityType::CARNIVORE)
    {
        carnivores.erase(std::remove_if(carnivores.begin(), carnivores.end(),
                                        [&](const auto &c)
                                        { return !c->isAlive() || c == entity_ptr; }),
                         carnivores.end());
    }
}
void Grid::moveEntity(const std::shared_ptr<Entity> &entity_ptr, int newR, int newC)
{
    if (!entity_ptr)
        return;
    if (isValid(entity_ptr->r, entity_ptr->c) && cells[entity_ptr->r][entity_ptr->c] == entity_ptr)
    {
        cells[entity_ptr->r][entity_ptr->c] = nullptr;
    }
    entity_ptr->r = newR;
    entity_ptr->c = newC;
    if (isValid(newR, newC))
        cells[newR][newC] = entity_ptr;
    else
        entity_ptr->kill();
}
void Grid::display() const
{
    std::cout << std::setw(5) << " ";
    for (int j = 0; j < GRID_WIDTH; ++j)
        std::cout << std::setw(2) << std::left << j;
    std::cout << std::endl;
    std::cout << std::setw(5) << " ";
    for (int j = 0; j < GRID_WIDTH; ++j)
        std::cout << "--";
    std::cout << std::endl;
    for (int i = 0; i < GRID_HEIGHT; ++i)
    {
        std::cout << std::setw(2) << std::right << i << " | ";
        for (int j = 0; j < GRID_WIDTH; ++j)
        {
            char displaySymbol = '*';
            if (cells[i][j])
            {
                if (cells[i][j]->type == EntityType::PLANT)
                    displaySymbol = 'P';
                else if (cells[i][j]->type == EntityType::HERBIVORE)
                    displaySymbol = (cells[i][j]->getGender() == Gender::MALE) ? 'H' : 'h';
                else if (cells[i][j]->type == EntityType::CARNIVORE)
                    displaySymbol = (cells[i][j]->getGender() == Gender::MALE) ? 'C' : 'c';
            }
            std::cout << displaySymbol << " ";
        }
        std::cout << std::endl;
    }
}
std::vector<std::pair<int, int>> Grid::getAdjacentEmptyCells(int r_coord, int c_coord) const
{
    std::vector<std::pair<int, int>> emptyCells_vec;
    int dr_arr[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dc_arr[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    for (int i = 0; i < 8; ++i)
    {
        int nr = r_coord + dr_arr[i];
        int nc = c_coord + dc_arr[i];
        if (isEmpty(nr, nc))
            emptyCells_vec.push_back({nr, nc});
    }
    return emptyCells_vec;
}
std::vector<std::shared_ptr<Entity>> Grid::findNearbyEntities(int r_coord, int c_coord, EntityType targetType, int range_val) const
{
    std::vector<std::shared_ptr<Entity>> found;
    for (int dr_scan = -range_val; dr_scan <= range_val; ++dr_scan)
    {
        for (int dc_scan = -range_val; dc_scan <= range_val; ++dc_scan)
        {
            if (dr_scan == 0 && dc_scan == 0)
                continue;
            int nr = r_coord + dr_scan;
            int nc = c_coord + dc_scan;
            if (isValid(nr, nc) && cells[nr][nc] && cells[nr][nc]->type == targetType && cells[nr][nc]->isAlive())
            {
                found.push_back(cells[nr][nc]);
            }
        }
    }
    return found;
}

// Method Definitions for Animal
Animal::Animal(int r_coord, int c_coord, EntityType type_val, char maleSymbol, char femaleSymbol, Gender gender_val,
               int maxAge_val, int maxEnergy_val, int visionRange_val, int moveCost_val,
               int gestationPeriod_val, int minBreedingAge_val, int energyToReproduce_val,
               int maxTurnsWithoutFood_val, double size_val)
    : Entity(r_coord, c_coord, type_val, (gender_val == Gender::MALE ? maleSymbol : femaleSymbol)),
      gender(gender_val), age(0), maxAge(maxAge_val), energy(maxEnergy_val / 2 + getRandomInt(0, maxEnergy_val / 4)),
      maxEnergy(maxEnergy_val), visionRange(visionRange_val), baseMoveCost(moveCost_val),
      reproductionCooldown(1), currentReproductionCooldown(0), isPregnant(false),
      gestationPeriod(gestationPeriod_val), currentGestationTime(0), minBreedingAge(minBreedingAge_val),
      energyToReproduce(energyToReproduce_val), turnsSinceLastMeal(0),
      maxTurnsWithoutFood(maxTurnsWithoutFood_val), size(size_val) {}

Gender Animal::getGender() const { return gender; }

void Animal::die(MonthlyStats &stats, bool eaten)
{
    if (!alive)
        return;
    kill();
    if (!eaten)
    {
        if (type == EntityType::HERBIVORE)
            stats.herbivoresDiedNatural++;
        else if (type == EntityType::CARNIVORE)
            stats.carnivoresDiedNatural++;
    }
}

void Animal::baseUpdate(MonthlyStats &stats, Season currentSeason)
{
    if (!alive)
        return;
    age++;
    turnsSinceLastMeal++;
    int actualMoveCost = baseMoveCost;
    if (currentSeason == Season::WINTER)
        actualMoveCost += 5;
    else if (currentSeason == Season::AUTUMN)
        actualMoveCost += 2;
    else if (currentSeason == Season::SUMMER)
        actualMoveCost = std::max(1, actualMoveCost - 2);
    energy -= (1 + actualMoveCost / 2);
    if (currentReproductionCooldown > 0)
        currentReproductionCooldown--;
    if (isPregnant)
    {
        currentGestationTime++;
        energy -= 3;
    }
    if (energy <= 0 || age > maxAge || turnsSinceLastMeal > maxTurnsWithoutFood)
        die(stats);
}

bool Animal::canReproduceInternal() const
{
    return gender == Gender::FEMALE && !isPregnant && age >= minBreedingAge && energy >= energyToReproduce && currentReproductionCooldown == 0;
}

void Animal::update(Grid &grid, MonthlyStats &stats, Season currentSeason)
{
    if (!alive)
        return;
    baseUpdate(stats, currentSeason);
    if (!alive)
        return;
    if (attemptEat(grid, stats, currentSeason))
    { /* Ate */
    }
    if (!alive)
        return;
    if (isPregnant && currentGestationTime >= gestationPeriod)
    {
        giveBirth(grid, stats);
        isPregnant = false;
        currentGestationTime = 0;
        currentReproductionCooldown = reproductionCooldown;
        energy -= maxEnergy / 3;
        if (energy <= 0 && alive)
            die(stats);
    }
    if (!alive)
        return;
    if (gender == Gender::FEMALE && !isPregnant && currentReproductionCooldown == 0 && age >= minBreedingAge && energy >= energyToReproduce)
    {
        std::vector<std::shared_ptr<Animal>> mates_found;
        if (this->type == EntityType::HERBIVORE)
        {
            for (const auto &herb_mate : grid.herbivores)
            {
                // Herbivore is now a complete type because its declaration was seen before Grid,
                // and Animal::update is defined after Herbivore's declaration.
                if (static_cast<const Animal *>(herb_mate.get()) != this && herb_mate->isAlive() && herb_mate->getGender() == Gender::MALE && herb_mate->age >= herb_mate->minBreedingAge)
                {
                    if (std::abs(this->r - herb_mate->r) <= 2 && std::abs(this->c - herb_mate->c) <= 2)
                        mates_found.push_back(herb_mate);
                }
            }
        }
        else if (this->type == EntityType::CARNIVORE)
        {
            for (const auto &carn_mate : grid.carnivores)
            {
                if (static_cast<const Animal *>(carn_mate.get()) != this && carn_mate->isAlive() && carn_mate->getGender() == Gender::MALE && carn_mate->age >= carn_mate->minBreedingAge)
                {
                    if (std::abs(this->r - carn_mate->r) <= 2 && std::abs(this->c - carn_mate->c) <= 2)
                        mates_found.push_back(carn_mate);
                }
            }
        }
        if (!mates_found.empty())
            attemptReproduce(grid, stats, mates_found, currentSeason);
    }
    if (!alive)
        return;
    move(grid, stats, currentSeason);
    if (energy <= 0 && alive)
        die(stats);
}

// --- Method Definitions for Plant ---
Plant::Plant(int r_coord, int c_coord)
    : Entity(r_coord, c_coord, EntityType::PLANT, 'P'), baseSpreadChance(35), maxAge(40), age(0) {}
std::string Plant::getSpeciesName() const { return "Plant"; }

void Plant::update(Grid &grid, MonthlyStats &stats, Season currentSeason) {
    if (!alive)
        return;

    age++;
    if (age > maxAge) {
        if (alive) {
            stats.plantsDiedNaturalAge++;
            kill();
        }
        return;
    }

    if (currentSeason == Season::WINTER) {
        if (getRandomInt(1, 100) <= WINTER_DEATH_CHANCE) {
            if (alive) {
                stats.plantsDiedWeather++;
                kill();
            }
            return;
        }
    } else if (currentSeason == Season::AUTUMN) {
        if (getRandomInt(1, 100) <= AUTUMN_DEATH_CHANCE) {
            if (alive) {
                stats.plantsDiedWeather++;
                kill();
            }
            return;
        }
    }

    if (!alive)
        return;

    // Existing spread logic
    int actualSpreadChance = baseSpreadChance;
    if (currentSeason == Season::WINTER)
        actualSpreadChance /= 2;
    else if (currentSeason == Season::SUMMER)
        actualSpreadChance = static_cast<int>(baseSpreadChance * 2);

    if (getRandomInt(1, 100) <= actualSpreadChance) {
        auto localEmptyCells = grid.getAdjacentEmptyCells(r, c);
        if (!localEmptyCells.empty() && getRandomInt(1, 100) <= 75) {
            int choice = getRandomInt(0, localEmptyCells.size() - 1);
            auto newPlant = std::make_shared<Plant>(localEmptyCells[choice].first, localEmptyCells[choice].second);
            if (grid.addEntity(newPlant))
                stats.plantsSpread++;
        }
    }

        // --- New logic for seasonal plant growth ---
    int newPlantChance = 0;
    if (currentSeason == Season::SPRING) {
        newPlantChance = 75;
    } else if (currentSeason == Season::SUMMER) {
        newPlantChance = 100;
    }

    if (newPlantChance > 0) {
        for (int i = 0; i < 2; ++i) { 
            if (getRandomInt(1, 100) <= newPlantChance) {
                int newR, newC;
                int attempts = 0;
                do {
                    newR = getRandomInt(0, GRID_HEIGHT - 1);
                    newC = getRandomInt(0, GRID_WIDTH - 1);
                    attempts++;
                    if (attempts > GRID_WIDTH * GRID_HEIGHT * 2)
                        break;
                } while (!grid.isEmpty(newR, newC));

                if (grid.isEmpty(newR, newC)) {
                    auto newPlant = std::make_shared<Plant>(newR, newC);
                    if (grid.addEntity(newPlant))
                        stats.plantsSpread++;
                }
            }
        }
    }
   
}
// --- Method Definitions for Herbivore ---
Herbivore::Herbivore(int r_coord, int c_coord, Gender gender_val)
    : Animal(r_coord, c_coord, EntityType::HERBIVORE, 'H', 'h', gender_val, 70, 120, 5, 10, 3, 2, 40, 3, 1.0) {}
std::string Herbivore::getSpeciesName() const { return "Herbivore"; }
bool Herbivore::attemptEat(Grid &grid, MonthlyStats &stats, Season currentSeason)
{
    if (!alive || energy >= maxEnergy * 0.95)
        return false;
    int actualEatingRadius = visionRange;
    if (currentSeason == Season::WINTER)
        actualEatingRadius = std::max(1, visionRange - 2);
    else if (currentSeason == Season::AUTUMN)
        actualEatingRadius = std::max(1, visionRange - 1);
    else if (currentSeason == Season::SUMMER)
        actualEatingRadius = visionRange + 2;
    for (int radius_scan = 1; radius_scan <= actualEatingRadius; ++radius_scan)
    {
        for (int dr = -radius_scan; dr <= radius_scan; ++dr)
        {
            for (int dc = -radius_scan; dc <= radius_scan; ++dc)
            {
                if (std::abs(dr) == radius_scan || std::abs(dc) == radius_scan)
                {
                    int nr = r + dr;
                    int nc = c + dc;
                    if (grid.isValid(nr, nc))
                    {                                              // Grid is complete type
                        auto plantEntity = grid.getEntity(nr, nc); // Grid is complete type
                        if (plantEntity && plantEntity->type == EntityType::PLANT && plantEntity->isAlive())
                        {
                            energy = std::min(maxEnergy, energy + 35);
                            plantEntity->kill();
                            stats.plantsEaten++;
                            stats.monthlyEvents.push_back("Herbivore at (" + std::to_string(this->r) + "," + std::to_string(this->c) + ") ate plant at (" + std::to_string(nr) + "," + std::to_string(nc) + ")");
                            grid.removeEntity(plantEntity, stats);
                            turnsSinceLastMeal = 0;
                            return true; // Grid is complete
                        }
                    }
                }
            }
        }
    }
    return false;
}
void Herbivore::move(Grid &grid, MonthlyStats &stats, Season currentSeason)
{
    if (!alive)
        return;
    if (energy <= 0)
    {
        if (alive)
            die(stats);
        return;
    }
    auto carnivoresNearby = grid.findNearbyEntities(r, c, EntityType::CARNIVORE, visionRange + (currentSeason == Season::SUMMER ? 1 : 0) - (currentSeason == Season::WINTER ? 1 : 0));
    if (!carnivoresNearby.empty())
    {
        double avgCarnR = 0, avgCarnC = 0;
        for (const auto &c_entity : carnivoresNearby)
        {
            avgCarnR += c_entity->r;
            avgCarnC += c_entity->c;
        }
        avgCarnR /= carnivoresNearby.size();
        avgCarnC /= carnivoresNearby.size();
        int bestR = r, bestC = c;
        double maxDistSq = -1;
        for (int dr_move = -1; dr_move <= 1; ++dr_move)
            for (int dc_move = -1; dc_move <= 1; ++dc_move)
            {
                if (dr_move == 0 && dc_move == 0)
                    continue;
                int nr_move = r + dr_move, nc_move = c + dc_move;
                if (grid.isEmpty(nr_move, nc_move))
                {
                    double dsq = (nr_move - avgCarnR) * (nr_move - avgCarnR) + (nc_move - avgCarnC) * (nc_move - avgCarnC);
                    if (dsq > maxDistSq)
                    {
                        maxDistSq = dsq;
                        bestR = nr_move;
                        bestC = nc_move;
                    }
                }
            }
        if (bestR != r || bestC != c)
        {
            grid.moveEntity(grid.getEntity(r, c), bestR, bestC);
            return;
        }
    }
    if (energy < maxEnergy * 0.9)
    {
        auto plantsNearby = grid.findNearbyEntities(r, c, EntityType::PLANT, visionRange);
        if (!plantsNearby.empty())
        {
            std::shared_ptr<Entity> closest = nullptr;
            double minDistSq = 1e9;
            for (const auto &p_entity : plantsNearby)
            {
                double dsq = (p_entity->r - r) * (p_entity->r - r) + (p_entity->c - c) * (p_entity->c - c);
                if (dsq < minDistSq)
                {
                    minDistSq = dsq;
                    closest = p_entity;
                }
            }
            if (closest)
            {
                int dr_move = (closest->r > r) ? 1 : ((closest->r < r) ? -1 : 0);
                int dc_move = (closest->c > c) ? 1 : ((closest->c < c) ? -1 : 0);
                int nr_move = r + dr_move, nc_move = c + dc_move;
                if (grid.isValid(nr_move, nc_move) && (grid.isEmpty(nr_move, nc_move) || (grid.getEntity(nr_move, nc_move) && grid.getEntity(nr_move, nc_move)->type == EntityType::PLANT)))
                {
                    grid.moveEntity(grid.getEntity(r, c), nr_move, nc_move);
                    return;
                }
            }
        }
    }
    auto emptyC = grid.getAdjacentEmptyCells(r, c);
    if (!emptyC.empty())
    {
        int ch = getRandomInt(0, emptyC.size() - 1);
        grid.moveEntity(grid.getEntity(r, c), emptyC[ch].first, emptyC[ch].second);
    }
}
void Herbivore::attemptReproduce(Grid &grid, MonthlyStats &stats, const std::vector<std::shared_ptr<Animal>> &potentialMates, Season currentSeason)
{
    if (!alive || !canReproduceInternal())
        return;
    double repMul = 1.0;
    if (currentSeason == Season::WINTER)
        repMul = 0.2;
    else if (currentSeason == Season::AUTUMN)
        repMul = 0.5;
    else if (currentSeason == Season::SUMMER)
        repMul = 1.5;
    if (getRandomDouble(0.0, 1.0) > repMul)
        return;
    for (const auto &mate_base : potentialMates)
    {
        if (!mate_base->isAlive() || static_cast<const Animal *>(mate_base.get()) == this)
            continue;
        if (mate_base->type != EntityType::HERBIVORE || mate_base->getGender() != Gender::MALE)
            continue;
        auto mate = std::static_pointer_cast<Herbivore>(mate_base);
        if (mate->age >= mate->minBreedingAge && std::abs(r - mate->r) <= 1 && std::abs(c - mate->c) <= 1)
        {
            isPregnant = true;
            currentGestationTime = 0;
            energy -= energyToReproduce / 2;
            if (mate->isAlive())
                mate->energy -= mate->energyToReproduce / 4;
            stats.monthlyEvents.push_back("Herbivore at (" + std::to_string(r) + "," + std::to_string(c) + ") mated.");
            return;
        }
    }
}
void Herbivore::giveBirth(Grid &grid, MonthlyStats &stats)
{
    if (!alive)
        return;
    std::vector<std::pair<int, int>> birthLocs = grid.getAdjacentEmptyCells(r, c);
    if (birthLocs.empty())
    {
        for (int dr_scan = -visionRange; dr_scan <= visionRange; ++dr_scan)
            for (int dc_scan = -visionRange; dc_scan <= visionRange; ++dc_scan)
            {
                if (dr_scan == 0 && dc_scan == 0)
                    continue;
                if (grid.isEmpty(r + dr_scan, c + dc_scan))
                    birthLocs.push_back({r + dr_scan, c + dc_scan});
            }
        if (!birthLocs.empty())
        { /* Loop guard fixed*/
        }
    }
    if (birthLocs.empty())
        return;
    int numOff = getRandomInt(1, 3);
    for (int i = 0; i < numOff && !birthLocs.empty(); ++i)
    {
        int idx = getRandomInt(0, birthLocs.size() - 1);
        auto pos = birthLocs[idx];
        birthLocs.erase(birthLocs.begin() + idx);
        Gender g = (getRandomInt(0, 1) == 0) ? Gender::MALE : Gender::FEMALE;
        auto newH = std::make_shared<Herbivore>(pos.first, pos.second, g);
        if (grid.addEntity(newH))
        {
            stats.herbivoresSpawned++;
            stats.monthlyEvents.push_back("Herbivore born at (" + std::to_string(pos.first) + "," + std::to_string(pos.second) + ")");
        }
    }
}

// Method Definitions for Carnivore
Carnivore::Carnivore(int r_coord, int c_coord, Gender gender_val)
    : Animal(r_coord, c_coord, EntityType::CARNIVORE, 'C', 'c', gender_val, 100, 120, 6, 15, 4, 5, 50, 2, 1.5) {}
std::string Carnivore::getSpeciesName() const { return "Carnivore"; }
bool Carnivore::attemptEat(Grid &grid, MonthlyStats &stats, Season currentSeason)
{
    if (!alive || energy >= maxEnergy * 0.9)
        return false;
    int eatRad = visionRange;
    if (currentSeason == Season::WINTER)
        eatRad = std::max(1, visionRange - 2);
    else if (currentSeason == Season::AUTUMN)
        eatRad = std::max(1, visionRange - 1);
    else if (currentSeason == Season::SUMMER)
        eatRad = visionRange + 1;
    for (int radius_scan = 1; radius_scan <= eatRad; ++radius_scan)
        for (int dr_scan = -radius_scan; dr_scan <= radius_scan; ++dr_scan)
            for (int dc_scan = -radius_scan; dc_scan <= radius_scan; ++dc_scan)
            {
                if (std::abs(dr_scan) == radius_scan || std::abs(dc_scan) == radius_scan)
                {
                    int nr_scan = r + dr_scan, nc_scan = c + dc_scan;
                    if (grid.isValid(nr_scan, nc_scan))
                    {
                        auto target = grid.getEntity(nr_scan, nc_scan);
                        if (target && target->type == EntityType::HERBIVORE && target->isAlive())
                        {
                            auto herb = std::static_pointer_cast<Herbivore>(target);
                            energy = std::min(maxEnergy, energy + 45);
                            herb->die(stats, true);
                            stats.herbivoresEaten++;
                            stats.monthlyEvents.push_back("Carnivore at (" + std::to_string(this->r) + "," + std::to_string(this->c) + ") ate herbivore at (" + std::to_string(nr_scan) + "," + std::to_string(nc_scan) + ")");
                            grid.removeEntity(herb, stats);
                            turnsSinceLastMeal = 0;
                            return true;
                        }
                    }
                }
            }
    return false;
}
void Carnivore::move(Grid &grid, MonthlyStats &stats, Season currentSeason)
{
    if (!alive)
        return;
    if (energy <= 0)
    {
        if (alive)
            die(stats);
        return;
    }
    if (energy < maxEnergy * 0.85)
    {
        auto herbsNearby = grid.findNearbyEntities(r, c, EntityType::HERBIVORE, visionRange);
        if (!herbsNearby.empty())
        {
            std::shared_ptr<Entity> closest = nullptr;
            double minD_sq = 1e9;
            for (const auto &h_entity : herbsNearby)
            {
                double d_sq = (h_entity->r - r) * (h_entity->r - r) + (h_entity->c - c) * (h_entity->c - c);
                if (d_sq < minD_sq)
                {
                    minD_sq = d_sq;
                    closest = h_entity;
                }
            }
            if (closest)
            {
                int dr_move = (closest->r > r) ? 1 : ((closest->r < r) ? -1 : 0);
                int dc_move = (closest->c > c) ? 1 : ((closest->c < c) ? -1 : 0);
                int nr_move = r + dr_move, nc_move = c + dc_move;
                if (grid.isValid(nr_move, nc_move) && (grid.isEmpty(nr_move, nc_move) || (grid.getEntity(nr_move, nc_move) && grid.getEntity(nr_move, nc_move)->type == EntityType::HERBIVORE)))
                {
                    grid.moveEntity(grid.getEntity(r, c), nr_move, nc_move);
                    return;
                }
            }
        }
    }
    auto emptyC = grid.getAdjacentEmptyCells(r, c);
    if (!emptyC.empty())
    {
        int ch = getRandomInt(0, emptyC.size() - 1);
        grid.moveEntity(grid.getEntity(r, c), emptyC[ch].first, emptyC[ch].second);
    }
}
void Carnivore::attemptReproduce(Grid &grid, MonthlyStats &stats, const std::vector<std::shared_ptr<Animal>> &potentialMates, Season currentSeason)
{
    if (!alive || !canReproduceInternal())
        return;
    double repMul = 1.0;
    if (currentSeason == Season::WINTER)
        repMul = 0.15;
    else if (currentSeason == Season::AUTUMN)
        repMul = 0.4;
    else if (currentSeason == Season::SUMMER)
        repMul = 1.3;
    if (getRandomDouble(0.0, 1.0) > repMul)
        return;
    for (const auto &mate_base : potentialMates)
    {
        if (!mate_base->isAlive() || static_cast<const Animal *>(mate_base.get()) == this)
            continue;
        if (mate_base->type != EntityType::CARNIVORE || mate_base->getGender() != Gender::MALE)
            continue;
        auto mate = std::static_pointer_cast<Carnivore>(mate_base);
        if (mate->age >= mate->minBreedingAge && std::abs(r - mate->r) <= 1 && std::abs(c - mate->c) <= 1)
        {
            isPregnant = true;
            currentGestationTime = 0;
            energy -= energyToReproduce / 2;
            if (mate->isAlive())
                mate->energy -= mate->energyToReproduce / 4;
            stats.monthlyEvents.push_back("Carnivore at (" + std::to_string(r) + "," + std::to_string(c) + ") mated.");
            return;
        }
    }
}
void Carnivore::giveBirth(Grid &grid, MonthlyStats &stats)
{
    if (!alive)
        return;
    std::vector<std::pair<int, int>> birthLocs = grid.getAdjacentEmptyCells(r, c);
    if (birthLocs.empty())
    {
        for (int dr_scan = -visionRange; dr_scan <= visionRange; ++dr_scan)
            for (int dc_scan = -visionRange; dc_scan <= visionRange; ++dc_scan)
            {
                if (dr_scan == 0 && dc_scan == 0)
                    continue;
                if (grid.isEmpty(r + dr_scan, c + dc_scan))
                    birthLocs.push_back({r + dr_scan, c + dc_scan});
            }
        if (!birthLocs.empty())
        { /* Loop guard fixed*/
        }
    }
    if (birthLocs.empty())
        return;
    int numOff = getRandomInt(1, 2);
    for (int i = 0; i < numOff && !birthLocs.empty(); ++i)
    {
        int idx = getRandomInt(0, birthLocs.size() - 1);
        auto pos = birthLocs[idx];
        birthLocs.erase(birthLocs.begin() + idx);
        Gender g = (getRandomInt(0, 1) == 0) ? Gender::MALE : Gender::FEMALE;
        auto newC = std::make_shared<Carnivore>(pos.first, pos.second, g);
        if (grid.addEntity(newC))
        {
            stats.carnivoresSpawned++;
            stats.monthlyEvents.push_back("Carnivore born at (" + std::to_string(pos.first) + "," + std::to_string(pos.second) + ")");
        }
    }
}

// --- Method Definitions for Simulation ---
Simulation::Simulation() : currentMonthSimTime(0), carnivoresStarvedLastMonth_sim(false), currentSeason_sim(Season::NONE), currentMonthOfYear_sim(0) {}

void Simulation::determineSeason()
{
    currentMonthOfYear_sim = (currentMonthSimTime - 1) % 12;
    if (isNorthernHemisphere_sim)
    {
        if (currentMonthOfYear_sim >= 2 && currentMonthOfYear_sim <= 4)
            currentSeason_sim = Season::SPRING;
        else if (currentMonthOfYear_sim >= 5 && currentMonthOfYear_sim <= 7)
            currentSeason_sim = Season::SUMMER;
        else if (currentMonthOfYear_sim >= 8 && currentMonthOfYear_sim <= 10)
            currentSeason_sim = Season::AUTUMN;
        else
            currentSeason_sim = Season::WINTER;
    }
    else
    {
        if (currentMonthOfYear_sim >= 8 && currentMonthOfYear_sim <= 10)
            currentSeason_sim = Season::SPRING;
        else if (currentMonthOfYear_sim >= 11 || currentMonthOfYear_sim <= 1)
            currentSeason_sim = Season::SUMMER;
        else if (currentMonthOfYear_sim >= 2 && currentMonthOfYear_sim <= 4)
            currentSeason_sim = Season::AUTUMN;
        else
            currentSeason_sim = Season::WINTER;
    }
    stats.currentMonthName = monthNames_sim[currentMonthOfYear_sim];
    stats.currentSeasonName = getSeasonName(currentSeason_sim);
}

void Simulation::initialize()
{
    int years;
    int numP, numH, numC;
    char hemiC;
    std::cout << "Choose Hemisphere (N for Northern, S for Southern): ";
    std::cin >> hemiC;
    isNorthernHemisphere_sim = (toupper(hemiC) == 'N');
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Enter number of years for simulation: ";
    std::cin >> years;
    totalMonthsToSim = years * 12;
    std::cout << "Enter initial number of plants: ";
    std::cin >> numP;
    std::cout << "Enter initial number of herbivores: ";
    std::cin >> numH;
    std::cout << "Enter initial number of carnivores: ";
    std::cin >> numC;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    auto place = [&](EntityType type_val, int count)
    {for(int i=0;i<count;++i){int r_coord,c_coord,attempts=0;do{r_coord=getRandomInt(0,GRID_HEIGHT-1);c_coord=getRandomInt(0,GRID_WIDTH-1);attempts++;}while(!grid.isEmpty(r_coord,c_coord)&&attempts<GRID_WIDTH*GRID_HEIGHT*2);
        if(grid.isEmpty(r_coord,c_coord)){std::shared_ptr<Entity>newEntity=nullptr; Gender g=(getRandomInt(0,1)==0)?Gender::MALE:Gender::FEMALE;
            if(type_val==EntityType::PLANT)newEntity=std::make_shared<Plant>(r_coord,c_coord); else if(type_val==EntityType::HERBIVORE)newEntity=std::make_shared<Herbivore>(r_coord,c_coord,g); else if(type_val==EntityType::CARNIVORE)newEntity=std::make_shared<Carnivore>(r_coord,c_coord,g);
            if(newEntity)grid.addEntity(newEntity);}else{std::cout<<"Warning: Could not place all initial entities.\n";break;}} };
    place(EntityType::PLANT, numP);
    place(EntityType::HERBIVORE, numH);
    place(EntityType::CARNIVORE, numC);
}

void Simulation::handleMigration()
{
    if (currentSeason_sim == Season::SPRING)
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
            if (grid.addMigratingAnimal(newAnimal))
            {
                actualImmigrated++;
                stats.monthlyEvents.push_back(newAnimal->getSpeciesName() + " immigrated to (" + std::to_string(newAnimal->r) + "," + std::to_string(newAnimal->c) + ").");
            }
        }
        stats.animalsImmigrated = actualImmigrated;
    }
    else if (currentSeason_sim == Season::AUTUMN)
    {
        int emigrateHerbivores = 0, emigrateCarnivores = 0, actualEmigrated = 0;
        if (grid.herbivores.size() > 2)
            emigrateHerbivores = getRandomInt(0, std::min((int)grid.herbivores.size() / 4, 2));
        if (grid.carnivores.size() > 1)
            emigrateCarnivores = getRandomInt(0, std::min((int)grid.carnivores.size() / 5, 1));
        for (int i = 0; i < emigrateHerbivores && !grid.herbivores.empty(); ++i)
        {
            int randIdx = getRandomInt(0, grid.herbivores.size() - 1);
            auto herbivoreToRemove = grid.herbivores[randIdx];
            stats.monthlyEvents.push_back("Herbivore at (" + std::to_string(herbivoreToRemove->r) + "," + std::to_string(herbivoreToRemove->c) + ") emigrated.");
            grid.removeEntity(herbivoreToRemove, stats);
            actualEmigrated++;
        }
        for (int i = 0; i < emigrateCarnivores && !grid.carnivores.empty(); ++i)
        {
            int randIdx = getRandomInt(0, grid.carnivores.size() - 1);
            auto carnivoreToRemove = grid.carnivores[randIdx];
            stats.monthlyEvents.push_back("Carnivore at (" + std::to_string(carnivoreToRemove->r) + "," + std::to_string(carnivoreToRemove->c) + ") emigrated.");
            grid.removeEntity(carnivoreToRemove, stats);
            actualEmigrated++;
        }
        stats.animalsEmigrated = actualEmigrated;
    }
}

void Simulation::runMonth()
{
    currentMonthSimTime++;
    stats.reset();
    determineSeason();
    std::cout << "\n--- Month: " << stats.currentMonthName << " " << (currentMonthSimTime - 1) / 12 + 1 << " (Season: " << stats.currentSeasonName << ") ---\n";
    auto plants_copy = grid.plants;
    auto herbivores_copy = grid.herbivores;
    auto carnivores_copy = grid.carnivores;
    for (const auto &c_ptr : carnivores_copy)
        if (c_ptr->isAlive())
            c_ptr->update(grid, stats, currentSeason_sim);
    for (const auto &h_ptr : herbivores_copy)
        if (h_ptr->isAlive())
            h_ptr->update(grid, stats, currentSeason_sim);
    for (const auto &p_ptr : plants_copy)
        if (p_ptr->isAlive())
            p_ptr->update(grid, stats, currentSeason_sim);
    handleMigration();
    auto cleanupDead = [&](auto &list_generic)
    { list_generic.erase(std::remove_if(list_generic.begin(), list_generic.end(), [&](const auto &e_ptr)
                                        {if(!e_ptr->isAlive()){if(grid.isValid(e_ptr->r,e_ptr->c)&&grid.cells[e_ptr->r][e_ptr->c]==e_ptr)grid.cells[e_ptr->r][e_ptr->c]=nullptr;return true;}return false; }),
                         list_generic.end()); };
    cleanupDead(grid.plants);
    cleanupDead(grid.herbivores);
    cleanupDead(grid.carnivores);
    stats.currentPlants = grid.plants.size();
    stats.currentHerbivores = grid.herbivores.size();
    stats.currentCarnivores = grid.carnivores.size();
    grid.display();
    stats.display();
    if ((grid.herbivores.empty() && grid.carnivores.empty() && currentMonthSimTime > 1) || (grid.plants.empty() && (grid.herbivores.size() > 0 || grid.carnivores.size() > 0) && currentMonthSimTime > 1) || (grid.herbivores.empty() && grid.carnivores.size() > 0 && currentMonthSimTime > 1))
    {
        if (grid.herbivores.empty() && grid.carnivores.empty())
            std::cout << "All animals have died. Simulation ends early.\n";
        else if (grid.plants.empty())
            std::cout << "All plants have died. Remaining animals will starve. Simulation ends early.\n";
        else if (grid.herbivores.empty())
            std::cout << "All herbivores have died. Carnivores will starve. Simulation ends early.\n";
        currentMonthSimTime = totalMonthsToSim + 1;
    }
    std::cout << "Press Enter to continue to the next month (or Q to quit)...";
    char q_input_char = std::cin.get();
    if (toupper(q_input_char) == 'Q')
    {
        currentMonthSimTime = totalMonthsToSim + 1;
        if (q_input_char != '\n')
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    else if (q_input_char != '\n')
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void Simulation::start()
{
    initialize();
    determineSeason();
    stats.currentMonthName = "Initial Setup";
    stats.currentSeasonName = getSeasonName(currentSeason_sim);
    stats.currentPlants = grid.plants.size();
    stats.currentHerbivores = grid.herbivores.size();
    stats.currentCarnivores = grid.carnivores.size();
    grid.display();
    std::cout << "\n--- Initial State ---\nSeason: " << stats.currentSeasonName << "\nInitial Plants: " << stats.currentPlants << "\nInitial Herbivores: "
              << stats.currentHerbivores << "\nInitial Carnivores: " << stats.currentCarnivores << "\nPress Enter to start simulation...";
    std::cin.get();
    while (currentMonthSimTime < totalMonthsToSim)
    {
        runMonth();
        if ((grid.herbivores.empty() && grid.carnivores.empty() && grid.plants.empty() && currentMonthSimTime > 5) || ((grid.herbivores.empty() || grid.plants.empty()) && grid.carnivores.empty() && currentMonthSimTime > 12 && (grid.herbivores.size() < 2 || grid.plants.size() < 5)))
        {
            std::cout << "\nEcosystem collapse or severe decline. Ending simulation.\n";
            break;
        }
    }
    std::cout << "\nSimulation Finished after " << std::min(currentMonthSimTime, totalMonthsToSim) << " months.\n";
}

int main()
{
    Simulation sim;
    sim.start();
    return 0;
}
