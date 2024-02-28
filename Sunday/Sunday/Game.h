#pragma once

#include <string>

// Enumeration for game colors
enum Colour
{
    RED,
    YELLOW,
    GREEN,
    BLUE
};

// Structure for house information
struct House
{
    Colour colour;
    int members;
    bool status;

    // Constructor to initialize the house
    House(Colour colour = Colour::RED, int members = 4, bool status = false)
        : colour(colour), members(members), status(status)
    {
    }
};

// Structure for player information
struct Player
{
    std::string name;
    House house;
};

// Function prototypes
void configureGame(Player players[], int numPlayers, Colour color);
void printResults(Player players[], int numPlayers);
