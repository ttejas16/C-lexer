#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#define BUFFER_LEN 1024

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Expected a file name as an argument\n");
        exit(EXIT_FAILURE);
    }

    Lexer lexer;
    lexer_initialize(&lexer);

    char *file_name = argv[1];

    FILE *fp = fopen(file_name, "r");
    if (!fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_LEN];
    memset(buffer, '\0', BUFFER_LEN);

    char *res = fgets(buffer, BUFFER_LEN, fp);
    while (res) {
        lexer.source = buffer;

        while (lexer_peek(&lexer) != '\0') {
            lexer_scan(&lexer);
        }

        lexer.position = 0;
        memset(buffer, '\0', BUFFER_LEN);
        res = fgets(buffer, BUFFER_LEN, fp);
    }

    print_tokens(&lexer);

    lexer_cleanup(&lexer);
    return 0;
}