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
var nodes []Node

type Node struct {
	count int
	char  rune
	left  *Node
	right *Node
}

// Counts the character frequencies
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

// Creates the Huffman codes
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

// Creates the codes by using a pre-order traversal of the tree
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

// Writes the compressed file
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

				int_value, _ := strconv.ParseInt(bit_buffer[:7], 2, 7)
				bit_buffer = bit_buffer[7:]
				char := rune(int_value)
				writer.WriteString(string(char))
			}
		}
	}

	// Encode any remaining bits
	int_value, _ := strconv.ParseInt(bit_buffer, 2, 7)
	bit_buffer = bit_buffer[7:]
	char := rune(int_value)
	writer.WriteString(string(char))

	writer.Flush()
}

// Compresses the passed file
func compress(input_file string) {
	get_char_frequencies(input_file)
	create_huffman_tree()
	huffman_codes = make(map[rune]string)
	generate_codes(nodes[0], "")
	write_compressed_file(input_file)
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
	} else {
		fmt.Println("Invalid action type")
	}
}
