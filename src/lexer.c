#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char lexer_advance(Lexer *lexer) {
    return lexer->source[lexer->position++];
}

static char lexer_peek(Lexer *lexer) { return lexer->source[lexer->position]; }

void lexer_initialize(Lexer *lexer) {
    lexer->line = 1;
    lexer->position = 0;
    lexer->head = NULL;
}

int is_seperator(char ch) {
    return (ch == '{' || ch == '}' || ch == '(' || ch == ')' || ch == ',' ||
            ch == ';' || ch == '\n' || ch == '\r');
}

void print_tokens(Lexer *lexer) {
    Token *current = lexer->head;

    while (current) {
        printf("%s \x1B[34m'%s'\n", get_token_name(current->type),
               current->value);
        printf("\x1B[37m");

        current = current->next;
    }
}

void lexer_cleanup(Lexer *lexer) {
    if (!lexer->head) {
        return;
    }

    Token *slow = lexer->head;
    Token *fast = lexer->head->next;

    while (slow) {
        free(slow);
        slow = fast;

        if (fast) {
            fast = fast->next;
        }
    }
}

Token *create_token(Lexer *lexer, TokenType type, char *value) {
    static Token *current_token = NULL;
    Token *ptr = (Token *)malloc(sizeof(Token));

    ptr->line = lexer->line;
    ptr->type = type;
    ptr->next = NULL;
    strncpy(ptr->value, value, MAX_ID_LEN);

    if (lexer->head == NULL) {
        lexer->head = ptr;
        current_token = ptr;
    } else {
        current_token->next = ptr;
        current_token = ptr;
    }

    return ptr;
}

Token *lexer_scan(Lexer *lexer) {
    while (isspace(lexer_peek(lexer)) || lexer_peek(lexer) == '\t') {
        lexer_advance(lexer);
    }

    Token *token = NULL;

    // printf("[%c]", lexer_peek(lexer));
    switch (lexer_peek(lexer)) {
        case '{':
            token = create_token(lexer, TOKEN_L_CURLY_BRACE, "{");
            break;

        case '}':
            token = create_token(lexer, TOKEN_R_CURLY_BRACE, "}");
            break;

        case '(':
            token = create_token(lexer, TOKEN_L_BRACE, "(");
            break;

        case ')':
            token = create_token(lexer, TOKEN_R_BRACE, ")");
            break;

        case ';':
            token = create_token(lexer, TOKEN_SEMICOLON, ";");
            break;

        case ',':
            token = create_token(lexer, TOKEN_COMMA, ",");
            break;

        case '+':
            token = create_token(lexer, TOKEN_PLUS, "+");
            break;

        case '-':
            token = create_token(lexer, TOKEN_MINUS, "-");
            break;

        case '=':
            token = create_token(lexer, TOKEN_EQUAL, "=");
            break;

        case '\n':
            lexer->line++;
            lexer_advance(lexer);
            break;

        case '\r':
            lexer_advance(lexer);
            break;

        default:
            break;
    }

    if (token == NULL && isalnum(lexer_peek(lexer))) {
        // printf("%d",MAX_ID_LEN);
        char token_value[MAX_ID_LEN];
        memset(token_value, '\0', MAX_ID_LEN);

        size_t start = lexer->position;
        while (!isspace(lexer_peek(lexer)) &&
               !is_seperator(lexer_peek(lexer))) {
            lexer_advance(lexer);
        }

        if (lexer->position - start > MAX_ID_LEN - 1) {
            printf("error at line %lu at position %lu\n", lexer->line,
                   lexer->position);
            printf("identifier length exceeds MAX_ID_LEN: %d\n", MAX_ID_LEN);
            exit(EXIT_FAILURE);
        }

        strncpy(token_value, &lexer->source[start], lexer->position - start);

        token = create_token(lexer, TOKEN_IDENTIFIER, token_value);
    } else if (token == NULL) {
        token = create_token(lexer, TOKEN_EOF, "");
        lexer_advance(lexer);
    } else {
        lexer_advance(lexer);
    }

    // printf("Found Id [%s] at %lu at line %lu", token_value, start,
    // lexer->line);
    return token;
}

const char *get_token_name(TokenType type) {
    switch (type) {
        case TOKEN_IDENTIFIER:
            return "TOKEN_IDENTIFIER";
        case TOKEN_KEYWORD:
            return "TOKEN_KEYWORD";
        case TOKEN_IF:
            return "TOKEN_IF";
        case TOKEN_ELSE:
            return "TOKEN_ELSE";
        case TOKEN_RETURN:
            return "TOKEN_RETURN";
        case TOKEN_INVALID:
            return "TOKEN_INVALID";
        case TOKEN_COMMA:
            return "TOKEN_COMMA";
        case TOKEN_SEMICOLON:
            return "TOKEN_SEMICOLON";
        case TOKEN_L_CURLY_BRACE:
            return "TOKEN_L_CURLY_BRACE";
        case TOKEN_R_CURLY_BRACE:
            return "TOKEN_R_CURLY_BRACE";
        case TOKEN_L_BRACE:
            return "TOKEN_L_BRACE";
        case TOKEN_R_BRACE:
            return "TOKEN_R_BRACE";
        case TOKEN_PLUS:
            return "TOKEN_PLUS";
        case TOKEN_MINUS:
            return "TOKEN_MINUS";
        case TOKEN_EQUAL:
            return "TOKEN_EQUAL";
        case TOKEN_EOF:
            return "TOKEN_EOF";

        default:
            return "UNKNOWN";
    }
}