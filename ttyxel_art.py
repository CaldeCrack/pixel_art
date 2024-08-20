#!/usr/bin/env python3

import argparse, sys, subprocess, json, random, os

parser = argparse.ArgumentParser(
	prog='TTYxelArt',
	description='TTYxelArt is a tool to compress text files that\nrepresent pixel art using ANSI escape codes and visualize them.')
subparsers = parser.add_subparsers(dest="subcommand", help="subcommands")

write = subparsers.add_parser("write", aliases=['w'], help='use write mode')
write.add_argument("input", help="input full file name")
write.add_argument("output", nargs='?', help="output file name")
write.add_argument("-a", "--alias", dest="alias", nargs='+', default=[], help="alias(es) for the pixel art")
write.add_argument("-d", "--desc", dest="desc", help="set a description text")
write.add_argument("-f", "--form", nargs=2, dest="form", help="set an alternate form using a name as indicator of the sprite")

read = subparsers.add_parser("read", aliases=['r'], help='use read mode')
read.add_argument("name", nargs='?', help="id or any alias of the sprite")
read.add_argument("-f", "--form", dest="form", default="regular", help="display an alternative form of the sprite")
read.add_argument("-r", "--random", dest="random", help="display a random sprite", action="store_true")
read.add_argument("-l", "--list", dest="list", help="show a list of available pixel arts", action="store_true")


# Display help if no arguments where specified
if len(sys.argv) == 1:
    parser.print_help()
    sys.exit(0)


# File locations
SCRIPT = os.path.realpath(__file__)
TXA_DIR = os.path.dirname(SCRIPT)


# Add data to JSON file in write mode
def args_to_JSON_file(args : argparse.Namespace) -> str | None:
	_, input_, output, aliases, desc, form = vars(args).values()
	alias = [elem.casefold() for elem in aliases]
	alias = list(dict.fromkeys(alias)) # Remove duplicates
	input_ = input_.split('.')[0]
	file_data : list = []
	name : str = output if output is not None else input_

	if name.isdigit():
		print("Error: pixel art name cannot be an integer number.")
		sys.exit(0)

	with open(f"{TXA_DIR}/sprites.json", 'r', encoding="utf8") as f:
		file_data = json.load(f)

	with open(f"{TXA_DIR}/sprites.json", 'w', encoding="utf8") as f:
		orig_name : str = name
		name = name.casefold()

		if form is not None: # Add form to the corresponding sprite
			form_name : str = form[0].casefold()
			sprite_name : str = form[1].casefold()
			file_name : str = f"{name}_{form_name}.bin"
			if os.path.isfile(f"{TXA_DIR}/sprites/{file_name}"):
				json.dump(file_data, f, indent=4)
				return None

			for sprite in file_data:
				if sprite_name in sprite.get("alias(es)"):
					sprite["forms"][form_name] = {"file": file_name, "desc": desc}
					break
			json.dump(file_data, f, indent=4)
			return file_name
		else: # Add sprite to the JSON
			file_name : str = f"{name}.bin"
			if os.path.isfile(f"{TXA_DIR}/sprites/{file_name}"):
				json.dump(file_data, f, indent=4)
				return None

			if name not in alias:
				alias.append(name)

			sprite_info = {
				"id": len(file_data),
				"name": orig_name,
				"alias(es)": alias,
				"forms": {
					"regular": {
						"file": file_name,
						"desc": desc
					}
				}
			}
			file_data.append(sprite_info)
			json.dump(file_data, f, indent=4)
			return file_name


def search_file_by_id(file_data, name : str, form : str) -> str | None:
	if not (int(name) < len(file_data)):
		print(f"Error: id out of bounds [0, {len(file_data) - 1}]")
		sys.exit(0)
	try:
		desc : str = file_data[int(name)]["forms"].get(form)["desc"]
		print(f"- {file_data[int(name)]["name"]}\n{desc + '\n' if desc is not None else ''}", end='')
		return file_data[int(name)]["forms"].get(form)["file"]
	except TypeError as e:
		print(f"Error: Form doesn't exist for {file_data[int(name)]["name"]}\nAvailable forms:")
		for sprite_form in file_data[int(name)]["forms"].keys():
			print(f"- {sprite_form}")
		return None
	except Exception as e:
		print(f"Unmanaged error: {e}")
		return None


def search_file_by_alias(file_data, name : str, form : str) -> str | None:
	for sprite in file_data:
		if name in sprite.get("alias(es)"):
			try:
				desc : str = sprite["forms"].get(form)["desc"]
				print(f"- {sprite["name"]}\n{desc + '\n' if desc is not None else ''}", end='')
				return sprite["forms"].get(form)["file"]
			except TypeError as e:
				print(f"Error: Form doesn't exist for {sprite["name"]}\nAvailable forms:")
				for sprite_form in sprite["forms"].keys():
					print(f"- {sprite_form}")
				return None
			except Exception as e:
				print(f"Unmanaged error: {e}")
				return None


def search_file(file_data, name : str, form : str) -> str | None:
	if name.isdigit():
		return search_file_by_id(file_data, name, form)
	else:
		return search_file_by_alias(file_data, name, form)


# Search binary file using JSON info
def args_to_binary_file(args : argparse.Namespace) -> str | None:
	_, name, form, show_random, show_list = vars(args).values()
	name = name.casefold() if name is not None else name

	file_data : dict = {}
	with open(f"{TXA_DIR}/sprites.json", 'r', encoding="utf8") as f:
		file_data = json.load(f)

	if show_random:
		sprite : dict = random.choice(file_data)
		random_form : str = random.choice(list(sprite.get("forms").keys()))
		desc : str = sprite["forms"].get(random_form)["desc"]
		print(f"- {sprite["name"]}\n{desc + '\n' if desc is not None else ''}", end='')
		return sprite["forms"].get(random_form)["file"]

	if show_list:
		for sprite in file_data:
			print(f"{sprite["id"]} - {sprite["name"]}")
		sys.exit(0)

	return search_file(file_data, name, form)


# Parse arguments to use the binary file
args : argparse.Namespace = parser.parse_args()
if args.subcommand == 'r': # Read mode
	file_name : str | None = args_to_binary_file(args)
	if file_name is None:
		print("Error: Sprite not found.")
		sys.exit(0)

	command_args = (f"{TXA_DIR}/txa", "-r", file_name)
	popen = subprocess.Popen(command_args, stdout=subprocess.PIPE)
	popen.wait()
	output = popen.stdout.read()
	print(output.decode('utf-8'), end='')
elif args.subcommand == 'w': # Write mode
	file_name : str | None = args_to_JSON_file(args)
	if file_name is None:
		print("Error: Pixel art already exists.")
		sys.exit(0)

	command_args = (f"{TXA_DIR}/txa", "-w", args.input, file_name)
	popen = subprocess.Popen(command_args, stdout=subprocess.PIPE)
	popen.wait()
	output = popen.stdout.read()
	print(output.decode('utf-8'), end='')
