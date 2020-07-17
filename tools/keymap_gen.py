"""
This module generates keymaps; i.e. functions the map native keycodes to fundament keycodes
based on the keymapping table:
https://docs.google.com/spreadsheets/d/1VPQTYbJqPITDeBr0M5-fUwX6cvYu9hFFXQVaXtD9sN0/edit?usp=sharing

Note, that this tool does NOT extend waf.
"""

import csv
import pathlib
from string import Template

REPO_ROOT = pathlib.Path(__file__).absolute().parents[1]
SYM_HEADER = REPO_ROOT.joinpath('lib', 'c', 'public', 'fundament', 'input.h')
SOURCE_ROOT = REPO_ROOT.joinpath('lib', 'c', 'private')
CSV_FILE_PATH = pathlib.Path(__file__).parent.joinpath('mappings.csv')
SEPARATOR = "//" + 78 * "=" + "\n"

HEADER_FILE_TEMPLATE = Template(
    (
        "#ifndef FUNDAMENT_INPUT_KEY_MAP_${PLATFORM}_H\n"
        "#define FUNDAMENT_INPUT_KEY_MAP_${PLATFORM}_H\n"
        "\n"
        "#include <stdint.h>\n"
        "\n"
        "${additional_includes}\n"
        "\n"
        "${separator}"
        "// This file was generated using the keymap_gen.py script,\n"
        "// which is part of the fundament project and subject to\n"
        "// its license.\n"
        "//\n"
        "// The function declared in this header maps ${platform}'s\n"
        "// keycodes to fundament's keycodes.\n"
        "${separator}"
        "\n"
        "\n"
        "//\n"
        "// Maps a ${platform} key code to a fundament key code.\n"
        "//\n"
        "uint16_t fn__imp_map_${platform}_key(${signature});\n"
        "\n"
        "#endif // FUNDAMENT_INPUT_KEY_MAP_${PLATFORM}_H\n"
    )
)

SOURCE_FILE_TEMPLATE = Template(
    (
        "#include \"input_key_map_${platform}.h\"\n"
        "#include <fundament/input.h>\n"
        "\n"
        "${additional_includes}"
        "\n"
        "uint16_t fn__imp_map_${platform}_key(${signature}) {\n"
        "    switch(key_code) {\n"
        "${switch_content}"
        "    }\n"
        "}\n"
        "\n"
    )
)

ENUM_TEMPLATE = Template(
    (
        "//\n"
        "// Enumerates all supported keys.\n"
        "//\n"
        "enum fn_key {\n"
        "${values}"
        "};\n"
    )
)

PLATFORM = {
    'win32' : ('#include <WinDef.h>', 'WAPARAM key_code', '#include <Winuser.h>\n'),
    'xcb' : ('', 'uint32_t key_code', '#include <xcb/input-event-codes.h>\n'),
    'appkit' : ('', 'uint16_t key_code', '#include <Carbon/Carbon.h>\n')
}

def main():
    mappings = {
        'win32': [],
        'xcb': [],
        'appkit': []
    }

    syms = ['fn_key_undefined']

    with open(CSV_FILE_PATH, 'r') as f:
        reader = csv.reader(f)
   
        red_first = False
        for row in reader:
            if not red_first:
                red_first = True
                continue 

            mappings['win32'].append((row[1], row[3]))
            mappings['xcb'].append((row[1], row[5]))
            mappings['appkit'].append((row[1], row[4])) 
            syms.append(row[1])

    sym_header_content = None
    with open(SYM_HEADER, 'r') as f:
        sym_header_content = f.readlines()

    start = None
    end = None
    for i in range(len(sym_header_content)):
        line = sym_header_content[i]
    
        if line.startswith('//@KEYS') and start is None:
            start = i

        if line.startswith('//@KEYS_END') and end is None:
            end = i
            break

    upper_half = []
    lower_half = [] 

    clean_sym_header = []
    for i in range(len(sym_header_content)):
        if i < start:
            upper_half.append(sym_header_content[i])
        elif i > end:
            lower_half.append(sym_header_content[i])

    vals = []
    for sym in syms:
        prefix = ' ' * 4
        line = f'{prefix}{sym},\n' 
        vals.append(line)

    vals = ''.join(vals)
    val = ENUM_TEMPLATE.substitute(values=vals)
    upper_half = ''.join(upper_half)
    lower_half = ''.join(lower_half)
    
    with open(SYM_HEADER, 'w') as f:
        f.write(upper_half)
        f.write(val)
        f.write(lower_half) 

    for platform, (additional_includes, signature, source_incl) in PLATFORM.items():
        imp_dir = SOURCE_ROOT.joinpath(platform)
        
        content = HEADER_FILE_TEMPLATE.substitute(
            PLATFORM=platform.upper(),
            platform=platform,
            additional_includes=additional_includes,
            signature=signature,
            separator=SEPARATOR
        )

        platform_dir = SOURCE_ROOT.joinpath(platform)
        header_file = platform_dir.joinpath(f'input_key_map_{ platform }.h')
        with open(header_file, 'w') as header:
            header.write(content)

        prefix = ' ' * 8
        lines = []
        for mapping in mappings[platform]:
            if not mapping[1]:
                line = f'{prefix}// No known mapping for {mapping[0]}.\n'
                lines.append(line)
                continue

            label = len(mapping[1])

            line = f'{prefix}case {mapping[1]}: return {mapping[0]};\n'
            lines.append(line)

        lines = ''.join(lines)
        
        content = SOURCE_FILE_TEMPLATE.substitute(
            platform=platform,
            signature=signature,
            switch_content=lines,
            additional_includes=source_incl
        )

        source_file = platform_dir.joinpath(f'input_key_map_{ platform }.c')
        with open(source_file, 'w') as source:
            source.write(content)

if __name__ == '__main__':
    main()
