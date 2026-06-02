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
    char last_verification[30];
} Equipment;

typedef struct Node {
    Equipment data;
    struct Node* next;
} Node;

typedef struct {
    int responded;
} PingResult;

typedef enum {WINDOWS, LINUX, MACOS } OS_TYPE;
OS_TYPE currentOS = WINDOWS;

typedef struct SensorReading {
    char code[20];
    char type[50];
    float value;
    char unit[5];
    char status[20];
    struct SensorReading* next;
} SensorReading;

typedef struct {
    SensorReading* top;
} SensorStack;



void clearScreen() {
    if (currentOS == WINDOWS) {
        system("cls");
    } else {
        system("clear");
    }
}

typedef struct TechnicalIncident {
    int ticketId;
    char targetCode[30];
    char type[20];
    char description[100];
    char priority[10];
    char timestamp[20];
    char technician[40];
    char status[15];
    struct TechnicalIncident* next;
} TechnicalIncident;

typedef struct {
    TechnicalIncident* front;
    TechnicalIncident* rear;
} IncidentQueue;

void getCurrentDateTime(char* buffer) {
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    strftime(buffer, 20, "%d-%m-%Y %H:%M", tm_info);
}

OS_TYPE detectOperatingSystem() {
    #if defined(_WIN32) || defined(_WIN64)
        return WINDOWS;
    #elif defined(__APPLE__) || defined(__MACH__)
        return MACOS;
    #elif defined(__linux__)
        return LINUX;
    #else
        return LINUX;
    #endif
}

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
    sprintf(newNode->data.last_verification, "%02d/%02d/%04d %02d:%02d", 
            tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
            tm.tm_hour, tm.tm_min);

    if (list == NULL || newNode->data.id < list->data.id) {
        newNode->next = list;
        return newNode;
    }
    else {
        Node* current = list;
        while (current->next != NULL && current->next->data.id < newNode->data.id) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }

    printf("\n>>> Equipment '%s' successfully registered on %s!\n", newNode->data.name, newNode->data.last_verification);
    return list;
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
    sprintf(current->data.last_verification, "%02d/%02d/%04d %02d:%02d", 
            tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
            tm.tm_hour, tm.tm_min);
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
    sprintf(current->data.last_verification, "%02d/%02d/%04d %02d:%02d", 
            tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
            tm.tm_hour, tm.tm_min);

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

void displayInventoryDashboard(Node* list) {
    int total = 0, operational = 0, faulty = 0, maintenance = 0, deactivated = 0;
    Node* current = list;

    while (current != NULL) {
        total++;
        if (strcmp(current->data.status, "Operational") == 0) {
            operational++;
        } else if (strcmp(current->data.status, "Faulty") == 0) {
            faulty++;
        } else if (strcmp(current->data.status, "Under Maintenance") == 0) {
            maintenance++;
        } else if (strcmp(current->data.status, "Deactivated") == 0) {
            deactivated++;
        }
        current = current->next;
    }

    printf("\n==================================================");
    printf("\n     MINI NOC - INFRASTRUCTURE HEALTH DASHBOARD   ");
    printf("\n==================================================");
    printf("\n  Total Monitored Assets:   %d", total);
    printf("\n ------------------------------------------------");
    printf("\n  [Active] Operational:     %d", operational);
    printf("\n  [Warning] Maintenance:    %d", maintenance);
    printf("\n  [Alert] Faulty / Down:    %d", faulty);
    printf("\n  [Offline] Deactivated:    %d", deactivated);
    printf("\n ------------------------------------------------");

    if (total > 0) {
        float integrityRate = ((float)operational / total) * 100;
        printf("\n  Overall Network Integrity: %.2f%%", integrityRate);

        if (integrityRate < 70.0) {
            printf("\n  [SECURITY ALERT] Critical number of offline assets!");
        } else {
            printf("\n  [SYSTEM STATUS] Infrastructure operating normally.");
        }
    } else {
        printf("\n  Network Integrity Rate:   0.0%% (No assets registered)");
    }
    printf("\n==================================================\n");
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
        printf("4. Search Equipment(ID, IP, MAC)\n");
        printf("5. Display Inventory Dashboard\n");
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
                clearScreen();
                listAllEquipment(list);
                break;
            case 2:
                clearScreen();
                listEquipmentByType(list);
                break;
            case 3:
                clearScreen();
                listEquipmentByStatus(list);
                break;
            case 4:
                clearScreen();
                searchEquipment(list);
                break;
            case 5:
                clearScreen();
                displayInventoryDashboard(list);
                break;
            case 0:
                clearScreen();
                printf("\nReturning to inventory menu...\n");
                break;
            default:
                clearScreen();
                printf("\nInvalid option. Please try again.\n");
                break;
        }
    } while (option != 0);
}

void saveInventoryToBinary(Node* list) {
    FILE* file = fopen("inventory.dat", "wb");
    if (file == NULL) {
        printf("\nError opening file for writing.\n");
        return;
    }

    Node* current = list;
    int count = 0;

    while (current != NULL) {
        fwrite(&current->data, sizeof(Equipment), 1, file);
        count++;
        current = current->next;
    }

    fclose(file);
    printf("\n>>> Success! %d assets securely backed up to 'inventory.dat'.\n", count);
}

