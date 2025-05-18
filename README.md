## A C Lexer in C

A lexer in C that tokenizes C source files into valid C tokens, creates a linked list of them and prints it.
## Getting Started

1. Clone the repository:

   ```bash
   git clone https://github.com/ttejas16/C-lexer
   cd C-lexer
   ```
2. Ensure you have a C compiler and `make` installed.

## Compilation & Running

* **Build the lexer:**

  ```bash
  make
  ```
* **Run the lexer on a source file:**

  ```bash
  make run arg=path/to/source.c
  ```
* **Clean build artifacts:**

  ```bash
  make clean
  ```

---
## Scanning Single-Character Tokens

A huge switch case for every valid token. Create the appropriate token and update the current token.
```C
Token *lexer_scan(Lexer *lexer) {
    while (isspace(lexer_peek(lexer)) || lexer_peek(lexer) == '\t') {
        lexer_advance(lexer);
    }

    Token *token = NULL;

    switch (lexer_peek(lexer)) {
        case '{':
            token = create_token(lexer, TOKEN_L_CURLY_BRACE, "{");
            break;

        case '}':
            token = create_token(lexer, TOKEN_R_CURLY_BRACE, "}");
            break;
        
        ...
    }
}
```

---
## Scanning Identifiers

When initializing the lexer add C keywords to a hash map.
```C
void lexer_initialize(Lexer *lexer) {
    hash_map_initialize(&keyword_map);

    lexer->line = 1;
    lexer->col = 1;
    lexer->position = 0;
    lexer->head = NULL;
}
```

check if lexeme starts with _ or any alphabet
```C
    ...
    if (token == NULL && (isalpha(lexer_peek(lexer)) || lexer_peek(lexer) == '_')) {
        token = scan_alphabets(lexer);
    }
    ... 
```

Then advance until any terminating or seperator character appears.<br/>Finally check if the keyword map has the lexeme and report `TOKEN_KEYWORD` or `TOKEN_IDENTIFIER` accordingly
```C
Token *scan_alphabets(Lexer *lexer) {
    char token_value[MAX_ID_LEN];
    memset(token_value, '\0', MAX_ID_LEN);

    size_t start = lexer->position;
    while (isalpha(lexer_peek(lexer)) || lexer_peek(lexer) == '_' || isdigit(lexer_peek(lexer))) {
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
```

---
## Scanning String and Character Literals
If the current token scanned was a doube quote, start scanning a string literal. <br/>
Until we encounter a closing double quote or `EOF`. 
```C
Token *lexer_scan(Lexer *lexer) {
    while (isspace(lexer_peek(lexer)) || lexer_peek(lexer) == '\t') {
        lexer_advance(lexer);
    }

    Token *token = NULL;

    switch (lexer_peek(lexer)) {
        ...
    }
    
    if (token && token->type == TOKEN_DOUBLE_QUOTE) {
        // scan string literal
    } 
    else if (token && token->type == TOKEN_SINGLE_QUOTE) {
        // scan char literal
    }

    ...
}
```
Similar method is used for character literals apart from checking the length of the scanned lexeme. <br/> i.e it should be 1

---
## Scanning Numeric Literals

Only supports 
 * Hex (without floating point), binary and octal literals.
 * Normal literals like `1234` with valid suffixes like `f`, `l`, etc.
 * Floating point literals.

Start scanning number literal if first character is a digit. <br/>
```C
    ...
    if (token == NULL && isdigit(lexer_peek(lexer))) {
        token = scan_numbers(lexer);
    }
    ...
```

Then scan for any number or alphabets(because of special literals). <br/>
After scanning the *entire* literal *then* validate it. (because idk how to do this on the fly XD)
```C
Token *scan_numbers(Lexer *lexer) {
    ...
    while (isalnum(lexer_peek(lexer)) || lexer_peek(lexer) == '.') {
        lexer_advance(lexer);
    }

    ...
    Token *token = create_token(lexer, TOKEN_NUMBER_LITERAL , token_value);

    if (strstr(token_value, ".") == NULL && token_value[0] == '0' && strlen(token_value) >= 2) {
        // we are on a special literal
        ...
        switch (token_value[1])
        {
        case 'x':
        case 'X':
            // hexadecimal
            ...
            break;

        case 'b':
        case 'B':
            // binary
            ...
            break;
        
        default:
            // octal 
            ...
            break;
        }
    }
    else {
        // we are on normal literal with or without suffix

        int suffix_start_index = strlen(token_value) - 1;
        while (suffix_start_index > -1 && isalpha(token_value[suffix_start_index])) {
            suffix_start_index--;
        }
        suffix_start_index++;

        ...
    }
    
    return token;
}
```