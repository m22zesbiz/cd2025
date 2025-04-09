#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEXEME_LEN 256  // Maximum length for identifiers or numbers

// Define the node structure for our linked list
typedef struct Node {
    char* value;  // The actual lexeme (e.g., "if", "count", "123")
    char* type;   // The token type string (e.g., IF_TOKEN, ID_TOKEN)
    struct Node* next;
} Node;

// Manual implementation of isalpha, isdigit, isalnum, and isspace
int is_alpha(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int is_digit(int c) {
    return c >= '0' && c <= '9';
}

int is_alnum(int c) {
    return is_alpha(c) || is_digit(c);
}

int is_space(int c) {
    return c == ' ' || c == '\n' || c == '\t';
}

// Function to add a token to the end of the linked list
void addNode(const char* value, char* type, Node** head, Node** tail) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed for token node\n");
        exit(1);
    }

    // Duplicate the value string
    newNode->value = (char*)malloc(strlen(value) + 1);
    if (newNode->value == NULL) {
        fprintf(stderr, "Memory allocation failed for token value\n");
        free(newNode);
        exit(1);
    }
    strcpy(newNode->value, value);

    newNode->type = type;
    newNode->next = NULL;

    if (*head == NULL) {
        *head = newNode;
        *tail = newNode;
    } else {
        (*tail)->next = newNode;
        *tail = newNode;
    }
}

// Function to free the linked list
void freeList(Node* head) {
    Node* current = head;
    Node* temp;
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp->value);
        free(temp);
    }
}