Node* loadInventoryFromBinary(Node* list) {
    FILE* file = fopen("inventory.dat", "rb");
    if (file == NULL) {
        printf("\nNo backup file found. Starting with an empty inventory.\n");
        return list;
    }

    Equipment tempEquipment;
    int count = 0;

    while (fread(&tempEquipment, sizeof(Equipment), 1, file) == 1) {
        Node* newNode = (Node*)malloc(sizeof(Node));
        if (newNode == NULL) {
            printf("\nMemory allocation failed while loading inventory.\n");
            fclose(file);
            return list;
        }
        newNode->data = tempEquipment;
        newNode->next = list;

        if (tempEquipment.id >= nextEquipementId) {
            nextEquipementId = tempEquipment.id + 1;
        }
        
        if (list == NULL || newNode->data.id < list->data.id) {
            newNode->next = list;
            list = newNode;
        } else {
            Node* current = list;
            while (current->next != NULL && current->next->data.id < newNode->data.id) {
                current = current->next;
            }
            newNode->next = current->next;
            current->next = newNode;
        }
        count++;
    }

    fclose(file);
    printf("\n>>> Success! %d assets loaded from 'inventory.dat' into inventory.\n", count);
    return list;
}

void factoryResetInventory(Node** list) {
    char password[20];
    printf("\n==================================================");
    printf("\n   [CRITICAL] SYSTEM FACTORY RESET GATEWAY");
    printf("\n==================================================");
    printf("\nEnter Admin Password to wipe system: ");
    readString(password, sizeof(password));

    if (strcmp(password, "admin123") != 0) {
        printf("\nInvalid password. Factory reset aborted.\n");
        return;
    }

    char confirmation;
    printf("\n>>> WARNING: This action will permanently delete ALL inventory data and cannot be undone.\n");
    printf("Are you sure you want to proceed? (y/n): ");
    scanf("%c", &confirmation);
    while (getchar() != '\n');

    if (confirmation != 'y' && confirmation != 'Y') {
        printf("\nFactory reset cancelled. No data was harmed.\n");
        return;
    }

    Node* current = *list;
    int count = 0;
    while (current != NULL) {
        Node* nextNode = current->next;
        free(current);
        current = nextNode;
        count++;
    }

    *list = NULL;

    extern int nextEquipementId;
    nextEquipementId = 1;

    remove("inventory.dat");

    printf("\n>>> Factory reset successful! %d assets deleted and system restored to default state.\n", count);
    printf(">>> The inventory is now empty and ready for new equipment registration.\n");
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
        printf("\n 6. Save Inventory to Backup File");
        printf("\n 7. [ADMIN] Factory Reset Inventory");
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
                clearScreen();
                list = addEquipment(list);
                break;
            case 2:
                clearScreen();
                list = removeEquipment(list);
                break;
            case 3:
                clearScreen();
                editEquipment(list);
                break;
            case 4:
                clearScreen();
                changeEquipmentStatus(list);
                break;
            case 5:
                clearScreen();
                menuReportsAndSearches(list);
                break;
            case 6:
                clearScreen();
                saveInventoryToBinary(list);
                break; 
            case 7:
                clearScreen();
                factoryResetInventory(&list);
                break;
            case 0:
                clearScreen();
                printf("\nReturning to main menu...\n");
                break;
            default:
                clearScreen();
                printf("\nInvalid option. Please try again.\n");
                break;
        }
    } while (option != 0);

    return list;
}

void writeToMonitorizationLog(int assetID, const char* ip, int success) {
    FILE* logFile = fopen("log_monitorization.txt", "a");
    if (logFile == NULL) {
        printf("Could not open log file!\n");
        return;
    }

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    fprintf(logFile, "[%02d/%02d/%04d %02d:%02d:%02d] TEST - Asset ID: %d | IP: %s | RESULT: %s\n",
            tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
            tm.tm_hour, tm.tm_min, tm.tm_sec,
            assetID, ip, (success ? "UP (Responded)" : "DOWN (No Response)"));
    
    if (!success) {

        static int incidentCounter = 5000;
        incidentCounter++;

        fprintf(logFile, "   ↳ [CRITICAL INCIDENT #INC-%d]\n", incidentCounter);
        fprintf(logFile, "     STATUS: OPEN | SEVERITY: HIGH\n");
        fprintf(logFile, "     ACTION REQUIRED: Check routing or physical connectivity for Asset ID %d.\n", assetID);
        fprintf(logFile, "   ----------------------------------------------------------------------\n");

        printf("Asset ID %d failed! Ticket #INC-%d generated in logs.\n", assetID, incidentCounter);     
    }
    fclose(logFile);
}

