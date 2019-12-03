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
	unordered_map<char, vector<bool>> mapping;

	// Pre-order tree traversal that prints the letter and frequency at each node
	void print(Character* node) {
		if (node == NULL) {
			return;
		}
		cout << node->letter << " " << node->freq << endl;
		print(node->left_child);
		print(node->right_child);
	}

	// In-order tree traversal that looks for a match to the passed coded value
	void checkForCodedValue(Character* node, vector<bool> code, char& result) {
		if (node == NULL) {
			return;
		}
		checkForCodedValue(node->left_child, code, result);
		if (node->letter != -1) {
			if (mapping[node->letter] == code) {
				result = node->letter;
			}
		}
		checkForCodedValue(node->right_child, code, result);
	}

public:
	// Constructor
	HuffmanTree(priority_queue<Character*, vector<Character*>, CompareChars> pq) {
		// Return right away if the passed priority queue is empty
		if (pq.size() == 0) {
			root = NULL;
			return;
		}

		// If the passed priority queue has only one value, then create a node for that value
		if (pq.size() == 1) {
			Character* top = pq.top();
			pq.pop();
			root = top;
			return;
		}

		// Remove values from the passed priority queue and create the Character nodes
		while (pq.size() > 1) {
			// Remove the top two characters from the priority queue and create nodes for them
			Character* first_node = pq.top();
			pq.pop();
			Character* second_node = pq.top();
			pq.pop();

			// Create the parent node and set approriate child and parent pointers
			Character* parent = new Character(-1, first_node->freq + second_node->freq);
			parent->right_child = first_node;
			parent->left_child = second_node;
			first_node->parent = parent;
			second_node->parent = parent;
			pq.push(parent);
		}

		root = pq.top();
		pq.pop();

		// Create the mapping
		vector<bool> path; // Start off with the path being empty
		createMapping(root, path);
	}

	// Creates the mapping for each character and its encoded value
	void createMapping(Character* node, vector<bool> path) {
		if (node == NULL) {
			return;
		}
		if (node->letter != -1) {
			mapping[node->letter] = path;
			return;
		}
		// If its not a char, add the direction traveled (0 for left, 1 for right)
		path.push_back(0);
		createMapping(node->left_child, path);
		path.pop_back();

		path.push_back(1);
		createMapping(node->right_child, path);
		path.pop_back();
	}

	// Print helper method
	void printHelper() {
		print(root);
	}

	// Returns the encoded value for a passed letter
	vector<bool> getEncodedValue(char letter) {
    cout << "here, with: " << int(letter) << endl;
		return mapping[letter];
	}

	// Encodes the passed string
	vector<bool> encodeString(string to_encode) {
		vector<bool> encoded_string;
		for (int i = 0; i < to_encode.size(); ++i) {
			vector<bool> code = mapping[to_encode[i]];
			for (int j = 0; j < code.size(); ++j) {
				encoded_string.push_back(code[j]);
			}
		}
		return encoded_string;
	}

	// Decodes the passed encoded string
	string decodeString(vector<bool> to_decode) {
		string decoded_string = "";
		char result = 0;
		vector<bool> coded_value;
		// Read a character from the encoded string and check if it matches the coded
		// value for a character in the Huffman Tree. If it doesn't, push the next
		// character and check again. If it does, push the matching character to the
		// decoded string and reset the string used to check.
		for (int i = 0; i < to_decode.size(); ++i) {
			coded_value.push_back(to_decode[i]);
			checkForCodedValue(root, coded_value, result);
			if (result != 0) {
				decoded_string += result;
				coded_value.clear();
				result = 0;
			}
		}
		return decoded_string;
	}
};

// Function that counts the frequencies of all letters in a file
priority_queue<Character*, vector<Character*>, CompareChars> countFrequencies(string filename) {
	vector<Character*> all_chars;
	ifstream reader;
	string current_line;
	reader.open(filename);

	// Make sure the file was opened successfully
	if (!reader.is_open()) {
		cout << "Error opening file!" << endl;
		priority_queue<Character*, vector<Character*>, CompareChars>  pq;
		return pq;
	}

	// Read the file line by line and count letter frequencies
	while (getline(reader, current_line)) {
		for (char current_letter : current_line) {

      if (int(current_letter) <= 32) { //check the ASCII table - these are all empty characters, or spaces.
        continue;
      }

      cout << current_letter << " " << int(current_letter) << endl;

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

	// Populate and return the priority queue
	priority_queue<Character*, vector<Character*>, CompareChars>  pq;
	for (Character* letter : all_chars) {
		pq.push(letter);
	}
	return pq;
}

int main(int argc, char* argv[]) {

  cout << "." << endl;

	priority_queue<Character*, vector<Character*>, CompareChars> pq = countFrequencies(argv[1]);

	// Create the Huffman Tree
	HuffmanTree tree(pq);

  tree.printHelper();

	// Check the encoded values
	cout << "Printing the encoded values" << endl;
	string letters = "apm";
  cout << letters.size() << endl;
	for (int i = 0; i < letters.size(); ++i) {
		cout << "'" << letters[i] << "'  =>  ";
		vector<bool> code = tree.getEncodedValue(letters[i]);
		for (int j = 0; j < code.size(); ++j) {
			cout << code[j];
		}
		cout << endl;
	}
	cout << endl;

  cout << "fdsds: " << endl;

  vector<bool> code = tree.getEncodedValue('\0');
  for (int j = 0; j < code.size(); ++j) {
    cout << code[j];
  }
  cout << endl;

	// Encode and decode a string
	vector<bool> encoded_value;
	string decoded_value;
	encoded_value = tree.encodeString("appm\0");
	cout << "appm  =>  ";
	// Print the encoded string
	for (int i = 0; i < encoded_value.size(); ++i) {
		cout << encoded_value[i];
	}
	decoded_value = tree.decodeString(encoded_value);
	cout << endl;
	// Print the encoded string
	for (int i = 0; i < encoded_value.size(); ++i) {
		cout << encoded_value[i];
	}
	cout << "  =>  " << decoded_value << endl;
}
