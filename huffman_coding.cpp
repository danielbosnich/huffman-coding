// Names: Daniel Bosnich & Pranav Subramanian
// Assignment: Final Project
// Course: CSCI-2275
// Term: Fall 2019

#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <vector>
#include <unordered_map>
using namespace std;

// Struct that implements a node in the Huffman Tree
struct Character {
	char letter;
	int freq;
	Character* parent;
	Character* left_child;
	Character* right_child;
	// Constructor
	Character(char letter, int freq) {
		this->letter = letter;
		this->freq = freq;
		parent = NULL;
		left_child = NULL;
		right_child = NULL;
	}
};

// Struct used to compare two characters when adding to the priority queue
struct CompareChars {
	bool operator() (const Character* char1, const Character* char2) {
		return char1->freq > char2->freq; // Return true if char1 has a higher frequency than char2
	}
};

// Class that implements the Huffman Tree
class HuffmanTree {
	Character* root;

	// Recursive tree traversal that prints the letter and frequency at each node
	void print(Character* node) {
		if (node == NULL) {
			return;
		}
		cout << node->letter << " " << node->freq << endl;
		print(node->left_child);
		print(node->right_child);
	}
public:
	unordered_map<char, vector<bool>> mapping;
	// Constructor
	HuffmanTree(priority_queue<Character*, vector<Character*>, CompareChars>& pq, int highestFrequency, bool isAuxillary) {
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

		while (pq.size() > 1) {
			cout << "here" << endl;

			// Take first 2 nodes
			Character* firstNode = pq.top();
			pq.pop();

			Character* secondNode = pq.top();
			pq.pop();

			Character* parent = new Character(-1, firstNode->freq + secondNode->freq);
			//cout << firstNode->c << " " << secondNode->c << " " << parent->freq << endl;
			parent->right_child = firstNode;
			parent->left_child = secondNode;

			firstNode->parent = parent;
			secondNode->parent = parent;

			pq.push(parent);
		}
		cout << "Priority queue size is: " << pq.size() << endl;
		root = pq.top();
		cout << root->letter << " " << root->freq << "\n\n\n";
		pq.pop();

		//create the mapping!
		vector<bool> v;
		createMapping(root, v);
	}

	// Brief comment for each method
	void createMapping(Character* n, vector<bool> v) {
		if (n == NULL) {
			return;
		}
		if (n->letter != -1) {
			mapping[n->letter] = v;
			return;
		}
		// If its not a char
		v.push_back(0);
		createMapping(n->left_child, v);
		v.pop_back();

		v.push_back(1);
		createMapping(n->right_child, v);
		v.pop_back();
	}

	// Print helper method
	void print_helper() {
		print(root);
	}

	// Brief comment for each method
	vector<bool> encodeString(string megaStr) {
		vector<bool> b;
		// Each bit would be represented by a bool
		return b;
	}

	// Brief comment for each method
	string decodeString(vector<bool> b) {
		return "";
	}
};


priority_queue<Character*, vector<Character*>, CompareChars> countFrequencies(int& highestFrequency) {
	vector<Character*> all_chars;
	ifstream reader;
	string current_line;
	reader.open("toCompress.txt"); // For testing purposes

	if (!reader.is_open()) {
		priority_queue<Character*, vector<Character*>, CompareChars>  pq;
		return pq;
	}

	while (getline(reader, current_line)) {
		for (char current_letter : current_line) {
			bool letter_found = false;
			// Check if the letter has already been created. If so, increment its frequency
			for (int i = 0; i < all_chars.size(); i++) {
				if (current_letter == all_chars[i]->letter) {
					letter_found = true;
					++all_chars[i]->freq;
					break;
				}
			}
			// If the letter doesn't exist then create the Character object
			if (!letter_found) {
				Character* ch = new Character(current_letter, 1);
				all_chars.push_back(ch);
			}
		}
	}

	priority_queue<Character*, vector<Character*>, CompareChars>  pq;
	highestFrequency = all_chars[0]->freq;

	for (Character* letter : all_chars) {
		if (letter->freq > highestFrequency) {
			highestFrequency = letter->freq;
		}
		pq.push(letter);
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

	priority_queue<Character*, vector<Character*>, CompareChars> pq = countFrequencies(highestFrequency);
	//vector<bool> b = tree.encodeString(megaStr); <-- for debugging
	//string megaCheck = tree.decodeString(b);

	cout << "The highest frequency is:" << highestFrequency << endl;

	HuffmanTree tree(pq, highestFrequency, false);

	tree.print_helper();

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




	/*
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
	*/
}