void enqueuePingFailureIncident(IncidentQueue* q, const char* failedIP, const char* deviceName) {
    TechnicalIncident* newNode = (TechnicalIncident*)malloc(sizeof(TechnicalIncident));
    if (newNode == NULL) return;

    static int netCounter = 2001;
    newNode->ticketId = netCounter++;

    if (deviceName != NULL && strlen(deviceName) > 0) {
        strcpy(newNode->targetCode, deviceName);
    } else {
        strcpy(newNode->targetCode, failedIP);
    }

    strcpy(newNode->type, "PING_FAILURE");
    sprintf(newNode->description, "CRITICAL: Device unreachable. ICMP echo request timed out for IP %s.", failedIP);
    strcpy(newNode->priority, "HIGH");
    strcpy(newNode->technician, "Network Operations Center");
    
    getCurrentDateTime(newNode->timestamp);
    strcpy(newNode->status, "Pending");
    newNode->next = NULL;

    if (q->rear == NULL) {
        q->front = q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
    
    printf("\n[AUTOMATED ALERT] Created Incident Ticket #%d for offline equipment: %s\n", 
           newNode->ticketId, newNode->targetCode);
}

PingResult processAssetPing(Node* target, IncidentQueue* q) {
    PingResult result;
    result.responded = 0;

    if (target == NULL) return result;

    printf("\n--------------------------------------------------");
    printf("\n[NOC] Testing connectivity to: %s (%s)", target->data.name, target->data.ip);
    printf("\n[NOC] Sending 2 ICMP echo requests... Please wait.");
    printf("\n--------------------------------------------------\n");

    char command[150];
    if (currentOS == WINDOWS) {
        sprintf(command, "ping -n 2 %s > resultado_ping.txt", target->data.ip);
    } else {
        sprintf(command, "ping -c 2 %s > resultado_ping.txt", target->data.ip);
    }
    system(command);

    FILE* file = fopen("resultado_ping.txt", "r");
    if (file != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            if (strstr(line, "TTL=") != NULL || strstr(line, "ttl=") != NULL || strstr(line, "bytes from") != NULL) {
                result.responded = 1;
                break;
            }
        }
        fclose(file);
    }

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(target->data.last_verification, "%02d/%02d/%04d %02d:%02d", 
            tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
            tm.tm_hour, tm.tm_min);

    if (result.responded) {
        strcpy(target->data.status, "Operational");
        printf("Asset ID %d responded successfully. Status: Operational.\n", target->data.id);
    } else {
        strcpy(target->data.status, "Faulty");
        printf("CRITICAL: Asset ID %d failed to respond! Status updated to: Faulty.\n", target->data.id);

        enqueuePingFailureIncident(q, target->data.ip, target->data.name);
    }

    writeToMonitorizationLog(target->data.id, target->data.ip, result.responded);

    return result;
}

void runGeneralNetworkTest(Node* list, IncidentQueue* q) {
    if (list ==NULL) {
        printf("\nThe inventory is empty. No assets available for testing.\n");
        return;
    }

    printf("\n=========================================");
    printf("\n     RUNNING GENERAL NETWORK PING TEST   ");
    printf("\n=========================================");

    Node* current = list;
    int totalTested = 0;
    int onlineAssets = 0;

    while (current != NULL) {
        totalTested++;
        PingResult r = processAssetPing(current, q);
        if (r.responded) {
            onlineAssets++;
        }
        current = current->next;
    }

    float avaliability = ((float)onlineAssets / totalTested) * 100;

    printf("\n=========================================");
    printf("\n       NETWORK TEST COMPLETED            ");
    printf("\n=========================================");
    printf("\n  Total Assets Checked: %d", totalTested);
    printf("\n  Devices Online:       %d", onlineAssets);
    printf("\n  Devices Offline:      %d", totalTested - onlineAssets);
    printf("\n  Overall SLA Health:   %.2f%%", avaliability);
    printf("\n=========================================\n");
}

void runLocalNetworkDiagnostic() {
    printf("\n=========================================");
    printf("\n     RUNNING LOCAL NETWORK DIAGNOSTIC     ");
    printf("\n=========================================");
    
    if (currentOS == WINDOWS) {
        system("ipconfig > resultado_rede_local.txt");
    } else {
        system("ip a > resultado_rede_local.txt 2>/dev/null || ifconfig > resultado_rede_local.txt");
    }

    printf("\nLocal network profile exported to 'resultado_rede_local.txt'.\n");
}

void runArpCacheDiagnostic() {
    printf("\n=========================================");
    printf("\n     RUNNING ARP CACHE DIAGNOSTIC         ");
    printf("\n=========================================");
    
    system("arp -a > resultado_arp.txt");

    printf("\nARP cache exported to 'resultado_arp.txt'.\n");
}

void runDnsLookupDiagnostic() {
    char domain[100];
    char command[150];

    printf("\n=========================================");
    printf("\n     RUNNING DNS LOOKUP DIAGNOSTIC        ");
    printf("\n=========================================");

    printf("\nEnter domain name (e.g., www.google.com, facebook.com): ");
    scanf("%99s", domain);

    sprintf(command, "nslookup %s > resultado_dns.txt", domain);

    printf("\n[NOC] Querying DNS servers for '%s'...", domain);

    system(command);

    printf("\nDNS lookup results for '%s' exported to 'resultado_dns.txt'.\n", domain);

}

