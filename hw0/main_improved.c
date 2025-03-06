#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ASCII_SIZE 128  // Standard ASCII has 128 characters

// Define the node structure for our linked list
typedef struct Node {
    char character;
    int count;
    struct Node* next;
} Node;

// Function to add a node to the linked list (in ASCII order)
void addNodeInOrder(Node** head, char c, int count) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    
    newNode->character = c;
    newNode->count = count;
    newNode->next = NULL;
    
    // If list is empty or new node should come before head
    if (*head == NULL || c < (*head)->character) {
        newNode->next = *head;
        *head = newNode;
        return;
    }
    
    // Find the node after which to insert the new node
    Node* current = *head;
    while (current->next != NULL && current->next->character < c) {
        current = current->next;
    }
    
    // Insert the new node
    newNode->next = current->next;
    current->next = newNode;
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
    
    // Use a direct access array for counting
    int charCounts[ASCII_SIZE] = {0};
    int c;
    
    // Read the file character by character - O(m) where m is file size
    while ((c = fgetc(file)) != EOF) {
        // Only count ASCII characters
        if (c >= 0 && c < ASCII_SIZE) {
            charCounts[c]++;
        }
    }
    
    fclose(file);
    
    // Build a linked list in ASCII order
    Node* head = NULL;
    for (int i = 0; i < ASCII_SIZE; i++) {
        if (charCounts[i] > 0) {
            addNodeInOrder(&head, (char)i, charCounts[i]);
        }
    }
    
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
