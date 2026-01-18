// Script that implements Huffman Coding assuming UTF-8 encoding
package huffman

import (
	"bufio"
	"errors"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"slices"
	"strconv"
	"strings"
)

const (
	codesDelimeter rune = 128

	delimeterBetweenCharAndCode = "~="
	delimeterBetweenCharacters  = "><"
)

type Node struct {
	count int
	char  *rune
	left  *Node
	right *Node
}

// Compress encodes the passed file
func Compress(inputFile string) error {
	charCounts, err := getCharFrequencies(inputFile)
	if err != nil {
		return fmt.Errorf("failed to calculate character frequencies: %w", err)
	}

	headNode := createHuffmanTree(charCounts)
	huffmanCodes := make(map[rune]string)
	generateCodes(headNode, huffmanCodes, "")

	err = writeCompressedFile(inputFile, huffmanCodes)
	if err != nil {
		return fmt.Errorf("failed to write the compressed file: %w", err)
	}

	return nil
}

func getCharFrequencies(inputFilepath string) (map[rune]int, error) {
	inputFile, err := os.Open(inputFilepath)
	if err != nil {
		return nil, fmt.Errorf("failed to open the input file '%s': %w", inputFilepath, err)
	}
	defer inputFile.Close()

	charCounts := make(map[rune]int)
	reader := bufio.NewReader(inputFile)
	for {
		// TODO: Ensure the size is not greater than 1
		char, _, err := reader.ReadRune()
		if errors.Is(err, io.EOF) {
			break
		}
		if err != nil {
			return nil, fmt.Errorf("failed to read rune: %w", err)
		}
		charCounts[char] += 1
	}
	return charCounts, nil
}

func createHuffmanTree(charCount map[rune]int) Node {
	var nodes []Node
	for char, count := range charCount {
		newNode := Node{
			count: count,
			char:  &char,
		}
		nodes = append(nodes, newNode)
	}

	// Construct the tree by creating nodes for the characters with the lowest frequency first and
	// then working backwards to the nodes with higher frequency (including "constructed" nodes) until
	// the head node is the only node left in the slice
	for len(nodes) > 1 {
		slices.SortStableFunc(nodes, func(a, b Node) int {
			return a.count - b.count
		})

		leftNode := nodes[0]
		rightNode := nodes[1]
		nodes = slices.Delete(nodes, 0, 2)

		parentNode := Node{
			count: leftNode.count + rightNode.count,
			left:  &leftNode,
			right: &rightNode,
		}
		nodes = append(nodes, parentNode)
	}

	return nodes[0]
}

// generateCodes creates the character codes by using a pre-order traversal of the binary tree
func generateCodes(node Node, huffmanCodes map[rune]string, path string) {
	if node.char != nil {
		huffmanCodes[*node.char] = path
	}
	if node.left != nil {
		leftPath := path + "0"
		generateCodes(*node.left, huffmanCodes, leftPath)
	}
	if node.right != nil {
		rightPath := path + "1"
		generateCodes(*node.right, huffmanCodes, rightPath)
	}
}

// writeCompressedFile encodes the passed file and writes it to an output file
func writeCompressedFile(inputFilepath string, huffmanCodes map[rune]string) error {
	inputFile, err := os.Open(inputFilepath)
	if err != nil {
		return fmt.Errorf("failed to open the input file '%s': %w", inputFilepath, err)
	}
	defer inputFile.Close()

	// Determine the output filepath
	dir, file := filepath.Split(inputFilepath)
	fileDetails := strings.Split(file, ".")
	filename := fileDetails[0]
	extension := fileDetails[1]
	compressedFilename := dir + filename + "_compressed." + extension

	outputFile, err := os.Create(compressedFilename)
	if err != nil {
		return fmt.Errorf("failed to create the compressed file '%s': %w", compressedFilename, err)
	}
	defer outputFile.Close()

	writer := bufio.NewWriter(outputFile)
	reader := bufio.NewReader(inputFile)

	var charCount int
	for char, code := range huffmanCodes {
		charCount += 1
		toWrite := string(char) + delimeterBetweenCharAndCode + string(code)
		if charCount < len(huffmanCodes) {
			toWrite += delimeterBetweenCharacters
		}
		writer.WriteString(toWrite)
	}
	writer.WriteString(string(codesDelimeter))

	const encodeBitSize = 7

	var bitBuffer string
	for {
		char, _, err := reader.ReadRune()
		if errors.Is(err, io.EOF) {
			break
		}
		if err != nil {
			return fmt.Errorf("failed to read rune: %w", err)
		}

		bitBuffer += huffmanCodes[char]
		for len(bitBuffer) >= encodeBitSize {
			toEncode := bitBuffer[:encodeBitSize]
			bitBuffer = bitBuffer[encodeBitSize:]

			intValue, err := strconv.ParseUint(toEncode, 2, 8)
			if err != nil {
				return fmt.Errorf("failed to convert binary string to int: %w", err)
			}
			writer.WriteString(string(rune(intValue)))
		}
	}

	if len(bitBuffer) > 0 {
		intValue, err := strconv.ParseUint(bitBuffer, 2, 8)
		if err != nil {
			return fmt.Errorf("failed to convert binary string to int: %w", err)
		}
		writer.WriteString(string(rune(intValue)))
	}

	writer.Flush()
	return nil
}

