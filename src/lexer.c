#include "lexer.h"
#include "hash_map.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// define global keyword map instead of passing it to every lexer function
Hash_map keyword_map;

char *keywords[] = {
    // c constructs
    "if", 
    "else",
    "for",
    "while",
    "do",
    "return",
    "include",
    "static",
    "extern",
    "typedef",
    "unsigned",
    "signed",
    "switch",
    "enum",
    "break",
    "case",
    "const",
    "continue"
    "sizeof"
    // datatypes
    "struct",
    "int",
    "char",
    "void",
    "long",
    "double",
    "float",
    "short",
    "union", 
    NULL
};


static char lexer_advance(Lexer *lexer) {
    lexer->col++;
    return lexer->source[lexer->position++];
}

char lexer_peek(Lexer *lexer) { return lexer->source[lexer->position]; }

void lexer_initialize(Lexer *lexer) {
    // init the keyword map as well
    hash_map_initialize(&keyword_map);

    lexer->line = 1;
    lexer->col = 1;
    lexer->position = 0;
    lexer->head = NULL;
}

int is_seperator(char ch) {
    return (ch == '{' || ch == '}' || ch == '(' || ch == ')' || ch == ',' ||
            ch == ';' || ch == '\n' || ch == '\r');
}

int is_terminating(char ch){
    return (ch == EOF || ch == '\0');
}

size_t get_token_length(Token *token) {
    if (!token) return 0;

    int result = 0;
    result += strlen(get_token_name(token->type));
    result += strlen(token->value);
    
    // to count digits in line number 
    int l = token->line;
    int l_count = 0;
    while (l)
    {
        l_count++;
        l = l / 10;

    }
    // to count digits in col number 
    int c = token->col;
    int c_count = 0;
    while (c)
    {
        c_count++;
        c = c / 10;
    }

    result += (l_count + c_count);

    // for extra characters like spaces and , and 5 for Ln and Col
    result += 14;
    return result;
}

