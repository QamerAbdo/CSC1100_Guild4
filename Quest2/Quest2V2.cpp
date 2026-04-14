#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <vector>
#include <random>
#include <cmath>
#include <chrono>
#include <limits>  // For input clearing
#ifdef _WIN32
#include <windows.h> // for UTF8
#endif
using namespace std;

// Dialogue class
class Dialogue {
public:
    string question, truth, lie;
    Dialogue(string question, string truth, string lie) {
        this->question = question;
        this->truth = truth;
        this->lie = lie;
    }
};

class Run {
    public:
        double score;
        int gameMinutes, gameSeconds, correct;
        Run(double score, int gameMinutes, int gameSeconds, int correct) {
            this->score = score;
            this->gameMinutes = gameMinutes;
            this->gameSeconds = gameSeconds;
            this->correct = correct;
        }
};

// Room class for the maze
class Room {
public:
    string description;
    bool hasSentinel;
    int sentinelIndex; // which question this sentinel asks
    int north, south, east, west; // room indices, -1 means no exit
    
    Room(string desc, bool hasSent, int sentIdx, int n, int s, int e, int w) 
        : description(desc), hasSentinel(hasSent), sentinelIndex(sentIdx),
          north(n), south(s), east(e), west(w) {}
};

//Map class to handle navigation
class GameMap {
private:
    vector<Room> rooms;
    int currentRoom;
    int sentinelsDefeated;
    int sentinelsToWin;
    
public:
    GameMap(int neededToWin) : sentinelsToWin(neededToWin), sentinelsDefeated(0) {
        // Create 5 rooms in a cross shape
        // Room 0: Starting room (center)
        rooms.emplace_back("You are in the CENTER HALL. Ancient torches flicker on the walls.", 
                          true, 0, 1, 2, 3, 4);
        
        // Room 1: North room
        rooms.emplace_back("You enter the NORTH CHAMBER. A mysterious sentinel blocks your path.", 
                          true, 1, -1, 0, -1, -1);
        
        // Room 2: South room
        rooms.emplace_back("You enter the SOUTH CRYPT. A hooded figure stands before you.", 
                          true, 2, 0, -1, -1, -1);
        
        // Room 3: East room
        rooms.emplace_back("You enter the EAST TOWER. Moonlight streams through a broken window.", 
                          true, 3, -1, -1, -1, 0);
        
        // Room 4: West room
        rooms.emplace_back("You enter the WEST LIBRARY. Books float mysteriously in the air.", 
                          true, 4, -1, -1, 0, -1);
        
        currentRoom = 0;
    }
    
    void showCurrentRoom() {
        cout << "\n========================================" << endl;
        cout << rooms[currentRoom].description << endl;
        cout << "========================================" << endl;
        
        // Show available exits
        cout << "Exits: ";
        if (rooms[currentRoom].north != -1) cout << "[N]orth ";
        if (rooms[currentRoom].south != -1) cout << "[S]outh ";
        if (rooms[currentRoom].east != -1) cout << "[E]ast ";
        if (rooms[currentRoom].west != -1) cout << "[W]est ";
        cout << endl;
        
        // Show progress
        cout << "⚔️ Sentinels defeated: " << sentinelsDefeated << "/" << sentinelsToWin << endl;
    }
    
    bool move(char direction) {
        int nextRoom = -1;
        char dirLower = tolower(direction);  // Convert to lowercase once
    
        switch(dirLower) {
            case 'n': 
                nextRoom = rooms[currentRoom].north; 
                break;
            case 's': 
                nextRoom = rooms[currentRoom].south; 
                break;
            case 'e': 
                nextRoom = rooms[currentRoom].east; 
                break;
            case 'w': 
                nextRoom = rooms[currentRoom].west; 
                break;
            default: 
                return false;
        }
    
        if (nextRoom != -1) {
            currentRoom = nextRoom;
        
            // Print the correct direction name based on the input
            string directionName;
            if (dirLower == 'n') directionName = "North";
            else if (dirLower == 's') directionName = "South";
            else if (dirLower == 'e') directionName = "East";
            else if (dirLower == 'w') directionName = "West";
        
        cout << "\n🚶 You move to the " << directionName << "..." << endl;
        return true;
    } else {
        cout << "❌ You cannot go that way! There's a wall." << endl;
        return false;
    }
}
    bool hasSentinelHere() {
        return rooms[currentRoom].hasSentinel;
    }
    
