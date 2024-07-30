#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
using namespace std;


// Write numeric value (maybe other types as well) to file
template<typename T>
void write_num(ofstream &file, const T value) {
	file.write(reinterpret_cast<const char *>(&value), sizeof(value));
}


void return_to_start(ifstream &file) {
	file.clear();
	file.seekg(0);
}


u_int8_t concatenateDigits(const vector<char> &digits) {
    u_int8_t number = 0;
    for(char digit : digits)
        number = number * 10 + (digit - '0');
    return number;
}


u_int8_t read_transparency(char *&ptr) {
	u_int8_t spaces = 0;
	while(*ptr != '\033') {
		spaces++;
		ptr += 2;
	}
	return spaces;
}


u_int32_t read_color(char *&ptr) {
	vector<u_int8_t> colors;

	ptr += 7; // Move to red color
	// One iteration per color
	for(int i = 0; i < 3; i++) {
		vector<char> digits;
		while(*ptr != ';' && *ptr != 'm') {
			digits.push_back(*ptr);
			ptr++;
		}
		colors.push_back(concatenateDigits(digits));
		ptr++;
	}
	ptr += 6; // Move to next pixel

	// Count repetitions for RLE
	u_int8_t alpha_mask = 0x80;
	u_int8_t repetitions = 1;

	char *pixel_ptr = ptr;
	while(*pixel_ptr == '\033') {
		char *color_ptr = pixel_ptr;
		color_ptr += 7;

		for(int i = 0; i < 3; i++) {
			vector<char> digits;
			while(*color_ptr != ';' && *color_ptr != 'm') {
				digits.push_back(*color_ptr);
				color_ptr++;
			}
			u_int8_t number = concatenateDigits(digits);
			if(colors[i] != number)
				goto end_while;
			color_ptr++;
		}

		repetitions++;
		color_ptr += 6;
		pixel_ptr = color_ptr;
	}
	end_while:
	ptr = pixel_ptr;

	// a-RLE-RGB bytes
	// idk if this will be affected by endianness
	u_int32_t rgba = (colors[2] << 24) + (colors[1] << 16) + (colors[0] << 8) + (alpha_mask + repetitions);
	return rgba;
}


void write_pixel_art(string filename) {
	ifstream input(filename);
	if(input.fail()) {
        cout << "Error: File does not exist\n\n";
		return;
	}
	ofstream output("output.bin");

	// Sprite
	const u_int8_t end_of_line = 0x00;
	string line;
	while(getline(input, line)) {
		char *ptr = (char *) line.c_str();
		while(*ptr) {
			if(*ptr == '\033') {
				u_int32_t color = read_color(ptr);
				write_num(output, color);
			} else if(*ptr == ' ') {
				u_int8_t transparency = read_transparency(ptr);
				write_num(output, transparency);
			}
		}
		write_num(output, end_of_line);
	}

	input.close();
	output.close();
}


bool nthBit(unsigned char c, int n) {
    static unsigned char mask[] = {128, 64, 32, 16, 8, 4, 2, 1};
    return ((c & mask[n]) != 0);
}


int getRLE(unsigned char c) {
	return (c & 0x7f);
}


void read_pixel_art(string filename) {
	ifstream input(filename);
	if(input.fail()) {
        cout << "Error: File does not exist\n\n";
		return;
	}
	char buffer;

	// Read file
	vector<unsigned int> colors(3);
	while(!input.eof()) {
		input.read(&buffer, 1);
		unsigned char buff = buffer;
		int rle = getRLE(buff);

		if(buff == 0x00) { // endline
			cout << '\n';
		} else if(nthBit(buff, 0)) { // color
			vector<char> rgb(3);
			input.read(rgb.data(), 3);
			for(int i = 0; i < 3; i++) {
				unsigned char c = rgb[i];
				colors[i] = c;
			}

			cout << "\033[48;2";
			for(int i = 0; i < 3; i++)
				cout << ";" << colors[i];
			cout << "m";
			for(int i = 0; i < rle; i++)
				cout << "  ";
			cout << "\033[0m";
		} else { // transparent background
			for(int i = 0; i < rle; i++)
				cout << "  ";
		}
	}

	input.close();
}


int main(int argc, char *argv[]) {
	if(argc != 3) {
		cout << "Usage: ./pixel_art [flag] [file]\n\n";
	} else if(string(argv[1]) == "-w") {
		write_pixel_art(argv[2]);
	} else if(string(argv[1]) == "-r") {
		read_pixel_art(argv[2]);
	}
	return 0;
}
