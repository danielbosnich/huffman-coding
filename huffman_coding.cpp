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

	// Tree traversal that looks for a match to the passed coded value and returns the character
	// if a match was found
	char checkForCodedValue(vector<bool> code) {
		Character* node = root;
		// Travel through the tree based on the passed code
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
			return -1;
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

			// Add to the list of letters
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

	// Prints the encoded values
	void printCodedValues() {
		cout << "Encoded value mapping:" << endl;
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
	queue<bool> encodeString(string to_encode) {
		queue<bool> encoded_string;
		for (int i = 0; i < to_encode.size(); ++i) {
			vector<bool> code = mapping[to_encode[i]];
			for (int j = 0; j < code.size(); ++j) {
				encoded_string.push(code[j]);
			}
		}
		return encoded_string;
	}

	// Returns a decoded string based on the passed vector of booleans by determining 
	// their corresponding character values in the Huffman Tree
	string decodeBits(queue<bool> to_decode) {
		string encoded_string = "";
		char result = -1;
		vector<bool> coded_value;
		bool next_bool;
		while (to_decode.size()) {
			// Read a bool from the passed queue and check if it matches the coded
			// value for a character in the Huffman Tree. If it doesn't, push the next
			// bool and check again. If it does, push the matching character to the
			// decoded string and reset vector of bools used to check.
			next_bool = to_decode.front();
			to_decode.pop();
			coded_value.push_back(next_bool);
			result = checkForCodedValue(coded_value);
			if (result != -1) {
				encoded_string += result;
				coded_value.clear();
			}
		}
		return encoded_string;
	}
};

// Counts the frequencies of all characters in a file
priority_queue<Character*, vector<Character*>, CompareChars> countFrequenciesInFile(string filename) {
	vector<Character*> all_chars;
	ifstream input_file;
	char current_char;
	input_file.open(filename);

	// Make sure the file was opened successfully
	if (!input_file.is_open()) {
		cout << "Error opening file!" << endl;
		priority_queue<Character*, vector<Character*>, CompareChars>  pq;
		return pq;
	}

	// Read the file character by character and count character frequencies
	while (input_file.get(current_char)) {
		bool char_found = false;
		// Check if the character has already been created. If so, increment its frequency
		for (int i = 0; i < all_chars.size(); ++i) {
			if (current_char == all_chars[i]->letter) {
				char_found = true;
				++all_chars[i]->freq;
				break;
			}
		}
		// If the character doesn't exist then create the Character object
		if (!char_found) {
			Character* ch = new Character(current_char, 1);
			all_chars.push_back(ch);
		}
	}
	input_file.close();

	// Populate and return the priority queue
	priority_queue<Character*, vector<Character*>, CompareChars>  pq;
	for (Character* char_node : all_chars) {
		pq.push(char_node);
	}
	return pq;
}

// Counts the frequencies of all characters in a string
priority_queue<Character*, vector<Character*>, CompareChars> countFrequencies(string str) {
	int run = 0;
	vector<Character*> all_chars;

	// Read the file line by line and count letter frequencies
	for (char current_char : str) {
		bool char_found = false;
		// Check if the letter has already been created. If so, increment its frequency
		for (int i = 0; i < all_chars.size(); i++) {
			if (current_char == all_chars[i]->letter) {
				char_found = true;
				++all_chars[i]->freq;
				break;
			}
		}
		// If the letter doesn't exist then create the Character object
		if (!char_found) {
			Character* ch = new Character(current_char, 1);
			all_chars.push_back(ch);
		}
	}

	// Populate and return the priority queue
	priority_queue<Character*, vector<Character*>, CompareChars>  pq;
	for (Character* char_node : all_chars) {
		pq.push(char_node);
	}
	return pq;
}

// Builds the priority queue based off of the most commonly used letters
priority_queue<Character*, vector<Character*>, CompareChars> mostCommonLetters() {
	priority_queue<Character*, vector<Character*>, CompareChars>  pq;
	// Most common lowercase letters(also including space and new line characters). No punctuation or numbers.
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
			int squared = pow(2, 7 - i);
			value += squared;
		}
	}
	// Then create the char
	char coded_char = (char)value;
	return coded_char;
}

// Compresses the passed text file based on the passed Huffman Coding binary tree
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
	string compressed_filename = filename.substr(0, filename.find(".txt")) + "_compressed.txt";
	output_file.open(compressed_filename, ifstream::binary);

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

