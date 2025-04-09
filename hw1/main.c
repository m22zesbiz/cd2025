#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // For strcmp, strcpy, strlen

// --- Token Type Definitions ---
// Using strings directly as requested in the output format
#define TYPE_TOKEN "TYPE_TOKEN"    // For 'int' keyword
#define MAIN_TOKEN "MAIN_TOKEN"    // For 'main' keyword
#define IF_TOKEN "IF_TOKEN"        // For 'if' keyword
#define ELSE_TOKEN "ELSE_TOKEN"    // For 'else' keyword
#define WHILE_TOKEN "WHILE_TOKEN"  // For 'while' keyword

#define ID_TOKEN "ID_TOKEN"                      // For identifiers
#define LITERAL_TOKEN "LITERAL_TOKEN"            // For integer literals
#define ASSIGN_TOKEN "ASSIGN_TOKEN"              // For '='
#define EQUAL_TOKEN "EQUAL_TOKEN"                // For '=='
#define GREATER_TOKEN "GREATER_TOKEN"            // For '>'
#define GREATEREQUAL_TOKEN "GREATEREQUAL_TOKEN"  // For '>='
#define LESS_TOKEN "LESS_TOKEN"                  // For '<'
#define LESSEQUAL_TOKEN "LESSEQUAL_TOKEN"        // For '<='
#define PLUS_TOKEN "PLUS_TOKEN"                  // For '+'
#define MINUS_TOKEN "MINUS_TOKEN"                // For '-'
#define LEFTPAREN_TOKEN "LEFTPAREN_TOKEN"        // For '('
#define RIGHTPAREN_TOKEN "RIGHTPAREN_TOKEN"      // For ')'
#define LEFTBRACE_TOKEN "LEFTBRACE_TOKEN"        // For '{'
#define RIGHTBRACE_TOKEN "RIGHTBRACE_TOKEN"      // For '}'
#define SEMICOLON_TOKEN "SEMICOLON_TOKEN"        // For ';'

#define MAX_LEXEME_LEN 256  // Maximum length for identifiers or numbers

// --- Linked List Node for Tokens ---
typedef struct TokenNode {
    char* value;  // The actual lexeme (e.g., "if", "count", "123")
    char* type;   // The token type string (e.g., IF_TOKEN, ID_TOKEN)
    struct TokenNode* next;
} TokenNode;

// --- Global Variables ---
TokenNode* tokenListHead = NULL;
TokenNode* tokenListTail = NULL;
const char* inputFileName = "input.c";  // Define the input file name

// --- Helper Functions ---

// Manual implementation of isalpha
int is_alpha(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

// Manual implementation of isdigit
int is_digit(int c) {
    return c >= '0' && c <= '9';
}

// Manual implementation of isalnum
int is_alnum(int c) {
    return is_alpha(c) || is_digit(c);
}

// Manual implementation of isspace (handling only space, newline, tab as per slides)
int is_space(int c) {
    return c == ' ' || c == '\n' || c == '\t';
}

// Function to add a token to the end of the linked list
void addToken(const char* value, char* type) {
    TokenNode* newNode = (TokenNode*)malloc(sizeof(TokenNode));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed for token node\n");
        // No file pointer available here, maybe add it as arg if needed for fclose?
        // For simplicity, just exit. Proper error handling might involve freeing list.
        exit(1);
    }

    // Duplicate the value string
    newNode->value = (char*)malloc(strlen(value) + 1);
    if (newNode->value == NULL) {
        fprintf(stderr, "Memory allocation failed for token value\n");
        free(newNode);
        // As above, proper error handling would be more complex.
        exit(1);
    }
    strcpy(newNode->value, value);

    newNode->type = type;  // Type is a predefined string literal, no need to copy
    newNode->next = NULL;

    if (tokenListHead == NULL) {
        tokenListHead = newNode;
        tokenListTail = newNode;
    } else {
        tokenListTail->next = newNode;
        tokenListTail = newNode;
    }
}

// Function to free the token linked list
void freeTokenList() {
    TokenNode* current = tokenListHead;
    TokenNode* temp;
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp->value);  // Free the duplicated value string
        free(temp);         // Free the node itself
    }
    tokenListHead = NULL;
    tokenListTail = NULL;
}

// Function to print the token list
void printTokenList() {
    TokenNode* current = tokenListHead;
    while (current != NULL) {
        // Specific check for the 'int' keyword mapping to TYPE_TOKEN
        // and ensuring correct output format
        if (strcmp(current->type, TYPE_TOKEN) == 0 && strcmp(current->value, "int") == 0) {
            printf("int: %s\n", current->type);
        } else {
            printf("%s: %s\n", current->value, current->type);
        }
        current = current->next;
    }
}

