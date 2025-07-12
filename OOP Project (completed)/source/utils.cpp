// utils.cpp
#include "../headers/utils.hpp"
#include <algorithm> // For std::swap

// --- Global Random Number Generation & Helpers ---
std::random_device rd;
std::mt19937 gen(rd()); // Definition

// Generates a random integer within a specified range (inclusive).
int getRandomInt(int min, int max)
{
    if (min > max)
        std::swap(min, max);
    if (min == max)
        return min;
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}

// Generates a random double within a specified range.
double getRandomDouble(double min, double max)
{
    if (min > max)
        std::swap(min, max);
    std::uniform_real_distribution<> distrib(min, max);
    return distrib(gen);
}

// Returns the string name of a given season.
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