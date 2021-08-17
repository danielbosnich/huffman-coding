// Unit tests for the Go implementation of Huffman Coding
package huffman

import (
	"bufio"
	"io"
	"os"
	"path/filepath"
	"runtime"
	"strings"
	"testing"
)

var testFilename = "plrabn12.txt"
var testCompressedFilename = "plrabn12_compressed.txt"
var testUncompressedFilename = "plrabn12_compressed_uncompressed.txt"

// Ensures that the original file matches the uncompressed file
func TestCompareFiles(t *testing.T) {
	// Determine operating system before setting the file paths
	remove := ""
	replace := ""
	if runtime.GOOS == "windows" {
		remove = "\\go"
		replace = "\\test"
	} else {
		remove = "/go"
		replace = "/test"
	}

	_, currentFilePath, _, _ := runtime.Caller(0)
	cwd := filepath.Dir(currentFilePath)
	testDir := strings.ReplaceAll(cwd, remove, replace)

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
		t.Error("There was an error opening the original file!")
		return
	}
	defer originalFile.Close()

	uncompressedFile, err := os.Open(testUncompressedFilepath)
	if err != nil {
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
