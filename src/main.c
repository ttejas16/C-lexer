#include "lexer.h"
#include<stdio.h>

int main(void) {
    Lexer lexer;
    // yet to handle other constructs :)
    char *source = "void main(){\tinta = 10 + 1;\r\nint b = a - 12;;\"\" char *ptr = hi }";
    
    lexer_initialize(&lexer);
    lexer.source = source;
    
    Token *token = lexer_scan(&lexer);

    while (token->type != TOKEN_EOF)
    {   
        token = lexer_scan(&lexer);
    }
    
    print_tokens(&lexer);

    lexer_cleanup(&lexer);
    return 0;
}       