#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
using namespace std;

const string TXA_DIR = "/usr/local/opt/ttyxelart";
const string DIRECTORY = TXA_DIR + "/sprites/";
const string BREAK = "00", COLOR = "01", INDEX = "10", SPACES = "11";
const int RESOLUTION = 8; // 256x256 px

class BitBuffer {
private:
    vector<u_int8_t> buffer;
    u_int8_t currentByte;
    int bitCount;

    void write_byte_to_file(ofstream& outFile, u_int8_t byte) {
        outFile.put(static_cast<char>(byte));
    }

public:
    BitBuffer() : currentByte(0), bitCount(0) {}

    // Write a string of bits to the buffer
    void write_bits(const string& bits) {
        for (char bit : bits) {
            currentByte = (currentByte << 1) | (bit - '0');
            bitCount++;

            if (bitCount == 8) {
                buffer.push_back(currentByte);
                currentByte = 0;
                bitCount = 0;
            }
        }
    }

    // Flush the buffer to a file, padding the last byte if necessary
    void flush_to_file(const string& fileName) {
        ofstream outFile(fileName, ios::binary);
        if (!outFile.is_open()) {
            throw ios_base::failure("Failed to open file for writing");
        }

        // Write all full bytes to the file
        for (u_int8_t byte : buffer)
            write_byte_to_file(outFile, byte);

        // Handle any remaining bits
        if (bitCount > 0) {
            currentByte <<= (8 - bitCount);
            currentByte |= (1 << (7 - bitCount));
            write_byte_to_file(outFile, currentByte);
        }

        outFile.close();
    }
};


template <typename T>
string to_binary_string(T number, int length=RESOLUTION) {
    if (number == 0)
        return "0";

    string binary = "";
    while (number > 0) {
        binary = (number % 2 == 0 ? "0" : "1") + binary;
        number /= 2;
	}

	binary = string(length - binary.size(), '0') + binary;
    return binary;
}


void return_to_start(ifstream &file) {
	file.clear();
	file.seekg(0);
}


uint concatenate_digits(const vector<char> &digits) {
    uint number = 0;
    for(char digit : digits)
        number = number * 10 + (digit - '0');
    return number;
}


string read_spaces(char *&ptr) {
	uint spaces = 0;
	while(*ptr != '\033') {
		spaces++;
		ptr += 2;
	}
	return to_binary_string(spaces);
}


pair<string, string> read_color(char *&ptr) {
	vector<uint> colors;

	ptr += 7; // Move to red color
	// One iteration per color
	for(int i = 0; i < 3; i++) {
		vector<char> digits;
		while(*ptr != ';' && *ptr != 'm') {
			digits.push_back(*ptr);
			ptr++;
		}
		colors.push_back(concatenate_digits(digits));
		ptr++;
	}
	ptr += 6; // Move to next pixel

	// Count repetitions for RLE
	uint repetitions = 1;

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
			uint number = concatenate_digits(digits);
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

	// RLE-RGB bytes
	// idk if this will be affected by endianness
	uint rgb = (colors[0] << 16) + (colors[1] << 8) + colors[2];
	return {to_binary_string(repetitions), to_binary_string(rgb, 24)};
}


void write_pixel_art(string file, string output_name) {
	ifstream input(file);
	if(input.fail()) {
        cout << "Error: File " << file << " does not exist.\n\n";
		return;
	}
	BitBuffer bitBuffer;

	// Sprite
	string line;
	while(getline(input, line)) {
		char *ptr = (char *) line.c_str();
		while(*ptr) {
			if(*ptr == '\033') {
				bitBuffer.write_bits(COLOR);
				pair<string, string> color_pair = read_color(ptr);
				string color_amount = color_pair.first, color = color_pair.second;
				bitBuffer.write_bits(color_amount);
				bitBuffer.write_bits(color);
			} else if(*ptr == ' ') {
				bitBuffer.write_bits(SPACES);
				string space_amount = read_spaces(ptr);
				bitBuffer.write_bits(space_amount);
			}
		}
		bitBuffer.write_bits(BREAK);
	}

	bitBuffer.flush_to_file(output_name);
	input.close();
}


bool nthBit(unsigned char c, int n) {
    static unsigned char mask[] = {128, 64, 32, 16, 8, 4, 2, 1};
    return ((c & mask[n]) != 0);
}


int getRLE(unsigned char c) {
	return (c & 0x7f);
}


void read_pixel_art(string file_name) {
	ifstream input(DIRECTORY + file_name);
	if(input.fail()) {
        cout << "Error: File " << file_name << " does not exist.\n\n";
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
	if(string(argv[1]) == "-w")
		write_pixel_art(argv[2], argv[3]);
	else if(string(argv[1]) == "-r")
		read_pixel_art(argv[2]);
	return 0;
}