void runRouteTracerDiagnostic() {
    char target[100];
    char command[150];

    printf("\n=========================================");
    printf("\n     RUNNING ROUTE TRACER DIAGNOSTIC      ");
    printf("\n=========================================");

    printf("\nEnter target IP or Domain (e.g., 1.1.1.1, ipv.pt): ");
    scanf("%99s", target);

    printf("\nTracing route to '%s'. Please wait (this may take a minute)...", target);

    if (currentOS == WINDOWS) {
        sprintf(command, "tracert %s > resultado_rota.txt", target);
    } else {
        sprintf(command, "traceroute %s > resultado_rota.txt 2>/dev/null || tracepath %s > resultado_rota.txt", target, target);
    }
    system(command);

    printf("\nRoute tracer results for '%s' exported to 'resultado_rota.txt'.\n", target);
}

void initSensorStack(SensorStack* s) {
    s->top = NULL;
}

void initIncidentQueue(IncidentQueue* q) {
    q->front = NULL;
    q->rear = NULL;
}

void pushSensorReading(SensorStack* s, SensorReading data) {
    SensorReading* newNode = (SensorReading*)malloc(sizeof(SensorReading));
    if (newNode != NULL) {
        *newNode = data;
        newNode->next = s->top;
        s->top = newNode;
    }
}

void enqueueSensorIncident(IncidentQueue* q, const char* sensorCode, const char* status) {
    TechnicalIncident* newNode = (TechnicalIncident*)malloc(sizeof(TechnicalIncident));
    if (newNode == NULL) return;
    
    static int sensorTicketCounter = 7001; 
    newNode->ticketId = sensorTicketCounter++;
    
    strcpy(newNode->targetCode, sensorCode);
    strcpy(newNode->type, "SENSOR_ANOMALY");
    
    sprintf(newNode->description, "ALERT: Sensor telemetry out of bounds. Current state marked as %s.", status);
    
    if (strcmp(status, "Critical") == 0 || strcmp(status, "CRITICAL") == 0) {
        strcpy(newNode->priority, "HIGH");
    } else {
        strcpy(newNode->priority, "MEDIUM");
    }
    
    strcpy(newNode->technician, "Automation Bot");
    
    getCurrentDateTime(newNode->timestamp);
    strcpy(newNode->status, "Pending");
    newNode->next = NULL;

    if (q->rear == NULL) { 
        q->front = q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }

    printf("\n[AUTOMATED ALERT] Created Incident Ticket #%d for sensor anomaly: %s (%s)\n", 
           newNode->ticketId, newNode->targetCode, status);
}

void importSensorReadings(SensorStack* s, IncidentQueue* q) {

    FILE* file = fopen("sensors_rack.txt", "r");
    if (file == NULL) {
        printf("\n[ERROR] Could not open 'sensors_rack.txt'. Ensure API download worked.\n");
        return;
    }

    FILE* regFile = fopen("log_sensores.txt", "w");

    char line[256];
    int importedCount = 0;
    int incidentCount = 0;

    while (fgets(line, sizeof(line), file)) {
        
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }
        
        if (strncmp(line, "CODE", 4) == 0 || strncmp(line, "code", 4) == 0) {
            continue;
        }

        SensorReading temp;

        char* token = strtok(line, ";\n\r");
        if (token != NULL) {
            strcpy(temp.code, token);
        } else continue;

        token = strtok(NULL, ";\n\r");
        if (token != NULL) strcpy(temp.type, token);

        token = strtok(NULL, ";\n\r");
        if (token != NULL) temp.value = atof(token);

        token = strtok(NULL, ";\n\r");
        if (token != NULL) strcpy(temp.unit, token);

        token = strtok(NULL, ";\n\r");
        if (token != NULL) strcpy(temp.status, token);

        pushSensorReading(s, temp);
        importedCount++;

        if (regFile != NULL) {
            fprintf(regFile, "Processed Sensor: %s | Value: %.2f %s | Status: %s\n", 
                    temp.code, temp.value, temp.unit, temp.status);
        }

        if (strcmp(temp.status, "WARNING") == 0 || 
            strcmp(temp.status, "CRITICAL") == 0 || 
            strcmp(temp.status, "GRID_FAILURE") == 0) {
            
            enqueueSensorIncident(q, temp.code, temp.status);
            incidentCount++;
        }
    }

    fclose(file);
    if (regFile != NULL) {
        fprintf(regFile, "\n==================================================\n");
        fprintf(regFile, "Total Records Loaded: %d | Total Alarms Enqueued: %d\n", importedCount, incidentCount);
        fclose(regFile);
    }

    printf("\n==================================================");
    printf("\n             FILE INGESTION COMPLETED             ");
    printf("\n==================================================");
    printf("\n  Readings Saved to Stack: %d", importedCount);
    printf("\n  Incidents Pushed to Queue: %d", incidentCount);
    printf("\n  Audit log saved to 'log_sensores.txt'");
    printf("\n==================================================\n");
}

int fetchSensorDataFromAPI() {
    printf("Connecting to http://sensorlab.innominatum.pt ...\n ");

    const char* cmd = "curl -s https://sensorlab.innominatum.pt/v1/sensors/export/legacy > C:\\Users\\asdia\\Desktop\\P1\\TP1\\sensors_rack.txt";

    int status = system(cmd);

    if (status == 0) {
        printf("Live sensor data synchronized successfully from API!\n");
        return 1;
    } else {
        printf("API unreachable. Falling back to cached local data.\n");
        return 0;
    }
}

