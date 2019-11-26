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
#include <bits/stdc++.h>
using namespace std;

struct Character {
  char c;
  int freq;
  Character(char ch, int fr) : c(ch), freq(fr) {}
};

struct compare_chars {
  bool operator() (Character const& char1, Character const& char2) { //remembered how to do this via http://www.cplusplus.com/forum/general/43047/
    return char1.freq > char2.freq; //return true if char 1 would come before char 2
  }
};

struct hTreeNode {
  Character* c;
  hTreeNode* parent;
  hTreeNode* leftChild;
  hTreeNode* rightChild;
};

class HuffmanTree {
  hTreeNode* root;

public:
  HuffmanTree(priority_queue<Character, vector<Character>, compare_chars> pq) {
    /*while(!pq.empty()) {
      Character top = pq.top();
      pq.pop();

      if (root == NULL) {
        hTreeNode h = new hTreeNode();
        h.c = top;
        h.leftChild = NULL;
        h.rightChild = NULL;
        h.parent = NULL;
        root = h;
        continue;
      }

      //otherwise, top and root paired together under a parent
      top =

      //if the root node you make has a freqency higher than anything in pq, make a left side...recurse??

    }*/
  }

  vector<bool> encodeString(string megaStr) {
    vector<bool> b;
    //each bit would be represented by a bool 
    return b;
  }

  string decodeString(vector<bool> b) {
    return "";
  }
};


priority_queue<Character, vector<Character>, compare_chars> countFrequencies() {
  vector<Character> allChars; //not using a 255 element long array because that wastes space, and might not account for non-ascii characters
  ifstream reader;
  string currentLine;
  string megaStr;
  reader.open("toCompress.txt");

  while(getline(reader, currentLine)) {
    megaStr+=currentLine + "\n";
    for (char c : currentLine) {
      bool found = false;
      for (int i = 0; i < allChars.size(); i++) {
        if (c == allChars[i].c) {
          found = true;
          allChars[i].freq++;
          break;
        }
      }
      if (!found) {
        allChars.push_back(Character(c, 1));
      }
    } //now we have counted all the frequencies of every character. let's make a priority_queue!
  }

  priority_queue<Character, vector<Character>, compare_chars>  pq;

  for (Character c : allChars) {
    pq.push(c);
  }

  return pq;
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
	priority_queue<Character, vector<Character>, compare_chars> pq = countFrequencies();
    HuffmanTree tree(pq);
	//vector<bool> b = tree.encodeString(megaStr);
    //string megaCheck = tree.decodeString(b);
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