// --- Main Scanner Logic ---
int main() {
    FILE* inputFile;
    int c;
    char lexemeBuffer[MAX_LEXEME_LEN];
    int lexemeIndex;

    // Open the input file
    inputFile = fopen(inputFileName, "r");
    if (inputFile == NULL) {
        fprintf(stderr, "Error opening file: %s\n", inputFileName);
        return 1;  // Indicate file opening error
    }

    // Read character by character from the file
    while ((c = fgetc(inputFile)) != EOF) {
        lexemeIndex = 0;  // Reset buffer index for each new token

        if (is_space(c)) {
            // Ignore whitespace
            continue;
        } else if (is_alpha(c)) {
            // Potential Identifier or Keyword
            lexemeBuffer[lexemeIndex++] = c;
            // Read subsequent alphanumeric or '_' characters
            while ((c = fgetc(inputFile)) != EOF && (is_alnum(c) || c == '_') && lexemeIndex < MAX_LEXEME_LEN - 1) {
                lexemeBuffer[lexemeIndex++] = c;
            }
            lexemeBuffer[lexemeIndex] = '\0';  // Null-terminate the string

            // Push back the character that ended the identifier/keyword
            if (c != EOF) {
                ungetc(c, inputFile);  // Use inputFile here
            }

            // Check if it's a keyword
            if (strcmp(lexemeBuffer, "int") == 0) {
                addToken(lexemeBuffer, TYPE_TOKEN);
            } else if (strcmp(lexemeBuffer, "main") == 0) {
                addToken(lexemeBuffer, MAIN_TOKEN);
            } else if (strcmp(lexemeBuffer, "if") == 0) {
                addToken(lexemeBuffer, IF_TOKEN);
            } else if (strcmp(lexemeBuffer, "else") == 0) {
                addToken(lexemeBuffer, ELSE_TOKEN);
            } else if (strcmp(lexemeBuffer, "while") == 0) {
                addToken(lexemeBuffer, WHILE_TOKEN);
            } else {
                // It's an identifier
                addToken(lexemeBuffer, ID_TOKEN);
            }
        } else if (is_digit(c)) {
            // Number literal
            lexemeBuffer[lexemeIndex++] = c;
            // Read subsequent digit characters
            while ((c = fgetc(inputFile)) != EOF && is_digit(c) && lexemeIndex < MAX_LEXEME_LEN - 1) {
                lexemeBuffer[lexemeIndex++] = c;
            }
            lexemeBuffer[lexemeIndex] = '\0';  // Null-terminate the string

            // Push back the character that ended the number
            if (c != EOF) {
                ungetc(c, inputFile);  // Use inputFile here
            }
            addToken(lexemeBuffer, LITERAL_TOKEN);
        } else {
            // Operators and Symbols
            lexemeBuffer[lexemeIndex++] = c;
            lexemeBuffer[lexemeIndex] = '\0';  // Initially assume single character

            int next_c;  // For lookahead

            switch (c) {
                case '=':
                    next_c = fgetc(inputFile);
                    if (next_c == '=') {
                        lexemeBuffer[lexemeIndex++] = next_c;
                        lexemeBuffer[lexemeIndex] = '\0';
                        addToken(lexemeBuffer, EQUAL_TOKEN);
                    } else {
                        if (next_c != EOF) ungetc(next_c, inputFile);  // Use inputFile
                        addToken(lexemeBuffer, ASSIGN_TOKEN);
                    }
                    break;
                case '>':
                    next_c = fgetc(inputFile);
                    if (next_c == '=') {
                        lexemeBuffer[lexemeIndex++] = next_c;
                        lexemeBuffer[lexemeIndex] = '\0';
                        addToken(lexemeBuffer, GREATEREQUAL_TOKEN);
                    } else {
                        if (next_c != EOF) ungetc(next_c, inputFile);  // Use inputFile
                        addToken(lexemeBuffer, GREATER_TOKEN);
                    }
                    break;
                case '<':
                    next_c = fgetc(inputFile);
                    if (next_c == '=') {
                        lexemeBuffer[lexemeIndex++] = next_c;
                        lexemeBuffer[lexemeIndex] = '\0';
                        addToken(lexemeBuffer, LESSEQUAL_TOKEN);
                    } else {
                        if (next_c != EOF) ungetc(next_c, inputFile);  // Use inputFile
                        addToken(lexemeBuffer, LESS_TOKEN);
                    }
                    break;
                case '+':
                    addToken(lexemeBuffer, PLUS_TOKEN);
                    break;
                case '-':
                    addToken(lexemeBuffer, MINUS_TOKEN);
                    break;
                case '(':
                    addToken(lexemeBuffer, LEFTPAREN_TOKEN);
                    break;
                case ')':
                    addToken(lexemeBuffer, RIGHTPAREN_TOKEN);
                    break;
                case '{':
                    addToken(lexemeBuffer, LEFTBRACE_TOKEN);
                    break;
                case '}':
                    addToken(lexemeBuffer, RIGHTBRACE_TOKEN);
                    break;
                case ';':
                    addToken(lexemeBuffer, SEMICOLON_TOKEN);
                    break;
                default:
                    fprintf(stderr, "Lexical Error: Unrecognized character '%c'\n", c);
                    break;
            }
        }
    }

    // Close the file now that we are done reading
    fclose(inputFile);

    // Print all recognized tokens
    printTokenList();

    // Free the allocated memory
    freeTokenList();

    return 0;
}
