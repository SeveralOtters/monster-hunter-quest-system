#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()
#include <thread>  // For std::this_thread::sleep_for
#include <chrono> // For std::chrono::seconds
#include <thread> // For std::thread
using namespace std;
/*
==============================
Monster Hunter Quest Simulator
==============================
Overview:
This program simulates a questing environment for a Monster Hunter game.
Users can create hunters, monsters, and generate quests from a pool of hunters and monsters.
When a hunter is successful in a quest, their rank and success rate increase.
If they fail, their success rate decreases.
As hunters stats improve, they can take on tougher quests (higher hunter rank assignments).
Hunter progress persists between runs of the program, as does monster data.
*/


class Monster {
    public:
    string species;
    short requiredHunterRank;

    // Constructor
    Monster(string a, short b) : species(a), requiredHunterRank(b) {}

    // Static vector to hold all monsters
    static vector<Monster> monsterList;

    // Log a monster to the vector
    static void logMonster(const Monster& monster) {
        monsterList.emplace_back(monster);
    }

    // Load monsters from a file
    static void loadMonsters() {
        ifstream file("monsters.txt");
        if (!file.is_open()) {
            cerr << "Error: Could not open 'monsters.txt'." << endl;
            return;
        }

        string species;
        short requiredRank;
        monsterList.clear();

        while (file >> species >> requiredRank) {
            monsterList.emplace_back(species, requiredRank);
        }

        file.close();

        if (monsterList.empty()) {
            cerr << "Error: No monsters loaded from 'monsters.txt'." << endl;
        } else {
            cout << "Monsters loaded" << endl;
        }
    }

    // Save monsters to a file
    static void saveMonsters() {
        if (monsterList.empty()) {
            cerr << "Warning: No monsters to save to 'monsters.txt'." << endl;
            return;
        }

        ofstream file("monsters.txt");
        if (!file.is_open()) {
            cerr << "Error: Could not open file 'monsters.txt' for writing." << endl;
            return;
        }

        for (const auto& monster : monsterList) {
            file << monster.species << " " << monster.requiredHunterRank << endl;
        }

        file.close();
        cout << "Monsters saved successfully to 'monsters.txt'." << endl;
    }

    // Generate a random monster
    static Monster randomMonster() {
        if (monsterList.empty()) {
            throw runtime_error("Error: No monsters available in monsterList.");
        }

        int randomIndex = rand() % monsterList.size();
        return monsterList[randomIndex];
    }

    // Display all monsters
    static void displayMonsters() {
        if (monsterList.empty()) {
            cout << "No monsters available." << endl;
            return;
        }

        cout << "List of Monsters:" << endl;
        for (const auto& monster : monsterList) {
            cout << "Species: " << monster.species 
                 << ", Required Hunter Rank: " << monster.requiredHunterRank << endl;
        }
    }
};

// Define the static member outside the class
vector<Monster> Monster::monsterList;

class Hunter {
    public:
    string name;
    short hunterRank;
    short hunterSuccessRate;
    bool inQuest;

    // Constructor
    Hunter(string a, short b, short c, bool d = false) {
        name = a;
        hunterRank = b;
        hunterSuccessRate = c;
        inQuest = d;
    }

    // Static vector to hold all hunters
    static vector<Hunter> hunterList;

    // Log a hunter to the vector
    static void logHunter(const Hunter& hunter) {
        hunterList.emplace_back(hunter);
    }

    // Load hunters from a file
    static void loadHunters() {
        ifstream file("hunters.txt");
        if (!file.is_open()) {
            cerr << "Error: Could not open file 'hunters.txt' for reading." << endl;
            return;
        }

        string name;
        short rank;
        short successRate;
        bool inQuest;

        // Clear the vector to avoid duplicates if this method is called multiple times
        hunterList.clear();

        while (file >> name >> rank >> successRate >> inQuest) { // Read name, rank, success rate, and inQuest
            Hunter hunter(name, rank, successRate, inQuest); // Use the constructor with inQuest
            hunterList.emplace_back(hunter); // Add the hunter to the vector
        }

        file.close();
        cout << "Hunters loaded" << endl;
    }

