package huffman

import (
	"bufio"
	"errors"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"testing"

	"github.com/stretchr/testify/require"
)

const (
	testFilename             = "plrabn12.txt"
	testCompressedFilename   = "plrabn12_compressed.txt"
	testUncompressedFilename = "plrabn12_compressed_uncompressed.txt"
)

func TestHuffmanCoding(t *testing.T) {
	cwd, err := os.Getwd()
	require.NoError(t, err, "Failed to get the working directory")
	testDir := filepath.Join(cwd, "..", "test")

	testFilepath := filepath.Join(testDir, testFilename)
	testCompressedFilepath := filepath.Join(testDir, testCompressedFilename)
	testUncompressedFilepath := filepath.Join(testDir, testUncompressedFilename)
	defer os.Remove(testCompressedFilepath)
	defer os.Remove(testUncompressedFilepath)

	t.Run("Compress", func(t *testing.T) {
		err = Compress(testFilepath)
		require.NoError(t, err, "Failed to compress the test file")
	})

	t.Run("Uncompress", func(t *testing.T) {
		err = Uncompress(testCompressedFilepath)
		require.NoError(t, err, "Failed to uncompress the compressed file")
	})

	originalFile, err := os.Open(testFilepath)
	require.NoError(t, err, "Failed to open the test file")
	defer originalFile.Close()

	uncompressedFile, err := os.Open(testUncompressedFilepath)
	require.NoError(t, err, "Failed to open the uncompressed file")
	defer uncompressedFile.Close()

	originalReader := bufio.NewReader(originalFile)
	uncompressedReader := bufio.NewReader(uncompressedFile)

	t.Run("CompareFiles", func(t *testing.T) {
		var bytesRead int
		for {
			originalByte, err1 := originalReader.ReadByte()
			uncompressedByte, err2 := uncompressedReader.ReadByte()
			if errors.Is(err1, io.EOF) && errors.Is(err2, io.EOF) {
				break
			}
			bytesRead += 1

			require.NotErrorIs(t, err1, io.EOF, "Test file is shorter than the uncompressed file")
			require.NotErrorIs(t, err2, io.EOF, "Uncompressed file is shorter than the test file")
			require.NoError(t, err1, "Failed to read byte from the test file")
			require.NoError(t, err2, "Failed to read byte from the uncompressed file")
			require.Equal(t, originalByte, uncompressedByte, fmt.Sprintf("Byte %d is different between the two files", bytesRead))
		}
	})
}
