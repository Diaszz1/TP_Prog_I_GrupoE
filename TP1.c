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
        int extraBytes = 0;
        while ((c = getchar()) != '\n' && c != EOF) {
            extraBytes++;
        }
        if (extraBytes > 0) {
            printf("Input exceeded limit. Automatically truncated to %d characters.\n", size - 1);
        }
    }
}


void displayEquipment(Node* list) {
    if (list ==NULL) {
        printf("\nCurrent inventory list is completely empty.\n");
        return;
    }

    printf("\n--- CURRENT EQUIPMENT INVENTORY ---\n");
    printf("%-5s | %-20s | %-15s | %-15s\n", "ID", "Name", "Type", "Status");
    printf("-----------------------------------------------------------------\n");

    Node* current = list;
    while (current != NULL) {
        printf("%-5d | %-20s | %-15s | %-15s\n", current->data.id, current->data.name, current->data.type, current->data.status);
        current = current->next;
    }
    printf("-----------------------------------------------------------------\n");
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

void chooseEquipmentStatus(char *targetDestination) {
    int choice;

    do {
        printf("\n--- Select Equipment Status ---\n");
        printf("1. Operational\n");
        printf("2. Faulty\n");
        printf("3. Under Maintenance\n");
        printf("4. Deactivated\n");
        printf("Select status option (1-4): ");
        
        if(scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            choice = -1;
        }
        getchar();

        switch (choice) {
            case 1:
                strcpy(targetDestination, "Operational");
                break;
            case 2:
                strcpy(targetDestination, "Faulty");
                break;
            case 3:
                strcpy(targetDestination, "Under Maintenance");
                break;
            case 4:
                strcpy(targetDestination, "Deactivated");
                break;
            default:
                printf("Invalid option. Please select a number between 1 and 4.\n");
        }
    } while (choice < 1 || choice > 4);
}

int isValidIP(const char *ip) {
    int num1, num2, num3, nunm4;
    char extra;

    if (sscanf(ip, "%d.%d.%d.%d%c", &num1, &num2, &num3, &nunm4, &extra) != 4) {
        return 0;
    }
    if (num1 < 0 || num1 > 255 || num2 < 0 || num2 > 255 || num3 < 0 || num3 > 255 || nunm4 < 0 || nunm4 > 255) {
        return 0;
    }
    return 1;
}

int isValidMAC(const char *mac) {
    int i;

    if (strlen(mac) != 17) {
        return 0;
    }

    for (i = 0; i < 17; i++) {
        if (i == 2 || i == 5 || i == 8 || i == 11 || i == 14) {
            if (mac[i] != ':' && mac[i] != '-') {
                return 0;
            }
        } else {
            char c = mac[i];
            if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
                return 0;
            }
        }
    }
    return 1;
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
    
    do {
        printf("IP Address (e.g., 192.168.1.50): ");
        readString(newNode->data.ip, sizeof(newNode->data.ip));
        if (!isValidIP(newNode->data.ip)) {
            printf("Invalid IPv4 format. Please enter a valid IP address");
        }
    } while (!isValidIP(newNode->data.ip));

    do {
        printf("MAC Address (e.g., 00:1A:2B:3C:4D:5E): ");
        readString(newNode->data.mac, sizeof(newNode->data.mac));
        if (!isValidMAC(newNode->data.mac)) {
            printf("Invalid MAC format (Use XX:XX:XX:XX:XX:XX). Please enter a valid MAC address\n");
        }
    } while (!isValidMAC(newNode->data.mac));

    printf("Location: ");
    readString(newNode->data.location, sizeof(newNode->data.location));

    chooseEquipmentStatus(newNode->data.status);

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

void editEquipment(Node* list) {
    if (list == NULL) {
        printf("The inventory is empty. Nothing to edit.\n");
        return;
    }

    displayEquipment(list);
    
    int targetId;
    printf("\n--- EDIT EQUIPMENT DETAILS---\n");
    printf("Enter the ID of the equipment to edit: ");
    if(scanf("%d", &targetId) != 1) {
        printf("Invalid ID Format:\n");
        while (getchar() != '\n');
        return;
    }
    getchar();

    Node* current = list;
    while (current != NULL && current->data.id != targetId) {
        current = current->next;
    }

    if (current == NULL) {
        printf("\n>>> Equipment with ID %d not found.\n", targetId);
        return;
    }

    printf("\nEditing details for equipment '%s' (ID: %d)\n", current->data.name, current->data.id);
    char choice;
    int dataChanged = 0;

    printf("\nCurrent Name: %s\n", current->data.name);
    printf("Change name? (y/n): ");
    scanf("%c", &choice); getchar();
    if (choice == 'y' || choice == 'Y') {
        printf("Enter New Name: ");
        readString(current->data.name, sizeof(current->data.name));
        dataChanged = 1;
    }

    printf("\nCurrent Type: %s\n", current->data.type);
    printf("Change type? (y/n): ");
    scanf("%c", &choice); getchar();
    if (choice == 'y' || choice == 'Y') {
        chooseEquipmentType(current->data.type);
        dataChanged = 1;
    }

    printf("\nCurrent Brand: %s\n", current->data.brand);
    printf("Change brand? (y/n): ");
    scanf("%c", &choice); getchar();
    if (choice == 'y' || choice == 'Y') {
        printf("Enter New Brand: ");
        readString(current->data.brand, sizeof(current->data.brand));
        dataChanged = 1;
    }

    printf("\nCurrent Model: %s\n", current->data.model);
    printf("Change model? (y/n): ");
    scanf("%c", &choice); getchar();
    if (choice == 'y' || choice == 'Y') {
        printf("Enter New Model: ");
        readString(current->data.model, sizeof(current->data.model));
        dataChanged = 1;
    }

    printf("\nCurrent IP Address: %s\n", current->data.ip);
    printf("Change IP address? (y/n): ");
    scanf("%c", &choice); getchar();
    if (choice == 'y' || choice == 'Y') {
        do {
            printf("Enter New IP Address (e.g., 192.168.1.1): ");
            readString(current->data.ip, sizeof(current->data.ip));
            if (!isValidIP(current->data.ip)) {
                printf("Invalid IPv4 format. Please enter a valid IP address.\n");
            }
        } while (!isValidIP(current->data.ip));
        dataChanged = 1;
    }

    printf("\nCurrent MAC Address: %s\n", current->data.mac);
    printf("Change MAC address? (y/n): ");
    scanf("%c", &choice); getchar();
    if (choice == 'y' || choice == 'Y') {
        do {
            printf("Enter New MAC Address (e.g., 00:1A:2B:3C:4D:5E): ");
            readString(current->data.mac, sizeof(current->data.mac));
            if (!isValidMAC(current->data.mac)) {
                printf("Invalid MAC format. Please enter a valid MAC address.\n");
            }
        } while (!isValidMAC(current->data.mac));
        dataChanged = 1;
    }

    printf("\nCurrent Location: %s\n", current->data.location);
    printf("Change location? (y/n): ");
    scanf("%c", &choice); getchar();
    if (choice == 'y' || choice == 'Y') {
        printf("Enter New Location: ");
        readString(current->data.location, sizeof(current->data.location));
        dataChanged = 1;
    }

    printf("\nCurrent Status: %s\n", current->data.status);
    printf("Change status? (y/n): ");
    scanf("%c", &choice); getchar();
    if (choice == 'y' || choice == 'Y') {
        chooseEquipmentStatus(current->data.status);
        dataChanged = 1;
    }

    if (dataChanged) {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        sprintf(current->data.last_verification, "%02d/%02d/%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
        printf("\n>>> Modifications saved! Verification timestamp updated to %s!\n", current->data.last_verification);
    } else {
        printf("\n>>> No changes made to equipment details.\n");
    }
}

void changeEquipmentStatus(Node* list) {
    if (list == NULL) {
        printf("The inventory is empty. No equipment to update.\n");
        return;
    }

    displayEquipment(list);

    int targetId;
    printf("\n--- CHANGE EQUIPMENT STATUS ---\n");
    printf("Enter the ID of the equipment to update: ");
    if(scanf("%d", &targetId) != 1) {
        printf("Invalid input. Please enter a number.\n");
        while (getchar() != '\n');
        return;
    }
    getchar();

    Node* current = list;
    while (current != NULL && current->data.id != targetId) {
        current = current->next;
    }

    if (current == NULL) {
        printf("\n>>> Equipment with ID %d not found.\n", targetId);
        return;
    }

    printf("\nCurrent Status of '%s' (ID: %d): %s\n", current->data.name, current->data.id, current->data.status);
    chooseEquipmentStatus(current->data.status);

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(current->data.last_verification, "%02d/%02d/%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);

    printf("\n>>> Status updated successfully! Verification timestamp updated to %s!\n", current->data.last_verification);
}

void listAllEquipment(Node* list) {
    if (list == NULL) {
        printf("\nCurrent inventory is completely empty. No assets to display.\n");
        return;
    }

    printf("\n==================================================\n");
    printf("           FULL EQUIPMENT INVENTORY               \n");
    printf("==================================================\n");

    Node* current = list;
    int count = 1;

    while (current != NULL) {
        printf("\n[%d] Asset ID: %d\n", count++, current->data.id);
        printf("--------------------------------------------------\n");
        printf("  Name:        %s\n", current->data.name);
        printf("  Type:        %s\n", current->data.type);
        printf("  Brand:       %s\n", current->data.brand);
        printf("  Model:       %s\n", current->data.model);
        printf("  IP Address:  %s\n", current->data.ip);
        printf("  MAC Address: %s\n", current->data.mac);
        printf("  Location:    %s\n", current->data.location);
        printf("  Status:      %s\n", current->data.status);
        printf("  Last Verify: %s\n", current->data.last_verification);
        printf("--------------------------------------------------\n");

        current = current->next;
    }
    printf("\n>>> End of asset report. Total equipment listed: %d\n", count - 1);
}

void listEquipmentByType(Node* list) {
    if (list == NULL) {
        printf("\nThe inventory is completely empty. No assets to display.\n");
        return;
    }

    char targetType[30];
    printf("\nSelect the type of equipment to list:\n");
    chooseEquipmentType(targetType);

    printf("\n==================================================\n");
    printf("         FILTERED REPORT: TYPE -> %s\n", targetType);
    printf("==================================================\n");

    Node* current = list;
    int count = 0;

    while (current != NULL) {
        // Se a string do tipo coincidir exatamente, imprime a ficha técnica
        if (strcmp(current->data.type, targetType) == 0) {
            count++;
            printf("\nAsset ID: %d | Name: %s\n", current->data.id, current->data.name);
            printf("--------------------------------------------------\n");
            printf("  Brand:       %s\n", current->data.brand);
            printf("  Model:       %s\n", current->data.model);
            printf("  IP Address:  %s\n", current->data.ip);
            printf("  MAC Address: %s\n", current->data.mac);
            printf("  Location:    %s\n", current->data.location);
            printf("  Status:      %s\n", current->data.status);
            printf("  Last Verify: %s\n", current->data.last_verification);
            printf("--------------------------------------------------\n");
        }
        current = current->next;
    }

    if (count == 0) {
        printf("\n>>> No equipment found registered under the type '%s'.\n", targetType);
    } else {
        printf("\n>>> End of filtered report. Total '%s' equipment listed: %d\n", targetType, count);
    }
}

void listEquipmentByStatus(Node* list) {
    if (list == NULL) {
        printf("\nThe inventory is completely empty. No assets to display.\n");
        return;
    }

    char targetStatus[30];

    printf("\nSelect the status of equipment to list:\n");
    chooseEquipmentStatus(targetStatus);

    printf("\n==================================================\n");
    printf("       FILTERED REPORT: STATUS -> %s\n", targetStatus);
    printf("==================================================\n");

    Node* current = list;
    int count = 0;

    while (current != NULL) {
        if (strcmp(current->data.status, targetStatus) == 0) {
            count++;
            printf("\nAsset ID: %d | Name: %s\n", current->data.id, current->data.name);
            printf("--------------------------------------------------\n");
            printf("  Type:        %s\n", current->data.type);
            printf("  Brand:       %s\n", current->data.brand);
            printf("  Model:       %s\n", current->data.model);
            printf("  IP Address:  %s\n", current->data.ip);
            printf("  MAC Address: %s\n", current->data.mac);
            printf("  Location:    %s\n", current->data.location);
            printf("  Last Verify: %s\n", current->data.last_verification);
            printf("--------------------------------------------------\n");
        }
        current = current->next;
    }

    if (count == 0) {
        printf("\n>>> No equipment found registered with the status '%s'.\n", targetStatus);
    } else {
        printf("\n>>> End of filtered report. Total '%s' equipment listed: %d\n", targetStatus, count);
    }
}

void searchEquipment(Node* list) {
    if (list == NULL) {
        printf("\nThe inventory is completely empty. No assets to search.\n");
        return;
    }

    int searchOption;
    printf("\n--- SEARCH ASSET GATEWAY ---\n");
    printf(" 1. Search by Asset ID (Code)\n");
    printf(" 2. Search by IP Address\n");
    printf(" 3. Search by MAC Address\n");
    printf("Select search option: ");

    if (scanf("%d", &searchOption) != 1) {
        while (getchar() != '\n');
        printf("\nInvalid criteria option.\n");
        return;
    }
    getchar();

    Node* current = list;
    int found = 0;

    if (searchOption == 1) {
        int targetId;
        printf("Enter target Asset ID: ");
        if (scanf("%d", &targetId) != 1) {
            while (getchar() != '\n');
            printf("\nInvalid Asset ID.\n");
            return;
        }
        getchar();

        while (current != NULL) {
            if (current->data.id == targetId) {
                found = 1;
                break;
            }
            current = current->next;
        }
    }

    else if (searchOption == 2) {
        char targetIP[16];
        printf("Enter target IP Address: ");
        readString(targetIP, sizeof(targetIP));

        while (current != NULL) {
            if (strcmp(current->data.ip, targetIP) == 0) {
                found = 1;
                break;
            }
            current = current->next;
        }
    }

    else if (searchOption == 3) {
        char targetMAC[18];
        printf("Enter target MAC Address: ");
        readString(targetMAC, sizeof(targetMAC));

        while (current != NULL) {
            if (strcmp(current->data.mac, targetMAC) == 0) {
                found = 1;
                break;
            }
            current = current->next;
        }
    }
    else {
        printf("\nInvalid search option.\n");
        return;
    }

    if (found && current != NULL) {
        printf("\n>>> Equipment found! Displaying details:\n");
        printf("--------------------------------------------------\n");
        printf("  Asset ID:     %d\n", current->data.id);
        printf("  Name:         %s\n", current->data.name);
        printf("  Type:         %s\n", current->data.type);
        printf("  Brand:        %s\n", current->data.brand);
        printf("  Model:        %s\n", current->data.model);
        printf("  IP Address:   %s\n", current->data.ip);
        printf("  MAC Address:  %s\n", current->data.mac);
        printf("  Location:     %s\n", current->data.location);
        printf("  Status:       %s\n", current->data.status);
        printf("  Last Verify:  %s\n", current->data.last_verification);
        printf("--------------------------------------------------\n");
    } else {
        printf("\n>>> No equipment found matching the search criteria.\n");
    }
}

void menuReportsAndSearches(Node* list) {
    int option;
    do {
        printf("\n=========================================\n");
        printf("        REPORTS & SEARCHES SUBMENU       \n");
        printf("=========================================\n");
        printf("1. List All Equipment\n");
        printf("2. List Equipment by Type\n");
        printf("3. List Equipment by Status\n");
        printf("4. Search Equipment\n");
        printf("0. Return to Inventory Menu\n");
        printf("=========================================\n");
        printf("Choose an option: ");

        if (scanf("%d", &option) != 1) {
            while (getchar() != '\n'); // Limpar buffer se digitarem letras
            option = -1;
        }
        getchar();

        switch (option) {
            case 1:
                listAllEquipment(list);
                break;
            case 2:
                listEquipmentByType(list);
                break;
            case 3:
                listEquipmentByStatus(list);
                break;
            case 4:
                searchEquipment(list);
                break;
            case 0:
                printf("\nReturning to inventory menu...\n");
                break;
            default:
                printf("\nInvalid option. Please try again.\n");
                break;
        }
    } while (option != 0);
}

Node* menuInventory(Node* list) {
    int option;

    do {
        printf("\n=========================================");
        printf("\n       MINI NOC SYSTEM - INVENTORY");
        printf("\n=========================================");
        printf("\n 1. Add New Equipment");
        printf("\n 2. Remove Equipment");
        printf("\n 3. Edit Equipment Details");
        printf("\n 4. Change Equipment Status");
        printf("\n 5. Reports & Searches Menu");
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
            case 3:
                editEquipment(list);
                break;
            case 4:
                changeEquipmentStatus(list);
                break;
            case 5:
                menuReportsAndSearches(list);
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