    // Save hunters to a file
    static void saveHunters() {
        ofstream file("hunters.txt");
        if (!file.is_open()) {
            cerr << "Error: Could not open file 'hunters.txt' for writing." << endl;
            return;
        }

        for (auto& hunter : hunterList) {
            hunter.inQuest = false; // Reset inQuest to false before saving
            file << hunter.name << " "
                 << hunter.hunterRank << " "
                 << hunter.hunterSuccessRate << " "
                 << hunter.inQuest << endl;
        }

        file.close();
        cout << "Hunters saved successfully to 'hunters.txt'." << endl;
    }

    // Display all hunters
    static void displayHunters() {
        if (hunterList.empty()) {
            cout << "No hunters available." << endl;
            return;
        }

        cout << "List of Hunters:" << endl;
        for (const auto& hunter : hunterList) {
            cout << "Name: " << hunter.name 
                 << ", Rank: " << hunter.hunterRank 
                 << ", Success Rate: " << hunter.hunterSuccessRate << "%" << endl;
        }
    }
};

// Define the static member outside the class
vector<Hunter> Hunter::hunterList;

class Quest {
    public:
    string name;
    string targetMonster;
    short requiredHunterRank;
    short timeLimit;
    vector<Hunter*> assignedHunters;

    // Constructor
    Quest(string a, string b, short c, short d, vector<Hunter*> e) {
        name = a;
        targetMonster = b;
        requiredHunterRank = c;
        timeLimit = d;
        assignedHunters = e;
    }

    // Vector to hold all quests
    static vector<Quest> questList;

    // Generate a quest
    static void generateQuest() {
        Monster randomMonster = Monster::randomMonster(); // Get a random monster
        string questName = "Quest for " + randomMonster.species; // Create a quest name
        short requiredHunterRank = randomMonster.requiredHunterRank; // Get the required hunter rank
        short timeLimit = 15 + (rand() % 36); // Generate a random number between 15 and 50
        short partySize = 1 + (rand() % 4); // Generate a random party size between 1 and 4
        vector<Hunter*> assignedHunters; // Use pointers to store references to hunters

        // Sequentially search for hunters who meet the requirements
        for (auto& hunter : Hunter::hunterList) {
            if (!hunter.inQuest && hunter.hunterRank >= requiredHunterRank) {
                assignedHunters.push_back(&hunter); // Add a pointer to the hunter
                hunter.inQuest = true; // Mark the hunter as in a quest
                if (assignedHunters.size() == static_cast<std::vector<Hunter*>::size_type>(partySize)) {
                    break; // Stop once the party size is reached
                }
            }
        }

        if (assignedHunters.empty()) {
            cout << "A quest for a " << randomMonster.species << "was not generated because there were no hunters fitting the requirements" << endl;
            return;
        }

        Quest newQuest(questName, randomMonster.species, requiredHunterRank, timeLimit, assignedHunters);
        questList.emplace_back(newQuest); // Add the new quest to the vector
        cout << "Quest generated for " << randomMonster.species << endl;
        cout << "Assigned Hunters: " << newQuest.assignedHunters.size() << endl;
    }

