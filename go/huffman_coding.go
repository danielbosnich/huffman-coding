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

var charCount map[rune]int
var huffmanCodes map[rune]string
var charCodes map[string]rune
var nodes []Node

type Node struct {
	count int
	char  rune
	left  *Node
	right *Node
}

// getCharFrequencies counts the character frequencies
func getCharFrequencies(inputFilepath string) {
	inputFile, err := os.Open(inputFilepath)
	if err != nil {
		fmt.Println("There was an error opening the input file!")
		return
	}
	defer inputFile.Close()

	charCount = make(map[rune]int)
	scanner := bufio.NewScanner(inputFile)
	for scanner.Scan() {
		line := scanner.Text()
		for _, char := range line {
			charCount[char] += 1
		}
	}
}

// createHuffmanTree creates the Huffman codes
func createHuffmanTree() {
	// First, create a Node for each character
	for char, count := range charCount {
		newNode := Node{count: count, char: char}
		nodes = append(nodes, newNode)
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
		leftNode := nodes[0]
		rightNode := nodes[1]
		nodes = nodes[2:]

		// Create the parent Node and add it to the slice
		parentCount := leftNode.count + rightNode.count
		parentNode := Node{count: parentCount, left: &leftNode, right: &rightNode}
		nodes = append(nodes, parentNode)
	}
}

// generateCodes creates the character codes by using a pre-order traversal of the binary tree
func generateCodes(node Node, path string) string {
	if node.char != 0 {
		huffmanCodes[node.char] = path
	}
	if node.left != nil {
		path += "0"
		path = generateCodes(*node.left, path)
	}
	if node.right != nil {
		path += "1"
		path = generateCodes(*node.right, path)
	}
	if len(path) > 0 {
		path = path[:len(path)-1]
	}

	return path
}

// writeCompressedFile encodes the passed file and writes it to an output file
func writeCompressedFile(inputFilepath string) {
	// Open both files
	inputFile, err := os.Open(inputFilepath)
	if err != nil {
		fmt.Println("There was an error opening the input file!")
		return
	}
	defer inputFile.Close()

	outputFile, err := os.Create("compressed_file.txt")
	if err != nil {
		fmt.Println("There was an error opening the output file!")
		return
	}
	defer outputFile.Close()

	// Open the output file and input file
	writer := bufio.NewWriter(outputFile)
	scanner := bufio.NewScanner(inputFile)

	// First, write the codes to the compressed file
	for char, code := range huffmanCodes {
		outputStr := string(char) + "~=" + string(code) + "><"
		writer.WriteString(outputStr)
	}
	writer.WriteString("\n$\n")

	// Read the input file characters and encode 7 bits at a time
	bitBuffer := ""
	for scanner.Scan() {
		line := scanner.Text()
		for _, char := range line {
			bitBuffer += huffmanCodes[char]

			for {
				if len(bitBuffer) < 7 {
					break
				}

				intValue, _ := strconv.ParseInt(bitBuffer[:7], 2, 8)
				bitBuffer = bitBuffer[7:]
				char := rune(intValue)
				writer.WriteString(string(char))
			}
		}
	}

	// Encode any remaining bits
	intValue, _ := strconv.ParseInt(bitBuffer, 2, 8)
	char := rune(intValue)
	writer.WriteString(string(char))

	writer.Flush()
}

// compress encodes the passed file
func compress(inputFile string) {
	getCharFrequencies(inputFile)
	createHuffmanTree()
	huffmanCodes = make(map[rune]string)
	generateCodes(nodes[0], "")
	writeCompressedFile(inputFile)
}

// readCodes reads the character codes from the compressed file
func readCodes(inputFilepath string) {
	inputFile, err := os.Open(inputFilepath)
	if err != nil {
		fmt.Println("There was an error opening the input file!")
		return
	}
	defer inputFile.Close()

	scanner := bufio.NewScanner(inputFile)

	// Read the character frequencies
	charCodes = make(map[string]rune)
	codes := ""
	for scanner.Scan() {
		line := scanner.Text()

		if line == "$" {
			break
		}
		codes += line
	}

	allCodes := strings.Split(codes, "><")
	numCodes := len(allCodes) - 1
	for codeNum, codeDetails := range allCodes {
		if codeNum == numCodes {
			break
		}

		parsedDetails := strings.Split(codeDetails, "~=")
		runes := []rune(parsedDetails[0])
		code := parsedDetails[1]
		charCodes[code] = runes[0]
	}
}

// uncompress parses and decodes the passed file
func uncompress(inputFilepath string) {
	// First, read the codes
	readCodes(inputFilepath)

	// Open both files
	inputFile, err := os.Open(inputFilepath)
	if err != nil {
		fmt.Println("There was an error opening the input file!")
		return
	}
	defer inputFile.Close()

	outputFile, err := os.Create("uncompressed_file.txt")
	if err != nil {
		fmt.Println("There was an error opening the output file!")
		return
	}
	defer outputFile.Close()

	// Open the output file and input file
	writer := bufio.NewWriter(outputFile)
	scanner := bufio.NewScanner(inputFile)

	// Read the rest of the input file and write characters to the output file as they are found
	bits := ""
	toCheck := ""
	readingCodes := true
	for scanner.Scan() {
		line := scanner.Text()

		if readingCodes {
			if line == "$" {
				readingCodes = false
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

				toCheck += string(bits[0])
				bits = bits[1:]

				if decodedChar, exists := charCodes[toCheck]; exists {
					writer.WriteString(string(decodedChar))
					toCheck = ""
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
