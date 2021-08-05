// Script that implements Huffman Coding assuming UTF-8 encoding
package main

import (
	"bufio"
	"fmt"
	"os"
)

var char_count map[rune]int

type Node struct {
	count int
	char  string
	left  *Node
	right *Node
}

// Counts the character frequencies
func get_char_frequencies(file *os.File) {
	char_count = make(map[rune]int)
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := scanner.Text()
		for _, char := range line {
			char_count[char] += 1
		}
	}
}

// Creates the Huffman codes
func create_huffman_codes() {
	fmt.Println("Creating the codes")
}

// Compresses the passed file
func compress(file *os.File) {
	get_char_frequencies(file)
	create_huffman_codes()

	for key, value := range char_count {
		fmt.Println("Key:", string(key), "Value:", value)
	}
}

// Runs the script
func main() {
	filepath := os.Args[1]
	action := os.Args[2]

	// Open the file
	file, err := os.Open(filepath)
	if err != nil {
		fmt.Println("Error opening file")
		file.Close()
		os.Exit(1)
	}

	if action == "compress" {
		fmt.Println("Compressing the file:", filepath)
		compress(file)
	} else if action == "uncompress" {
		fmt.Println("Uncompressing the file:", filepath)
	} else {
		fmt.Println("Invalid action type")
	}
	file.Close()
}