    void defeatSentinel() {
        if (rooms[currentRoom].hasSentinel) {
            rooms[currentRoom].hasSentinel = false;
            sentinelsDefeated++;
            cout << "✨ The sentinel crumbles to dust! ✨" << endl;
        }
    }
    
    bool hasWon() {
        return sentinelsDefeated >= sentinelsToWin;
    }
    
    int getCurrentSentinelIndex() {
        return rooms[currentRoom].sentinelIndex;
    }
    
    int getRemainingSentinels() {
        return sentinelsToWin - sentinelsDefeated;
    }
};

// Function to read dialogues from file 
vector<Dialogue> getDialogues() {
    vector<Dialogue> dialogues;
    string fileName = "dialogues.txt";
    ifstream infile(fileName);
    if (!infile) {
        cerr << "Error opening file: " << fileName << endl;
        exit(EXIT_FAILURE);
    } else {
        string question, truth, lie;
        while (getline(infile, question) && getline(infile, truth) && getline(infile, lie)) {
            dialogues.emplace_back(Dialogue(question, truth, lie));
        }
        infile.close();
    }
    return dialogues;
}

class Sentinel {
public:
    bool isTruthful;
    Sentinel(bool isTruthful) {
        this->isTruthful = isTruthful;
    }
};

void displayDialogue(const Dialogue& dialogue, const Sentinel& sentinel) {
    cout << "\n💬 You ask: " << dialogue.question << endl;
    if (sentinel.isTruthful) {
        cout << "🗣️ The sentinel says: " << dialogue.truth << endl;
    } else {
        cout << "🗣️ The sentinel says: " << dialogue.lie << endl;
    }
}

bool askUserAnswer() {
    cout << "\n🤔 Do you think the sentinel is telling the TRUTH or LYING?" << endl;
    cout << "Enter 't' for truth or 'l' for lie: ";
    
    while (true) {
        char userAnswer;
        cin >> userAnswer;
        userAnswer = tolower(userAnswer);
        
        if (userAnswer == 't') {
            return true;
        } else if (userAnswer == 'l') {
            return false;
        } else {
            cout << "Invalid input. Please enter 't' for truth or 'l' for lie: ";
            continue;
        }
    }
}

