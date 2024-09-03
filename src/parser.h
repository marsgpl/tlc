#ifndef TLC_PARSER_H
#define TLC_PARSER_H

#include "error.h"
#include "file.h"
#include "config.h"
#include "tokens.h"

tlc_tokens *tlc_parse_file(tlc_file *file, tlc_config *config);

#endif
