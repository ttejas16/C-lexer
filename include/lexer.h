#ifndef _LEXER_
#define _LEXER_
#define MAX_ID_LEN 256
#include <stdlib.h>

typedef enum {
    TOKEN_IDENTIFIER,
    TOKEN_KEYWORD,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_RETURN,
    TOKEN_INVALID,
    TOKEN_COMMA,
    TOKEN_SEMICOLON,
    TOKEN_L_CURLY_BRACE,
    TOKEN_R_CURLY_BRACE,
    TOKEN_L_BRACE,
    TOKEN_R_BRACE,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_EQUAL,
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    char value[MAX_ID_LEN];
    size_t line;
} Token;

typedef struct {
    char *source;
    size_t line;
    size_t position;
} Lexer;

Token* lexer_scan(Lexer *lexer);
void lexer_initialize(Lexer *lexer);
const char* get_token_name(TokenType type);
#endif