    // Run the quest
    void runQuest() {
        if (assignedHunters.empty()) {
            cerr << "Error: No hunters assigned to the quest. Cannot run the quest." << endl;
            return;
        }

        // Seed the random number generator for this quest
        // This was seeded earlier in main but I found if I didn't reseed it here, the results were the same every time.
        srand(static_cast<unsigned>(time(0)) + reinterpret_cast<uintptr_t>(this));

        // Simulate the quest timer
        while (timeLimit > 0) {
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Wait for 1 second
            timeLimit--; // Decrement the time limit
        }

        // Calculate the average success rate
        int totalSuccessRate = 0;
        for (const auto& hunter : assignedHunters) {
            if (hunter != nullptr) { // Check if the pointer is valid
                totalSuccessRate += hunter->hunterSuccessRate;
            } else {
                cerr << "Warning: Null pointer found in assignedHunters." << endl;
            }
        }
        int averageSuccessRate = assignedHunters.empty() ? 0 : totalSuccessRate / assignedHunters.size();

        // Generate a random chance
        int randomChance = rand() % 100;

        // Determine quest outcome
        bool success = averageSuccessRate >= randomChance;

        // Update hunters based on the quest outcome
        for (auto& hunter : assignedHunters) {
            if (success) {
                hunter->hunterSuccessRate = min(hunter->hunterSuccessRate + 5, 100);
                hunter->hunterRank++;
            } else {
                hunter->hunterSuccessRate = max(hunter->hunterSuccessRate - 5, 0);
            }
            hunter->inQuest = false;
        }

        if (success) {
            cout << name << " complete!" << endl;
        } else {
            cout << name << " failed." << endl;
        }
    }
};

// Define the static member outside the class
vector<Quest> Quest::questList;

int main() {
    // Seed the random number generator
    srand(static_cast<unsigned>(time(0)));

    // Load hunters and monsters
    // VERY IMPORTANT: This must be doen first.
    Hunter::loadHunters();
    Monster::loadMonsters();

    cout << "Welcome to the Monster Hunter Simulation!" << endl;

    while (true) {
        cout << "1. Simulate Quests" << endl;
        cout << "2. Display Monsters" << endl;
        cout << "3. Display Hunters" << endl;
        cout << "4. Add Monster" << endl;
        cout << "5. Add Hunter" << endl;
        cout << "6. Save and Exit" << endl;
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        if (choice == 1) {
            // Simulate quests
            int numberOfQuests;
            cout << "How many quests would you like to run? ";
            cin >> numberOfQuests;

            if (numberOfQuests <= 0) {
                cerr << "Invalid number of quests. Returning to menu." << endl;
                continue;
            }

            // Generate the specified number of quests
            for (int i = 0; i < numberOfQuests; ++i) {
                Quest::generateQuest();
            }

            // Run all quests concurrently
            vector<std::thread> questThreads;
            for (auto& quest : Quest::questList) {
                questThreads.emplace_back(&Quest::runQuest, &quest); // Run each quest in a separate thread
            }

            cout << "Huntering monsters..." << endl;

            // Wait for all threads to finish
            for (auto& thread : questThreads) {
                if (thread.joinable()) {
                    thread.join();
                }
            }

            cout << "All quests completed." << endl;
        } else if (choice == 2) {
            // Display monsters
            Monster::displayMonsters();
        } else if (choice == 3) {
            // Display hunters
            Hunter::displayHunters();
        } else if (choice == 4) {
            // Add monster
            string species;
            short requiredHunterRank;
            cout << "Enter the monster's species: ";
            cin >> species;
            cout << "Enter the required hunter rank: ";
            cin >> requiredHunterRank;

            Monster::monsterList.emplace_back(species, requiredHunterRank);
            cout << "Monster added successfully!" << endl;
        } else if (choice == 5) {
            // Add hunter
            string name;
            short hunterRank, hunterSuccessRate;
            cout << "Enter the hunter's name: ";
            cin >> name;
            cout << "Enter the hunter's rank: ";
            cin >> hunterRank;
            cout << "Enter the hunter's success rate (0-100): ";
            cin >> hunterSuccessRate;

            Hunter::hunterList.emplace_back(name, hunterRank, hunterSuccessRate);
            cout << "Hunter added successfully!" << endl;
        } else if (choice == 6) {
            // Save and exit
            cout << "Saving data and exiting the program. Goodbye!" << endl;

            // Save hunters and monsters to file
            Hunter::saveHunters();
            Monster::saveMonsters();

            break;
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }

    return 0;
}
