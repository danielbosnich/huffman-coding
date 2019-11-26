// Names: Daniel Bosnich & Pranav Subramanian
// Assignment: Final Project
// Course: CSCI-2275
// Term: Fall 2019

#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <stack>
#include <vector>
using namespace std;

struct Character {
  char c;
  int freq;
  Character(char ch, int fr) : c(ch), freq(fr) {}
};

// Prints the menu with information on how to use the program
void printMenu() {
	cout << "======Main Menu======" << endl;
	cout << "1. Encode a word" << endl;
	cout << "2. Quit" << endl;
}

// Gets user input for the word to encode
string getWordInput() {
	string word_input;
	cout << "Enter a word:" << endl;
	cin >> word_input;
	return word_input;
}

int main() {
	// Program variables
	bool quit_program = false;
	int user_input;
	string word_input;
	// Print the menu and read user input until the user chooses to exit
	while (!quit_program) {
		printMenu();
		cin >> user_input;
		switch (user_input) {
		case 1:
			word_input = getWordInput();
			cout << "Encoding word" << endl;
			break;
		case 2:
			cout << "Goodbye!" << endl;
			quit_program = true;
			break;
		default:
			// If the input is incorrect, prompt the user to try again
			cin.clear();
			cin.ignore();
			cout << "Incorrect entry, please try again" << endl;
		}
	}
	return 0;
}