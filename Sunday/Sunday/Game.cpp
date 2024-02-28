#include <iostream>
#include <string>
#include <iomanip>

#include "Game.h"

// Function to print game results
void printResults(Player players[], int numPlayers)
{
    std::cout << "\nGame Results:\n";
    std::cout << "| %-20s | %-10s | %-5s |\n"
              << std::setw(20) << std::left << "Name" << std::setw(10) << std::left << "House" << std::setw(5)
              << "Members" << std::endl;
    for (int i = 0; i < numPlayers; i++)
    {
        std::string houseName;
        switch (players[i].house.colour)
        {
            case Colour::RED: houseName = "Red"; break;
            case Colour::YELLOW: houseName = "Yellow"; break;
            case Colour::GREEN: houseName = "Green"; break;
            case Colour::BLUE: houseName = "Blue"; break;
        }
        std::cout << "| %-20s | %-10s | %-5d |\n"
                  << std::setw(20) << std::left << players[i].name << std::setw(10) << std::left << houseName
                  << std::setw(5) << players[i].house.members << std::endl;
    }
}

// Function to configure the game
void configureGame(Player players[], int numPlayers, Colour color)
{
    // Check for available houses
    bool available = false;
    for (int i = 0; i < numPlayers; i++)
    {
        if (!players[i].house.status && players[i].house.colour == color)
        {
            available = true;
            break;
        }
    }

    if (!available)
    {
        std::cout << "House " << static_cast<char>(color) << " is not available. Please choose another color.\n";
        return;
    }

    // Assign house to players
    for (int i = 0; i < numPlayers; i++)
    {
        if (!players[i].house.status)
        {
            players[i].house.colour = color;
            players[i].house.status = true;
            break;
        }
    }
}
