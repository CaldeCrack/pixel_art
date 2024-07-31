# TTYxelArt 

Tool to compress text files that represent pixel art using ANSI escape codes and visualize them.

## Usage

Under development but for now use the following commands:

```sh
g++ pixel_art.cpp -o pixel_art -O3
./pixel_art -w 001.txt
./pixel_art -r output.bin
```

This should display a [Bulbasaur](https://github.com/shinya/pokemon-terminal-art/blob/main/fullcolor/bw/001.txt) in the terminal.

## About the project

I know almost every image file format will have a smaller size to store the same pixel art but this project aims to compress text files that represent pixel art using [ANSI escape codes](https://en.wikipedia.org/wiki/ANSI_escape_code).

This could be useful for people who don't use a terminal that supports image rendering or simply prefers the concept of displaying pixel art using pure text.

The compression is achieved by creating a custom binary file with the data being reduced using [RLE](https://en.wikipedia.org/wiki/Run-length_encoding) (run-length encoding) and skipping characters that are not essential info. If you have any suggestion in how to compress the data even further feel welcome to do it (aside from converting the binary file into [zip](https://en.wikipedia.org/wiki/ZIP_(file_format)) like formats).

I want a better name for the repo but I can't think of one right now.
