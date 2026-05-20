#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    int id;
    char name[50];
    char type[30];
    char brand[30];
    char model[30];
    char ip[16];
    char mac[18];
    char location[50];
    char status[20];
    char last_verification[20];
} Equipment;

typedef struct Node {
    Equipment date;
    struct Node* next;
} Node;

int nextEquipementId = 1;

void readString(char *variable, int size) {
    fgets(variable, size, stdin);
    size_t len = strlen(variable);
    if (len > 0 && variable[len - 1] == '\n') {
        variable[len - 1] = '\0';
    } else {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
}

int main() {
    Node* equipmentList = NULL;
    int option;

    do {
        printf("\n=========================================");
        printf("\n       MINI NOC SYSTEM - MAIN MENU");
        printf("\n=========================================");
        printf("\n 0. Exit Program");
        printf("\n=========================================");
        printf("\nChoose an option: ");
        
        if(scanf("%d", &option) != 1) {
            printf("Invalid input. Please enter a number.\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }
        getchar();

        switch (option) {
            case 0:
                printf("Exiting program. Goodbye!\n");
                break;
            default:
                printf("Invalid option. Please try again.\n");
                break;
        }
    } while (option != 0);

    return 0;
}