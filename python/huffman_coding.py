"""
Script that implements Huffman Coding assuming UTF-8 encoding
"""

import argparse
from operator import attrgetter
import os


class Node():
    """Class that represents a node in the tree"""
    def __init__(self, *, count, char=None):
        """Initializes a Node object

        Args:
            count (int): Character count
            char (str): Character of the node. Defaults to None because this
                class also represents nodes without characters
        """
        self.count = count
        self.char = char
        self.left = None
        self.right = None


class HuffmanCoding():
    """Class that implements Huffman Coding"""
    def __init__(self, filepath):
        """Initializes a Huffman Coding object

        Args:
            filepath (str): File to either encode or decode
        """
        self.file = filepath
        self.parent = None
        self.char_count = {}
        self.char_codes = {}

    def set_codes(self, node, path):
        """Pre-order traversal of the binary tree to determine the code for each node

        Args:
            node (Node): Current Node object in the traversal
            path (str): Binary path to the passed node
        Returns:
            str: Binary path to the parent node
        """
        if node.char is not None:
            self.char_codes[node.char] = path
        if node.left is not None:
            path += '0'
            path = self.set_codes(node.left, path)
        if node.right is not None:
            path += '1'
            path = self.set_codes(node.right, path)

        return path[:-1]

    def get_char_frequencies(self):
        """Reads the input file and determines the character frequencies"""
        with open(self.file, encoding='utf-8') as input_file:
            while True:
                if not (char := input_file.read(1)):
                    break

                if char in self.char_count:
                    self.char_count[char] += 1
                else:
                    self.char_count[char] = 1

    def create_huffman_codes(self):
        """Creates the huffman codes"""
        self.get_char_frequencies()
        frequencies = []
        for char, count in self.char_count.items():
            frequencies.append(Node(count=count, char=char))

        # Create the tree
        while len(frequencies) >= 2:
            frequencies.sort(key=attrgetter('count'))
            first_item = frequencies.pop(0)
            second_item = frequencies.pop(0)

            parent_count = first_item.count + second_item.count
            parent_node = Node(count=parent_count)
            parent_node.left = first_item
            parent_node.right = second_item
            frequencies.append(parent_node)

        self.parent = parent_node

        # Set the codes for all nodes
        self.set_codes(parent_node, '')

    def encode_file(self):
        """Compresses the file"""
        self.create_huffman_codes()
        dot_index = self.file.rfind('.')
        compressed_filename = self.file[:dot_index] + '_compressed' + self.file[dot_index:]
        bit_buffer = ''
        with open(compressed_filename, 'w', encoding='utf-8', newline='') as output_file:
            with open(self.file, encoding='utf-8') as input_file:
                # First, write the codes so the file can be uncompressed
                for char, code in self.char_codes.items():
                    output_file.write(f'{char}~={code}><')
                output_file.write('\n$\n')

                # Read the characters and encode 7 bits at a time
                while True:
                    if not (char := input_file.read(1)):
                        break

                    bit_buffer += self.char_codes[char]
                    while len(bit_buffer) > 7:
                        encoded_char = chr(int(bit_buffer[:7], 2))
                        output_file.write(encoded_char)
                        bit_buffer = bit_buffer[7:]

            # Encode any remaining bits
            if bit_buffer:
                encoded_char = chr(int(bit_buffer, 2))
                output_file.write(encoded_char)

    def decode_file(self):
        """Uncompresses the compressed file"""
        input_file_size = os.stat(self.file).st_size
        dot_index = self.file.rfind('.')
        uncompressed_file = (self.file[:dot_index]
                             + '_uncompressed'
                             + self.file[dot_index:])
        with open(self.file, encoding='utf-8', newline='') as input_file:
            with open(uncompressed_file, 'w', encoding='utf-8') as output_file:
                offset = 0
                # Read the codes and save them to the instance dictionary
                codes = ''
                for line in input_file:
                    offset += len(line)
                    if line == '$\n':
                        break
                    codes += line

                for value in codes.split('><')[:-1]:
                    char, code = value.split('~=')
                    self.char_codes[code] = char

                # Then read and decode the rest of the file
                bits = ''
                to_check = ''
                while True:
                    if not (char := input_file.read(1)):
                        break

                    # The last character needs to use 'b' as the format spec
                    # so that the bit string is not padded with zeros
                    offset += 1
                    if offset == input_file_size:
                        bits += format(ord(char), 'b')
                    else:
                        bits += format(ord(char), '07b')

                    while bits:
                        to_check += bits[0]
                        bits = bits[1:]
                        try:
                            char = self.char_codes[to_check]
                        except KeyError:
                            pass
                        else:
                            output_file.write(char)
                            to_check = ''


def parse_args():
    """Parses the passed arguments"""
    parser = argparse.ArgumentParser()
    parser.add_argument('action', type=str, help='Compress or Uncompress')
    parser.add_argument('file', type=str, help='File to perform action on')
    args = parser.parse_args()
    return args.action.lower(), args.file

def main():
    """Runs the script"""
    action, filepath = parse_args()
    huffman_obj = HuffmanCoding(filepath)
    if action == 'compress':
        huffman_obj.encode_file()
    elif action == 'uncompress':
        huffman_obj.decode_file()
    else:
        print('Invalid action. Should be Compress or Uncompress')

if __name__ == '__main__':
    main()