// Decompresses the passed file based on the passed Huffman Coding binary tree
void decompressFile(HuffmanTree& tree, string filename, string original_filename) {
	// Open the compressed file
	ifstream compressed_file;
	queue<bool> input_bits;
	char next_char;
	string binary;

	// Make sure the file is opened in binary mode
	compressed_file.open(filename, ifstream::binary);
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
	decompressed_file.open(original_filename.substr(0, original_filename.find(".txt")) + "_decompressed.txt");
	decompressed_file << decompressed_string;
	decompressed_file.close();
}

// Prints the menu with information on how to use the program
void printMenu() {
	cout << "=====MAIN MENU=====" << endl;
	cout << "1. Compress and decompress a string" << endl;
	cout << "2. Compress and decompress a file; print output to console" << endl;
	cout << "3. Compress and decompress a file; create text files in current directory" << endl;
	cout << "4. Quit!" << endl << endl;
	return;
}

// Gets user input for either the string to compress or the name of the file to compress
string getUserInput(bool is_filename, string message = "") {
	string user_input;
	cin.ignore();
	if (is_filename) {
		cout << "Enter the filename or path of the file that you wish to compress" << endl;
	}
	else {
		cout << "Enter the string that you wish to compress" << endl;
	}
	// Print the message if one was passed
	if (message != "") {
		cout << message << endl;
	}
	getline(cin, user_input);
	cout << endl;
	return user_input;
}

int main() {
	int user_input;
	bool quit_program = false;
	while (!quit_program) {
		printMenu();
		cin >> user_input;
		switch (user_input) {
		case 1:
		{
			// Get the string to compress
			string string_to_compress = getUserInput(false);

			// Create the Huffman Tree based on the priority queue
			priority_queue<Character*, vector<Character*>, CompareChars> pq = countFrequencies(string_to_compress);
			HuffmanTree tree(pq);

			// Print the mapping of coded values
			tree.printCodedValues();

			// Print the encoded string, as 1's and 0's
			cout << endl << "Encoded string:" << endl;
			queue<bool> compressed_string_bools = tree.encodeString(string_to_compress);
			queue<bool> print_bools = compressed_string_bools;
			while (print_bools.size()) {
				cout << print_bools.front();
				print_bools.pop();
			}
			cout << endl;

			// Print the decoded string, from the 1's and 0's above
			cout << endl << "Decoded string:" << endl;
			string decoded_string_from_bools = tree.decodeBits(compressed_string_bools);
			cout << decoded_string_from_bools << endl << endl << endl;
			break;
		}
		case 2:
		{
			// Get the filename
			string filename = getUserInput(true);

			// Create the Huffman Tree based on the priority queue, making the priority_queue based on the 
			// frequencies of characters in the file
			priority_queue<Character*, vector<Character*>, CompareChars> pq = countFrequenciesInFile(filename);
			HuffmanTree tree(pq);

			// Print the different encoded values
			tree.printCodedValues();

			// Convert the file to a string
			string file_as_string;
			char next_char;
			ifstream input_file(filename);
			while (input_file.get(next_char)) {
				file_as_string += next_char;
			}

			// Print the encoded string, as 1's and 0's
			cout << endl << "Encoded string:" << endl;
			queue<bool> compressed_string_bools = tree.encodeString(file_as_string);
			queue<bool> print_bools = compressed_string_bools;
			while (print_bools.size()) {
				cout << print_bools.front();
				print_bools.pop();
			}
			cout << endl;

			// Decode and print the string from the 1's and 0's above
			cout << endl << "Decoded string:" << endl;
			string decoded_string_from_bools = tree.decodeBits(compressed_string_bools);
			cout << decoded_string_from_bools << endl << endl << endl;
			break;
		}
		case 3:
		{
			// Get the filename
			string message = "*File should only contain lower case letters and no punctuation*";
			string file_to_compress = getUserInput(true, message);

			// Create the Huffman Tree based on the priority queue, this time using the most common letters!
			priority_queue<Character*, vector<Character*>, CompareChars> pq = mostCommonLetters();
			HuffmanTree tree(pq);

			// Print the different encoded values
			tree.printCodedValues();

			// Compress and then decompress the passed file
			string compressed_filename = file_to_compress.substr(0, file_to_compress.find(".txt")) + "_compressed.txt";
			compressFile(tree, file_to_compress);
			decompressFile(tree, compressed_filename, file_to_compress);
			break;
		}
		case 4:
			cout << "Goodbye!" << endl;
			quit_program = true;
			break;
		default:
			// If the input is incorrect, prompt the user to try again
			cin.clear();
			cin.ignore(INT_MAX, '\n');
			cout << "Invalid input! Please try again." << endl << endl;
		}
	}
}
