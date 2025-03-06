#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the node structure for our linked list
typedef struct Node {
    char character;
    int count;
    struct Node* next;
} Node;

// Function to find or create a node for a character in the linked list
Node* findOrCreate(Node** head, char c) {
    // If list is empty, create the first node
    if (*head == NULL) {
        *head = (Node*)malloc(sizeof(Node));
        if (*head == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
        (*head)->character = c;
        (*head)->count = 0;
        (*head)->next = NULL;
        return *head;
    }
    
    // First check if the character already exists
    Node* current = *head;
    while (current != NULL) {
        if (current->character == c) {
            return current;
        }
        current = current->next;
    }
    
    // If not found, add to the end of the list
    current = *head;
    while (current->next != NULL) {
        current = current->next;
    }
    
    // Create and append new node
    current->next = (Node*)malloc(sizeof(Node));
    if (current->next == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    
    current->next->character = c;
    current->next->count = 0;
    current->next->next = NULL;
    
    return current->next;
}

// Function to free the linked list
void freeList(Node* head) {
    Node* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

int main() {
    FILE* file = fopen("main.c", "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }
    
    Node* head = NULL;
    int c;
    
    // Read the file character by character
    while ((c = fgetc(file)) != EOF) {
        Node* node = findOrCreate(&head, (char)c);
        node->count++;
    }
    
    fclose(file);
    
    // Print the result
    Node* current = head;
    while (current != NULL) {
        printf("%c : %d\n", current->character, current->count);
        current = current->next;
    }
    
    // Free the allocated memory
    freeList(head);
    
    return 0;
}