void menuConnectivity(Node* list, IncidentQueue* q) {
    int option;
    do {
        printf("\n=========================================");
        printf("\n   MINI NOC SYSTEM - CONNECTIVITY TESTS");
        printf("\n=========================================");
        printf("\n 1. Ping a Specific Asset IP");
        printf("\n 2. Run General Network Test (Ping Sweep)");
        printf("\n 3. Run Local Network Diagnostic (ipconfig)");
        printf("\n 4. Run ARP Cache Diagnostic (arp -a)");
        printf("\n 5. Run DNS Lookup Diagnostic (nslookup)");
        printf("\n 6. Run Route Tracer Diagnostic (tracert)");
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
            case 1: {
                clearScreen();
                if (list == NULL) {
                    printf("\nThe inventory is empty. No assets available for testing.\n");
                    break;
                }
                listAllEquipment(list);
                int searchId;
                printf("\nEnter the ID of the asset to ping: ");
                if (scanf("%d", &searchId) == 1) {
                    getchar();

                    Node* current = list;
                    while (current != NULL && current->data.id != searchId) {
                        current = current->next;
                    }

                    if (current != NULL) {
                        processAssetPing(current, q);
                    } else {
                        printf("\n>>> No equipment found with ID %d.\n", searchId);
                    }
                }
                break;
            }
            case 2:
                clearScreen();
                runGeneralNetworkTest(list, q);
                break;
            case 3:
                clearScreen();
                runLocalNetworkDiagnostic();
                break;
            case 4:
                clearScreen();
                runArpCacheDiagnostic();
                break;
            case 5:
                clearScreen();
                runDnsLookupDiagnostic();
                break;
            case 6:
                clearScreen();
                runRouteTracerDiagnostic();
                break;
            case 0:
                clearScreen();
                printf("\nReturning to main menu...\n");
                break;
            default:
                clearScreen();
                printf("\nInvalid option. Please try again.\n");
                break;
        }
    } while (option != 0);
}

void listRecentReadings(SensorStack* s) {
    if (s->top == NULL) {
        printf("\nNo sensor readings available in the stack.\n");
        return;
    }

    printf("\n=========================================================================");
    printf("\n                      RECENT SENSOR READINGS (LIFO)                      ");
    printf("\n=========================================================================");
    printf("\n%-15s %-25s %-10s %-10s %-15s", "CODE", "TYPE", "VALUE", "UNIT", "STATUS");
    printf("\n-------------------------------------------------------------------------");

    SensorReading* current = s->top;
    while (current != NULL) {
        printf("\n%-15s %-25s %-10.2f %-10s %-15s", current->code, current->type, current->value, current->unit, current->status);
        current = current->next;
    }

    printf("\n=========================================================================\n");

}

void searchSensorByCode(SensorStack* s) {
    if (s->top == NULL) {
        printf("\nNo sensor readings available in the stack.\n");
        return;
    }

    char searchCode[20];

    printf("Enter Sensor Code to search (e.g., TEMP_RACK, UPS_BAT): ");
    scanf("%19s", searchCode);
    while (getchar() != '\n');

    clearScreen();

    SensorReading* current = s->top;
    int foundCount = 0;

    printf("\n=========================================================================");
    printf("\n                      SEARCH RESULTS FOR: %s", searchCode);
    printf("\n=========================================================================");
    printf("\n%-15s %-25s %-10s %-10s %-15s", "CODE", "TYPE", "VALUE", "UNIT", "STATUS");
    printf("\n-------------------------------------------------------------------------");

    while (current != NULL) {
        if (strcasecmp(current->code, searchCode) == 0) {
            printf("\n%-15s %-25s %-10.2f %-10s %-15s", current->code, current->type, current->value, current->unit, current->status);
            foundCount++;
        }
        current = current->next;
    }

    printf("\n=========================================================================");
    if (foundCount == 0) {
        printf("\n>>> No sensor readings found with code '%s'.\n", searchCode);
    } else {
        printf("\n>>> Total readings found with code '%s': %d\n", searchCode, foundCount);
    }
    printf("\n=========================================================================\n");
}

void displayAnomalousReadings(SensorStack* s) {
    if (s->top == NULL) {
        printf("\nNo sensor readings available in the stack.\n");
        return;
    }

    SensorReading* current = s->top;
    int anomalyCount = 0;

    printf("\n=========================================================================");
    printf("\n                    ACTIVE SENSOR ANOMALIES DETECTED                     ");
    printf("\n=========================================================================");
    printf("\n%-15s %-25s %-10s %-10s %-15s", "CODE", "TYPE", "VALUE", "UNIT", "STATUS");
    printf("\n-------------------------------------------------------------------------");

    while (current != NULL) {
        if (strcmp(current->status, "WARNING") == 0 ||
            strcmp(current->status, "CRITICAL") == 0 ||
            strcmp(current->status, "GRID_FAILURE") == 0) {

            printf("\n%-15s %-25s %-10.2f %-10s %-15s", current->code, current->type, current->value, current->unit, current->status);
            anomalyCount++;
    }
    current = current->next;
    }

    printf("\n=========================================================================");
    if (anomalyCount == 0) {
        printf("\n>>> No active sensor anomalies detected in the stack.\n");
    } else {
        printf("\n>>> Total anomalous readings detected: %d\n", anomalyCount);
    }
    printf("\n=========================================================================\n");
}