// Uncompress parses and decodes the passed file
func Uncompress(inputFilepath string) error {
	charCodes, lenCodeBytes, err := readCodes(inputFilepath)
	if err != nil {
		return fmt.Errorf("failed to read character encodings: %w", err)
	}

	inputFile, err := os.Open(inputFilepath)
	if err != nil {
		return fmt.Errorf("failed to open the input file '%s': %w", inputFilepath, err)
	}
	defer inputFile.Close()

	dir, file := filepath.Split(inputFilepath)
	fileDetails := strings.Split(file, ".")
	filename := fileDetails[0]
	extension := fileDetails[1]
	uncompressedFilename := dir + filename + "_uncompressed." + extension

	outputFile, err := os.Create(uncompressedFilename)
	if err != nil {
		return fmt.Errorf("failed to create the uncompressed file '%s': %w", uncompressedFilename, err)
	}
	defer outputFile.Close()

	writer := bufio.NewWriter(outputFile)
	reader := bufio.NewReader(inputFile)

	var bits, toCheck string

	inputFile.Seek(int64(lenCodeBytes), 0)
	fileInfo, err := os.Stat(inputFilepath)
	if err != nil {
		return fmt.Errorf("failed to get stats the input file '%s': %w", inputFilepath, err)
	}

	fileSize := fileInfo.Size()
	numBytesRead := lenCodeBytes
	for {
		char, _, err := reader.ReadRune()
		if errors.Is(err, io.EOF) {
			break
		}
		if err != nil {
			return fmt.Errorf("failed to read rune: %w", err)
		}
		numBytesRead += 1

		// The last character needs to use 'b' as the format spec
		// so that the bit string is not padded with zeros
		if numBytesRead == int(fileSize) {
			bits += fmt.Sprintf("%b", char)
			bits += "0"
		} else {
			bits += fmt.Sprintf("%07b", char)
		}

		for len(bits) > 0 {
			toCheck += string(bits[0])
			bits = bits[1:]

			if decodedChar, ok := charCodes[toCheck]; ok {
				writer.WriteString(string(decodedChar))
				toCheck = ""
			}
		}
	}

	writer.Flush()
	return nil
}

func readCodes(inputFilepath string) (map[string]rune, int, error) {
	inputFile, err := os.Open(inputFilepath)
	if err != nil {
		return nil, 0, errors.New("failed opening the input file")
	}
	defer inputFile.Close()

	charCodes := make(map[string]rune)
	reader := bufio.NewReader(inputFile)

	var numBytesRead int
	var codes string
	for {
		char, size, err := reader.ReadRune()
		if err != nil {
			return nil, 0, fmt.Errorf("failed to read rune: %w", err)
		}
		numBytesRead += size

		if char == codesDelimeter {
			break
		}
		codes += string(char)
	}

	allCodes := strings.Split(codes, "><")
	for i := range len(allCodes) {
		codeDetails := allCodes[i]
		parsedDetails := strings.Split(codeDetails, "~=")
		runes := []rune(parsedDetails[0])
		code := parsedDetails[1]
		charCodes[code] = runes[0]
	}

	return charCodes, numBytesRead, nil
}
