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

int main() {
    const int TOTAL_QUESTIONS = 8; // Total number of questions to ask the user
    // Load dialogues from file
    vector<Dialogue> dialogues = getDialogues();
    // Initialize random number generator (Mersenne Twister)
    random_device rd;
    mt19937 gen(rd());
    bernoulli_distribution boolean(0.5);
    uniform_int_distribution<> integer(0, dialogues.size() - 1);
    // Win Timer
    auto startTime = chrono::steady_clock::now();
    // Keep track of the number of questions asked, and the user's score
    int questionsAsked = 0, correct = 0;
    // Main game loop
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
    int gameMinutes = gameDuration / 60; // Convert seconds to minutes
    int gameSeconds = gameDuration % 60; // Get the remaining seconds
    double score = round(static_cast<double>(correct) / TOTAL_QUESTIONS * 10000) / 100; // Calculate score as a percentage
    cout << "You have reached the end of the game!" << endl
    << "Score: " << correct << "/" << TOTAL_QUESTIONS << " (" << score << "%) Correct" << endl // Display Score
    << "Game Timer: " << gameMinutes << " minutes and " << gameSeconds << " seconds!" << endl; // Game Timer
    return 0;
}