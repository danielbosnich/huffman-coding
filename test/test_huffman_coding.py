"""
Unit tests for the Python implementation of Huffman Coding
"""

import os
from pathlib import Path
from unittest import TestCase

from python.huffman_coding import HuffmanCoding

# Test constants
TEST_FILENAME = 'plrabn12.txt'
COMPRESSED_FILENAME = 'plrabn12_compressed.txt'
UNCOMPRESSED_FILENAME = 'plrabn12_compressed_uncompressed.txt'


class HuffmanCodingTests(TestCase):
    """Tests compressing example text file with the Python Huffman Coding
    implementation and then uncompressing the compressed file"""

    @classmethod
    def setUpClass(cls):
        """Determines the test directory"""
        test_file_dir = Path.cwd().joinpath('test')
        print('Test file dir:', test_file_dir)
        cls.test_filepath = str(
            test_file_dir.joinpath(TEST_FILENAME)
        )
        cls.compressed_filepath = str(
            test_file_dir.joinpath(COMPRESSED_FILENAME)
        )
        cls.uncompressed_filepath = str(
            test_file_dir.joinpath(UNCOMPRESSED_FILENAME)
        )

    @classmethod
    def tearDownClass(cls):
        """Deletes the new files that were created"""
        os.remove(cls.compressed_filepath)
        os.remove(cls.uncompressed_filepath)

    def test_1_compress_file(self):
        """Tests compressing a file using Huffman Coding"""
        huffman_obj = HuffmanCoding(self.test_filepath)
        huffman_obj.encode_file()
    
    def test_2_uncompress_file(self):
        """Tests uncompressing the compressed file"""
        huffman_obj = HuffmanCoding(self.compressed_filepath)
        huffman_obj.decode_file()

    def test_3_compare_files(self):
        """Ensures that the uncompressed file matches the original file"""
        with open(self.test_filepath) as original_file:
            with open(self.uncompressed_filepath) as uncompressed_file:
                for original_line in original_file:
                    uncompressed_line = uncompressed_file.readline()
                    self.assertEqual(original_line, uncompressed_line)
            
                # Lastly, try reading another line and ensure it is an empty string
                next_line = uncompressed_file.readline()
                self.assertEqual('', next_line)
