// Script that implements Huffman Coding assuming UTF-8 encoding
package main

import (
	"bufio"
	"fmt"
	"os"
	"sort"
	"strconv"
	"strings"
)

var char_count map[rune]int
var huffman_codes map[rune]string
var char_codes map[string]rune
var nodes []Node

type Node struct {
	count int
	char  rune
	left  *Node
	right *Node
}

// get_char_frequencies counts the character frequencies
func get_char_frequencies(input_filepath string) {
	input_file, err := os.Open(input_filepath)
	if err != nil {
		fmt.Println("There was an error opening the input file!")
		return
	}
	defer input_file.Close()

	char_count = make(map[rune]int)
	scanner := bufio.NewScanner(input_file)
	for scanner.Scan() {
		line := scanner.Text()
		for _, char := range line {
			char_count[char] += 1
		}
	}
}

// create_huffman_tree creates the Huffman codes
func create_huffman_tree() {
	// First, create a Node for each character
	for char, count := range char_count {
		new_node := Node{count: count, char: char}
		nodes = append(nodes, new_node)
	}

	// Create the tree
	for {
		if len(nodes) < 2 {
			break
		}

		// Sort the remaining Nodes by count
		sort.Slice(nodes, func(i, j int) bool {
			return nodes[i].count < nodes[j].count
		})

		// Remove the two Nodes with the lowest counts
		left_node := nodes[0]
		right_node := nodes[1]
		nodes = nodes[2:]

		// Create the parent Node and add it to the slice
		parent_count := left_node.count + right_node.count
		parent_node := Node{count: parent_count, left: &left_node, right: &right_node}
		nodes = append(nodes, parent_node)
	}
}

// generate_codes creates the character codes by using a pre-order traversal of the binary tree
func generate_codes(node Node, path string) string {
	if node.char != 0 {
		huffman_codes[node.char] = path
	}
	if node.left != nil {
		path += "0"
		path = generate_codes(*node.left, path)
	}
	if node.right != nil {
		path += "1"
		path = generate_codes(*node.right, path)
	}
	if len(path) > 0 {
		path = path[:len(path)-1]
	}

	return path
}

// write_compressed_file encodes the passed file and writes it to an output file
func write_compressed_file(input_filepath string) {
	// Open both files
	input_file, err := os.Open(input_filepath)
	if err != nil {
		fmt.Println("There was an error opening the input file!")
		return
	}
	defer input_file.Close()

	output_file, err := os.Create("compressed_file.txt")
	if err != nil {
		fmt.Println("There was an error opening the output file!")
		return
	}
	defer output_file.Close()

	// Open the output file and input file
	writer := bufio.NewWriter(output_file)
	scanner := bufio.NewScanner(input_file)

	// First, write the codes to the compressed file
	for char, code := range huffman_codes {
		output_str := string(char) + "~=" + string(code) + "><"
		writer.WriteString(output_str)
	}
	writer.WriteString("\n$\n")

	// Read the input file characters and encode 7 bits at a time
	bit_buffer := ""
	for scanner.Scan() {
		line := scanner.Text()
		for _, char := range line {
			bit_buffer += huffman_codes[char]

			for {
				if len(bit_buffer) < 7 {
					break
				}

				int_value, _ := strconv.ParseInt(bit_buffer[:7], 2, 8)
				bit_buffer = bit_buffer[7:]
				char := rune(int_value)
				writer.WriteString(string(char))
			}
		}
	}

	// Encode any remaining bits
	int_value, _ := strconv.ParseInt(bit_buffer, 2, 8)
	char := rune(int_value)
	writer.WriteString(string(char))

	writer.Flush()
}

// compress encodes the passed file
func compress(input_file string) {
	get_char_frequencies(input_file)
	create_huffman_tree()
	huffman_codes = make(map[rune]string)
	generate_codes(nodes[0], "")
	write_compressed_file(input_file)
}

// read_codes reads the character codes from the compressed file
func read_codes(input_filepath string) {
	input_file, err := os.Open(input_filepath)
	if err != nil {
		fmt.Println("There was an error opening the input file!")
		return
	}
	defer input_file.Close()

	scanner := bufio.NewScanner(input_file)

	// Read the character frequencies
	char_codes = make(map[string]rune)
	codes := ""
	for scanner.Scan() {
		line := scanner.Text()

		if line == "$" {
			break
		}
		codes += line
	}

	all_codes := strings.Split(codes, "><")
	num_codes := len(all_codes) - 1
	for code_num, code_details := range all_codes {
		if code_num == num_codes {
			break
		}

		parsed_details := strings.Split(code_details, "~=")
		runes := []rune(parsed_details[0])
		code := parsed_details[1]
		char_codes[code] = runes[0]
	}
}

// uncompress parses and decodes the passed file
func uncompress(input_filepath string) {
	// First, read the codes
	read_codes(input_filepath)

	// Open both files
	input_file, err := os.Open(input_filepath)
	if err != nil {
		fmt.Println("There was an error opening the input file!")
		return
	}
	defer input_file.Close()

	output_file, err := os.Create("uncompressed_file.txt")
	if err != nil {
		fmt.Println("There was an error opening the output file!")
		return
	}
	defer output_file.Close()

	// Open the output file and input file
	writer := bufio.NewWriter(output_file)
	scanner := bufio.NewScanner(input_file)

	// Read the rest of the input file and write characters to the output file as they are found
	bits := ""
	to_check := ""
	reading_codes := true
	for scanner.Scan() {
		line := scanner.Text()

		if reading_codes {
			if line == "$" {
				reading_codes = false
			}
		} else {
			fmt.Println("Reading the next line")
			for _, char := range line {
				bits += fmt.Sprintf("%07b", char)
			}

			for {
				if len(bits) == 0 {
					break
				}
				if len(to_check) < 20 {
					fmt.Println("Checking", to_check)
				}
				to_check += string(bits[0])
				bits = bits[1:]

				if decoded_char, exists := char_codes[to_check]; exists {
					writer.WriteString(string(decoded_char))
					to_check = ""
				}
			}
		}
	}

	writer.Flush()
}

// Runs the script
func main() {
	action := os.Args[1]
	action = strings.ToLower(action)
	filepath := os.Args[2]

	if action == "compress" {
		fmt.Println("Compressing the file:", filepath)
		compress(filepath)
	} else if action == "uncompress" {
		fmt.Println("Uncompressing the file:", filepath)
		uncompress(filepath)
	} else {
		fmt.Println("Invalid action type")
	}
}
