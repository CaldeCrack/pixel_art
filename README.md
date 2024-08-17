# TTYxelArt 

Tool to compress text files that represent pixel art using ANSI escape codes and visualize them.

## Installation

### Debian based distros

Use the following commands:

```sh
git clone https://github.com/CaldeCrack/tty-pixel-art.git
cd tty-pixel-art
sudo ./install.sh
```

Make sure it is installed by executing:

```sh
txa
```

This should display the program's help page.

Support for other platforms in the future (presumably).

## Usage

- Main program:

```sh
> txa -h
usage: TTYxelArt [-h] {write,w,read,r} ...

TTYxelArt is a tool to compress text files that represent pixel art using ANSI escape codes and visualize them.

positional arguments:
	{write,w,read,r}  subcommands
	write (w)       use write mode
	read (r)        use read mode

options:
	-h, --help        show this help message and exit
```

- Write mode:
```sh
> txa w -h
usage: TTYxelArt write [-h] [-a ALIAS [ALIAS ...]] [-d DESC] [-f FORM FORM] input [output]

positional arguments:
	input                 input full file name
	output                output file name

options:
	-h, --help            show this help message and exit
	-a ALIAS [ALIAS ...], --alias ALIAS [ALIAS ...]
						alias(es) for the pixel art
	-d DESC, --desc DESC  set a description text
	-f FORM FORM, --form FORM FORM
						set an alternate form using a name as indicator of the sprite
```

- Read mode:
```sh
> txa r -h
usage: TTYxelArt read [-h] [-f FORM] [-r] [-l] [name]

positional arguments:
	name                  any alias of the sprite to display

options:
	-h, --help            show this help message and exit
	-f FORM, --form FORM  display an alternative form of the sprite
	-r, --random          display a random sprite
	-l, --list            show a list of available pixel arts
```

### Example

Using the [001.txt](https://github.com/shinya/pokemon-terminal-art/blob/main/fullcolor/bw/001.txt) file that comes with the repo it will display a **Bulbasaur** in the terminal.

```
txa w 001.txt bulbasaur -a Bulbasaur BULBASAUR -d "🌿 The first pokemon of the pokedex 🌿"
txa r bulbasaur
```

## About the project

I know almost every image file format will have a smaller size to store the same pixel art but this project aims to compress text files that represent pixel art using [ANSI escape codes](https://en.wikipedia.org/wiki/ANSI_escape_code).

This could be useful for people who don't use a terminal that supports image rendering or simply prefers the concept of displaying pixel art using pure text.

The compression is achieved by creating a custom binary file with the data being reduced using [RLE](https://en.wikipedia.org/wiki/Run-length_encoding) (run-length encoding) and skipping characters that are not essential info. If you have any suggestion in how to compress the data even further feel free to do it (aside from converting the binary file into [zip](https://en.wikipedia.org/wiki/ZIP_(file_format)) like formats).

## TO-DOs

- [x] Reinstall script.
- [x] Case insensitive search.
- [x] Search by id.
- [x] Check sprite does not already exist when writing a new one.
- [ ] Add option to delete sprites (completely or a form different from the regular one).
- [ ] Give random and list flags more options.
- [ ] Add flag to group sprites (E.g. `<...> -g Pokemon` to set the sprite into the Pokemon group).
- [ ] Transform PNGs and/or other image formats to my compressed format.
- [ ] Add folders with already compatible files for this compression method.
- [ ] Upgrade this README.
- [ ] Give credits and mentions to everything related to this project.
