// Unit tests for the Go implementation of Huffman Coding
package huffman

import (
	"bufio"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"testing"
)

const (
	testFilename             = "plrabn12.txt"
	testCompressedFilename   = "plrabn12_compressed.txt"
	testUncompressedFilename = "plrabn12_compressed_uncompressed.txt"
)

// Ensures that the original file matches the uncompressed file
func TestCompareFiles(t *testing.T) {
	cwd, err := os.Getwd()
	if err != nil {
		t.Errorf("Unable to determine the current working directory. err: %v", err)
	}
	testDir := filepath.Join(cwd, "..", "test")

	testFilepath := filepath.Join(testDir, testFilename)
	testCompressedFilepath := filepath.Join(testDir, testCompressedFilename)
	testUncompressedFilepath := filepath.Join(testDir, testUncompressedFilename)
	defer os.Remove(testCompressedFilepath)
	defer os.Remove(testUncompressedFilepath)

	// Compress and uncompress the files first
	Compress(testFilepath)
	Uncompress(testCompressedFilepath)

	// Open both files
	originalFile, err := os.Open(testFilepath)
	if err != nil {
		fmt.Println(err)
		t.Error("There was an error opening the original file!")
		return
	}
	defer originalFile.Close()

	uncompressedFile, err := os.Open(testUncompressedFilepath)
	if err != nil {
		fmt.Println(err)
		t.Error("There was an error opening the uncompressed file!")
	}
	defer uncompressedFile.Close()

	originalReader := bufio.NewReader(originalFile)
	uncompressedReader := bufio.NewReader(uncompressedFile)

	for {
		originalByte, err1 := originalReader.ReadByte()
		uncompressedByte, err2 := uncompressedReader.ReadByte()
		if err1 == io.EOF && err2 == io.EOF {
			break
		} else if err1 == io.EOF && err2 != io.EOF {
			t.Error("The two files are not the same length")
		} else if err1 != io.EOF && err2 == io.EOF {
			t.Error("The two files are not the same length")
		}

		if originalByte != uncompressedByte {
			t.Error("The two files are not equivalent")
			return
		}
	}
}