int main() {
    FILE* file = fopen("input.c", "r");
    int c;
    char lexemeBuffer[MAX_LEXEME_LEN];
    int lexemeIndex;
    Node* nodeHead = NULL;
    Node* nodeTail = NULL;

    if (file == NULL) {
        perror("Error opening file 'input.c'");
        return 1;
    }

    // Read character by character from the file
    while ((c = fgetc(file)) != EOF) {
        lexemeIndex = 0;

        if (is_space(c)) {
            // Ignore whitespace
            continue;
        } else if (is_alpha(c)) {
            // Potential Identifier or Keyword
            lexemeBuffer[lexemeIndex++] = c;
            // Read subsequent alphanumeric or '_' characters
            while ((c = fgetc(file)) != EOF && (is_alnum(c) || c == '_') && lexemeIndex < MAX_LEXEME_LEN - 1) {
                lexemeBuffer[lexemeIndex++] = c;
            }
            lexemeBuffer[lexemeIndex] = '\0';  // Null-terminate the string

            // Push back the character that ended the identifier/keyword
            if (c != EOF) {
                ungetc(c, file);
            }

            // Check if it's a keyword
            if (strcmp(lexemeBuffer, "int") == 0) {
                addNode(lexemeBuffer, "TYPE_TOKEN", &nodeHead, &nodeTail);
            } else if (strcmp(lexemeBuffer, "main") == 0) {
                addNode(lexemeBuffer, "MAIN_TOKEN", &nodeHead, &nodeTail);
            } else if (strcmp(lexemeBuffer, "if") == 0) {
                addNode(lexemeBuffer, "IF_TOKEN", &nodeHead, &nodeTail);
            } else if (strcmp(lexemeBuffer, "else") == 0) {
                addNode(lexemeBuffer, "ELSE_TOKEN", &nodeHead, &nodeTail);
            } else if (strcmp(lexemeBuffer, "while") == 0) {
                addNode(lexemeBuffer, "WHILE_TOKEN", &nodeHead, &nodeTail);
            } else {
                // It's an identifier
                addNode(lexemeBuffer, "ID_TOKEN", &nodeHead, &nodeTail);
            }
        } else if (is_digit(c)) {
            // Number literal
            lexemeBuffer[lexemeIndex++] = c;
            // Read subsequent digit characters
            // Check bounds carefully: lexemeIndex < MAX_LEXEME_LEN - 1
            while ((c = fgetc(file)) != EOF && is_digit(c) && lexemeIndex < MAX_LEXEME_LEN - 1) {
                lexemeBuffer[lexemeIndex++] = c;
            }
            lexemeBuffer[lexemeIndex] = '\0';  // Null-terminate the string

            // Push back the character that ended the number
            if (c != EOF) {
                ungetc(c, file);
            }
            addNode(lexemeBuffer, "LITERAL_TOKEN", &nodeHead, &nodeTail);
        } else {
            // Operators and Symbols
            lexemeBuffer[lexemeIndex++] = c;
            // lexemeBuffer[lexemeIndex] = '\0'; // Set NULL later after potential second char

            int next_c;  // For lookahead

            switch (c) {
                case '=':
                    next_c = fgetc(file);
                    if (next_c == '=') {
                        lexemeBuffer[lexemeIndex++] = next_c;
                        lexemeBuffer[lexemeIndex] = '\0';  // Null terminate now
                        addNode(lexemeBuffer, "EQUAL_TOKEN", &nodeHead, &nodeTail);
                    } else {
                        lexemeBuffer[lexemeIndex] = '\0';  // Null terminate single char
                        if (next_c != EOF) ungetc(next_c, file);
                        addNode(lexemeBuffer, "ASSIGN_TOKEN", &nodeHead, &nodeTail);
                    }
                    break;
                case '>':
                    next_c = fgetc(file);
                    if (next_c == '=') {
                        lexemeBuffer[lexemeIndex++] = next_c;
                        lexemeBuffer[lexemeIndex] = '\0';  // Null terminate now
                        addNode(lexemeBuffer, "GREATEREQUAL_TOKEN", &nodeHead, &nodeTail);
                    } else {
                        lexemeBuffer[lexemeIndex] = '\0';  // Null terminate single char
                        if (next_c != EOF) ungetc(next_c, file);
                        addNode(lexemeBuffer, "GREATER_TOKEN", &nodeHead, &nodeTail);
                    }
                    break;
                case '<':
                    next_c = fgetc(file);
                    if (next_c == '=') {
                        lexemeBuffer[lexemeIndex++] = next_c;
                        lexemeBuffer[lexemeIndex] = '\0';  // Null terminate now
                        addNode(lexemeBuffer, "LESSEQUAL_TOKEN", &nodeHead, &nodeTail);
                    } else {
                        lexemeBuffer[lexemeIndex] = '\0';  // Null terminate single char
                        if (next_c != EOF) ungetc(next_c, file);
                        addNode(lexemeBuffer, "LESS_TOKEN", &nodeHead, &nodeTail);
                    }
                    break;
                // Single character tokens
                case '+':
                    lexemeBuffer[lexemeIndex] = '\0';
                    addNode(lexemeBuffer, "PLUS_TOKEN", &nodeHead, &nodeTail);
                    break;
                case '-':
                    lexemeBuffer[lexemeIndex] = '\0';
                    addNode(lexemeBuffer, "MINUS_TOKEN", &nodeHead, &nodeTail);
                    break;
                case '(':
                    lexemeBuffer[lexemeIndex] = '\0';
                    addNode(lexemeBuffer, "LEFTPAREN_TOKEN", &nodeHead, &nodeTail);
                    break;
                case ')':
                    lexemeBuffer[lexemeIndex] = '\0';
                    addNode(lexemeBuffer, "RIGHTPAREN_TOKEN", &nodeHead, &nodeTail);
                    break;
                case '{':
                    lexemeBuffer[lexemeIndex] = '\0';
                    addNode(lexemeBuffer, "LEFTBRACE_TOKEN", &nodeHead, &nodeTail);
                    break;
                case '}':
                    lexemeBuffer[lexemeIndex] = '\0';
                    addNode(lexemeBuffer, "RIGHTBRACE_TOKEN", &nodeHead, &nodeTail);
                    break;
                case ';':
                    lexemeBuffer[lexemeIndex] = '\0';
                    addNode(lexemeBuffer, "SEMICOLON_TOKEN", &nodeHead, &nodeTail);
                    break;
                default:
                    lexemeBuffer[lexemeIndex] = '\0';  // Null terminate even for error
                    fprintf(stderr, "Lexical Error: Unrecognized character '%c'\n", c);
                    break;
            }
        }
    }

    // Close the file now that we are done reading
    fclose(file);

    // Print the result
    Node* current = nodeHead;
    while (current != NULL) {
        if (strcmp(current->type, "TYPE_TOKEN") == 0 && strcmp(current->value, "int") == 0) {
            printf("int: %s\n", current->type);
        } else {
            printf("%s: %s\n", current->value, current->type);
        }
        current = current->next;
    }

    // Free the allocated memory
    freeList(nodeHead); 

    return 0;
}