void displayPendingIncidents(IncidentQueue* q) {
    if (q == NULL || q->front == NULL) {
        printf("\n=========================================================================");
        printf("\n                      INCIDENT MANAGEMENT QUEUE                          ");
        printf("\n=========================================================================");
        printf("\n[SUCCESS] No pending technical incidents in the queue.");
        printf("\n=========================================================================\n");
        return;
    }

    printf("\n=================================================================================================================");
    printf("\n                                         ACTIVE TECHNICAL INCIDENTS (FIFO)                                       ");
    printf("\n=================================================================================================================");
    printf("\n%-10s %-15s %-15s %-12s %-10s %-20s", "TICKET ID", "TARGET/ASSET", "TYPE", "PRIORITY", "STATUS", "TIMESTAMP");
    printf("\n-----------------------------------------------------------------------------------------------------------------");

    TechnicalIncident* current = q->front;
    while (current != NULL) {
        printf("\n#%-9d %-15s %-15s %-12s %-10s %-20s", 
               current->ticketId, current->targetCode, current->type, current->priority, current->status, current->timestamp);
        printf("\n  --> Description: %s", current->description);
        printf("\n  --> Assigned to: %s", current->technician);
        printf("\n-----------------------------------------------------------------------------------------------------------------");
        current = current->next;
    }
    printf("\n=================================================================================================================\n");
}    

