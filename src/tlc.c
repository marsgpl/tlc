#include "tlc.h"

static int print_help(void) {
    printf(TLC_INFO_HEADER
        TLC_INFO_VERSION
        "\n" TLC_INFO_USAGE "\n"
        TLC_INFO_OPTIONS
        TLC_INFO_EXAMPLES,
            TLC_VERSION,
            TLC_CONFIG_PATH_DEFAULT);
    return 0;
}

static int print_version(void) {
    printf(TLC_INFO_HEADER TLC_INFO_VERSION, TLC_VERSION);
    return 0;
}

int main(int argc, const char **argv) {
    tlc_args *args = tlc_parse_args(argc, argv);
    tlc_check_args(args);

    if (args->print_help) {
        return print_help();
    } else if (args->print_version) {
        return print_version();
    }

    tlc_config *config = tlc_init_config(args->config_path);
    tlc_check_config(config);
    tlc_file *file = tlc_read_file(args->input_path);
    tlc_tokens *tokens = tlc_parse_file(file, config);

    tlc_print_args(args);
    tlc_print_config(config);
    tlc_print_file(file, "input file");
    tlc_print_tokens(tokens, file);

    tlc_free_tokens(tokens);
    tlc_free_file(file);
    tlc_free_config(config);
    tlc_free_args(args);
}
