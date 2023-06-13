#! /usr/bin/python3

import os
import stat
import argparse
import re
import string


"""
Generate a header file with #defines from configuration file.

By default the configuration is generated from .config file. A different
file can be specified using --config-file <config_file> argument.

The generated file is saved as config.h by default. A different file name
can be specified using --header-file <header_file> argument.

"""

def check_config_val(config_val):
    ret = False

    if (config_val[0] == 'y') or (config_val[0] == 'n'):
        if len(config_val) == 1:
            return True

    if (config_val[0] == '"') and (config_val[-1] == '"'):
        return True

    if config_val.isnumeric() is True:
        return True
    
    if (config_val[0] == '0') and ((config_val[1] == 'x') or config_val[1] == 'X'):
        config_val = config_val[2:]
        if all(c in string.hexdigits for c in config_val) is True:
            return True
    
    return False

def process_config(config_file):

    processed_configs = []

    config_gen_regex = re.compile(r'(CONFIG_\w+)=(.+)')
    unset_bool_regex = re.compile(r'# (CONFIG_\w+) is not set')

    with open(config_file, 'r') as in_file:
        for line in in_file:
            match = config_gen_regex.search(line)
            if match is not None:
                config_name = match.group(1)
                config_val = match.group(2)

                if check_config_val(config_val) is True:
                    processed_configs.append((config_name, config_val))
                else:
                    print('Error in {}={}'.format(config_name, config_val))
                    exit(-1)
            else:
                match = unset_bool_regex.search(line)
                if match is not None:
                    config_name = match.group(1)
                    config_val = 'n'
                    processed_configs.append((config_name, config_val))

    return processed_configs

def write_config_header(config_list, header_file):

    config_names = list(zip(*config_list))[0]
    config_values = list(zip(*config_list))[1]

#    start_idx = config_names.index('CONFIG_APPLICATION_START_MARK')
#    if config_values[start_idx] != 'y':
#        print('{} is not set'.format(config_names[start_idx]))
#        exit(-1)
    
#    end_idx = config_names.index('CONFIG_APPLICATION_END_MARK')
#    if config_values[end_idx] != 'y':
#        print('{} is not set'.format(config_names[end_idx]))
#        exit(-1)
    
    config_names = config_names[:]
    config_values = config_values[:]

    with open(header_file, 'w') as out_file:

        header_file = os.path.basename(header_file)
        header_file = os.path.splitext(header_file)[0]
        guard = '__{}_H__'.format(header_file.upper())

        out_file.write('#ifndef\t{}\n\n'.format(guard))
        out_file.write('#define\t{}\n\n'.format(guard))

        for name, val in zip(config_names, config_values):
            if val == 'y':
                val = '1'
            elif val == 'n':
                val = '0'
            
            out_file.write('#define {}\t{}\n'.format(name, val))
        
        out_file.write('\n#endif /* {} */\n'.format(guard))


def main():
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description=__doc__)
    
    parser.add_argument(
        "--header-file",
        metavar="HEADER_FILE",
        help="""
        Path to write the generated header file to. If not specified, it 
        defaults to config.h""")

    parser.add_argument(
        "--config-file",
        metavar="CONFIG_FILE",
        help="""
        The configuration header is generated from this file. If not specified it
        defaults to .config
        """)

    args = parser.parse_args()

    if args.header_file is not None:
        header_file = args.header_file
    else:
        header_file = 'config.h'
    
    if args.config_file is not None:
        config_file = args.config_file
    else:
        config_file = '.config'
    
    if os.path.isfile(config_file) is False:
        print('Error: {} no such file'.format(header_file))
        exit(-1)
    
    fstat = os.stat(config_file)
    if bool(fstat.st_mode & stat.S_IRUSR) is not True:
        print('Error: Cannot read {}. Permission denied'.format(config_file))
        exit(-1)
    
    if os.path.isfile(header_file) is True:
        fstat = os.stat(header_file)
        if bool(fstat.st_mode & stat.S_IWUSR) is not True:
            print('Error: Cannot write to {}. Permission denied'.format(header_file))
            exit(-1)
    else:
        full_path = os.path.realpath(header_file)
        dir = os.path.dirname(full_path)
        fstat = os.stat(dir)
        if bool(fstat.st_mode & stat.S_IWUSR) is not True:
            print('Error: Cannot create {}. Permission denied'.format(header_file))
            exit(-1)
    
    processed_configs = process_config(config_file)

    # Check for variables defined more than once
    config_names = list(zip(*processed_configs))[0]
    seen = set()
    for item in config_names:
        if item not in seen:
            seen.add(item)
        else:
            print('Error: {} is defined more than once'.format(item))    
            exit(-1)

    write_config_header(processed_configs, header_file)

if __name__ == "__main__":
    main()