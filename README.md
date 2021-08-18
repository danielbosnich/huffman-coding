# huffman-coding

![Go status](https://github.com/danielbosnich/huffman-coding/actions/workflows/go.yml/badge.svg)
![Python status](https://github.com/danielbosnich/huffman-coding/actions/workflows/python.yml/badge.svg)

For our final project in Data Structures at CU, Pravan Subramanian and I wrote a program in C++ that implements Huffman Coding.

Since then, I've decided to expand on this repo by implementing Huffman Coding in additional programming languages.  First up was Python!  This program includes the character keys at the beginning of the compressed file so that any file compressed by the program can also be uncompressed.

Next up was Go!  I used the same format of including the character keys in the encoded file as I did with the Python implementation.  So the Go program is able to uncompress a file compressed by the Python program and vice versa.

## Python program usage

```
Usage: huffman_coding.py [-h] action file

Positional arguments:
  action      Compress or Uncompress
  file        File to perform action on

Optional arguments:
  -h, --help  show this help message and exit
```
