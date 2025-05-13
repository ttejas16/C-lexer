#ifndef _LEXER_
#define _LEXER_
#define MAX_ID_LEN 256
#include <stdlib.h>

#include "hash_map.h"

typedef enum {
    TOKEN_IDENTIFIER,
    TOKEN_KEYWORD,
    TOKEN_INVALID,
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_SEMICOLON,
    TOKEN_COLON,
    TOKEN_L_CURLY_BRACE,
    TOKEN_R_CURLY_BRACE,
    TOKEN_L_BRACE,
    TOKEN_R_BRACE,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_EQUAL,
    TOKEN_ASTERISK,
    TOKEN_FORWARDSLASH,
    TOKEN_UNDERSCORE,
    TOKEN_PIPE,
    TOKEN_AMPERSAND,
    TOKEN_EXCLAMATION,
    TOKEN_HASHTAG,
    TOKEN_L_ANGLE_BRACE,
    TOKEN_R_ANGLE_BRACE,
    TOKEN_L_SQUARE_BRACE,
    TOKEN_R_SQUARE_BRACE,
    TOKEN_QUESTIONMARK,
    TOKEN_DOUBLE_QUOTE,
    TOKEN_SINGLE_QUOTE,
    TOKEN_MODULO,
    TOKEN_XOR,
    TOKEN_NUMBER_LITERAL,
    TOKEN_STRING_LITERAL,
    TOKEN_EOF
} TokenType;

typedef struct Token {
    TokenType type;
    char value[MAX_ID_LEN];
    size_t line;
    struct Token *next;
} Token;

typedef struct {
    Token *head;
    char *source;
    size_t line;
    size_t position;
} Lexer;

extern char *keywords[];
extern Hash_map keyword_map;

Token *lexer_scan(Lexer *lexer);
void lexer_initialize(Lexer *lexer);
const char *get_token_name(TokenType type);
void print_tokens(Lexer *lexer);
void lexer_cleanup(Lexer *lexer);
char lexer_peek(Lexer *lexer);

#endif