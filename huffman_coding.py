"""
Script that implements Huffman Coding assuming UTF-8 encoding
"""

import argparse
from operator import attrgetter


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
        """Initializes a HuffmanCoding object

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
            node: Current Node object in the traversal
            path (list): Binary path to the passed node
        """
        if node.char is not None:
            path_str = ''.join(str(value) for value in path)
            self.char_codes[node.char] = path_str
        if node.left is not None:
            path.append(0)
            self.set_codes(node.left, path)
        if node.right is not None:
            path.append(1)
            self.set_codes(node.right, path)
        if path:
            del path[-1]

    def get_char_frequencies(self):
        """Reads the input file and determines the character frequencies"""
        with open(self.file, 'r') as input_file:
            for char in input_file.read():
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

        # Set the encoding of all nodes
        self.set_codes(parent_node, [])

        self.encode_file()

    def encode_file(self):
        """Encodes the filepath"""
        dot_index = self.file.rfind('.')
        compressed_filename = self.file[:dot_index] + '_compressed' + self.file[dot_index:]
        bit_buffer = ''
        with open(compressed_filename, 'w') as output_file:
            with open(self.file, 'r') as input_file:
                # First, write the codes so the file can be uncompressed
                for char, code in self.char_codes.items():
                    output_file.write(f'{char},{code}; ')

                # Read the file character by character and compress the data
                # 7 bytes at a time
                for char in input_file.read():
                    bit_buffer += self.char_codes[char]

                    if len(bit_buffer) > 7:
                        encoded_char = chr(int(bit_buffer[:7], 2))
                        output_file.write(encoded_char)
                        bit_buffer = bit_buffer[7:]

            # Encode any remaining characters
            if bit_buffer:
                encoded_char = chr(int(bit_buffer, 2))
                output_file.write(encoded_char)


def parse_args():
    """Parses the passed arguments"""
    parser = argparse.ArgumentParser()
    parser.add_argument('action', type=str, help='Compress or Decompress')
    parser.add_argument('file', type=str, help='File to perform action on')
    args = parser.parse_args()
    return args.action.lower(), args.file

def main():
    """Runs the script"""
    action, filepath = parse_args()
    huffman_obj = HuffmanCoding(filepath)
    if action == 'compress':
        huffman_obj.create_huffman_codes()
    elif action == 'decompress':
        print('Trying to decompress')
    else:
        print('Invalid action. Should be Compress or Decompress')

if __name__ == '__main__':
    main()