void menuSensors(SensorStack* stack, IncidentQueue* queue) {
    int option = -1;

    do {
        printf("\n=========================================");
        printf("\n     MINI NOC SYSTEM - SENSORS    ");
        printf("\n=========================================");
        printf("\n  1. List All Recent Readings (Full Stack)");
        printf("\n  2. Search Readings by Sensor Code");
        printf("\n  3. Display Anomalous Readings Only");
        printf("\n  4. Display Pending Incident Tickets");
        printf("\n  0. Return to Main Menu");
        printf("\n=========================================");
        printf("\nSelect an option: ");
        
        if (scanf("%d", &option) != 1) {
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n'); 

        switch (option) {
            case 1:
                clearScreen();
                listRecentReadings(stack);
                break;
            case 2:
                clearScreen();
                searchSensorByCode(stack);
                break;
            case 3:
                clearScreen();
                displayAnomalousReadings(stack);
                break;
            case 4:
                clearScreen();
                displayPendingIncidents(queue);
                break;
            case 0:
                printf("\nReturning to Main Menu...\n");
                break;
            default:
                printf("\n[ERROR] Invalid choice. Try again.\n");
        }
    } while (option != 0);
}

int validateAssetExistence(Node* invHead, SensorStack* s, const char* userInput, char* foundCode) {
    Node* currentEq = invHead;
    while (currentEq != NULL) {
        if (strcmp(currentEq->data.name, userInput) == 0 || 
            strcmp(currentEq->data.ip, userInput) == 0) {
            strcpy(foundCode, currentEq->data.name);
        }
        currentEq = currentEq->next;
    }

    if (s != NULL && s->top != NULL) {
        SensorReading* currentSensor = s->top;
        while (currentSensor != NULL) {
            if (strcmp(currentSensor->code, userInput) == 0 || 
                strcmp(currentSensor->type, userInput) == 0) {
                strcpy(foundCode, currentSensor->code);
                return 1;
            }
            currentSensor = currentSensor->next;
        }
    }

    return 0;
}

void createManualIncident(IncidentQueue* q, Node* invHead, SensorStack* s) {
    char userInput[50];
    char officialCode[30];
    
    printf("\n=========================================");
    printf("\n        MANUAL INCIDENT REGISTRATION     ");
    printf("\n=========================================");
    
    printf("\nEnter Equipment/Sensor (Name, Code or IP): ");
    fgets(userInput, sizeof(userInput), stdin);
    userInput[strcspn(userInput, "\n")] = 0; // Remove o \n

    if (!validateAssetExistence(invHead, s, userInput, officialCode)) {
        printf("\n[ERROR] Asset '%s' could not be found by Name, Code or IP!", userInput);
        printf("\nTicket creation aborted. Please enter a valid registered asset.\n");
        printf("=========================================\n");
        return;
    }

    TechnicalIncident* newNode = (TechnicalIncident*)malloc(sizeof(TechnicalIncident));
    if (newNode == NULL) {
        printf("\n[ERROR] System memory allocation failure.\n");
        return;
    }

    static int seqCounter = 8001;
    newNode->ticketId = seqCounter++;
    
    strcpy(newNode->targetCode, officialCode);
    strcpy(newNode->type, "MANUAL");

    printf("Issue Description: ");
    fgets(newNode->description, sizeof(newNode->description), stdin);
    newNode->description[strcspn(newNode->description, "\n")] = 0;

    int pChoice = 3;
    printf("Priority Level (1 - High, 2 - Medium, 3 - Low): ");
    if (scanf("%d", &pChoice) != 1) pChoice = 3;
    while (getchar() != '\n');

    if (pChoice == 1) strcpy(newNode->priority, "HIGH");
    else if (pChoice == 2) strcpy(newNode->priority, "MEDIUM");
    else strcpy(newNode->priority, "LOW");

    printf("Assigned Technician: ");
    fgets(newNode->technician, sizeof(newNode->technician), stdin);
    newNode->technician[strcspn(newNode->technician, "\n")] = 0;

    getCurrentDateTime(newNode->timestamp);
    strcpy(newNode->status, "Pending");
    newNode->next = NULL;

    if (q->rear == NULL) {
        q->front = q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }

    printf("\n[SUCCESS] Ticket #%d pushed to the technical queue successfully!\n", newNode->ticketId);
    printf("=========================================\n");
}

void dequeueAndProcessIncident(IncidentQueue* q) {
    if (q == NULL || q->front == NULL) {
        printf("\n=========================================");
        printf("\n          INCIDENT DESPATCH CENTER       ");
        printf("\n=========================================");
        printf("\n[INFO] No pending incidents to process.\n");
        printf("=========================================\n");
        return;
    }

    TechnicalIncident* temp = q->front;

    printf("\n=========================================");
    printf("\n          PROCESSING TICKET #%d          ", temp->ticketId);
    printf("\n=========================================");
    printf("\nTarget/Asset:  %s", temp->targetCode);
    printf("\nAlert Type:    %s", temp->type);
    printf("\nPriority:      %s", temp->priority);
    printf("\nDescription:   %s", temp->description);
    printf("\nLogged Time:   %s", temp->timestamp);
    printf("\n-----------------------------------------");
    printf("\n[STATUS] Dispatching technician: %s", temp->technician);
    printf("\n[SUCCESS] Ticket solved and removed from active queue!");
    printf("\n=========================================\n");

    q->front = q->front->next;

    if (q->front == NULL) {
        q->rear = NULL;
    }

    free(temp);
}

void displayIncidentByStatus(IncidentQueue* q, const char* targetStatus, const char* title) {
    if (q == NULL || q->front == NULL) {
        printf("\n=========================================================================");
        printf("\n                      %s                          ", title);
        printf("\n=========================================================================");
        printf("\n[INFO] No incidents found.");
        printf("\n=========================================================================\n");
        return;
    }

    int count = 0;
    printf("\n=========================================================================");
    printf("\n                                         %s                                       ", title);
    printf("\n=========================================================================");
    printf("\n%-10s %-15s %-15s %-12s %-10s %-20s", "TICKET ID", "TARGET/ASSET", "TYPE", "PRIORITY", "STATUS", "TIMESTAMP");
    printf("\n-----------------------------------------------------------------------------------------------------------------");

    TechnicalIncident* current = q->front;
    while (current != NULL) {
        if (strcmp(current->status, targetStatus) == 0) {
            printf("\n#%-9d %-15s %-15s %-12s %-10s %-20s", 
                   current->ticketId, current->targetCode, current->type, current->priority, current->status, current->timestamp);
            printf("\n  --> Description: %s", current->description);
            printf("\n  --> Assigned to: %s", current->technician);
            printf("\n-----------------------------------------------------------------------------------------------------------------");
            count++;
        }
        current = current->next;
    }

    if (count == 0) {
        printf("\n[INFO] No incidents with status '%s' found.\n", targetStatus);
    }
    printf("\n=========================================================================\n");
}   

void displayIncidentsByAsset(IncidentQueue* q) {
    if (q == NULL || q->front == NULL) {
        printf("\n[INFO] No incidents in the queue.\n");
        return;
    }

    char targetInput[30];
    printf("\nInsert Asset Name, Code or IP: ");
    fgets(targetInput, sizeof(targetInput), stdin);
    targetInput[strcspn(targetInput, "\n")] = 0;

    printf("\n=================================================================================================================");
    printf("\n                                   INCIDENTS FOR ASSET: %s", targetInput);
    printf("\n=================================================================================================================");
    
    int count = 0;
    TechnicalIncident* current = q->front;
    while (current != NULL) {
        if (strcmp(current->targetCode, targetInput) == 0) {
            printf("\n#%-9d %-15s %-15s %-12s %-10s %-20s", 
                   current->ticketId, current->targetCode, current->type, current->priority, current->status, current->timestamp);
            printf("\n  --> Description: %s", current->description);
            printf("\n-----------------------------------------------------------------------------------------------------------------");
            count++;
        }
        current = current->next;
    }

    if (count == 0) {
        printf("\n[INFO] No incidents found for the specified asset.");
    }
    printf("\n=================================================================================================================\n");
}

void displayIncidentsByPriority(IncidentQueue* q) {
    if (q == NULL || q->front == NULL) {
        printf("\n[INFO] No incidents in the queue.\n");
        return;
    }

    int choice;
    char targetPriority[10] = "LOW";
    printf("\nSelect Priority to filter:\n1. HIGH\n2. MEDIUM\n3. LOW\nChoice: ");
    if (scanf("%d", &choice) != 1) choice = 3;
    while (getchar() != '\n');

    if (choice == 1) strcpy(targetPriority, "HIGH");
    else if (choice == 2) strcpy(targetPriority, "MEDIUM");

    printf("\n=================================================================================================================");
    printf("\n                                   INCIDENTS WITH PRIORITY: %s", targetPriority);
    printf("\n=================================================================================================================");

    int count = 0;
    TechnicalIncident* current = q->front;
    while (current != NULL) {
        if (strcmp(current->priority, targetPriority) == 0) {
            printf("\n#%-9d %-15s %-15s %-12s %-10s %-20s", 
                   current->ticketId, current->targetCode, current->type, current->priority, current->status, current->timestamp);
            printf("\n  --> Description: %s", current->description);
            printf("\n-----------------------------------------------------------------------------------------------------------------");
            count++;
        }
        current = current->next;
    }

    if (count == 0) {
        printf("\n[INFO] No incidents found with priority: %s", targetPriority);
    }
    printf("\n=================================================================================================================\n");
}

void menuIncidentReports(IncidentQueue* q) {
    int option = -1;
    do {
        printf("\n=========================================");
        printf("\n      SUBMENU - REPORTS & QUERIES        ");
        printf("\n=========================================");
        printf("\n  1. List All Pending Incidents");
        printf("\n  2. List All In Progress Incidents");
        printf("\n  3. List All Completed Incidents");
        printf("\n  4. Search Incidents by Asset");
        printf("\n  5. Search Incidents by Priority");
        printf("\n  0. Back to Incident Menu");
        printf("\n=========================================");
        printf("\nSelect an option: ");

        if (scanf("%d", &option) != 1) {
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        switch (option) {
            case 1:
                clearScreen();
                displayIncidentByStatus(q, "Pending", "PENDING INCIDENTS");
                break;
            case 2:
                clearScreen();
                displayIncidentByStatus(q, "In Progress", "IN PROGRESS INCIDENTS");
                break;
            case 3:
                clearScreen();
                displayIncidentByStatus(q, "Completed", "COMPLETED INCIDENTS");
                break;
            case 4:
                clearScreen();
                displayIncidentsByAsset(q);
                break;
            case 5:
                clearScreen();
                displayIncidentsByPriority(q);
                break;
            case 0:
                clearScreen();
                printf("\nReturning to Incident Menu...\n");
                break;
            default:
                clearScreen();
                printf("\nInvalid option. Please try again.\n");
                break;
        }
    } while (option != 0);
}

void menuIncidents(IncidentQueue* queue, Node* invHead, SensorStack* sensorStack) {
    int option = -1;

    do {
        printf("\n=========================================");
        printf("\n     MODULO 4 - INCIDENT MANAGEMENT     ");
        printf("\n=========================================");
        printf("\n  1. Log new Incident Manually");
        printf("\n  2. Run Automated Despatch");
        printf("\n  3. View Incident Reports");
        printf("\n  0. Return to Main Menu");
        printf("\n=========================================");
        printf("\nSelect an option: ");

        if (scanf("%d", &option) != 1) {
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        switch (option) {
            case 1:
                clearScreen();
                createManualIncident(queue, invHead, sensorStack);
                break;
            case 2:
                clearScreen();
                dequeueAndProcessIncident(queue);
                break;
            case 3:
                clearScreen();
                menuIncidentReports(queue);
                break;
            case 0:
                printf("\nReturning to Main Menu");
                break;
            default:
                printf("Invalid option. Please try again.\n");
        }

    } while (option != 0);  
}

int main() {
    Node* equipmentList = NULL;
    equipmentList = loadInventoryFromBinary(equipmentList);
    currentOS = detectOperatingSystem();

    if (currentOS == WINDOWS) {
        printf("\n>>> [SYSTEM] Windows Environment Detected Automatically.\n");
    } else if (currentOS == MACOS) {
        printf("\n>>> [SYSTEM] macOS Environment Detected Automatically.\n");
    } else {
        printf("\n>>> [SYSTEM] Linux Environment Detected Automatically.\n");
    }

    SensorStack sensorStack;
    IncidentQueue incidentQueue;
    initSensorStack(&sensorStack);
    
    incidentQueue.front = NULL;
    incidentQueue.rear = NULL;

    fetchSensorDataFromAPI();

    importSensorReadings(&sensorStack, &incidentQueue);

    int option;

    do {
        printf("\n=========================================");
        printf("\n       MINI NOC SYSTEM - MAIN MENU");
        printf("\n=========================================");
        printf("\n 1. Inventory Management");
        printf("\n 2. Connectivity & Network Tests");
        printf("\n 3. Sensor Data");
        printf("\n 4. Incident Managment");
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
                clearScreen();
                equipmentList = menuInventory(equipmentList);
                clearScreen();
                break;
            case 2:
                clearScreen();
                menuConnectivity(equipmentList, &incidentQueue);
                clearScreen();
                break;
            case 3:
                clearScreen();
                menuSensors(&sensorStack, &incidentQueue);
                clearScreen();
                break;
            case 4:
                clearScreen();
                menuIncidents(&incidentQueue, equipmentList, &sensorStack);
                break;
            case 0:
                clearScreen();
                printf("\nSaving session data to backup file...\n"); 
                saveInventoryToBinary(equipmentList);
                printf("\nExiting program. Goodbye!\n");
                break;
            default:
                clearScreen();
                printf("Invalid option. Please try again.\n");
                break;
        }
    } while (option != 0);

    return 0;
}