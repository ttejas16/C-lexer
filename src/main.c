#include "lexer.h"
#include<stdio.h>

int main(void){
    Lexer lexer;
    // yet to handle other constructs :)
    char *source = "void main(){\tinta = 10 + 1;\r\nint b = a - 12; }";
    
    lexer_initialize(&lexer);
    lexer.source = source;
    
    Token *token = lexer_scan(&lexer);
        // printf("%s",get_token_name(token->type));
    while (token->type != TOKEN_EOF)
    {   
        printf("%s \x1B[34m'%s'\n", get_token_name(token->type), token->value);
        printf("\x1B[37m");
        free(token);
        token = lexer_scan(&lexer);
    }
    
    free(token);
    return 0;
}       