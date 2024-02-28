#include <iostream>

#include "Game.h"

int main()
{
    int numPlayers;

    std::cout << "Enter the number of players (2 or 4): ";
    std::cin >> numPlayers;

    if (numPlayers != 2 && numPlayers != 4)
    {
        std::cout << "Invalid number of players! Please enter 2 or 4.\n";
        return 1;
    }

    // create player structure with numPlayers size
	Player players[10];
	for (int i = 0; i < numPlayers; i++)
    {
		players[i] = Player();
	}

    // Initialize player names (replace with desired names)
    players[0].name = "Player 1";
    players[1].name = "Player 2";
    if (numPlayers == 4)
    {
        players[2].name = "Player 3";
        players[3].name = "Player 4";
    }

    // Initialize house information for each player
    for (int i = 0; i < numPlayers; i++)
    {
        players[i].house = House(); // Use the default constructor
    }

    Colour color;

    std::cout << "Game Master, configure the game for " << numPlayers << " players:\n";
    for (int i = 0; i < numPlayers; i++)
    {
        char choice;
        do
        {
            std::cout << "Player " << i + 1 << ", choose your house color (R, Y, G, B): ";
            std::cin >> choice;
            choice = toupper(choice); // Convert to uppercase for easier comparison

            switch (choice)
            {
                case 'R': color = Colour::RED; break;
                case 'Y': color = Colour::YELLOW; break;
                case 'G': color = Colour::GREEN; break;
                case 'B': color = Colour::BLUE; break;
                default: std::cout << "Invalid choice! Please enter R, Y, G, or B.\n";
            }
        } while (choice != 'R' && choice != 'Y' && choice != 'G' && choice != 'B');

        configureGame(players, numPlayers, color); // Assign the chosen color
    }

    // Print the game results
    printResults(players, numPlayers);

    return 0;
}
