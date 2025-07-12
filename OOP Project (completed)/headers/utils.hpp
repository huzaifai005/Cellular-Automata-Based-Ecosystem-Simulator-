// utils.h
#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <random> // Required for std::random_device, std::mt19937, etc.
#include "constants.hpp" // For Season enum

// --- Global Random Number Generation & Helpers ---
extern std::mt19937 gen; // Declaration, defined in utils.cpp

// Generates a random integer within a specified range (inclusive).
int getRandomInt(int min, int max);

// Generates a random double within a specified range.
double getRandomDouble(double min, double max);

// Returns the string name of a given season.
std::string getSeasonName(Season season);

#endif // UTILS_H