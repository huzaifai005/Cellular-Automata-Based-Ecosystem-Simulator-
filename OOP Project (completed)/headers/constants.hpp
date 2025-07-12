// constants.h
#ifndef CONSTANTS_H
#define CONSTANTS_H

// --- Configuration Constants ---
const int GRID_WIDTH = 20;
const int GRID_HEIGHT = 20;
const int MAX_POPULATION = GRID_WIDTH * GRID_HEIGHT;
const int MAX_SIMULATION_YEARS = 10;

// --- Enums ---
// Represents the type of an entity on the grid.
enum class EntityType
{
    EMPTY,
    PLANT,
    HERBIVORE,
    CARNIVORE
};

// Represents the gender of an animal entity.
enum class Gender
{
    MALE,
    FEMALE,
    NONE 
};

// Represents the current season in the simulation.
enum class Season
{
    WINTER,
    SPRING,
    SUMMER,
    AUTUMN,
    NONE 
};

#endif // CONSTANTS_H