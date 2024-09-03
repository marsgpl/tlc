#ifndef TLC_H
#define TLC_H

#include "args.h"
#include "file.h"
#include "config.h"
#include "tokens.h"
#include "parser.h"

#define TLC_VERSION "0.0.1"

#define TLC_INFO_HEADER "Typed Lua Compiler\n"
#define TLC_INFO_VERSION "Version: %s\n"
#define TLC_INFO_USAGE "Usage: tlc [options] [input_path]\n"
#define TLC_INFO_OPTIONS "Options:\n" \
"    -h, --help       Print this help message and exit\n" \
"    -v, --version    Print tlc version and exit\n" \
"    -c, --config     Path to tlc config file\n" \
"                     Default: %s\n" \
"    -i, --input      Path to source file or directory\n" \
"                     Explicitly usable if path starts with -\n" \
"    -o, --output     Path to destination file or directory\n" \
"                     Default: print to STDOUT\n"
#define TLC_INFO_EXAMPLES "Examples:\n" \
"    tlc -o result.lua example.tl              # single file\n" \
"    tlc --config custom.conf -i src -o dst    # directory\n"

#endif
