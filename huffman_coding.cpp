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
#include <cmath>
#include <bitset>
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
	vector<char> letters;
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

	// Post-order tree traversal used by the destructor
	void deleteNode(Character* node) {
		if (node == NULL) {
			return;
		}
		deleteNode(node->left_child);
		deleteNode(node->right_child);
		delete node;
	}

	// Tree traversal that looks for a match to the passed coded value
	char checkForCodedValue(vector<bool> code) {
		Character* node = root;
		for (int i = 0; i < code.size(); ++i) {
			if (code[i] == 0) {
				node = node->left_child;
			}
			else {
				node = node->right_child;
			}
		}
		// Check if resulting node is a character
		if (node->letter != -1) {
			if (mapping[node->letter] == code) {
				return node->letter;
			}
		}
		else {
			return '0';
		}
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

			//add to the list of letters
			if (first_node->letter != -1) {
				letters.push_back(first_node->letter);
			}
			if (second_node->letter != -1) {
				letters.push_back(second_node->letter);
			}

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

	// Destructor
	~HuffmanTree() {
		deleteNode(root);
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
	void printTree() {
		print(root);
	}

	// Prints the coded values
	void printCodedValues() {
		cout << "Printing the encoded values" << endl;
		for (int i = 0; i < letters.size(); ++i) {
			cout << "'" << letters[i] << "'  =>  ";
			vector<bool> code = mapping[letters[i]];
			for (int j = 0; j < code.size(); ++j) {
				cout << code[j];
			}
			cout << endl;
		}
	}

	// Returns the encoded value for a passed character
	vector<bool> getEncodedValue(char letter) {
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

	// Returns a decoded the passed vector of booleans by determining their corresponding
	// character values based on the Huffman Coding
	string decodeBits(queue<bool> to_decode) {
		string encoded_string = "";
		char result;
		vector<bool> coded_value;
		bool next_bool;
		while (to_decode.size()) {
			// Read a bool from the passed vector and check if it matches the coded
			// value for a character in the Huffman Tree. If it doesn't, push the next
			// bool and check again. If it does, push the matching character to the
			// decoded string and reset vector of bools used to check.
			next_bool = to_decode.front();
			to_decode.pop();
			coded_value.push_back(next_bool);
			result = checkForCodedValue(coded_value);
			if (result != '0') {
				encoded_string += result;
				coded_value.clear();
			}
		}
		return encoded_string;
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

	int run = 0;

	// Read the file line by line and count letter frequencies
	while (getline(reader, current_line)) {
		if (run == 1) { //want to add spaces to the tree to replace newlines. If there is a file that's just words separated by newlines, we want to retain those as spaces and accurately count their frequency in the tree
			char letter = ' ';
			bool letter_found = false;
			// Check if the letter has already been created. If so, increment its frequency
			for (int i = 0; i < all_chars.size(); i++) {
				if (letter == all_chars[i]->letter) {
					letter_found = true;
					++all_chars[i]->freq;
					break;
				}
			}
			// If the letter doesn't exist then create the Character object
			if (!letter_found) {
				Character* ch = new Character(letter, 1);
				all_chars.push_back(ch);
			}
		}

		for (char current_letter : current_line) {

			if (int(current_letter) < 32) { //check the ASCII table - these are all empty characters, except spaces.
				continue;
			}

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
		run++;
	}

	reader.close();

	// Populate and return the priority queue
	priority_queue<Character*, vector<Character*>, CompareChars>  pq;
	for (Character* letter : all_chars) {
		pq.push(letter);
	}
	return pq;
}

// Builds the priority queue based off of the most commonly used letters
priority_queue<Character*, vector<Character*>, CompareChars> mostCommonLetters() {
	priority_queue<Character*, vector<Character*>, CompareChars>  pq;
	// Most common letters (also including the space and new line characters). No punctuation for now.
	// https://en.wikipedia.org/wiki/Letter_frequency
	char most_common_chars[28] = { ' ', 'e', 't', 'a', 'o', 'i', 'n', 's', 'h', 'r', 'd', 'l',
		'c', 'u', 'm', 'w', 'f', 'g', 10, 'y', 'p', 'b', 'v', 'k', 'j', 'x', 'q', 'z' };
	for (int i = 0; i < 28; ++i) {
		Character* letter = new Character(most_common_chars[i], 28 - i);
		pq.push(letter);
	}
	return pq;
}

// Converts the passed vector of 8 booleans (representing a byte) to an integer value and then
// to the corresponding ASCII character
char byteToChar(vector<bool> byte) {
	// Create the integer value from the bits
	int value = 0;
	for (int i = 0; i < 8; ++i) {
		if (byte[i]) {
			int squared = pow(2, 7-i);
			value = value + squared;
		}
	}
	// Then create the char
	char coded_char = (char)value;
	return coded_char;
}

// Compress the passed text file based on the passed Huffman Coding binary tree
void compressFile(HuffmanTree& tree, string filename) {
	// Encode the passed text file and write it to a new 'compressed' text file
	queue<bool> bits;
	char coded_char;
	vector<bool> coded_value;

	// Open the original file
	ifstream input_file;
	char next_char;
	input_file.open(filename);
	if (!input_file.is_open()) {
		cout << "Error opening file!" << endl;
		return;
	}

	// Read the file character by character and push the coded bits to the queue
	while (input_file.get(next_char)) {
		//cout << next_char;
		coded_value = tree.getEncodedValue(next_char);
		for (int i = 0; i < coded_value.size(); ++i) {
			bits.push(coded_value[i]);
		}
	}
	input_file.close();
	cout << endl << endl;

	// Open the output file which will contain the compressed version
	ofstream output_file;
	output_file.open("compressed.txt", ifstream::binary);

	// Go through the bits, create a byte for each section of 8, determine the corresonding
	// ASCII character, and then write that character to the output file
	while (bits.size() > 8) {
		vector<bool> byte;
		for (int i = 0; i < 8; ++i) {
			bool next_bit;
			next_bit = bits.front();
			bits.pop();
			byte.push_back(next_bit);
		}
		// Determine the character and write it to the output file
		coded_char = byteToChar(byte);
		output_file << coded_char;
	}

	// Handle the remaining bits by adding zeros to the end to create a byte
	vector<bool> byte;
	for (int i = 0; i < 8; ++i) {
		if (!bits.empty()) {
			bool next_bit;
			next_bit = bits.front();
			bits.pop();
			byte.push_back(next_bit);
		}
		else {
			byte.push_back(0);
		}
	}
	// Determine the character and write it to the output file
	coded_char = byteToChar(byte);
	output_file << coded_char;

	// Close the output file
	output_file.close();
}

// Decompress the passed file
void decompressFile(HuffmanTree& tree, string filename) {
	// Open the compressed file
	ifstream compressed_file;
	queue<bool> input_bits;
	char next_char;
	string binary;

	// Make sure the file is opened in binary mode
	compressed_file.open("compressed.txt", ifstream::binary);
	if (!compressed_file.is_open()) {
		cout << "Error opening file!" << endl;
		return;
	}
	// Read the file character by character and append the bits to the queue
	while (compressed_file.get(next_char)) {
		int value = next_char;
		binary = bitset<8>(value).to_string();
		//cout << binary << "  =>  " << next_char << "  =>  " << value << endl;
		for (int i = 0; i < 8; ++i) {
			if (binary[i] == '1') {
				input_bits.push(1);
			}
			else {
				input_bits.push(0);
			}
		}
	}
	compressed_file.close();

	// Write the decompressed version to a file
	string decompressed_string = tree.decodeBits(input_bits);
	ofstream decompressed_file;
	decompressed_file.open("decompressed.txt");
	decompressed_file << decompressed_string;
	decompressed_file.close();
}

int main(int argc, char* argv[]) {
	// Create the Huffman Tree based on the priority queue
	priority_queue<Character*, vector<Character*>, CompareChars> pq = mostCommonLetters();
	HuffmanTree tree(pq);

	// Print the different encoded values
	tree.printCodedValues();

	// Compress and then decompress the passed file
	compressFile(tree, argv[1]);
	string compressed_filename = "compressed.txt";
	decompressFile(tree, compressed_filename);

	return 0;
}
