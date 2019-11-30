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
	unordered_map<char, string> mapping;

	// Pre-order tree traversal that prints the letter and frequency at each node
	void print(Character* node) {
		if (node == NULL) {
			return;
		}
		cout << node->letter << " " << node->freq << endl;
		print(node->left_child);
		print(node->right_child);
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

		cout << "Priority queue size is: " << pq.size() << endl;
		root = pq.top();
		pq.pop();

		// Create the mapping
		string path = ""; // Start off with an empty string
		createMapping(root, path);
	}

	// Creates the mapping for each character and its encoded value
	void createMapping(Character* node, string path) {
		if (node == NULL) {
			return;
		}
		if (node->letter != -1) {
			mapping[node->letter] = path;
			return;
		}
		// If its not a char, add the direction traveled (0 for left, 1 for right)
		path.append("0");
		createMapping(node->left_child, path);
		path.pop_back();

		path.append("1");
		createMapping(node->right_child, path);
		path.pop_back();
	}

	// Print helper method
	void printHelper() {
		print(root);
	}

	// Returns the encoded value for a passed letter
	string getEncodedValue(char letter) {
		return mapping[letter];
	}

	// Encodes the passed string
	string encodeString(string to_encode) {
		string encoded_string = "";
		for (int i = 0; i < to_encode.size(); ++i) {
			encoded_string.append(mapping[to_encode[i]]);
		}
		return encoded_string;
	}

	// Decodes the passed string
	string decodeString(string to_decode) {
		string decoded_string = "";
		// Still needs to be implemented
		return decoded_string;
	}
};

// Method that counts the frequencies of all letters in a file
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
	priority_queue<Character*, vector<Character*>, CompareChars> pq = countFrequencies(argv[1]);

	// Create the Huffman Tree
	HuffmanTree tree(pq);

	// Check the encoded values
	cout << endl << "Printing the encoded values" << endl;
	cout << "'a' => " << tree.getEncodedValue('a') << endl;
	cout << "'p' => " << tree.getEncodedValue('p') << endl;
	cout << "'m' => " << tree.getEncodedValue('m') << endl << endl;

	// Encode and decode a string
	string encoded_value;
	string decoded_value;
	encoded_value = tree.encodeString("appm");
	cout << "appm  =>  " << encoded_value << endl;
	decoded_value = tree.decodeString(encoded_value);
	cout << encoded_value << "  =>  " << decoded_value << endl;
}
