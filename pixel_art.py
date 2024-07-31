import subprocess, argparse, sys


parser = argparse.ArgumentParser(
	prog='TTYxelArt',
	description='TTYxelArt is a tool to compress text files that\nrepresent pixel art using ANSI escape codes and visualize them.')
subparsers = parser.add_subparsers(dest="subcommand", help="subcommands")

write = subparsers.add_parser("write", aliases=['w'], help='use write mode')
write.add_argument("input", help="input full file name")
write.add_argument("output", nargs='?', default=None, help="output file name")
# write.add_argument("-a", "--alias", dest="alias", help="alias for the pixel art")
# write.add_argument("-f", "--form", dest="form", help="set an alternate form")
# write.add_argument("-d", "--display", dest="display", help="set a display text")

read = subparsers.add_parser("read", aliases=['r'], help='use read mode')
read_group = read.add_mutually_exclusive_group(required=True)
read_group.add_argument("name", nargs='?', help="pixel art to display")
# read_group.add_argument("-r", "--random", dest="random", help="display a random pixel art", action="store_true")


# Display help if no arguments where specified
if len(sys.argv) == 1:
    parser.print_help()
    sys.exit(0)


# Parse arguments to use the binary file
args = parser.parse_args()
if args.subcommand == 'r': # read mode
	command_args = ("./pixel_art", "-r", args.name)
	popen = subprocess.Popen(command_args, stdout=subprocess.PIPE)
	popen.wait()
	output = popen.stdout.read()
	print(output.decode('utf-8'), end='')
else: # write mode
	command_args = ("./pixel_art", "-w", args.input)
	if args.output is not None:
		command_args += (args.output, )
	popen = subprocess.Popen(command_args, stdout=subprocess.PIPE)
	popen.wait()
	output = popen.stdout.read()
	print(output.decode('utf-8'), end='')
