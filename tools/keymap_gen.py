"""
This script generates a function for each supported platform,
which maps the underlying platform's native keycodes to fundament's keycodes.

The mappings are based of this online spreadsheet:
https://docs.google.com/spreadsheets/d/1VPQTYbJqPITDeBr0M5-fUwX6cvYu9hFFXQVaXtD9sN0/edit?usp=sharing

Note, that this tool does NOT extend waf.
"""

import csv
from dataclasses import dataclass
import pathlib
from string import Template

TOOLS_DIR = pathlib.Path(__file__).absolute().parent
TEMPLATES_DIR = TOOLS_DIR.joinpath('templates')
CSV_FILE_PATH = TOOLS_DIR.joinpath('mappings.csv')
REPO_ROOT = TOOLS_DIR.parent
SOURCE_ROOT = REPO_ROOT.joinpath('lib', 'c', 'private')

HEADER_FILE_TEMPLATE = TEMPLATES_DIR.joinpath('input_key_map_platform.h.template')
SOURCE_FILE_TEMPLATE = TEMPLATES_DIR.joinpath('input_key_map_platform.c.template') 
INPUT_FILE_TEMPLATE = TEMPLATES_DIR.joinpath('input.h.template')

@dataclass
class PlatformInfo:
    platform: str
    header_includes: list
    argument_type: str
    source_includes: list

platforms = {}
platforms['win32'] = PlatformInfo('win32', '#include <WinDef.h>', 'WPARAM', '#include <WinUser.h>')
platforms['xcb'] = PlatformInfo('xcb', '', 'uint32_t', '#include <linux/input-event-codes.h>')
platforms['appkit'] = PlatformInfo('appkit', '', 'uint16_t', '#include <Carbon/Carbon.h>')

@dataclass
class Mapping:
    description: str
    fundament: str
    usb: str
    win32: str
    appkit: str
    xlib: str
    xcb: str 
    
    def get_case(self, platform: str) -> str:
        native = getattr(self, platform, None)
        if not native:
            return ' ' * 8 + f'// No known mapping for {self.fundament}\n' 

        return ' ' * 8 + f'case {native}: return {self.fundament};\n' 

def main():
    global HEADER_FILE_TEMPLATE
    global SOURCE_FILE_TEMPLATE
    global INPUT_FILE_TEMPLATE

    with open(HEADER_FILE_TEMPLATE) as f:
        HEADER_FILE_TEMPLATE = Template(f.read())

    with open(SOURCE_FILE_TEMPLATE) as f:
        SOURCE_FILE_TEMPLATE = Template(f.read())

    with open(INPUT_FILE_TEMPLATE) as f:
        INPUT_FILE_TEMPLATE = Template(f.read())

    mappings = []

    with open(CSV_FILE_PATH, 'r') as f:
        reader = csv.reader(f)
        next(reader, None)
        
        for row in reader:
            mappings.append(Mapping(*row))

    for platform in platforms.values():
        header = HEADER_FILE_TEMPLATE.substitute(
            PLATFORM=platform.platform.upper(),
            platform=platform.platform,
            additional_includes=platform.header_includes,
            argument_type=platform.argument_type 
        )
    
        switches = []
        for mapping in mappings:
            switch = mapping.get_case(platform.platform)
            switches.append(switch)

        switches.append(' ' * 8 + 'default: return fn_key_undefined;')
        switches = ''.join(switches)

        source = SOURCE_FILE_TEMPLATE.substitute(
            platform=platform.platform,
            additional_includes=platform.source_includes,
            argument_type=platform.argument_type,
            switches=switches
        )

        impl_dir = SOURCE_ROOT.joinpath(platform.platform)
        header_file = impl_dir.joinpath(f'input_key_map_{platform.platform}.h')
        source_file = impl_dir.joinpath(f'input_key_map_{platform.platform}.c')

        with open(header_file, 'w') as f:
            f.write(header)

        with open(source_file, 'w') as f:
            f.write(source)

    values = []
    values.append('fn_key_undefined,\n')
    
    prefix = 4 * ' '
    for mapping in mappings:
        values.append(f'{prefix}{mapping.fundament},\n')

    values.append(f'{prefix}fn_key_count')
    values = ''.join(values) 

    input_file = REPO_ROOT.joinpath('lib', 'c', 'public', 'fundament', 'input.h')
    with open(input_file, 'w') as f:
        content = INPUT_FILE_TEMPLATE.substitute(values=values)
        f.write(content)

if __name__ == '__main__':
    main()
