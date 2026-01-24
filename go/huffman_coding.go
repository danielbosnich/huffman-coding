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
	endOfCodes            = "\n\n"
	encodingEquals        = "~="
	delimiterBetweenCodes = "><"
)

type Node struct {
	count int
	char  *rune
	left  *Node
	right *Node
}

// Compress encodes the passed file
func Compress(inputFilepath string) error {
	inputFile, err := os.Open(inputFilepath)
	if err != nil {
		return fmt.Errorf("failed to open the input file '%s': %w", inputFilepath, err)
	}
	defer inputFile.Close()

	charCounts, err := getCharFrequencies(inputFile)
	if err != nil {
		return fmt.Errorf("failed to calculate character frequencies: %w", err)
	}

	_, err = inputFile.Seek(0, io.SeekStart)
	if err != nil {
		return fmt.Errorf("failed to seek back to the start of the input file: %w", err)
	}

	headNode := createHuffmanTree(charCounts)
	huffmanCodes := make(map[rune]string)
	generateCodes(headNode, huffmanCodes, "")

	compressedFilepath := filepathOfCompressedFile(inputFilepath)
	err = writeCompressedFile(inputFile, compressedFilepath, huffmanCodes)
	if err != nil {
		return fmt.Errorf("failed to write the compressed file '%s': %w", compressedFilepath, err)
	}

	return nil
}

func getCharFrequencies(inputFile *os.File) (map[rune]int, error) {
	charCounts := make(map[rune]int)
	reader := bufio.NewReader(inputFile)
	for {
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
		nodes = nodes[2:]

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

func writeCompressedFile(inputFile *os.File, outputFilepath string, huffmanCodes map[rune]string) error {
	outputFile, err := os.Create(outputFilepath)
	if err != nil {
		return fmt.Errorf("failed to create the compressed file '%s': %w", outputFilepath, err)
	}
	defer outputFile.Close()

	writer := bufio.NewWriter(outputFile)
	reader := bufio.NewReader(inputFile)

	var charCount int
	for char, code := range huffmanCodes {
		charCount += 1
		charEncoding := string(char) + encodingEquals + code
		if charCount < len(huffmanCodes) {
			charEncoding += delimiterBetweenCodes
		}

		_, err = writer.WriteString(charEncoding)
		if err != nil {
			return fmt.Errorf("failed to write character encoding to output file: %w", err)
		}
	}

	_, err = writer.WriteString(endOfCodes)
	if err != nil {
		return fmt.Errorf("failed to write code delimiter to output file: %w", err)
	}

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

			_, err = writer.WriteString(string(rune(intValue)))
			if err != nil {
				return fmt.Errorf("failed to write character to output file: %w", err)
			}
		}
	}

	if len(bitBuffer) > 0 {
		intValue, err := strconv.ParseUint(bitBuffer, 2, 8)
		if err != nil {
			return fmt.Errorf("failed to convert binary string to int: %w", err)
		}

		_, err = writer.WriteString(string(rune(intValue)))
		if err != nil {
			return fmt.Errorf("failed to write character to output file: %w", err)
		}
	}

	err = writer.Flush()
	if err != nil {
		return fmt.Errorf("failed to flush output file: %w", err)
	}
	return nil
}

// Uncompress parses and decodes the passed file
func Uncompress(inputFilepath string) error {
	inputFile, err := os.Open(inputFilepath)
	if err != nil {
		return fmt.Errorf("failed to open the input file '%s': %w", inputFilepath, err)
	}
	defer inputFile.Close()

	numBytesRead, charCodes, err := readCodes(inputFile)
	if err != nil {
		return fmt.Errorf("failed to read character encodings: %w", err)
	}

	// This Seek is usually necessary since the default Reader buffer size can be larger
	// than the length of the character encodings written at the beginning of the file
	_, err = inputFile.Seek(int64(numBytesRead), io.SeekStart)
	if err != nil {
		return fmt.Errorf("failed to seek to the end of the codes: %w", err)
	}

	uncompressedFilepath := filepathOfUncompressedFile(inputFilepath)
	err = writeUncompressedFile(inputFile, uncompressedFilepath, charCodes, numBytesRead)
	if err != nil {
		return fmt.Errorf("failed to write the uncompressed file: '%s': %w", uncompressedFilepath, err)
	}

	return nil
}

func writeUncompressedFile(inputFile *os.File, uncompressedFilepath string, charCodes map[string]rune, bytesAlreadyRead int) error {
	fileInfo, err := inputFile.Stat()
	if err != nil {
		return fmt.Errorf("failed to get info for the input file: %w", err)
	}
	fileSize := int(fileInfo.Size())

	outputFile, err := os.Create(uncompressedFilepath)
	if err != nil {
		return fmt.Errorf("failed to create the uncompressed file '%s': %w", uncompressedFilepath, err)
	}
	defer outputFile.Close()

	writer := bufio.NewWriter(outputFile)
	reader := bufio.NewReader(inputFile)

	numBytesRead := bytesAlreadyRead
	var bits, toCheck string
	for {
		char, size, err := reader.ReadRune()
		if errors.Is(err, io.EOF) {
			break
		}
		if err != nil {
			return fmt.Errorf("failed to read rune: %w", err)
		}
		numBytesRead += size

		// The last character needs to use 'b' as the format spec
		// so that the bit string is not padded with zeros
		if numBytesRead == fileSize {
			bits += fmt.Sprintf("%b", char)
		} else {
			bits += fmt.Sprintf("%07b", char)
		}

		for len(bits) > 0 {
			toCheck += string(bits[0])
			bits = bits[1:]

			if decodedChar, ok := charCodes[toCheck]; ok {
				_, err = writer.WriteString(string(decodedChar))
				if err != nil {
					return fmt.Errorf("failed to write character to output file: %w", err)
				}
				toCheck = ""
			}
		}
	}

	err = writer.Flush()
	if err != nil {
		return fmt.Errorf("failed to flush output file: %w", err)
	}
	return nil
}

func readCodes(inputFile *os.File) (int, map[string]rune, error) {
	reader := bufio.NewReader(inputFile)

	var numBytesRead int
	var codes string
	for {
		char, size, err := reader.ReadRune()
		if err != nil {
			return 0, nil, fmt.Errorf("failed to read rune: %w", err)
		}
		numBytesRead += size
		codes += string(char)

		if len(codes) >= 2 && codes[len(codes)-2:] == endOfCodes {
			codes = codes[:len(codes)-2]
			break
		}
	}

	huffmanCodes := make(map[string]rune)

	allCodes := strings.SplitSeq(codes, delimiterBetweenCodes)
	for codeInfo := range allCodes {
		charAndCode := strings.Split(codeInfo, encodingEquals)
		char := charAndCode[0]
		code := charAndCode[1]
		huffmanCodes[code] = rune(char[0])
	}

	return numBytesRead, huffmanCodes, nil
}

func filepathOfCompressedFile(inputFilepath string) string {
	dir, file := filepath.Split(inputFilepath)
	fileDetails := strings.Split(file, ".")
	filename := fileDetails[0]
	extension := fileDetails[1]

	return dir + filename + "_compressed." + extension
}

func filepathOfUncompressedFile(inputFilepath string) string {
	dir, file := filepath.Split(inputFilepath)
	fileDetails := strings.Split(file, ".")
	filename := fileDetails[0]
	extension := fileDetails[1]

	return dir + filename + "_uncompressed." + extension
}
