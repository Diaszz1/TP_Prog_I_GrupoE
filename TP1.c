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
    Equipment data;
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

Node* addEquipment(Node* list) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation failed. Unable to add equipment.\n");
        return list;
    }

    newNode->data.id = nextEquipementId++;

    printf("\n--- REGISTER NEW EQUIPMENT (ID: %d) ---\n", newNode->data.id);
    printf("Name: ");
    readString(newNode->data.name, sizeof(newNode->data.name));
    
    chooseEquipmentType(newNode->data.type);

    printf("Brand: ");
    readString(newNode->data.brand, sizeof(newNode->data.brand));
    printf("Model: ");
    readString(newNode->data.model, sizeof(newNode->data.model));
    printf("IP Address: ");
    readString(newNode->data.ip, sizeof(newNode->data.ip));
    printf("MAC Address: ");
    readString(newNode->data.mac, sizeof(newNode->data.mac));
    printf("Location: ");
    readString(newNode->data.location, sizeof(newNode->data.location));

// Set default status
    strcpy(newNode->data.status, "Operational");

// Set last verification date to current date
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(newNode->data.last_verification, "%02d/%02d/%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);

    newNode->next = list;

    printf("\n>>> Equipment '%s' successfully registered on %s!\n", newNode->data.name, newNode->data.last_verification);
    return newNode;
}

Node* removeEquipment(Node* list) {
    if (list == NULL) {
        printf("The inventory is empty.\n");
        return list;
    }

    displayEquipment(list);

    int targetId;
    printf("\n--- REMOVE EQUIPMENT ---\n");
    printf("Enter the ID of the equipment to remove: ");
    if(scanf("%d", &targetId) != 1) {
        printf("Invalid input. Please enter a number.\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return list;
    }
    getchar();

    Node* current = list;
    Node* previous = NULL;

    while (current != NULL && current->data.id != targetId) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("\n>>> Equipment with ID %d not found.\n", targetId);
        return list;
    }

    if (strcmp(current->data.status, "Operational") != 0) {
        printf("\n>>> Cannot remove equipment with ID %d because it is currently '%s'.\n", current->data.id, current->data.status);
        printf(">>> Only 'Operational' equipment can be removed from inventory.\n");
        return list;
    }

    if (previous == NULL) {
        list = current->next;
    } else {
        previous->next = current->next;
    }

    printf("\n>>> Equipment with ID %d removed successfully.\n", targetId);

    free(current);

    return list;
}

void displayEquipment(Node* list) {
    if (list ==NULL) {
        printf("\nCurrent inventory list is completely empty.\n");
        return;
    }

    printf("\n--- CURRENT EQUIPMENT INVENTORY ---\n");
    printf("%-5s | %-20s | %-15s | %-15s\n", "ID", "Name", "Type", "Status");
    printf("--------------------------------─────────────────────────────\n");

    Node* current = list;
    while (current != NULL) {
        printf("%-5d | %-20s | %-15s | %-15s\n", current->data.id, current->data.name, current->data.type, current->data.status);
        current = current->next;
    }
    printf("-------------------------------------------------------------\n");
}

void chooseEquipmentType(char *targetDestination) {
    int choice;

    do {
        printf("\n--- Select Equipment Type ---\n");
        printf("1. Router\n");
        printf("2. Switch\n");
        printf("3. Access Point\n");
        printf("4. Server or NAS\n");
        printf("5. Network Printer\n");
        printf("6. IP Camera\n");
        printf("7. Sensors\n");
        printf("8. UPS\n");
        printf("Select type option (1-8): ");

        if(scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            choice = -1;
        }
        getchar();

        switch (choice) {
            case 1:
                strcpy(targetDestination, "Router");
                break;
            case 2:
                strcpy(targetDestination, "Switch");
                break;
            case 3:
                strcpy(targetDestination, "Access Point");
                break;
            case 4:
                strcpy(targetDestination, "Server or NAS");
                break;
            case 5:
                strcpy(targetDestination, "Network Printer");
                break;
            case 6:
                strcpy(targetDestination, "IP Camera");
                break;
            case 7:
                strcpy(targetDestination, "Sensors");
                break;
            case 8:
                strcpy(targetDestination, "UPS");
                break;
            default:
                printf("Invalid option. Please select a number between 1 and 8.\n");
        }
    } while (choice < 1 || choice > 8);
}

Node* menuInventory(Node* list) {
    int option;

    do {
        printf("\n=========================================");
        printf("\n       MINI NOC SYSTEM - INVENTORY");
        printf("\n=========================================");
        printf("\n 1. Add New Equipment");
        printf("\n 2. Remove Equipment");
        printf("\n 0. Return to Main Menu");
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
            case 1:
                list = addEquipment(list);
                break;
            case 2:
                list = removeEquipment(list);
                break;
            case 0:
                printf("\nReturning to main menu...\n");
                break;
            default:
                printf("\nInvalid option. Please try again.\n");
                break;
        }
    } while (option != 0);

    return list;
}

int main() {
    Node* equipmentList = NULL;
    int option;

    do {
        printf("\n=========================================");
        printf("\n       MINI NOC SYSTEM - MAIN MENU");
        printf("\n=========================================");
        printf("\n 1. Inventory Management");
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
            case 1:
                equipmentList = menuInventory(equipmentList);
                break;
            case 0:
                printf("\nExiting program. Goodbye!\n");
                break;
            default:
                printf("Invalid option. Please try again.\n");
                break;
        }
    } while (option != 0);

    return 0;
}