void print_tokens(Lexer *lexer) {
    Token *current = lexer->head;

    int first_break_point = 50;
    int second_break_point = 100;
    
    int chars_printed = 0;
    while (current) {
        int total_len = get_token_length(current);

        if (total_len < first_break_point) {
            int padding_len = first_break_point - total_len;
            chars_printed += first_break_point;

            printf("%s \x1B[34m'%s'", get_token_name(current->type), current->value);
            printf("\x1B[37m Ln %lu, Col %lu", current->line, current->col);
            printf("%-*s", padding_len, "");
        }
        else if(total_len >= first_break_point && total_len <= second_break_point) {
            int padding_len = second_break_point - total_len;
            chars_printed += second_break_point;

            printf("%s \x1B[34m'%s'", get_token_name(current->type), current->value);
            printf("\x1B[37m Ln %lu, Col %lu", current->line, current->col);
            printf("%-*s", padding_len, "");
        }
        else {
            if (chars_printed > 0) {
                printf("\n%s \x1B[34m'%s'", get_token_name(current->type), current->value);
                printf("\x1B[37m Ln %lu, Col %lu\n", current->line, current->col);
            }
            else {
                printf("%s \x1B[34m'%s'", get_token_name(current->type), current->value);
                printf("\x1B[37m Ln %lu, Col %lu", current->line, current->col);
            }

            chars_printed = 0;
        }
        
        if (chars_printed > second_break_point) {
            chars_printed = 0;
            printf("\n");
        }

        current = current->next;
    }

    if (fgetc(stdout) != '\n')
    {
        printf("\n");
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

    lexer->head = NULL;

    // free keyword map as well
    map_free(&keyword_map);
}

Token *create_token(Lexer *lexer, TokenType type, char *value) {
    static Token *current_token = NULL;
    Token *ptr = (Token *)malloc(sizeof(Token));

    ptr->line = lexer->line;
    
    // this is because scanning these tokens advances lexer position therefore its
    // column position
    switch (type)
    {
    case TOKEN_IDENTIFIER:
    case TOKEN_STRING_LITERAL:
    case TOKEN_CHAR_LITERAL:
    case TOKEN_NUMBER_LITERAL:
    case TOKEN_KEYWORD:
        ptr->col = lexer->col - strlen(value);
        break;
    
    default:
        ptr->col = lexer->col;
        break;
    }

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

Token *scan_alphabets(Lexer *lexer) {
    char token_value[MAX_ID_LEN];
    memset(token_value, '\0', MAX_ID_LEN);

    size_t start = lexer->position;
    while (!isspace(lexer_peek(lexer)) && !is_seperator(lexer_peek(lexer)) &&
           !is_terminating(lexer_peek(lexer)) && (isalpha(lexer_peek(lexer)) || lexer_peek(lexer) == '_')) {
        lexer_advance(lexer);
    }

    if (lexer->position - start > MAX_ID_LEN - 1) {
        printf("error at line %lu at position %lu\n", lexer->line,
               lexer->position);
        printf("identifier length exceeds MAX_ID_LEN: %d\n", MAX_ID_LEN);
        exit(EXIT_FAILURE);
    }

    strncpy(token_value, &lexer->source[start], lexer->position - start);

    if (map_has(&keyword_map, token_value) == 1)
    {
        return create_token(lexer, TOKEN_KEYWORD, token_value);
    }
    
    return create_token(lexer, TOKEN_IDENTIFIER, token_value);
}

Token *scan_numbers(Lexer *lexer) {
    char token_value[MAX_ID_LEN];
    memset(token_value, '\0', MAX_ID_LEN);

    size_t start = lexer->position;
    while (!isspace(lexer_peek(lexer)) && !is_seperator(lexer_peek(lexer)) &&
           !is_terminating(lexer_peek(lexer)) && isdigit(lexer_peek(lexer))) {
        lexer_advance(lexer);
    }

    if (lexer->position - start > MAX_ID_LEN - 1) {
        printf("error at line %lu at position %lu\n", lexer->line,
               lexer->position);
        printf("identifier length exceeds MAX_ID_LEN: %d\n", MAX_ID_LEN);
        exit(EXIT_FAILURE);
    }

    strncpy(token_value, &lexer->source[start], lexer->position - start);
    Token *token = create_token(lexer, TOKEN_NUMBER_LITERAL , token_value);
    return token;
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
        
        case '.':
            token = create_token(lexer, TOKEN_DOT, ".");
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

        case ':':
            token = create_token(lexer, TOKEN_COLON, ":");
            break;

        case '*':
            token = create_token(lexer, TOKEN_ASTERISK, "*");
            break;

        case '/':
            token = create_token(lexer, TOKEN_FORWARDSLASH, "/");
            break;

        case '|':
            token = create_token(lexer, TOKEN_PIPE, "|");
            break;

        case '&':
            token = create_token(lexer, TOKEN_AMPERSAND, "&");
            break;

        case '!':
            token = create_token(lexer, TOKEN_EXCLAMATION, "!");
            break;

        case '#':
            token = create_token(lexer, TOKEN_HASHTAG, "#");
            break;

        case '<':
            token = create_token(lexer, TOKEN_L_ANGLE_BRACE, "<");
            break;

        case '>':
            token = create_token(lexer, TOKEN_R_ANGLE_BRACE, ">");
            break;

        case '[':
            token = create_token(lexer, TOKEN_L_SQUARE_BRACE, "[");
            break;

        case ']':
            token = create_token(lexer, TOKEN_R_SQUARE_BRACE, "]");
            break;

        case '?':
            token = create_token(lexer, TOKEN_QUESTIONMARK, "?");
            break;

        case '\"':
            token = create_token(lexer, TOKEN_DOUBLE_QUOTE, "\"");
            break;

        case '\'':
            token = create_token(lexer, TOKEN_SINGLE_QUOTE, "\'");
            break;

        case '%':
            token = create_token(lexer, TOKEN_MODULO, "%");
            break;

        case '^':
            token = create_token(lexer, TOKEN_XOR, "^");
            break;

        case '\n':
            lexer->line++;
            lexer->col = 1;
            lexer_advance(lexer);
            break;

        case '\r':
            lexer_advance(lexer);
            break;

        case '\0':
            // hit end of line
            return NULL;

        default:
            break;
    }

    // check if the token scanned was a double quote if yes
    // scan until the next double quote appears or EOF or \0 appears
    if (token && token->type == TOKEN_DOUBLE_QUOTE)
    {
        lexer_advance(lexer); // move one character ahead first

        char token_value[MAX_ID_LEN];
        memset(token_value, '\0', MAX_ID_LEN);

        size_t start = lexer->position;
        while (lexer_peek(lexer) != '\"' && !is_terminating(lexer_peek(lexer)))
        {
            lexer_advance(lexer);
        }
        
        strncpy(token_value, &lexer->source[start], lexer->position - start);
        
        // scan string literal but dont update the current token
        Token *string_literal_token = create_token(lexer, TOKEN_STRING_LITERAL, token_value);

        if (is_terminating(lexer_peek(lexer)))
        {
            fprintf(stderr,
                    "missing terminating \" character for string literal at Ln %lu, Col %lu\n", 
                    string_literal_token->line, string_literal_token->col);
            exit(EXIT_FAILURE);
        }
        else{
            token = create_token(lexer, TOKEN_DOUBLE_QUOTE, "\"");
        }
        lexer_advance(lexer);
    }
    else if (token && token->type == TOKEN_SINGLE_QUOTE)
    {
        // this part is same as that for string literal but we just
        // check for a ' and we check if length of literal is more than 1 and report an error
        lexer_advance(lexer);

        char token_value[MAX_ID_LEN];
        memset(token_value, '\0', MAX_ID_LEN);

        size_t start = lexer->position;
        while (lexer_peek(lexer) != '\'' && !is_terminating(lexer_peek(lexer)))
        {
            lexer_advance(lexer);
        }

        strncpy(token_value, &lexer->source[start], lexer->position - start);
        
        Token *char_literal_token = create_token(lexer, TOKEN_CHAR_LITERAL, token_value);

        if (is_terminating(lexer_peek(lexer)))
        {
            fprintf(stderr,
                    "missing terminating \' character for char literal at Ln %lu, Col %lu\n", 
                    char_literal_token->line, char_literal_token->col);
            exit(EXIT_FAILURE);
        }
        else if (strlen(token_value) > 1)
        {
            fprintf(stderr, 
                    "multi-character character literal at Ln %lu, Col %lu \n",
                    char_literal_token->line, char_literal_token->col);
            exit(EXIT_FAILURE);
        }
        else{
            token = create_token(lexer, TOKEN_SINGLE_QUOTE, "\'");
        }
        lexer_advance(lexer);

    }
    else if (token == NULL && (isalpha(lexer_peek(lexer)) || lexer_peek(lexer) == '_')) {
        token = scan_alphabets(lexer);
    } 
    else if (token == NULL && isdigit(lexer_peek(lexer))) {
        token = scan_numbers(lexer);
    }
    else if (token == NULL) {
        token = create_token(lexer, TOKEN_INVALID, "");
        lexer_advance(lexer);
    } else {
        lexer_advance(lexer);
    }

    return token;
}



const char *get_token_name(TokenType type) {
    switch (type) {
        case TOKEN_IDENTIFIER:
            return "TOKEN_IDENTIFIER";

        case TOKEN_KEYWORD:
            return "TOKEN_KEYWORD";

        case TOKEN_INVALID:
            return "TOKEN_INVALID";

        case TOKEN_COMMA:
            return "TOKEN_COMMA";
        
        case TOKEN_DOT:
            return "TOKEN_DOT";

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

        case TOKEN_COLON:
            return "TOKEN_COLON";

        case TOKEN_ASTERISK:
            return "TOKEN_ASTERISK";

        case TOKEN_FORWARDSLASH:
            return "TOKEN_FORWARDSLASH";

        case TOKEN_PIPE:
            return "TOKEN_PIPE";

        case TOKEN_AMPERSAND:
            return "TOKEN_AMPERSAND";

        case TOKEN_EXCLAMATION:
            return "TOKEN_EXCLAMATION";

        case TOKEN_HASHTAG:
            return "TOKEN_HASHTAG";

        case TOKEN_L_ANGLE_BRACE:
            return "TOKEN_L_ANGLE_BRACE";

        case TOKEN_R_ANGLE_BRACE:
            return "TOKEN_R_ANGLE_BRACE";

        case TOKEN_L_SQUARE_BRACE:
            return "TOKEN_L_SQUARE_BRACE";

        case TOKEN_R_SQUARE_BRACE:
            return "TOKEN_R_SQUARE_BRACE";

        case TOKEN_QUESTIONMARK:
            return "TOKEN_QUESTIONMARK";

        case TOKEN_DOUBLE_QUOTE:
            return "TOKEN_DOUBLE_QUOTE";

        case TOKEN_SINGLE_QUOTE:
            return "TOKEN_SINGLE_QUOTE";

        case TOKEN_MODULO:
            return "TOKEN_MODULO";

        case TOKEN_XOR:
            return "TOKEN_XOR";

        case TOKEN_NUMBER_LITERAL:
            return "TOKEN_NUMBER_LITERAL";

        case TOKEN_STRING_LITERAL:
            return "TOKEN_STRING_LITERAL";

        case TOKEN_CHAR_LITERAL:
            return "TOKEN_CHAR_LITERAL";

        case TOKEN_EOF:
            return "TOKEN_EOF";

        default:
            return "TOKEN_INVALID";
    }
}