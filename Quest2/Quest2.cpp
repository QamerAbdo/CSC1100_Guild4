/* The purpose of this program is to create a text game where the user asks sentinels
a question and determines whether the answer is true or false. */
#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <vector>
#include <random>
#include <cmath>
#include <chrono>
using namespace std;

// Dialogue class to hold the question, truthful answer, and lie
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

// Function to read dialogues from a file and return a vector of Dialogue objects
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

// Sentinel class to represent a sentinel that can be either truthful or lying
class Sentinel {
public:
    bool isTruthful;
    Sentinel(bool isTruthful) {
        this->isTruthful = isTruthful;
    }
};

// Function to display the dialogue based on the sentinel's truthfulness
void displayDialogue(const Dialogue& dialogue, const Sentinel& sentinel) {
    cout << "You ask: " << dialogue.question << endl;
    if (sentinel.isTruthful) {
        cout << "The sentinel says: " << dialogue.truth << endl;
    } else {
        cout << "The sentinel says: " << dialogue.lie << endl;
    }
}

// Function to ask the user for their answer and return it
bool askUserAnswer() {
    cout << "Is the sentinel telling the truth? (y/n): ";
    while (true) {
        char userAnswer;
        cin >> userAnswer;
        userAnswer = tolower(userAnswer); // Convert to lowercase for consistency
        if (userAnswer == 'y') {
            return true;
        } else if (userAnswer == 'n') {
            return false;
        } else {
            cout << "Invalid input. Please enter 'y' for truth or 'n' for lie." << endl;
            continue;
        }
    }
}

// Function to play the game
Run playGame(int TOTAL_QUESTIONS, const vector<Dialogue>& dialogues, mt19937& gen, bernoulli_distribution& boolean, uniform_int_distribution<>& integer) {
    // Win Timer
    auto startTime = chrono::steady_clock::now();
    // Keep track of the number of questions asked, and the user's score
    int questionsAsked = 0, correct = 0;
    while (questionsAsked < TOTAL_QUESTIONS) { // Ask TOTAL_QUESTIONS questions
        // Create a sentinel with a random truthfulness
        Sentinel sentinel(boolean(gen));
        // Select a random dialogue
        Dialogue dialogue = dialogues[integer(gen)];
        // Display the dialogue to the user
        displayDialogue(dialogue, sentinel);
        // Ask user whether the sentinel is telling the truth or lying
        bool userAnswer = askUserAnswer();
        // Check if the user's answer is correct
        if (userAnswer == sentinel.isTruthful) {
            cout << "Correct!\n" << endl;
            correct++;
        } else {
            cout << "Incorrect!\n" << endl;
        }
        questionsAsked++;
    }
    // On Win
    auto endTime = chrono::steady_clock::now(); // End the timer
    long long gameDuration = chrono::duration_cast<chrono::seconds>(endTime - startTime).count();
    int gameMinutes = gameDuration / 60;                                                // Convert seconds to minutes
    int gameSeconds = gameDuration % 60;                                                // Get the remaining seconds
    double score = round(static_cast<double>(correct) / TOTAL_QUESTIONS * 10000) / 100; // Calculate score as a percentage
    Run run(score, gameMinutes, gameSeconds, correct); // Create a Run object to hold the stats
    cout << "You have reached the end of the game!" << endl
         << "Score: " << correct << "/" << TOTAL_QUESTIONS << " (" << score << "%)" << endl // Display Score
         << "Game Timer: " << gameMinutes << " m " << gameSeconds << "s" << endl << endl; // Game Timer
    return run; // Return the Run object with the stats
}

int main() {
    const int TOTAL_QUESTIONS = 8; // Total number of questions to ask the user
    // Load dialogues from file
    vector<Dialogue> dialogues = getDialogues();
    // Initialize random number generator (Mersenne Twister)
    random_device rd;
    mt19937 gen(rd());
    bernoulli_distribution boolean(0.5);
    uniform_int_distribution<> integer(0, dialogues.size() - 1);
    vector<Run> runs; // Vector to hold Run objects, if needed for future use
    while (true) {
        cout << "Welcome to the Sentinel Game!" << endl
             << "In this game, you will ask sentinels a question and determine whether they are telling the truth or lying." << endl
             << "You will be asked a total of 8 questions. Good luck!" << endl
             << "Press Enter to start the game..." << endl;
        cin.get(); // Wait for the user to press Enter
        runs.push_back(playGame(TOTAL_QUESTIONS, dialogues, gen, boolean, integer)); // Play the game and store the run stats
        cout << "Do you want to play again? (y/n): ";
        char playAgain;
        cin >> playAgain;
        playAgain = tolower(playAgain); // Convert to lowercase for consistency
        while (playAgain != 'y' && playAgain != 'n') {
            cout << "Invalid input. Please enter 'y' to play again or 'n' to exit." << endl;
            cin >> playAgain;
            playAgain = tolower(playAgain); // Convert to lowercase for consistency
        }
        if (playAgain == 'y') {
            cout << "Playing Again..." << endl << endl;
        } else {
            cout << "Thank you for playing!" << endl << endl;
            break;
        }
        cin.ignore(); // Remove input buffer for cin.get()
    }
    for (const Run& run : runs) {
        cout << "Run " << &run - &runs[0] + 1 << " | Score: " << run.correct << "/8 (" 
             << run.score << "%), Time: " << run.gameMinutes << "m " << run.gameSeconds << "s" << endl;
    }
        cout << endl << "Press Enter to exit program..." << endl;
    cin.ignore(); // Remove input buffer for cin.get()
    cin.get(); // Wait for user input before closing the program
    return 0;
}