// MODIFIED: playGame now uses the maze
Run playGame(int SENTINELS_TO_WIN, const vector<Dialogue>& dialogues, mt19937& gen) {
    // Create the game map
    GameMap gameMap(SENTINELS_TO_WIN);
    
    // Start timer
    auto startTime = chrono::steady_clock::now();
    int correctAnswers = 0;
    int totalQuestionsAsked = 0;
    
    cout << "\n🎮 WELCOME TO THE SENTINEL MAZE! 🎮" << endl;
    cout << "Your goal: Defeat " << SENTINELS_TO_WIN << " sentinels by correctly identifying" << endl;
    cout << "whether they are telling the truth or lying." << endl;
    cout << "Navigate through the rooms using N, S, E, W commands.\n" << endl;
    
    // Game loop
    while (!gameMap.hasWon()) {
        gameMap.showCurrentRoom();
        
        // If there's a sentinel here, we must fight it
        if (gameMap.hasSentinelHere()) {
            cout << "\n⚠️ A SENTINEL BLOCKS YOUR PATH! ⚠️" << endl;
            cout << "You must answer correctly to proceed.\n" << endl;
            
            // Get random truthfulness for this sentinel
            bernoulli_distribution boolean(0.5);
            Sentinel sentinel(boolean(gen));
            
            // Get the dialogue for this room
            Dialogue dialogue = dialogues[gameMap.getCurrentSentinelIndex()];
            
            // Ask the question
            displayDialogue(dialogue, sentinel);
            bool userAnswer = askUserAnswer();
            totalQuestionsAsked++;
            
            // Check answer
            if (userAnswer == sentinel.isTruthful) {
                cout << "\n✅ CORRECT! The sentinel vanishes!" << endl;
                correctAnswers++;
                gameMap.defeatSentinel();
                cout << "🎉 " << gameMap.getRemainingSentinels() << " sentinels remaining! 🎉" << endl;
            } else {
                cout << "\n❌ WRONG! The sentinel attacks you!" << endl;
                cout << "You retreat to recover..." << endl;
                cout << "💀 You lose some progress! The sentinel remains. 💀" << endl;
                // Note: They don't defeat the sentinel, so they'll have to try again
            }
            
            cout << "\nPress Enter to continue...";
            cin.ignore();
            cin.get();
        } 
        // If no sentinel, they can move freely
        else {
            cout << "\n🏃 The room is empty. You can move safely." << endl;
            cout << "Where would you like to go? (N/S/E/W): ";
            char direction;
            cin >> direction;
            
            if (!gameMap.move(direction)) {
                // Invalid move already prints error message in move()
            }
        }
    }
    
    // Win condition reached
    auto endTime = chrono::steady_clock::now();
    long long gameDuration = chrono::duration_cast<chrono::seconds>(endTime - startTime).count();
    int gameMinutes = gameDuration / 60;
    int gameSeconds = gameDuration % 60;
    double score = round(static_cast<double>(correctAnswers) / totalQuestionsAsked * 10000) / 100;
    
    cout << "\n✨✨✨ VICTORY! ✨✨✨" << endl;
    cout << "You have defeated all " << SENTINELS_TO_WIN << " sentinels!" << endl;
    cout << "Score: " << correctAnswers << "/" << totalQuestionsAsked << " (" << score << "%)" << endl;
    cout << "Time: " << gameMinutes << "m " << gameSeconds << "s" << endl;
    
    Run run(score, gameMinutes, gameSeconds, correctAnswers);
    return run;
}

int main() {
    const int SENTINELS_TO_WIN = 3; // Only need to defeat 3 sentinels (out of 5 rooms)
    
    // Load dialogues from file
    vector<Dialogue> dialogues = getDialogues();
    
    // Random number generator
    random_device rd;
    mt19937 gen(rd());

    // Set console to UTF8 for emojis
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif

    vector<Run> runs;
    
    while (true) {
        cout << "\n========================================" << endl;
        cout << "     THE SENTINEL MAZE CHALLENGE" << endl;
        cout << "========================================" << endl;
        cout << "Defeat " << SENTINELS_TO_WIN << " sentinels by proving" << endl;
        cout << "you can tell truth from lies!" << endl;
        cout << "\nPress Enter to begin your quest...";
        cin.ignore();
        cin.get();
        
        runs.push_back(playGame(SENTINELS_TO_WIN, dialogues, gen));
        
        cout << "\nDo you want to play again? (y/n): ";
        char playAgain;
        cin >> playAgain;
        playAgain = tolower(playAgain);
        
        while (playAgain != 'y' && playAgain != 'n') {
            cout << "Please enter 'y' or 'n': ";
            cin >> playAgain;
            playAgain = tolower(playAgain);
        }
        
        if (playAgain == 'n') {
            break;
        }
        
        cin.ignore(); // Clear buffer for next game
    }
    
    // Display all previous runs
    cout << "\n📊 GAME HISTORY 📊" << endl;
    for (size_t i = 0; i < runs.size(); i++) {
        cout << "Run " << i+1 << ": " << runs[i].correct << "/" << SENTINELS_TO_WIN 
             << " correct (" << runs[i].score << "%) - Time: " 
             << runs[i].gameMinutes << "m " << runs[i].gameSeconds << "s" << endl;
    }
    
    cout << "\nThanks for playing! Press Enter to exit...";
    cin.ignore();
    cin.get();
    
    return 0;
}
