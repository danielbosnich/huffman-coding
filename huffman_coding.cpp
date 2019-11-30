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
  Character* parent;
  Character* leftChild;
  Character* rightChild;
  Character(char ch, int fr) : c(ch), freq(fr), parent(NULL), leftChild(NULL), rightChild(NULL) {};
  Character() : c(-1), freq(0), parent(NULL), leftChild(NULL), rightChild(NULL) {};
};

struct compare_chars {
  bool operator() (Character* const& char1, Character* const& char2) { //remembered how to do this via http://www.cplusplus.com/forum/general/43047/
    return char1->freq > char2->freq; //return true if char 1 would come before char 2
  }
};

class HuffmanTree {

public:
  Character* root; //needs to be accessible
  unordered_map<char, vector<bool>> mapping;

  HuffmanTree(priority_queue<Character*, vector<Character*>, compare_chars> &pq, int highestFrequency, bool isAuxillary) {
    if (pq.size() == 0) {
      root = NULL;
      return;
    }

    if (pq.size() == 1) {
      Character* top = pq.top();
      pq.pop();
      root = top;
      return;
    }

    while(pq.size() > 1) {
      cout << "here" << endl;

      //take first 2 nodes
      Character* firstNode = pq.top();
      pq.pop();

      Character* secondNode = pq.top();
      pq.pop();

      Character* parent = new Character(-1, firstNode->freq + secondNode->freq);
      //cout << firstNode->c << " " << secondNode->c << " " << parent->freq << endl;
      parent->rightChild = firstNode;
      parent->leftChild = secondNode;

      firstNode->parent = parent;
      secondNode->parent = parent;

      pq.push(parent);
    }
    cout << pq.size();
    root = pq.top();
    cout << root->c << " " << root->freq << "\n\n\n";
    pq.pop();

    //create the mapping!
    vector<bool> v;
    createMapping(root, v);
  }

  void createMapping(Character* n, vector<bool> v) {
    if (n == NULL) {
      return;
    }
    if (n->c != -1) {
      mapping[n->c] = v;
      return;
    }
    //if its not a char
    v.push_back(0);
    createMapping(n->leftChild, v);
    v.pop_back();

    v.push_back(1);
    createMapping(n->rightChild, v);
    v.pop_back();
  }

  void print(Character* n) {
    if (n == NULL) {
      return;
    }
    cout << n->c << " " << n->freq << endl;
    print(n->leftChild);
    print(n->rightChild);
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


priority_queue<Character*, vector<Character*>, compare_chars> countFrequencies(int &highestFrequency) {
  vector<Character*> allChars; //not using a 255 element long array because that wastes space, and might not account for non-ascii characters
  ifstream reader;
  string currentLine;
  string megaStr;
  reader.open("toCompress.txt");

  if(!reader.is_open()) {
    priority_queue<Character*, vector<Character*>, compare_chars>  pq;
    return pq;
  }

  while(getline(reader, currentLine)) {
    megaStr+=currentLine + "\n";
    for (char c : currentLine) {
      bool found = false;
      for (int i = 0; i < allChars.size(); i++) {
        if (c == allChars[i]->c) {
          found = true;
          allChars[i]->freq++;
          break;
        }
      }
      if (!found) {
        Character* ch = new Character(c, 1);
        allChars.push_back(ch);
      }
    } //now we have counted all the frequencies of every character. let's make a priority_queue!
  }

  priority_queue<Character*, vector<Character*>, compare_chars>  pq;

  highestFrequency = allChars[0]->freq;

  for (Character* c : allChars) {
    if (c->freq > highestFrequency) {
      highestFrequency = c->freq;
    }
    pq.push(c);
  }

  return pq;
};




// Prints the menu with information on how to use the program
void printMenu() {
	cout << "======Main Menu======" << endl;
	cout << "1. Encode a word" << endl; //maybe have this say give a file name?
  cout << "2. Decode that word" << endl;
	cout << "3. Quit" << endl;
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
  int highestFrequency;

  priority_queue<Character*, vector<Character*>, compare_chars> pq = countFrequencies(highestFrequency);
  //vector<bool> b = tree.encodeString(megaStr); <-- for debugging
  //string megaCheck = tree.decodeString(b);

  cout << highestFrequency << endl;

  HuffmanTree tree(pq, highestFrequency, false);

  tree.print(tree.root);

  for (bool x : tree.mapping['a']) {
    cout << x;
  }
  cout << endl;
  for (bool x : tree.mapping['p']) {
    cout << x;
  }
  cout << endl;
  for (bool x : tree.mapping['m']) {
    cout << x;
  }
  cout << endl << endl << endl;

  //^^ also debugging, check that the tree is right





	bool quit_program = false;
	int user_input;
	string word_input; //i dont think we should read user input - we should be compressing files, treating them as large strings??
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
      cout << "Decoding word" << endl;
      break;
		case 3:
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
