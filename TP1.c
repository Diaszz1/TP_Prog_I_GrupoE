#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CLR_CYAN    "\x1b[36m"
#define CLR_GREEN   "\x1b[32m"
#define CLR_RED     "\x1b[31m"
#define CLR_RESET   "\x1b[0m"
#define CLR_BOLD    "\x1b[1m"
#define CLR_YELLOW  "\x1b[33m"
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
    char closedtimestamp[20];
    char technician[40];
    char status[15];
    struct TechnicalIncident* next;
} TechnicalIncident;

typedef struct {
    TechnicalIncident* front;
    TechnicalIncident* rear;
} IncidentQueue;

typedef struct NetworkConfig {
    char equipmentCode[50];
    char configType[30];
    char oldValue[50];
    char newValue[50];
    char timestamp[30];
    char technician[50];
    struct NetworkConfig* next;
} NetworkConfig;

typedef struct {
    NetworkConfig* top;
} ConfigStack;

void getCurrentDateTime(char* buffer) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(buffer, "%02d/%02d/%04d %02d:%02d", 
            tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, 
            tm.tm_hour, tm.tm_min);
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

    if (list == NULL) {
        printf("\n\t" CLR_RED "[INFO] Current inventory list is completely empty." CLR_RESET "\n");
        return;
    }

    printf("\n\t" CLR_CYAN "--- CURRENT EQUIPMENT INVENTORY ---" CLR_RESET "\n\n");
    
    printf("\t" CLR_BOLD CLR_CYAN "%-5s | %-25s | %-15s | %-15s" CLR_RESET "\n", "ID", "Name", "Type", "Status");
    printf("\t-------------------------------------------------------------------------\n");

    Node* current = list;
    while (current != NULL) {
        printf("\t%-5d | %-25s | %-15s | ", current->data.id, current->data.name, current->data.type);
        
        if (strcmp(current->data.status, "Operational") == 0) {
            
            printf(CLR_GREEN "%-15s" CLR_RESET "\n", current->data.status);
        } else {
            printf(CLR_RED "%-15s" CLR_RESET "\n", current->data.status);
        }

        current = current->next;
    }
    printf("\t-------------------------------------------------------------------------\n");
}

void chooseEquipmentType(char *targetDestination) {
    int choice;
    do {
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "        " CLR_BOLD "SELECT EQUIPMENT TYPE" CLR_RESET "          " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "                                       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "1." CLR_RESET " Router                            " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "2." CLR_RESET " Switch                            " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "3." CLR_RESET " Access Point                      " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "4." CLR_RESET " Server or NAS                     " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "5." CLR_RESET " Network Printer                   " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "6." CLR_RESET " IP Camera                         " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "7." CLR_RESET " Sensors                           " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "8." CLR_RESET " UPS                               " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "                                       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t  Select type option (1-8) " CLR_BOLD "-->" CLR_RESET " ");

        if(scanf("%d", &choice) != 1) {
            printf("\n\t" CLR_RED "[ERROR] Invalid input. Please enter a number." CLR_RESET "\n");
            while (getchar() != '\n');
            printf("\n\tPress Enter to continue...");
            getchar();
            clearScreen();
            choice = -1;
            continue;
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
                clearScreen();
                printf("\n\t" CLR_RED "[WARNING] Invalid option. Select a number between 1 and 8." CLR_RESET "\n");
                printf("\n\tPress Enter to try again...");
                getchar();
                clearScreen();
                break;
        }
    } while (choice < 1 || choice > 8);
}

void chooseEquipmentStatus(char *targetDestination) {
    int choice;
    do {
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "        " CLR_BOLD "SELECT EQUIPMENT STATUS" CLR_RESET "        " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "                                       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "1." CLR_RESET " Operational                       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "2." CLR_RESET " Faulty                            " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "3." CLR_RESET " Under Maintenance                 " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "4." CLR_RESET " Deactivated                       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "                                       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t  Select status option (1-4) " CLR_BOLD "-->" CLR_RESET " ");

        if(scanf("%d", &choice) != 1) {
            printf("\n\t" CLR_RED "[ERROR] Invalid input. Please enter a number." CLR_RESET "\n");
            while (getchar() != '\n');
            printf("\n\tPress Enter to continue...");
            getchar();
            clearScreen();
            choice = -1;
            continue;
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
                clearScreen();
                printf("\n\t" CLR_RED "[WARNING] Invalid option. Select a number between 1 and 4." CLR_RESET "\n");
                printf("\n\tPress Enter to try again...");
                getchar();
                clearScreen();
                break;
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
        printf("\n\t" CLR_RED "[ERROR] Memory allocation failed. Unable to add equipment." CLR_RESET "\n");
        return list;
    }

    newNode->data.id = nextEquipementId++;

    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "      " CLR_BOLD "REGISTER NEW EQUIPMENT" CLR_RESET "           " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "| " CLR_RESET "     " CLR_BOLD "ASSET GENERATED ID:" CLR_RESET " " CLR_GREEN "%-3d" CLR_RESET "          " CLR_CYAN "|" CLR_RESET, newNode->data.id);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET "\n");

    printf("\t" CLR_BOLD "-> Name: " CLR_RESET);
    readString(newNode->data.name, sizeof(newNode->data.name));
    
    chooseEquipmentType(newNode->data.type);

    printf("\n\t" CLR_BOLD "-> Brand: " CLR_RESET);
    readString(newNode->data.brand, sizeof(newNode->data.brand));
    printf("\t" CLR_BOLD "-> Model: " CLR_RESET);
    readString(newNode->data.model, sizeof(newNode->data.model));
    
    do {
        printf("\t" CLR_BOLD "-> IP Address (e.g., 192.168.1.50): " CLR_RESET);
        readString(newNode->data.ip, sizeof(newNode->data.ip));
        if (!isValidIP(newNode->data.ip)) {
            printf("\t" CLR_RED "[ERROR] Invalid IPv4 format. Please try again." CLR_RESET "\n");
        }
    } while (!isValidIP(newNode->data.ip));

    do {
        printf("\t" CLR_BOLD "-> MAC Address (e.g., 00:1A:2B:3C:4D:5E): " CLR_RESET);
        readString(newNode->data.mac, sizeof(newNode->data.mac));
        if (!isValidMAC(newNode->data.mac)) {
            printf("\t" CLR_RED "[ERROR] Invalid MAC format (Use XX:XX:XX:XX:XX:XX)." CLR_RESET "\n");
        }
    } while (!isValidMAC(newNode->data.mac));

    printf("\t" CLR_BOLD "-> Location/Room: " CLR_RESET);
    readString(newNode->data.location, sizeof(newNode->data.location));

    chooseEquipmentStatus(newNode->data.status);

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(newNode->data.last_verification, "%02d/%02d/%04d %02d:%02d", 
            tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
            tm.tm_hour, tm.tm_min);

    if (list == NULL || newNode->data.id < list->data.id) {
        newNode->next = list;
        list = newNode; 
    }
    else {
        Node* current = list;
        while (current->next != NULL && current->next->data.id < newNode->data.id) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }

    printf("\n\t" CLR_GREEN "[SUCCESS] Equipment '%s' successfully registered!" CLR_RESET "\n", newNode->data.name);
    printf("\tTimestamp: %s\n", newNode->data.last_verification);
    printf("\n\tPress Enter to continue...");
    getchar();
    clearScreen();

    return list;
}

Node* removeEquipment(Node* list) {
    
    if (list == NULL) {
        printf("\n\t" CLR_RED "[INFO] The inventory is empty. Nothing to remove." CLR_RESET "\n");
        return list;
    }

    displayEquipment(list);

    int targetId;
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "           " CLR_BOLD "REMOVE EQUIPMENT" CLR_RESET "            " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    
    printf("\n\t  Enter the ID to remove " CLR_BOLD "-->" CLR_RESET " ");
    if(scanf("%d", &targetId) != 1) {
        printf("\n\t" CLR_RED "[ERROR] Invalid input. Please enter a number." CLR_RESET "\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        printf("\n\tPress Enter to continue...");
        getchar();
        clearScreen();
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
        printf("\n\t" CLR_RED "[WARNING] Equipment with ID %d not found." CLR_RESET "\n", targetId);
        printf("\n\tPress Enter to return...");
        getchar();
        clearScreen();
        return list;
    }

    if (strcmp(current->data.status, "Operational") != 0) {
        printf("\n\t" CLR_RED "[DENIED] Cannot remove equipment with ID %d." CLR_RESET "\n", current->data.id);
        printf("\tCurrent Status: " CLR_RED "%s" CLR_RESET "\n", current->data.status);
        printf("\t" CLR_BOLD "Note:" CLR_RESET " Only 'Operational' assets can be removed.\n");
        printf("\n\tPress Enter to return...");
        getchar();
        clearScreen();
        return list;
    }

    if (previous == NULL) {
        list = current->next;
    } else {
        previous->next = current->next;
    }

    free(current);

    printf("\n\t" CLR_GREEN "[SUCCESS] Equipment with ID %d removed successfully." CLR_RESET "\n", targetId);
    printf("\n\tPress Enter to continue...");
    getchar();
    clearScreen();

    return list;
}

void editEquipment(Node* list) {

    if (list == NULL) {
        printf("\n\t" CLR_RED "[INFO] The inventory is empty. Nothing to edit." CLR_RESET "\n");
        return;
    }

    displayEquipment(list);
    
    int targetId;
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "         " CLR_BOLD "EDIT EQUIPMENT DETAILS" CLR_RESET "        " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    
    printf("\n\t  Enter the ID to edit " CLR_BOLD "-->" CLR_RESET " ");
    if(scanf("%d", &targetId) != 1) {
        printf("\n\t" CLR_RED "[ERROR] Invalid ID Format." CLR_RESET "\n");
        while (getchar() != '\n');
        printf("\n\tPress Enter to continue...");
        getchar();
        clearScreen();
        return;
    }
    getchar();

    Node* current = list;
    while (current != NULL && current->data.id != targetId) {
        current = current->next;
    }

    if (current == NULL) {
        printf("\n\t" CLR_RED "[WARNING] Equipment with ID %d not found." CLR_RESET "\n", targetId);
        printf("\n\tPress Enter to return...");
        getchar();
        clearScreen();
        return;
    }

    clearScreen();
    printf("\n\t" CLR_CYAN "=== Editing asset: " CLR_RESET CLR_BOLD "'%s'" CLR_RESET CLR_CYAN " (ID: %d) ===" CLR_RESET "\n\n", current->data.name, current->data.id);
    char choice;
    int dataChanged = 0;

    printf("\tCurrent Name: " CLR_BOLD "%s" CLR_RESET "\n", current->data.name);
    printf("\tChange name? (y/n): ");
    scanf("%c", &choice); getchar();
    if (choice == 'y' || choice == 'Y') {
        printf("\tEnter New Name: ");
        readString(current->data.name, sizeof(current->data.name));
        dataChanged = 1;
    }
    printf("\t---------------------------------------------\n");

    printf("\tCurrent Type: " CLR_BOLD "%s" CLR_RESET "\n", current->data.type);
    printf("\tChange type? (y/n): ");
    scanf("%c", &choice); getchar();
    if (choice == 'y' || choice == 'Y') {
        chooseEquipmentType(current->data.type);
        dataChanged = 1;
        clearScreen();
        printf("\n\t" CLR_CYAN "=== Editing asset: " CLR_RESET CLR_BOLD "'%s'" CLR_RESET CLR_CYAN " (ID: %d) ===" CLR_RESET "\n\n", current->data.name, current->data.id);
    }
    printf("\t---------------------------------------------\n");

    printf("\tCurrent Brand: " CLR_BOLD "%s" CLR_RESET "\n", current->data.brand);
    printf("\tChange brand? (y/n): ");
    scanf("%c", &choice); getchar();
    if (choice == 'y' || choice == 'Y') {
        printf("\tEnter New Brand: ");
        readString(current->data.brand, sizeof(current->data.brand));
        dataChanged = 1;
    }
    printf("\t---------------------------------------------\n");

    printf("\tCurrent Model: " CLR_BOLD "%s" CLR_RESET "\n", current->data.model);
    printf("\tChange model? (y/n): ");
    scanf("%c", &choice); getchar();
    if (choice == 'y' || choice == 'Y') {
        printf("\tEnter New Model: ");
        readString(current->data.model, sizeof(current->data.model));
        dataChanged = 1;
    }
    printf("\t---------------------------------------------\n");

    printf("\tCurrent IP Address: " CLR_BOLD "%s" CLR_RESET "\n", current->data.ip);
    printf("\tChange IP address? (y/n): ");
    scanf("%c", &choice); getchar();
    if (choice == 'y' || choice == 'Y') {
        do {
            printf("\tEnter New IP Address (e.g., 192.168.1.1): ");
            readString(current->data.ip, sizeof(current->data.ip));
            if (!isValidIP(current->data.ip)) {
                printf("\t" CLR_RED "[ERROR] Invalid IPv4 format. Please try again." CLR_RESET "\n");
            }
        } while (!isValidIP(current->data.ip));
        dataChanged = 1;
    }
    printf("\t---------------------------------------------\n");

    printf("\tCurrent MAC Address: " CLR_BOLD "%s" CLR_RESET "\n", current->data.mac);
    printf("\tChange MAC address? (y/n): ");
    scanf("%c", &choice); getchar();
    if (choice == 'y' || choice == 'Y') {
        do {
            printf("\tEnter New MAC Address (e.g., 00:1A:2B:3C:4D:5E): ");
            readString(current->data.mac, sizeof(current->data.mac));
            if (!isValidMAC(current->data.mac)) {
                printf("\t" CLR_RED "[ERROR] Invalid MAC format (Use XX:XX:XX:XX:XX:XX)." CLR_RESET "\n");
            }
        } while (!isValidMAC(current->data.mac));
        dataChanged = 1;
    }
    printf("\t---------------------------------------------\n");

    printf("\tCurrent Location: " CLR_BOLD "%s" CLR_RESET "\n", current->data.location);
    printf("\tChange location? (y/n): ");
    scanf("%c", &choice); getchar();
    if (choice == 'y' || choice == 'Y') {
        printf("\tEnter New Location: ");
        readString(current->data.location, sizeof(current->data.location));
        dataChanged = 1;
    }
    printf("\t---------------------------------------------\n");

    printf("\tCurrent Status: " CLR_BOLD "%s" CLR_RESET "\n", current->data.status);
    printf("\tChange status? (y/n): ");
    scanf("%c", &choice); getchar();
    if (choice == 'y' || choice == 'Y') {
        chooseEquipmentStatus(current->data.status);
        dataChanged = 1;
        clearScreen();
    }

    if (dataChanged) {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        sprintf(current->data.last_verification, "%02d/%02d/%04d %02d:%02d", 
                tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                tm.tm_hour, tm.tm_min);
        
        printf("\n\t" CLR_GREEN "[SUCCESS] Modifications saved successfully!" CLR_RESET "\n");
        printf("\tVerification timestamp updated to: %s\n", current->data.last_verification);
    } else {
        printf("\n\t" CLR_CYAN "[INFO] No changes made to equipment details." CLR_RESET "\n");
    }

    printf("\n\tPress Enter to return...");
    getchar();
    clearScreen();
}

void changeEquipmentStatus(Node* list) {
    if (list == NULL) {
        printf("\n\t" CLR_RED "[INFO] The inventory is empty. No equipment to update." CLR_RESET "\n");
        return;
    }

    displayEquipment(list);

    int targetId;
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "         " CLR_BOLD "CHANGE EQUIPMENT STATUS" CLR_RESET "       " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    
    printf("\n\t  Enter the ID to update " CLR_BOLD "-->" CLR_RESET " ");
    if(scanf("%d", &targetId) != 1) {
        printf("\n\t" CLR_RED "[ERROR] Invalid input. Please enter a number." CLR_RESET "\n");
        while (getchar() != '\n');
        printf("\n\tPress Enter to continue...");
        getchar();
        clearScreen();
        return;
    }
    getchar();

    Node* current = list;
    while (current != NULL && current->data.id != targetId) {
        current = current->next;
    }

    if (current == NULL) {
        printf("\n\t" CLR_RED "[WARNING] Equipment with ID %d not found." CLR_RESET "\n", targetId);
        printf("\n\tPress Enter to return...");
        getchar();
        clearScreen();
        return;
    }

    clearScreen();
    printf("\n\t" CLR_CYAN "=== Updating Status for: " CLR_RESET CLR_BOLD "%s" CLR_RESET CLR_CYAN " (ID: %d) ===" CLR_RESET "\n", current->data.name, current->data.id);
    printf("\tCurrent Status: " CLR_BOLD "%s" CLR_RESET "\n", current->data.status);
    
    chooseEquipmentStatus(current->data.status);
    clearScreen();

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(current->data.last_verification, "%02d/%02d/%04d %02d:%02d", 
            tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
            tm.tm_hour, tm.tm_min);

    printf("\n\t" CLR_GREEN "[SUCCESS] Status updated successfully!" CLR_RESET "\n");
    printf("\tNew Asset Status: " CLR_BOLD "%s" CLR_RESET "\n", current->data.status);
    printf("\tVerification Timestamp: %s\n", current->data.last_verification);
    
    printf("\n\tPress Enter to return to menu...");
    getchar();
    clearScreen();
}

void listAllEquipment(Node* list) {

    if (list == NULL) {
        printf("\n\t" CLR_RED "[INFO] Current inventory is completely empty. No assets to display." CLR_RESET "\n");
        return;
    }

    printf("\n====================================================================\n");
    printf("                  " CLR_CYAN CLR_BOLD "FULL EQUIPMENT INVENTORY LIST" CLR_RESET "                  \n");
    printf("====================================================================\n");

    Node* current = list;
    int count = 1;

    while (current != NULL) {
        printf("\n " CLR_GREEN " > " CLR_RESET CLR_BOLD "ASSET [%d]" CLR_RESET " -> ID: " CLR_YELLOW "%d" CLR_RESET "\n", count++, current->data.id);
        printf("  +----------------------------------------------------------------+\n");
        
        printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Name:", current->data.name);
        printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Type:", current->data.type);
        printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Brand/Model:", current->data.brand); 
        printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Model:", current->data.model);
        printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "IP Address:", current->data.ip);
        printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "MAC Address:", current->data.mac);
        printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Location:", current->data.location);
       
        if (strcmp(current->data.status, "Operational") == 0 ) {
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " " CLR_GREEN "%-46s" CLR_RESET " |\n", "Status:", current->data.status);
        } else {
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " " CLR_RED "%-46s" CLR_RESET " |\n", "Status:", current->data.status);
        }
        
        printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Last Verify:", current->data.last_verification);
        printf("  +----------------------------------------------------------------+\n");

        current = current->next;
    }
    
    printf("\n" CLR_CYAN ">>> End of asset report. Total equipment listed: %d" CLR_RESET "\n", count - 1);
}

void listEquipmentByType(Node* list) {

    if (list == NULL) {
        printf("\n\t" CLR_RED "[INFO] The inventory is completely empty. No assets to display." CLR_RESET "\n");
        return;
    }

    char targetType[30];
    chooseEquipmentType(targetType);
    clearScreen();

    printf("\n====================================================================\n");
    printf("              " CLR_CYAN CLR_BOLD "FILTERED REPORT: TYPE -> %s" CLR_RESET "              \n", targetType);
    printf("====================================================================\n");

    Node* current = list;
    int count = 0;

    while (current != NULL) {
        if (strcmp(current->data.type, targetType) == 0) {
            count++;
            
            printf("\n " CLR_GREEN " > " CLR_RESET CLR_BOLD "FILTERED ASSET [%d]" CLR_RESET " -> ID: " CLR_YELLOW "%d" CLR_RESET "\n", count, current->data.id);
            printf("  +----------------------------------------------------------------+\n");
            
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Name:", current->data.name);
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Brand:", current->data.brand);
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Model:", current->data.model);
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "IP Address:", current->data.ip);
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "MAC Address:", current->data.mac);
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Location:", current->data.location);
            
            if (strcmp(current->data.status, "Active") == 0 || 
                strcmp(current->data.status, "Online") == 0 || 
                strcmp(current->data.status, "Operational") == 0) {
                
                printf("  |  " CLR_BOLD "%-15s" CLR_RESET " " CLR_GREEN "%-46s" CLR_RESET " |\n", "Status:", current->data.status);
            } else {
                printf("  |  " CLR_BOLD "%-15s" CLR_RESET " " CLR_RED "%-46s" CLR_RESET " |\n", "Status:", current->data.status);
            }
            
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Last Verify:", current->data.last_verification);
            printf("  +----------------------------------------------------------------+\n");
        }
        current = current->next;
    }

    if (count == 0) {
        printf("\n\t" CLR_RED "[INFO] No equipment found registered under the type '%s'." CLR_RESET "\n", targetType);
    } else {
        printf("\n" CLR_CYAN ">>> End of filtered report. Total '%s' equipment listed: %d" CLR_RESET "\n", targetType, count);
    }
    
    printf("\n\tPress Enter to return...");
    getchar();
    clearScreen();
}

void listEquipmentByStatus(Node* list) {

    if (list == NULL) {
        printf("\n\t" CLR_RED "[INFO] The inventory is completely empty. No assets to display." CLR_RESET "\n");
        return;
    }

    char targetStatus[30];
    chooseEquipmentStatus(targetStatus);
    clearScreen();

    int isOperational = (strcmp(targetStatus, "Active") == 0 || 
                         strcmp(targetStatus, "Online") == 0 || 
                         strcmp(targetStatus, "Operational") == 0);

    printf("\n====================================================================\n");
    if (isOperational) {
        printf("             " CLR_GREEN CLR_BOLD "FILTERED REPORT: STATUS -> %s" CLR_RESET "             \n", targetStatus);
    } else {
        printf("             " CLR_RED CLR_BOLD "FILTERED REPORT: STATUS -> %s" CLR_RESET "             \n", targetStatus);
    }
    printf("====================================================================\n");

    Node* current = list;
    int count = 0;

    while (current != NULL) {
        if (strcmp(current->data.status, targetStatus) == 0) {
            count++;
            
            if (isOperational) {
                printf("\n " CLR_GREEN " > " CLR_RESET CLR_BOLD "ASSET [%d]" CLR_RESET " -> ID: " CLR_YELLOW "%d" CLR_RESET "\n", count, current->data.id);
            } else {
                printf("\n " CLR_RED " > " CLR_RESET CLR_BOLD "ASSET [%d]" CLR_RESET " -> ID: " CLR_YELLOW "%d" CLR_RESET "\n", count, current->data.id);
            }
            printf("  +----------------------------------------------------------------+\n");
            
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Name:", current->data.name);
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Type:", current->data.type);
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Brand:", current->data.brand);
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Model:", current->data.model);
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "IP Address:", current->data.ip);
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "MAC Address:", current->data.mac);
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Location:", current->data.location);
            
            if (isOperational) {
                printf("  |  " CLR_BOLD "%-15s" CLR_RESET " " CLR_GREEN "%-46s" CLR_RESET " |\n", "Status:", current->data.status);
            } else {
                printf("  |  " CLR_BOLD "%-15s" CLR_RESET " " CLR_RED "%-46s" CLR_RESET " |\n", "Status:", current->data.status);
            }
            
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Last Verify:", current->data.last_verification);
            printf("  +----------------------------------------------------------------+\n");
        }
        current = current->next;
    }

    if (count == 0) {
        printf("\n\t" CLR_RED "[INFO] No equipment found registered with the status '%s'." CLR_RESET "\n", targetStatus);
    } else {
        if (isOperational) {
            printf("\n" CLR_GREEN ">>> End of filtered report. Total '%s' assets: %d" CLR_RESET "\n", targetStatus, count);
        } else {
            printf("\n" CLR_RED ">>> End of filtered report. Total '%s' assets: %d" CLR_RESET "\n", targetStatus, count);
        }
    }
    
    printf("\n\tPress Enter to return...");
    getchar();
    clearScreen();
}

void searchEquipment(Node* list) {

    if (list == NULL) {
        printf("\n\t" CLR_RED "[INFO] The inventory is completely empty. No assets to search." CLR_RESET "\n");
        return;
    }

    int searchOption;
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "         " CLR_BOLD "SEARCH ASSET GATEWAY" CLR_RESET "          " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "                                       " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "1." CLR_RESET " Search by Asset ID (Code)         " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "2." CLR_RESET " Search by IP Address              " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "3." CLR_RESET " Search by MAC Address             " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "                                       " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t  Select search option " CLR_BOLD "-->" CLR_RESET " ");

    if (scanf("%d", &searchOption) != 1) {
        printf("\n\t" CLR_RED "[ERROR] Invalid criteria option." CLR_RESET "\n");
        while (getchar() != '\n');
        printf("\n\tPress Enter to continue...");
        getchar();
        clearScreen();
        return;
    }
    getchar();
    clearScreen();

    Node* current = list;
    int found = 0;

    if (searchOption == 1) {
        int targetId;
        printf("\n\t" CLR_BOLD "Enter target Asset ID: " CLR_RESET);
        if (scanf("%d", &targetId) != 1) {
            printf("\n\t" CLR_RED "[ERROR] Invalid Asset ID format." CLR_RESET "\n");
            while (getchar() != '\n');
            return;
        }
        getchar();
        clearScreen();

        while (current != NULL) {
            if (current->data.id == targetId) {
                found = 1;
                break;
            }
            current = current->next;
        }

        if (found && current != NULL) {
            printf("\n\t" CLR_GREEN "[SUCCESS] Asset found matching ID: %d" CLR_RESET "\n", targetId);
            printf("  +----------------------------------------------------------------+\n");
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46d |\n", "Asset ID:", current->data.id);
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Name:", current->data.name);
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Type:", current->data.type);
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Brand:", current->data.brand);
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Model:", current->data.model);
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "IP Address:", current->data.ip);
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "MAC Address:", current->data.mac);
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Location:", current->data.location);
            
            if (strcmp(current->data.status, "Active") == 0 || strcmp(current->data.status, "Online") == 0 || strcmp(current->data.status, "Operational") == 0) {
                printf("  |  " CLR_BOLD "%-15s" CLR_RESET " " CLR_GREEN "%-46s" CLR_RESET " |\n", "Status:", current->data.status);
            } else {
                printf("  |  " CLR_BOLD "%-15s" CLR_RESET " " CLR_RED "%-46s" CLR_RESET " |\n", "Status:", current->data.status);
            }
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Last Verify:", current->data.last_verification);
            printf("  +----------------------------------------------------------------+\n");
        } else {
            printf("\n\t" CLR_RED "[INFO] No equipment found with ID %d." CLR_RESET "\n", targetId);
        }
    }

    else if (searchOption == 2) {
        char targetIP[16];
        printf("\n\t" CLR_BOLD "Enter target IP Address: " CLR_RESET);
        readString(targetIP, sizeof(targetIP));
        clearScreen();

        printf("\n\t" CLR_CYAN "[SYSTEM] Scanning cluster for IP: %s..." CLR_RESET "\n", targetIP);
        
        while (current != NULL) {
            if (strcmp(current->data.ip, targetIP) == 0) {
                found++;
                printf("\n " CLR_GREEN " > " CLR_RESET CLR_BOLD "MATCH #%d" CLR_RESET "\n", found);
                printf("  +----------------------------------------------------------------+\n");
                printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46d |\n", "Asset ID:", current->data.id);
                printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Name:", current->data.name);
                printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Type:", current->data.type);
                printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Brand:", current->data.brand);
                printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Model:", current->data.model);
                printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "IP Address:", current->data.ip);
                printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "MAC Address:", current->data.mac);
                printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Location:", current->data.location);
                
                if (strcmp(current->data.status, "Active") == 0 || strcmp(current->data.status, "Online") == 0 || strcmp(current->data.status, "Operational") == 0) {
                    printf("  |  " CLR_BOLD "%-15s" CLR_RESET " " CLR_GREEN "%-46s" CLR_RESET " |\n", "Status:", current->data.status);
                } else {
                    printf("  |  " CLR_BOLD "%-15s" CLR_RESET " " CLR_RED "%-46s" CLR_RESET " |\n", "Status:", current->data.status);
                }
                printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Last Verify:", current->data.last_verification);
                printf("  +----------------------------------------------------------------+\n");
            }
            current = current->next;
        }

        if (found == 0) {
            printf("\n\t" CLR_RED "[INFO] No equipment found with IP %s." CLR_RESET "\n", targetIP);
        } else {
            printf("\n\t" CLR_CYAN ">>> Scan complete. Found %d asset(s) sharing IP %s." CLR_RESET "\n", found, targetIP);
        }
    }

    else if (searchOption == 3) {
        char targetMAC[18];
        printf("\n\t" CLR_BOLD "Enter target MAC Address: " CLR_RESET);
        readString(targetMAC, sizeof(targetMAC));
        clearScreen();

        while (current != NULL) {
            if (strcmp(current->data.mac, targetMAC) == 0) {
                found = 1;
                break;
            }
            current = current->next;
        }

        if (found && current != NULL) {
            printf("\n\t" CLR_GREEN "[SUCCESS] Asset found matching MAC: %s" CLR_RESET "\n", targetMAC);
            printf("  +----------------------------------------------------------------+\n");
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46d |\n", "Asset ID:", current->data.id);
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Name:", current->data.name);
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Type:", current->data.type);
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Brand:", current->data.brand);
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Model:", current->data.model);
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "IP Address:", current->data.ip);
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "MAC Address:", current->data.mac);
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Location:", current->data.location);
            
            if (strcmp(current->data.status, "Active") == 0 || strcmp(current->data.status, "Online") == 0 || strcmp(current->data.status, "Operational") == 0) {
                printf("  |  " CLR_BOLD "%-15s" CLR_RESET " " CLR_GREEN "%-46s" CLR_RESET " |\n", "Status:", current->data.status);
            } else {
                printf("  |  " CLR_BOLD "%-15s" CLR_RESET " " CLR_RED "%-46s" CLR_RESET " |\n", "Status:", current->data.status);
            }
            printf("  |  " CLR_BOLD "%-15s" CLR_RESET " %-46s |\n", "Last Verify:", current->data.last_verification);
            printf("  +----------------------------------------------------------------+\n");
        } else {
            printf("\n\t" CLR_RED "[INFO] No equipment found with MAC %s." CLR_RESET "\n", targetMAC);
        }
    }
    else {
        printf("\n\t" CLR_RED "[WARNING] Invalid search option selection." CLR_RESET "\n");
    }

    printf("\n\tPress Enter to return...");
    getchar();
    clearScreen();
}

void displayInventoryDashboard(Node* list) {
    int total = 0, operational = 0, faulty = 0, maintenance = 0, deactivated = 0;
    Node* current = list;

    while (current != NULL) {
        total++;
        if (strcmp(current->data.status, "Operational") == 0 || 
            strcmp(current->data.status, "Active") == 0 || 
            strcmp(current->data.status, "Online") == 0) {
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

    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "       " CLR_BOLD "INFRASTRUCTURE HEALTH" CLR_RESET "           " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "  Total Monitored Assets:  " CLR_BOLD "%-12d" CLR_RESET " " CLR_CYAN "|" CLR_RESET, total);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "[Active] Operational:" CLR_RESET "     %-12d " CLR_CYAN "|" CLR_RESET, operational);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_YELLOW "[Warning] Maintenance:" CLR_RESET "    %-12d " CLR_CYAN "|" CLR_RESET, maintenance);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_RED "[Alert] Faulty / Down:" CLR_RESET "    %-12d " CLR_CYAN "|" CLR_RESET, faulty);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_RESET "[Offline] Deactivated:" CLR_RESET "    %-12d " CLR_CYAN "|" CLR_RESET, deactivated);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);

    if (total > 0) {
        float integrityRate = ((float)operational / total) * 100;
        
        if (integrityRate < 70.0) {
            printf("\n\t" CLR_CYAN "|" CLR_RESET "  Network Integrity:       " CLR_BOLD CLR_RED "%-6.2f%%" CLR_RESET "     " CLR_CYAN "|" CLR_RESET, integrityRate);
            printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
            printf("\n\t  " CLR_BOLD CLR_RED "[SECURITY ALERT] Critical offline assets!" CLR_RESET "\n");
        } else {
            printf("\n\t" CLR_CYAN "|" CLR_RESET "  Network Integrity:       " CLR_BOLD CLR_GREEN "%-6.2f%%" CLR_RESET "     " CLR_CYAN "|" CLR_RESET, integrityRate);
            printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
            printf("\n\t  " CLR_GREEN "[SYSTEM STATUS] Infrastructure normal." CLR_RESET "\n");
        }
    } else {
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  Network Integrity:       " CLR_YELLOW "0.00%%" CLR_RESET "        " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t  " CLR_YELLOW "[INFO] No assets registered in cluster." CLR_RESET "\n");
    }
    printf("\t" CLR_CYAN "+---------------------------------------+" CLR_RESET "\n");

    printf("\n\tPress Enter to return...");
    getchar();
    clearScreen();
}

void menuReportsAndSearches(Node* list) {
    int option;

    do {
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "       " CLR_BOLD "REPORTS & SEARCHES SUBMENU" CLR_RESET "      " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "                                       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "1." CLR_RESET " List All Equipment                " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "2." CLR_RESET " List Equipment by Type            " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "3." CLR_RESET " List Equipment by Status          " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "4." CLR_RESET " Search Equipment (ID, IP, MAC)    " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "5." CLR_RESET " Display Inventory Dashboard       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "                                       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_RED "0. Return to Inventory Menu" CLR_RESET "          " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t  Select an option " CLR_BOLD "-->" CLR_RESET " ");

        if (scanf("%d", &option) != 1) {
            printf("\n\t" CLR_RED "[ERROR] Invalid input. Please enter a number." CLR_RESET "\n");
            while (getchar() != '\n');
            printf("\n\tPress Enter to continue...");
            getchar();
            clearScreen();
            option = -1;
            continue;
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
                printf("\n" CLR_CYAN "[SYSTEM] Returning to Inventory Menu..." CLR_RESET "\n");
                break;
            default:
                clearScreen();
                printf("\n\t" CLR_RED "[WARNING] Invalid option. Please try again." CLR_RESET "\n");
                printf("\n\tPress Enter to return to menu...");
                getchar();
                clearScreen();
                break;
        }
    } while (option != 0);
}

void saveInventoryToBinary(Node* list) {

    FILE* file = fopen("inventory.dat", "wb");
    if (file == NULL) {
        printf("\n\t" CLR_RED "[ERROR] Critical failure: Unable to open 'inventory.dat' for writing." CLR_RESET "\n");
        printf("\tCheck directory permissions or disk space.\n");
        printf("\n\tPress Enter to return...");
        getchar();
        clearScreen();
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

    printf("\n\t" CLR_GREEN "[SUCCESS] Backup operations completed!" CLR_RESET);
    printf("\n\t" CLR_CYAN ">>> %d assets securely written to 'inventory.dat'." CLR_RESET "\n", count);
}

Node* loadInventoryFromBinary(Node* list) {

    FILE* file = fopen("inventory.dat", "rb");
    if (file == NULL) {
        printf("\n\t" CLR_YELLOW "[INFO] No backup file found. Starting with an empty inventory." CLR_RESET "\n");
        return list;
    }

    Equipment tempEquipment;
    int count = 0;

    while (fread(&tempEquipment, sizeof(Equipment), 1, file) == 1) {
        Node* newNode = (Node*)malloc(sizeof(Node));
        if (newNode == NULL) {
            printf("\n\t" CLR_RED "[ERROR] Memory allocation failed while loading inventory." CLR_RESET "\n");
            fclose(file);
            return list;
        }
        
        newNode->data = tempEquipment;
        newNode->next = NULL;

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

    printf("\n\t" CLR_GREEN "[SUCCESS] Storage synchronization complete!" CLR_RESET);
    printf("\n\t" CLR_CYAN ">>> %d assets securely loaded from 'inventory.dat'." CLR_RESET "\n", count);

    return list;
}

void factoryResetInventory(Node** list) {

    char password[20];
    
    printf("\n\t" CLR_RED "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_RED "|" CLR_RESET "      " CLR_BOLD CLR_RED "SYSTEM FACTORY RESET GATEWAY" CLR_RESET "     " CLR_RED "|" CLR_RESET);
    printf("\n\t" CLR_RED "+---------------------------------------+" CLR_RESET);
    
    printf("\n\t  " CLR_BOLD "Enter Admin Password" CLR_RESET " --> ");
    readString(password, sizeof(password));

    if (strcmp(password, "admin123") != 0) {
        printf("\n\t" CLR_RED "[DENIED] Invalid password. Factory reset aborted." CLR_RESET "\n");
        printf("\n\tPress Enter to return...");
        getchar();
        clearScreen();
        return;
    }

    char confirmation;
    clearScreen();
    
    printf("\n\t" CLR_RED "+---------------------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_BOLD CLR_RED "  [CRITICAL WARNING] PERMANENT DESTRUCTION OF DATA   " CLR_RESET);
    printf("\n\t" CLR_RED "+---------------------------------------------------+" CLR_RESET);
    printf("\n\t  This action will permanently delete ALL inventory");
    printf("\n\t  data from live cluster memory and disk backup.");
    printf("\n\t  This operation cannot be undone.");
    printf("\n\t" CLR_RED "+---------------------------------------------------+" CLR_RESET);
    
    printf("\n\t  Are you absolutely sure to proceed? (y/n): ");
    scanf("%c", &confirmation);
    while (getchar() != '\n');

    if (confirmation != 'y' && confirmation != 'Y') {
        printf("\n\t" CLR_GREEN "[CANCELLED] Factory reset aborted. Data remains intact." CLR_RESET "\n");
        printf("\n\tPress Enter to continue...");
        getchar();
        clearScreen();
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

    clearScreen();
    printf("\n\t" CLR_GREEN "[SUCCESS] System infrastructure restored to default state!" CLR_RESET);
    printf("\n\t" CLR_CYAN ">>> %d assets permanently purged from core memory." CLR_RESET, count);
    printf("\n\t" CLR_CYAN ">>> Backup file 'inventory.dat' unlinked from disk." CLR_RESET "\n");
    printf("\n\tThe station is now ready for a clean deployment initialization.\n");
    
    printf("\n\tPress Enter to return to main terminal...");
    getchar();
    clearScreen();
}

Node* menuInventory(Node* list) {
    int option;

    do {
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "       " CLR_BOLD "MINI NOC SYSTEM - INVENTORY" CLR_RESET "     " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "                                       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "1." CLR_RESET " Add New Equipment                 " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "2." CLR_RESET " Remove Equipment                  " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "3." CLR_RESET " Edit Equipment Details            " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "4." CLR_RESET " Change Equipment Status           " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "5." CLR_RESET " Reports & Searches Menu           " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "6." CLR_RESET " Save Inventory to Backup File     " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "7." CLR_RESET " [ADMIN] Factory Reset Inventory   " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "                                       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_RED "0. Return to Main Menu" CLR_RESET "               " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t  Select an option " CLR_BOLD "-->" CLR_RESET " ");
        
        if(scanf("%d", &option) != 1) {
            printf("\n\t" CLR_RED "[ERROR] Invalid input. Please enter a number." CLR_RESET "\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            printf("\n\tPress Enter to continue...");
            getchar();
            clearScreen();
            continue;
        }
        getchar();

        switch (option) {
            case 1:
                clearScreen();
                list = addEquipment(list);
                clearScreen();
                break;
            case 2:
                clearScreen();
                list = removeEquipment(list);
                clearScreen();
                break;
            case 3:
                clearScreen();
                editEquipment(list);
                clearScreen();
                break;
            case 4:
                clearScreen();
                changeEquipmentStatus(list);
                clearScreen();
                break;
            case 5:
                clearScreen();
                menuReportsAndSearches(list);
                clearScreen();
                break;
            case 6:
                clearScreen();
                saveInventoryToBinary(list);
                clearScreen();
                break; 
            case 7:
                clearScreen();
                factoryResetInventory(&list);
                clearScreen();
                break;
            case 0:
                clearScreen();
                printf("\n" CLR_CYAN "[SYSTEM] Returning to Main Menu..." CLR_RESET "\n");
                break;
            default:
                clearScreen();
                printf("\n\t" CLR_RED "[WARNING] Invalid option. Please try again." CLR_RESET "\n");
                printf("\n\tPress Enter to return to menu...");
                getchar();
                clearScreen();
                break;
        }
    } while (option != 0);

    return list;
}

void writeToMonitorizationLog(int assetID, const char* ip, int success) {

    FILE* logFile = fopen("log_monitorization.txt", "a");
    if (logFile == NULL) {
        printf("\n\t" CLR_RED "[ERROR] Core system failure: Could not open 'log_monitorization.txt'!" CLR_RESET "\n");
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

        fprintf(logFile, "   -> [CRITICAL INCIDENT #INC-%d]\n", incidentCounter);
        fprintf(logFile, "     STATUS: OPEN | SEVERITY: HIGH\n");
        fprintf(logFile, "     ACTION REQUIRED: Check routing or physical connectivity for Asset ID %d.\n", assetID);
        fprintf(logFile, "   ----------------------------------------------------------------------\n");

        printf("\n\t" CLR_RED "+---------------------------------------+" CLR_RESET);
        printf("\n\t" CLR_RED "|" CLR_RESET "  " CLR_BOLD CLR_RED "[ALERT] MONITORIZATION LINK FAILURE" CLR_RESET "  " CLR_RED "|" CLR_RESET);
        printf("\n\t" CLR_RED "+---------------------------------------+" CLR_RESET);
        printf("\n\t  " CLR_BOLD "Target IP:" CLR_RESET "  %-15s", ip);
        printf("\n\t  " CLR_BOLD "Asset ID:" CLR_RESET "   %-5d", assetID);
        printf("\n\t  " CLR_BOLD "Ticket:" CLR_RESET "     " CLR_YELLOW "#INC-%d" CLR_RESET " (" CLR_RED "STATUS: OPEN" CLR_RESET ")", incidentCounter);
        printf("\n\t" CLR_RED "+---------------------------------------+" CLR_RESET "\n");
    }

    fclose(logFile);
}

void enqueuePingFailureIncident(IncidentQueue* q, const char* failedIP, const char* deviceName) {

    TechnicalIncident* newNode = (TechnicalIncident*)malloc(sizeof(TechnicalIncident));
    if (newNode == NULL) {
        printf("\n\t" CLR_RED "[ERROR] Kernel Alert: Memory allocation failed for Incident Ticket." CLR_RESET "\n");
        return;
    }

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
    
    printf("\n\t" CLR_RED "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_RED "|" CLR_RESET "    " CLR_BOLD CLR_RED "[AUTOMATED NOC TELEMETRY ALERT]" CLR_RESET "    " CLR_RED "|" CLR_RESET);
    printf("\n\t" CLR_RED "+---------------------------------------+" CLR_RESET);
    printf("\n\t  " CLR_BOLD "Ticket ID:" CLR_RESET "  " CLR_YELLOW "#%d" CLR_RESET, newNode->ticketId);
    printf("\n\t  " CLR_BOLD "Incident:" CLR_RESET "   %s", newNode->type);
    printf("\n\t  " CLR_BOLD "Target:" CLR_RESET "     %s", newNode->targetCode);
    printf("\n\t  " CLR_BOLD "Severity:" CLR_RESET "   " CLR_RED "HIGH (Queue Level-1)" CLR_RESET);
    printf("\n\t" CLR_RED "+---------------------------------------+" CLR_RESET "\n");
}

PingResult processAssetPing(Node* target, IncidentQueue* q) {
    PingResult result;
    result.responded = 0;

    if (target == NULL) return result;

    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "         " CLR_BOLD "ICMP CONNECTIVITY TEST" CLR_RESET "        " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t  " CLR_BOLD "Target:" CLR_RESET "   %s", target->data.name);
    printf("\n\t  " CLR_BOLD "IP Addr:" CLR_RESET "  %s", target->data.ip);
    printf("\n\t  " CLR_CYAN "[SYSTEM] Sending 2 echo requests..." CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET "\n");

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
        printf("\n\t" CLR_GREEN "[SUCCESS] Asset ID %d linked up! Status: Operational." CLR_RESET "\n", target->data.id);
    } else {
        strcpy(target->data.status, "Faulty");
        printf("\n\t" CLR_RED "[CRITICAL] Asset ID %d failed to respond! State: Faulty." CLR_RESET "\n", target->data.id);

        enqueuePingFailureIncident(q, target->data.ip, target->data.name);
    }

    writeToMonitorizationLog(target->data.id, target->data.ip, result.responded);

    printf("\n\tPress Enter to continue...");
    getchar();
    clearScreen();

    return result;
}

void runGeneralNetworkTest(Node* list, IncidentQueue* q) {
    if (list == NULL) {
        printf("\n\t" CLR_RED "[INFO] The inventory is empty. No assets available for testing." CLR_RESET "\n");
        return;
    }

    clearScreen();
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "     " CLR_BOLD "INITIALIZING GLOBAL SLA SCAN" CLR_RESET "      " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET "\n");

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

    float availability = ((float)onlineAssets / totalTested) * 100;
    int offlineAssets = totalTested - onlineAssets;

    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "         " CLR_BOLD "NETWORK TEST COMPLETED" CLR_RESET "        " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "  Total Assets Checked:    %-12d " CLR_CYAN "|" CLR_RESET, totalTested);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "  Devices Online:          " CLR_GREEN "%-12d" CLR_RESET " " CLR_CYAN "|" CLR_RESET, onlineAssets);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "  Devices Offline:         " CLR_RED "%-12d" CLR_RESET " " CLR_CYAN "|" CLR_RESET, offlineAssets);
    
    if (availability < 70.0) {
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  Overall SLA Health:      " CLR_BOLD CLR_RED "%-6.2f%%" CLR_RESET "     " CLR_CYAN "|" CLR_RESET, availability);
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t  " CLR_BOLD CLR_RED "[CRITICAL SLA ALARM] Infrastructure degraded!" CLR_RESET "\n");
    } else {
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  Overall SLA Health:      " CLR_BOLD CLR_GREEN "%-6.2f%%" CLR_RESET "     " CLR_CYAN "|" CLR_RESET, availability);
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t  " CLR_GREEN "[SLA STATUS] Network availability within safe thresholds." CLR_RESET "\n");
    }
    printf("\t" CLR_CYAN "+---------------------------------------+" CLR_RESET "\n");

    printf("\n\tPress Enter to return to management console...");
    getchar();
    clearScreen();
}

void runLocalNetworkDiagnostic() {

    clearScreen();
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "     " CLR_BOLD "RUNNING LOCAL NETWORK DIAGNOSTIC" CLR_RESET "   " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t  " CLR_CYAN "[SYSTEM] Querying host network sub-layer..." CLR_RESET "\n");

    if (currentOS == WINDOWS) {
        system("ipconfig > resultado_rede_local.txt");
    } else {
        system("ip a > resultado_rede_local.txt 2>/dev/null || ifconfig > resultado_rede_local.txt");
    }

    printf("\n\t" CLR_GREEN "[SUCCESS] Local topology mapping completed!" CLR_RESET);
    printf("\n\t" CLR_CYAN ">>> Profile exported to 'resultado_rede_local.txt'." CLR_RESET "\n");
    
    printf("\n\tPress Enter to return to main console...");
    getchar();
    clearScreen();
}

void runArpCacheDiagnostic() {
    clearScreen();
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "       " CLR_BOLD "RUNNING ARP CACHE DIAGNOSTIC" CLR_RESET "     " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t  " CLR_CYAN "[SYSTEM] Dumping Address Resolution table..." CLR_RESET "\n");

    system("arp -a > resultado_arp.txt");

    printf("\n\t" CLR_GREEN "[SUCCESS] ARP cache binding map extracted!" CLR_RESET);
    printf("\n\t" CLR_CYAN ">>> Data exported to 'resultado_arp.txt'." CLR_RESET "\n");
    
    printf("\n\tPress Enter to return to main console...");
    getchar();
    clearScreen();
}

void runDnsLookupDiagnostic() {
    char domain[100];
    char command[150];

    clearScreen();
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "       " CLR_BOLD "RUNNING DNS LOOKUP DIAGNOSTIC" CLR_RESET "    " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    
    printf("\n\t  Enter domain name (e.g., google.com): ");
    
    if (scanf("%99s", domain) != 1) {
        printf("\n\t" CLR_RED "[ERROR] Invalid domain input format." CLR_RESET "\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');
    clearScreen();

    sprintf(command, "nslookup %s > resultado_dns.txt 2>&1", domain);

    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t  " CLR_BOLD "Target Domain:" CLR_RESET "  %s", domain);
    printf("\n\t  " CLR_CYAN "[SYSTEM] Querying active DNS cluster..." CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET "\n");

    system(command);

    printf("\n\t" CLR_GREEN "[SUCCESS] Domain Name Resolution complete!" CLR_RESET);
    printf("\n\t" CLR_CYAN ">>> Records exported to 'resultado_dns.txt'." CLR_RESET "\n");
    
    printf("\n\tPress Enter to return to main console...");
    getchar();
    clearScreen();
}

void runRouteTracerDiagnostic() {
    char target[100];
    char command[150];

    clearScreen();
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "      " CLR_BOLD "RUNNING ROUTE TRACER DIAGNOSTIC" CLR_RESET "   " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    
    printf("\n\t  Enter target IP or Domain (e.g., 1.1.1.1): ");
    
    if (scanf("%99s", target) != 1) {
        printf("\n\t" CLR_RED "[ERROR] Invalid gateway target format." CLR_RESET "\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');
    clearScreen();

    if (currentOS == WINDOWS) {
        sprintf(command, "tracert %s > resultado_rota.txt", target);
    } else {
        sprintf(command, "traceroute %s > resultado_rota.txt 2>/dev/null || tracepath %s > resultado_rota.txt", target, target);
    }

    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t  " CLR_BOLD "Routing Target:" CLR_RESET " %s", target);
    printf("\n\t  " CLR_YELLOW "[NOC ALERT] Tracing paths hop-by-hop..." CLR_RESET);
    printf("\n\t  " CLR_CYAN "This may take up to 60s. Please wait." CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET "\n");

    system(command);

    printf("\n\t" CLR_GREEN "[SUCCESS] Network topology path fully discovered!" CLR_RESET);
    printf("\n\t" CLR_CYAN ">>> Hop history written to 'resultado_rota.txt'." CLR_RESET "\n");
    
    printf("\n\tPress Enter to return to main console...");
    getchar();
    clearScreen();
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
    if (newNode == NULL) {
        printf("\n\t" CLR_RED "[ERROR] Kernel Alert: Memory allocation failed for Sensor Ticket." CLR_RESET "\n");
        return;
    }
    
    static int sensorTicketCounter = 7001; 
    newNode->ticketId = sensorTicketCounter++;
    
    strcpy(newNode->targetCode, sensorCode);
    strcpy(newNode->type, "SENSOR_ANOMALY");
    
    sprintf(newNode->description, "ALERT: Sensor telemetry out of bounds. Current state marked as %s.", status);
    
    int isCritical = (strcmp(status, "Critical") == 0 || strcmp(status, "CRITICAL") == 0);
    if (isCritical) {
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

    const char* alertColor = isCritical ? CLR_RED : CLR_YELLOW;
    
    printf("\n\t%s+---------------------------------------+%s", alertColor, CLR_RESET);
    printf("\n\t%s|%s    " CLR_BOLD "AUTOMATED SENSOR TELEMETRY ALERT" CLR_RESET "   %s|%s", alertColor, CLR_RESET, alertColor, CLR_RESET);
    printf("\n\t%s+---------------------------------------+%s", alertColor, CLR_RESET);
    printf("\n\t  " CLR_BOLD "Ticket ID:" CLR_RESET "  " CLR_YELLOW "#%d" CLR_RESET, newNode->ticketId);
    printf("\n\t  " CLR_BOLD "Sensor:" CLR_RESET "     %s", newNode->targetCode);
    printf("\n\t  " CLR_BOLD "Telemetry:"  CLR_RESET "  %s", status);
    printf("\n\t  " CLR_BOLD "Priority:" CLR_RESET "   %s%s (Queue Level-2)%s", alertColor, newNode->priority, CLR_RESET);
    printf("\n\t%s+---------------------------------------+%s\n", alertColor, CLR_RESET);
}

void importSensorReadings(SensorStack* s, IncidentQueue* q) {
    FILE* file = fopen("sensors_rack.txt", "r");
    if (file == NULL) {
        printf("\n\t" CLR_RED "[ERROR] Could not open 'sensors_rack.txt'. Ensure API download worked." CLR_RESET "\n");
        return;
    }

    FILE* regFile = fopen("log_sensores.txt", "w");
    if (regFile == NULL) {
        printf("\n\t" CLR_YELLOW "[WARNING] Audit subsystem offline. Proceeding without 'log_sensores.txt'." CLR_RESET "\n");
    }

    char line[256];
    int importedCount = 0;
    int incidentCount = 0;

    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "         " CLR_BOLD "DATA INGESTION ENGINE" CLR_RESET "         " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t  " CLR_CYAN "[SYSTEM] Processing CSV stream..." CLR_RESET "\n");

    while (fgets(line, sizeof(line), file)) {
        
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') {
            continue;
        }
  
        if (strstr(line, "codigo_sensor") != NULL) {
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

    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "        " CLR_BOLD "FILE INGESTION COMPLETED" CLR_RESET "        " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "  Readings Saved to Stack:   " CLR_GREEN "%-11d" CLR_RESET " " CLR_CYAN "|" CLR_RESET, importedCount);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "  Incidents Enqueued:        " CLR_YELLOW "%-11d" CLR_RESET " " CLR_CYAN "|" CLR_RESET, incidentCount);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t  " CLR_GREEN "[SUCCESS] Audit log written to 'log_sensores.txt'" CLR_RESET "\n");
    printf("\t" CLR_CYAN "+---------------------------------------+" CLR_RESET "\n");
}

int fetchSensorDataFromAPI() {
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "         " CLR_BOLD "API TELEMETRY GATEWAY" CLR_RESET "         " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t  Connecting to https://sensorlab.innominatum.pt ...\n");

    const char* cmd = "curl -s https://sensorlab.innominatum.pt/v1/sensors/export/legacy > \"C:\\Users\\asdia\\Desktop\\P1\\TP1\\sensors_rack.txt\"";

    int status = system(cmd);

    if (status == 0) {
        printf("\n\t" CLR_GREEN "[SUCCESS] Remote cluster synchronized!" CLR_RESET);
        printf("\n\t" CLR_CYAN ">>> Live sensor data pulled into 'sensors_rack.txt'." CLR_RESET "\n");
        
        printf("\n\tPress Enter to continue...");
        getchar();
        clearScreen();
        return 1;
    } else {
        printf("\n\t" CLR_YELLOW "[WARNING] API Endpoint unreachable (HTTP/Timeout Failure)." CLR_RESET);
        printf("\n\t" CLR_CYAN ">>> Falling back to cached local data structures." CLR_RESET "\n");
        return 0;
    }
}

void menuConnectivity(Node* list, IncidentQueue* q) {
    int option;

    #define CLR_CYAN    "\x1b[36m"
    #define CLR_GREEN   "\x1b[32m"
    #define CLR_RED     "\x1b[31m"
    #define CLR_RESET   "\x1b[0m"
    #define CLR_BOLD    "\x1b[1m"

    do {
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "   " CLR_BOLD "MINI NOC SYSTEM - CONNECTIVITY TESTS" CLR_RESET " " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "                                       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "1." CLR_RESET " Ping a Specific Asset IP          " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "2." CLR_RESET " Run General Network Test (Sweep)  " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "3." CLR_RESET " Run Local Network Diagnostic      " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "4." CLR_RESET " Run ARP Cache Diagnostic          " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "5." CLR_RESET " Run DNS Lookup Diagnostic         " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "6." CLR_RESET " Run Route Tracer Diagnostic       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "                                       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_RED "0. Return to Main Menu" CLR_RESET "               " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t  Select an option " CLR_BOLD "-->" CLR_RESET " ");

        if(scanf("%d", &option) != 1) {
            printf("\n\t" CLR_RED "[ERROR] Invalid input. Please enter a number." CLR_RESET "\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            printf("\n\tPress Enter to continue...");
            getchar();
            clearScreen();
            continue;
        }
        getchar();

        switch (option) {
            case 1: {
                clearScreen();
                if (list == NULL) {
                    printf("\n\t" CLR_RED "[WARNING] The inventory is empty. No assets available for testing." CLR_RESET "\n");
                    printf("\n\tPress Enter to return...");
                    getchar();
                    clearScreen();
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
                        printf("\n" CLR_RED ">>> No equipment found with ID %d." CLR_RESET "\n", searchId);
                        printf("\nPress Enter to continue...");
                        getchar();
                    }
                }
                clearScreen();
                break;
            }
            case 2:
                clearScreen();
                runGeneralNetworkTest(list, q);
                clearScreen();
                break;
            case 3:
                clearScreen();
                runLocalNetworkDiagnostic();
                clearScreen();
                break;
            case 4:
                clearScreen();
                runArpCacheDiagnostic();
                clearScreen();
                break;
            case 5:
                clearScreen();
                runDnsLookupDiagnostic();
                clearScreen();
                break;
            case 6:
                clearScreen();
                runRouteTracerDiagnostic();
                clearScreen();
                break;
            case 0:
                clearScreen();
                printf("\n" CLR_CYAN "[SYSTEM] Returning to Main Menu..." CLR_RESET "\n");
                break;
            default:
                clearScreen();
                printf("\n\t" CLR_RED "[WARNING] Invalid option. Please try again." CLR_RESET "\n");
                printf("\n\tPress Enter to return to menu...");
                getchar();
                clearScreen();
                break;
        }
    } while (option != 0);
}

void listRecentReadings(SensorStack* s) {
    if (s->top == NULL) {
        printf("\n\t" CLR_RED "[INFO] No sensor readings available in the telemetry stack." CLR_RESET "\n");
        return;
    }

    clearScreen();
    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "                      " CLR_BOLD "RECENT SENSOR READINGS (LIFO)" CLR_RESET "                      " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);
    printf("\n\t  " CLR_BOLD "%-15s %-20s %-10s %-10s %-15s" CLR_RESET, "CODE", "TYPE", "VALUE", "UNIT", "STATUS");
    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);

    SensorReading* current = s->top;
    
    while (current != NULL) {
        const char* statusColor = CLR_RESET;
        if (strcmp(current->status, "CRITICAL") == 0 || strcmp(current->status, "GRID_FAILURE") == 0) {
            statusColor = CLR_RED;
        } else if (strcmp(current->status, "WARNING") == 0) {
            statusColor = CLR_YELLOW;
        } else if (strcmp(current->status, "OK") == 0 || strcmp(current->status, "OPERATIONAL") == 0) {
            statusColor = CLR_GREEN;
        }

        printf("\n\t  %-15s %-20s %-10.2f %-10s %s%-15s%s", 
               current->code, current->type, current->value, current->unit, 
               statusColor, current->status, CLR_RESET);
        
        current = current->next;
    }

    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET "\n");

    printf("\n\tPress Enter to return to telemetry sub-menu...");
    getchar();
    clearScreen();
}

void searchSensorByCode(SensorStack* s) {
    if (s->top == NULL) {
        printf("\n\t" CLR_RED "[INFO] No sensor readings available in the stack." CLR_RESET "\n");
        return;
    }

    char searchCode[20];

    printf("\n\tEnter Sensor Code to search (e.g., TEMP_RACK): ");
    if (scanf("%19s", searchCode) != 1) {
        printf("\n\t" CLR_RED "[ERROR] Invalid input." CLR_RESET "\n");
    }
    while (getchar() != '\n');

    clearScreen();

    SensorReading* current = s->top;
    int foundCount = 0;

    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "                     " CLR_BOLD "SEARCH RESULTS: %-15s" CLR_RESET "                    " CLR_CYAN "|" CLR_RESET, searchCode);
    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);
    printf("\n\t  " CLR_BOLD "%-15s %-20s %-10s %-10s %-15s" CLR_RESET, "CODE", "TYPE", "VALUE", "UNIT", "STATUS");
    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);

    while (current != NULL) {
        if (strcasecmp(current->code, searchCode) == 0) {
            const char* statusColor = CLR_RESET;
            if (strcmp(current->status, "CRITICAL") == 0 || strcmp(current->status, "GRID_FAILURE") == 0) statusColor = CLR_RED;
            else if (strcmp(current->status, "WARNING") == 0) statusColor = CLR_YELLOW;
            else if (strcmp(current->status, "OK") == 0 || strcmp(current->status, "OPERATIONAL") == 0) statusColor = CLR_GREEN;

            printf("\n\t  %-15s %-20s %-10.2f %-10s %s%-15s%s", 
                   current->code, current->type, current->value, current->unit, 
                   statusColor, current->status, CLR_RESET);
            foundCount++;
        }
        current = current->next;
    }

    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);
    
    if (foundCount == 0) {
        printf("\n\t" CLR_RED "[RESULT] No readings found matching '%s'." CLR_RESET "\n", searchCode);
    } else {
        printf("\n\t" CLR_GREEN "[RESULT] Found %d records for target code '%s'." CLR_RESET "\n", foundCount, searchCode);
    }

    printf("\n\tPress Enter to return to telemetry sub-menu...");
    getchar();
    clearScreen();
}

void displayAnomalousReadings(SensorStack* s) {
    if (s->top == NULL) {
        printf("\n\t" CLR_RED "[INFO] Stack is empty. No sensor data to analyze." CLR_RESET "\n");
        return;
    }

    clearScreen();
    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "                     " CLR_BOLD "ACTIVE SENSOR ANOMALIES" CLR_RESET "                     " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);
    printf("\n\t  " CLR_BOLD "%-15s %-20s %-10s %-10s %-15s" CLR_RESET, "CODE", "TYPE", "VALUE", "UNIT", "STATUS");
    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);

    SensorReading* current = s->top;
    int anomalyCount = 0;

    while (current != NULL) {
        if (strcmp(current->status, "WARNING") == 0 ||
            strcmp(current->status, "CRITICAL") == 0 ||
            strcmp(current->status, "GRID_FAILURE") == 0) {

            const char* statusColor = (strcmp(current->status, "WARNING") == 0) ? CLR_YELLOW : CLR_RED;

            printf("\n\t  %-15s %-20s %-10.2f %-10s %s%-15s%s", 
                   current->code, current->type, current->value, current->unit, 
                   statusColor, current->status, CLR_RESET);
            anomalyCount++;
        }
        current = current->next;
    }

    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);
    
    if (anomalyCount == 0) {
        printf("\n\t" CLR_GREEN "[SYSTEM] No anomalies detected. Infrastructure healthy." CLR_RESET "\n");
    } else {
        printf("\n\t" CLR_RED "[ALERT] Total anomalous readings detected: %d" CLR_RESET "\n", anomalyCount);
    }
    printf("\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET "\n");

    printf("\n\tPress Enter to return to telemetry sub-menu...");
    getchar();
    clearScreen();
}

void displayPendingIncidents(IncidentQueue* q) {
    if (q == NULL || q->front == NULL) {
        clearScreen();
        printf("\n\t" CLR_GREEN "+---------------------------------------+" CLR_RESET);
        printf("\n\t" CLR_GREEN "|" CLR_RESET "        " CLR_BOLD "INCIDENT MANAGEMENT QUEUE" CLR_RESET "      " CLR_GREEN "|" CLR_RESET);
        printf("\n\t" CLR_GREEN "+---------------------------------------+" CLR_RESET);
        printf("\n\t  [SYSTEM] Queue empty. No active incidents.");
        printf("\n\t" CLR_GREEN "+---------------------------------------+" CLR_RESET "\n");
        return;
    }

    clearScreen();
    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "                      " CLR_BOLD "ACTIVE TECHNICAL INCIDENTS" CLR_RESET "                       " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);
    printf("\n\t  " CLR_BOLD "%-10s %-15s %-15s %-10s %-15s" CLR_RESET, "ID", "ASSET", "TYPE", "PRIORITY", "STATUS");
    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);

    TechnicalIncident* current = q->front;
    while (current != NULL) {
        const char* pColor = (strcmp(current->priority, "HIGH") == 0) ? CLR_RED : CLR_YELLOW;
        
        printf("\n\t  " CLR_YELLOW "#%-8d" CLR_RESET " %-15s %-15s %s%-10s%s %-15s", 
               current->ticketId, current->targetCode, current->type, 
               pColor, current->priority, CLR_RESET, current->status);
        
        printf("\n\t  " CLR_CYAN "-> " CLR_RESET "Tech: %s | Time: %s", current->technician, current->timestamp);
        printf("\n\t  " CLR_CYAN "-> " CLR_RESET "Desc: %s", current->description);
        printf("\n\t" CLR_CYAN "-------------------------------------------------------------------------" CLR_RESET);
        
        current = current->next;
    }
    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET "\n");

    printf("\n\tPress Enter to return to management console...");
    getchar();
    clearScreen();
}    

void menuSensors(SensorStack* stack, IncidentQueue* queue) {
    int option = -1;

    #define CLR_CYAN    "\x1b[36m"
    #define CLR_GREEN   "\x1b[32m"
    #define CLR_RED     "\x1b[31m"
    #define CLR_RESET   "\x1b[0m"
    #define CLR_BOLD    "\x1b[1m"

    do {
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "        " CLR_BOLD "MINI NOC SYSTEM - SENSORS" CLR_RESET "       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "                                       " CLR_CYAN);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "1." CLR_RESET " List All Recent Readings (Full Stack)" CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "2." CLR_RESET " Search Readings by Sensor Code     " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "3." CLR_RESET " Display Anomalous Readings Only    " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "4." CLR_RESET " Display Pending Incident Tickets   " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "                                       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_RED "0. Return to Main Menu" CLR_RESET "               " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t  Select an option " CLR_BOLD "-->" CLR_RESET " ");
        
        if (scanf("%d", &option) != 1) {
            printf("\n\t" CLR_RED "[ERROR] Invalid input. Please enter a number." CLR_RESET "\n");
            while (getchar() != '\n');
            printf("\n\tPress Enter to continue...");
            getchar();
            clearScreen();
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
                clearScreen();
                printf("\n" CLR_CYAN "[SYSTEM] Returning to Main Menu..." CLR_RESET "\n");
                break;
            default:
                clearScreen();
                printf("\n\t" CLR_RED "[WARNING] Invalid option. Please try again." CLR_RESET "\n");
                printf("\n\tPress Enter to return to menu...");
                getchar();
                clearScreen();
                break;
        }
    } while (option != 0);
}

int validateAssetExistence(Node* invHead, SensorStack* s, const char* userInput, char* foundCode) {
    if (userInput == NULL || strlen(userInput) == 0) return 0;

    char cleanInput[50];
    strncpy(cleanInput, userInput, sizeof(cleanInput) - 1);
    cleanInput[sizeof(cleanInput) - 1] = '\0';
    
    int len = strlen(cleanInput);
    while (len > 0 && (cleanInput[len - 1] == '\n' || cleanInput[len - 1] == '\r' || cleanInput[len - 1] == ' ')) {
        cleanInput[len - 1] = '\0';
        len--;
    }

    Node* currEquip = invHead;
    while (currEquip != NULL) {
        char idStr[15];
        sprintf(idStr, "%d", currEquip->data.id);

        if (strcmp(currEquip->data.name, cleanInput) == 0 || 
            strcmp(currEquip->data.ip, cleanInput) == 0 || 
            strcmp(idStr, cleanInput) == 0) {
            
            if (foundCode != NULL) strcpy(foundCode, currEquip->data.name);
            return 1;
        }
        currEquip = currEquip->next;
    }

    if (s != NULL) {
        SensorReading* currSensor = s->top;
        while (currSensor != NULL) {
            if (strcmp(currSensor->code, cleanInput) == 0) {
                if (foundCode != NULL) {
                    strcpy(foundCode, currSensor->code);
                }
                return 1;
            }
            currSensor = currSensor->next;
        }
    }

    return 0;
}

void createManualIncident(IncidentQueue* q, Node* invHead, SensorStack* s) {
    char userInput[50];
    char finalAssetCode[30];

    clearScreen();
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "        " CLR_BOLD "REGISTER MANUAL INCIDENT" CLR_RESET "       " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    
    printf("\n\t  Enter Asset/Sensor Name, IP or Code: ");
    fgets(userInput, sizeof(userInput), stdin);
    userInput[strcspn(userInput, "\n")] = 0;

    if (!validateAssetExistence(invHead, s, userInput, finalAssetCode)) {
        printf("\n\t" CLR_RED "[ERROR] Asset/Sensor '%s' not found in registry." CLR_RESET "\n", userInput);
        printf("\n\tPress Enter to return...");
        getchar();
        clearScreen();
        return;
    }

    TechnicalIncident* newNode = (TechnicalIncident*)malloc(sizeof(TechnicalIncident));
    if (newNode == NULL) return;

    static int manualCounter = 1001;
    newNode->ticketId = manualCounter++;
    strcpy(newNode->targetCode, finalAssetCode);

    printf("\n\t  Incident Type (e.g., REPAIR, CONFIG): ");
    fgets(newNode->type, sizeof(newNode->type), stdin);
    newNode->type[strcspn(newNode->type, "\n")] = 0;

    printf("\t  Problem Description: ");
    fgets(newNode->description, sizeof(newNode->description), stdin);
    newNode->description[strcspn(newNode->description, "\n")] = 0;

    int pOption;
    printf("\t  Priority (1-HIGH, 2-MEDIUM, 3-LOW): ");
    if (scanf("%d", &pOption) != 1) pOption = 3;
    while (getchar() != '\n');

    if (pOption == 1) strcpy(newNode->priority, "HIGH");
    else if (pOption == 2) strcpy(newNode->priority, "MEDIUM");
    else strcpy(newNode->priority, "LOW");

    strcpy(newNode->technician, "Not Assigned");
    strcpy(newNode->status, "Pending");
    getCurrentDateTime(newNode->timestamp);
    newNode->closedtimestamp[0] = '\0';
    newNode->next = NULL;

    if (q->rear == NULL) {
        q->front = q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }

    printf("\n\t" CLR_GREEN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_GREEN "|" CLR_RESET "  " CLR_BOLD "[SUCCESS] Ticket #%d created." CLR_RESET "   " CLR_GREEN "|" CLR_RESET, newNode->ticketId);
    printf("\n\t" CLR_GREEN "+---------------------------------------+" CLR_RESET "\n");
    
    printf("\n\tPress Enter to return to management console...");
    getchar();
    clearScreen();
}

void displayIncidentByStatus(IncidentQueue* q, const char* targetStatus, const char* title) {
    if (q == NULL || q->front == NULL) {
        clearScreen();
        printf("\n\t" CLR_RED "+---------------------------------------+" CLR_RESET);
        printf("\n\t" CLR_RED "|" CLR_RESET "           " CLR_BOLD "INCIDENT FILTER" CLR_RESET "           " CLR_RED "|" CLR_RESET);
        printf("\n\t" CLR_RED "+---------------------------------------+" CLR_RESET);
        printf("\n\t  [SYSTEM] Queue empty. No incidents to filter.");
        printf("\n\t" CLR_RED "+---------------------------------------+" CLR_RESET "\n");
        return;
    }

    clearScreen();
    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "                      %s                       " CLR_CYAN "|" CLR_RESET, title);
    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);
    printf("\n\t  " CLR_BOLD "%-10s %-15s %-15s %-10s %-15s" CLR_RESET, "ID", "ASSET", "TYPE", "PRIORITY", "STATUS");
    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);

    TechnicalIncident* current = q->front;
    int count = 0;

    while (current != NULL) {
        if (strcmp(current->status, targetStatus) == 0) {
            const char* pColor = (strcmp(current->priority, "HIGH") == 0) ? CLR_RED : CLR_YELLOW;
            
            printf("\n\t  " CLR_YELLOW "#%-8d" CLR_RESET " %-15s %-15s %s%-10s%s %-15s", 
                   current->ticketId, current->targetCode, current->type, 
                   pColor, current->priority, CLR_RESET, current->status);
            
            printf("\n\t  " CLR_CYAN "-> " CLR_RESET "Tech: %s | Time: %s", current->technician, current->timestamp);
            printf("\n\t  " CLR_CYAN "-> " CLR_RESET "Desc: %s", current->description);
            printf("\n\t" CLR_CYAN "-------------------------------------------------------------------------" CLR_RESET);
            count++;
        }
        current = current->next;
    }

    if (count == 0) {
        printf("\n\t" CLR_YELLOW "[INFO] No incidents found with status: %s" CLR_RESET "\n", targetStatus);
    } else {
        printf("\n\t" CLR_GREEN "[SUCCESS] Filtered %d records." CLR_RESET "\n", count);
    }

    printf("\n\tPress Enter to return...");
    getchar();
    clearScreen();
}   

void displayIncidentsByAsset(IncidentQueue* q) {
    if (q == NULL || q->front == NULL) {
        clearScreen();
        printf("\n\t" CLR_YELLOW "[INFO] Incident queue is currently empty." CLR_RESET "\n");
        return;
    }

    char targetInput[30];
    clearScreen();
    
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "         " CLR_BOLD "FILTER INCIDENTS BY ASSET" CLR_RESET "        " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t  Enter Asset Name, Code or IP: ");
    
    fgets(targetInput, sizeof(targetInput), stdin);
    targetInput[strcspn(targetInput, "\n")] = 0;
    clearScreen();

    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "                   " CLR_BOLD "HISTORY FOR: %-15s" CLR_RESET "                 " CLR_CYAN "|" CLR_RESET, targetInput);
    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);
    printf("\n\t  " CLR_BOLD "%-10s %-15s %-15s %-10s %-15s" CLR_RESET, "ID", "ASSET", "TYPE", "PRIORITY", "STATUS");
    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);

    int count = 0;
    TechnicalIncident* current = q->front;
    
    while (current != NULL) {
        if (strcmp(current->targetCode, targetInput) == 0) {
            const char* pColor = (strcmp(current->priority, "HIGH") == 0) ? CLR_RED : CLR_YELLOW;
            
            printf("\n\t  " CLR_YELLOW "#%-8d" CLR_RESET " %-15s %-15s %s%-10s%s %-15s", 
                   current->ticketId, current->targetCode, current->type, 
                   pColor, current->priority, CLR_RESET, current->status);
            
            printf("\n\t  " CLR_CYAN "-> " CLR_RESET "Desc: %s", current->description);
            printf("\n\t" CLR_CYAN "-------------------------------------------------------------------------" CLR_RESET);
            count++;
        }
        current = current->next;
    }

    if (count == 0) {
        printf("\n\t" CLR_YELLOW "[INFO] No incident history found for asset '%s'." CLR_RESET "\n", targetInput);
    } else {
        printf("\n\t" CLR_GREEN "[SUCCESS] Extracted %d incident record(s)." CLR_RESET "\n", count);
    }

    printf("\n\tPress Enter to return to management console...");
    getchar();
    clearScreen();
}

void displayIncidentsByPriority(IncidentQueue* q) {
    if (q == NULL || q->front == NULL) {
        clearScreen();
        printf("\n\t" CLR_RED "[INFO] Queue empty. No incidents to filter." CLR_RESET "\n");
        return;
    }

    int choice;
    char targetPriority[10] = "LOW";

    clearScreen();
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "         " CLR_BOLD "FILTER INCIDENTS BY PRIORITY" CLR_RESET "        " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t  1. " CLR_RED "HIGH" CLR_RESET "\n\t  2. " CLR_YELLOW "MEDIUM" CLR_RESET "\n\t  3. LOW");
    printf("\n\t  Choice: ");
    
    if (scanf("%d", &choice) != 1) choice = 3;
    while (getchar() != '\n');

    if (choice == 1) strcpy(targetPriority, "HIGH");
    else if (choice == 2) strcpy(targetPriority, "MEDIUM");
    else strcpy(targetPriority, "LOW");

    clearScreen();
    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "                " CLR_BOLD "PRIORITY LEVEL: %-10s" CLR_RESET "                 " CLR_CYAN "|" CLR_RESET, targetPriority);
    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);
    printf("\n\t  " CLR_BOLD "%-10s %-15s %-15s %-10s %-15s" CLR_RESET, "ID", "ASSET", "TYPE", "PRIORITY", "STATUS");
    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);

    int count = 0;
    TechnicalIncident* current = q->front;
    
    while (current != NULL) {
        if (strcmp(current->priority, targetPriority) == 0) {
            const char* pColor = (strcmp(current->priority, "HIGH") == 0) ? CLR_RED : CLR_YELLOW;
            
            printf("\n\t  " CLR_YELLOW "#%-8d" CLR_RESET " %-15s %-15s %s%-10s%s %-15s", 
                   current->ticketId, current->targetCode, current->type, 
                   pColor, current->priority, CLR_RESET, current->status);
            
            printf("\n\t  " CLR_CYAN "-> " CLR_RESET "Desc: %s", current->description);
            printf("\n\t" CLR_CYAN "-------------------------------------------------------------------------" CLR_RESET);
            count++;
        }
        current = current->next;
    }

    if (count == 0) {
        printf("\n\t" CLR_YELLOW "[INFO] No incidents found with priority: %s" CLR_RESET "\n", targetPriority);
    } else {
        printf("\n\t" CLR_GREEN "[SUCCESS] Found %d incident record(s)." CLR_RESET "\n", count);
    }

    printf("\n\tPress Enter to return...");
    getchar();
    clearScreen();
}

void displayCompletedHistory(TechnicalIncident* historyHead) {
    if (historyHead == NULL) {
        clearScreen();
        printf("\n\t" CLR_GREEN "+---------------------------------------+" CLR_RESET);
        printf("\n\t" CLR_GREEN "|" CLR_RESET "        " CLR_BOLD "INCIDENT ARCHIVE EMPTY" CLR_RESET "         " CLR_GREEN "|" CLR_RESET);
        printf("\n\t" CLR_GREEN "+---------------------------------------+" CLR_RESET);
        printf("\n\t  [SYSTEM] No archived/completed incidents found.");
        printf("\n\t" CLR_GREEN "+---------------------------------------+" CLR_RESET "\n");
        return;
    }

    clearScreen();
    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "                      " CLR_BOLD "HISTORIC: COMPLETED INCIDENTS" CLR_RESET "                      " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);
    printf("\n\t  " CLR_BOLD "%-10s %-15s %-15s %-12s" CLR_RESET, "ID", "ASSET", "TYPE", "TECH");
    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);

    TechnicalIncident* current = historyHead;
    int count = 0;

    while (current != NULL) {
        printf("\n\t  " CLR_GREEN "#%-8d" CLR_RESET " %-15s %-15s %-12s", 
               current->ticketId, current->targetCode, current->type, current->technician);
        
        printf("\n\t  " CLR_CYAN "-> " CLR_RESET "Opened: %s", current->timestamp);
        printf("\n\t  " CLR_CYAN "-> " CLR_RESET "Closed: %s", current->closedtimestamp);
        printf("\n\t  " CLR_CYAN "-> " CLR_RESET "Summary: %s", current->description);
        printf("\n\t" CLR_CYAN "-------------------------------------------------------------------------" CLR_RESET);
        
        current = current->next;
        count++;
    }

    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);
    printf("\n\t " CLR_GREEN "[SUCCESS] Displaying %d historical records." CLR_RESET "\n", count);

    printf("\n\tPress Enter to return to management console...");
    getchar();
    clearScreen();
}

void menuIncidentReports(IncidentQueue* q, TechnicalIncident** historyHead) {
    int option = -1;

    do {
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "      " CLR_BOLD "SUBMENU - REPORTS & QUERIES" CLR_RESET "      " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "                                       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "1." CLR_RESET " List All Pending Incidents        " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "2." CLR_RESET " List All In Progress Incidents    " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "3." CLR_RESET " List All Completed Incidents      " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "4." CLR_RESET " Search Incidents by Asset         " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "5." CLR_RESET " Search Incidents by Priority      " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "                                       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_RED "0. Back to Incident Menu" CLR_RESET "             " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t  Select an option " CLR_BOLD "-->" CLR_RESET " ");

        if (scanf("%d", &option) != 1) {
            printf("\n\t" CLR_RED "[ERROR] Invalid input. Please enter a number." CLR_RESET "\n");
            while (getchar() != '\n');
            printf("\n\tPress Enter to continue...");
            getchar();
            clearScreen();
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
                displayCompletedHistory(*historyHead);
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
                printf("\n" CLR_CYAN "[SYSTEM] Returning to Incident Menu..." CLR_RESET "\n");
                break;
            default:
                clearScreen();
                printf("\n\t" CLR_RED "[WARNING] Invalid option. Please try again." CLR_RESET "\n");
                printf("\n\tPress Enter to return to menu...");
                getchar();
                clearScreen();
                break;
        }
    } while (option != 0);
}

void processNextIncident(IncidentQueue* q, Node* invHead, TechnicalIncident** historyHead) {
    if (q == NULL || q->front == NULL) {
        printf("\n=========================================================================");
        printf("\n                    RESOLUTION & INCIDENT PROCESSING                     ");
        printf("\n=========================================================================");
        printf("\n[INFO] No incidents in the queue.");
        printf("\n=========================================================================\n");
        return;
    }

    TechnicalIncident* activeTicket = q->front;

    printf("\n=========================================================================");
    printf("\n                 INCIDENT PROCESSING (FIFO)                        ");
    printf("\n=========================================================================");
    printf("\n  TICKET ID:    #%d", activeTicket->ticketId);
    printf("\n  Target/Asset: %s", activeTicket->targetCode);
    printf("\n  Alert Type:   %s", activeTicket->type);
    printf("\n  Current Status: [%s]", activeTicket->status);
    printf("\n  Priority:     %s", activeTicket->priority);
    printf("\n  Description:  %s", activeTicket->description);
    printf("\n=========================================================================");

    int choice = -1;
    
    if (strcmp(activeTicket->status, "Pending") == 0) {
        printf("\nThe next incident in the queue is PENDING.");
        printf("\n  1. Set incident to IN PROGRESS (Start Handling)");
        printf("\n  0. Exit without changes");
        printf("\nChoose an option: ");
        fflush(stdout);

        if (scanf("%d", &choice) != 1) { while (getchar() != '\n'); return; }
        while (getchar() != '\n');

        if (choice == 1) {
            printf("Insert the technician name: ");
            fgets(activeTicket->technician, sizeof(activeTicket->technician), stdin);
            activeTicket->technician[strcspn(activeTicket->technician, "\n")] = 0;

            strcpy(activeTicket->status, "In Progress");
            printf("\n>>> [SUCCESS] Ticket #%d is now 'In Progress'.\n", activeTicket->ticketId);
        }
    } 
    else if (strcmp(activeTicket->status, "In Progress") == 0) {
        printf("\nThe next incident in the queue is IN PROGRESS.");
        printf("\n  1. RESOLVE INCIDENT (Move to Completed History)");
        printf("\n  0. Exit without changes");
        printf("\nChoose an option: ");
        fflush(stdout);

        if (scanf("%d", &choice) != 1) { while (getchar() != '\n'); return; }
        while (getchar() != '\n');

        if (choice == 1) {
            char solution[100];
            printf("Briefly describe the technical solution applied: ");
            fflush(stdout);
            fgets(solution, sizeof(solution), stdin);
            solution[strcspn(solution, "\n")] = 0;

            char finalDesc[120];
            sprintf(finalDesc, "%s (SOLUCION: %s)", activeTicket->description, solution);
            strncpy(activeTicket->description, finalDesc, sizeof(activeTicket->description) - 1);

            strcpy(activeTicket->status, "Completed");
            
            getCurrentDateTime(activeTicket->closedtimestamp); 

            char savedTime[20];
            strcpy(savedTime, activeTicket->closedtimestamp);

            Node* currEquip = invHead;
            while (currEquip != NULL) {
                if (strcmp(currEquip->data.name, activeTicket->targetCode) == 0) {
                    strcpy(currEquip->data.status, "Operational");
                    break;
                }
                currEquip = currEquip->next;
            }

            q->front = q->front->next;
            if (q->front == NULL) {
                q->rear = NULL;
            }

            activeTicket->next = *historyHead;
            *historyHead = activeTicket;

            printf("\n>>> [SUCCESS] Ticket #%d closed and archived in the Completed History at %s!\n", 
                   activeTicket->ticketId, savedTime);
        }
    }
}

void menuIncidents(IncidentQueue* queue, Node* invHead, SensorStack* sensorStack, TechnicalIncident** historyHead) {
    int option = -1;
    do {
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "     " CLR_BOLD "MINI NOC SYSTEM - INCIDENTS" CLR_RESET "       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "                                       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "1." CLR_RESET " Log New Incident Manually         " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "2." CLR_RESET " Process Next Incident in Queue    " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "3." CLR_RESET " View Incident Reports             " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "                                       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_RED "0. Return to Main Menu" CLR_RESET "               " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t  Select an option " CLR_BOLD "-->" CLR_RESET " ");

        if (scanf("%d", &option) != 1) {
            printf("\n\t" CLR_RED "[ERROR] Invalid input. Please enter a number." CLR_RESET "\n");
            while (getchar() != '\n');
            printf("\n\tPress Enter to continue...");
            getchar();
            clearScreen();
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
                processNextIncident(queue, invHead, historyHead);
                break;
            case 3:
                clearScreen();
                menuIncidentReports(queue, historyHead);
                break;
            case 0:
                clearScreen();
                printf("\n" CLR_CYAN "[SYSTEM] Returning to Main Menu..." CLR_RESET "\n");
                break;
            default:
                clearScreen();
                printf("\n\t" CLR_RED "[WARNING] Invalid option. Please try again." CLR_RESET "\n");
                printf("\n\tPress Enter to return to menu...");
                getchar();
                clearScreen();
                break;
        }

    } while (option != 0);  
}

void saveIncidentsToBinary(IncidentQueue* q) {
    if (q == NULL || q->front == NULL) {
        printf("\n\t" CLR_YELLOW "[INFO] Queue empty. No data to persist to disk." CLR_RESET "\n");
        return;
    }

    FILE* file = fopen("incidents_back.dat", "wb");
    if (file == NULL) {
        printf("\n\t" CLR_RED "[ERROR] Could not initialize binary backup file." CLR_RESET "\n");
        return;
    }

    int count = 0;
    TechnicalIncident* current = q->front;

    while (current != NULL) {
        fwrite(current, sizeof(TechnicalIncident), 1, file);
        count++;
        current = current->next;
    }

    fclose(file);

    printf("\n\t" CLR_GREEN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_GREEN "|" CLR_RESET "           " CLR_BOLD "SYSTEM BACKUP" CLR_RESET "             " CLR_GREEN "|" CLR_RESET);
    printf("\n\t" CLR_GREEN "+---------------------------------------+" CLR_RESET);
    printf("\n\t  [SUCCESS] %d incidents serialized.", count);
    printf("\n\t  [PATH]    'incidents_back.dat' updated.");
    printf("\n\t" CLR_GREEN "+---------------------------------------+" CLR_RESET "\n");
}

void loadIncidentsFromBinary(IncidentQueue* q) {
    FILE* file = fopen("incidents_backup.dat", "rb");
    if (file == NULL) {
        printf("\n\t" CLR_YELLOW "[INFO] No existing backup found. Initializing fresh queue." CLR_RESET "\n");
        return;
    }

    TechnicalIncident tempRecord;
    int count = 0;

    while (fread(&tempRecord, sizeof(TechnicalIncident), 1, file) == 1) {
        TechnicalIncident* newNode = (TechnicalIncident*)malloc(sizeof(TechnicalIncident));
        if (newNode == NULL) {
            printf("\n\t" CLR_RED "[CRITICAL] Memory allocation failure during restoration." CLR_RESET "\n");
            break;
        }

        *newNode = tempRecord;
        newNode->next = NULL;

        if (q->rear == NULL) {
            q->front = q->rear = newNode;
        } else {
            q->rear->next = newNode;
            q->rear = newNode;
        }
        count++;
    }
    
    fclose(file);

    printf("\n\t" CLR_GREEN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_GREEN "|" CLR_RESET "        " CLR_BOLD "SYSTEM RESTORATION" CLR_RESET "         " CLR_GREEN "|" CLR_RESET);
    printf("\n\t" CLR_GREEN "+---------------------------------------+" CLR_RESET);
    printf("\n\t  [SUCCESS] %d incidents restored from disk.", count);
    printf("\n\t" CLR_GREEN "+---------------------------------------+" CLR_RESET "\n");
}

void saveHistoryToBinary(TechnicalIncident* historyHead) {
    FILE *file = fopen("history.dat", "wb");
    if (file == NULL) return;
    TechnicalIncident* current = historyHead;
    while (current != NULL) {
        fwrite(current, sizeof(TechnicalIncident), 1, file);
        current = current->next;
    }
    fclose(file);
}

TechnicalIncident* loadHistoryFromBinary() {
    FILE * file = fopen("history.dat", "rb");
    if (file == NULL) return NULL;
    TechnicalIncident* head = NULL;
    TechnicalIncident temp;
    while (fread(&temp, sizeof(TechnicalIncident), 1, file) == 1) {
        TechnicalIncident* newNode = (TechnicalIncident*)malloc(sizeof(TechnicalIncident));
        if (newNode != NULL) {
            *newNode = temp;
            newNode->next = head;
            head = newNode;
        }
    }
    fclose(file);
    return head;
}

void pushConfiguration(ConfigStack* stack, char* code, char* type, char* oldVal, char* newVal, char* tech) {
    NetworkConfig* newLog = (NetworkConfig*)malloc(sizeof(NetworkConfig));
    if (newLog == NULL) {
        printf("\n\t" CLR_RED "[CRITICAL] Memory allocation failed for configuration log." CLR_RESET "\n");
        return;
    }

    strcpy(newLog->equipmentCode, code);
    strcpy(newLog->configType, type);
    strcpy(newLog->oldValue, oldVal);
    strcpy(newLog->newValue, newVal);
    strcpy(newLog->technician, tech);
    getCurrentDateTime(newLog->timestamp);

    newLog->next = stack->top;
    stack->top = newLog;

    printf("\n\t" CLR_GREEN "[LOG SUCCESS] Configuration change recorded for asset: %s" CLR_RESET, code);
    printf("\n\t" CLR_CYAN ">>> Change type: %s | Technician: %s" CLR_RESET "\n", type, tech);
}

void registerNewConfiguration(ConfigStack* stack, Node* invHead, SensorStack* sensorStack) {
    char code[50], typeStr[30], newVal[50], tech[30], oldVal[50] = "";
    int typeChoice = 0;

    clearScreen();
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "        " CLR_BOLD "REGISTER NEW CONFIGURATION" CLR_RESET "       " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);

    printf("\n\t " CLR_BOLD "INVENTORY ASSETS:" CLR_RESET);
    if (!invHead) printf("\n\t  -> No assets registered.");
    else {
        Node* curr = invHead;
        while (curr) {
            printf("\n\t  > [%-10s] Location: %s", curr->data.name, curr->data.location);
            curr = curr->next;
        }
    }

    printf("\n\n\t " CLR_BOLD "SYSTEM SENSORS:" CLR_RESET);
    if (!sensorStack || !sensorStack->top) printf("\n\t  -> No sensors registered.");
    else {
        SensorReading* sens = sensorStack->top;
        while (sens) {
            printf("\n\t  > [%-10s] Value: %.2f %s", sens->code, sens->value, sens->unit);
            sens = sens->next;
        }
    }
    
    printf("\n\n\tEnter target Code to modify: ");
    fgets(code, sizeof(code), stdin);
    code[strcspn(code, "\n")] = 0;

    Node* curr = invHead;
    int found = 0;
    while (curr) {
        if (strcmp(curr->data.name, code) == 0) { found = 1; break; }
        curr = curr->next;
    }

    SensorReading* targetSensor = NULL;
    if (!found) {
        SensorReading* sens = sensorStack->top;
        while (sens) {
            if (strcmp(sens->code, code) == 0) { found = 2; targetSensor = sens; break; }
            sens = sens->next;
        }
    }

    if (!found) {
        printf("\n\t" CLR_RED "[ERROR] Asset/Sensor '%s' not found." CLR_RESET "\n", code);
        printf("\n\tPress Enter to return..."); getchar(); clearScreen();
        return;
    }

    if (found == 1) {
        printf("\n\tSelect Parameter:\n\t1. IP Address\n\t2. Location\n\t3. Status\n\tChoice: ");
        scanf("%d", &typeChoice); while(getchar() != '\n');

        if(typeChoice == 1) { strcpy(typeStr, "IP Address"); strcpy(oldVal, curr->data.ip); }
        else if(typeChoice == 2) { strcpy(typeStr, "Location"); strcpy(oldVal, curr->data.location); }
        else { strcpy(typeStr, "Status"); strcpy(oldVal, curr->data.status); }
        
        printf("\tEnter new value: "); fgets(newVal, sizeof(newVal), stdin); newVal[strcspn(newVal, "\n")] = 0;
        if(typeChoice == 1) strcpy(curr->data.ip, newVal);
        else if(typeChoice == 2) strcpy(curr->data.location, newVal);
        else strcpy(curr->data.status, newVal);
    } 
    else {
        printf("\n\t1. Sensor Status\n\t2. Simulated Value\n\tChoice: ");
        scanf("%d", &typeChoice); while(getchar() != '\n');

        if(typeChoice == 1) { 
            strcpy(typeStr, "Status"); strcpy(oldVal, targetSensor->status);
            printf("\tNew Status: "); fgets(newVal, sizeof(newVal), stdin); newVal[strcspn(newVal, "\n")] = 0;
            strcpy(targetSensor->status, newVal);
        } else {
            strcpy(typeStr, "Value"); sprintf(oldVal, "%.2f", targetSensor->value);
            printf("\tNew Value: "); fgets(newVal, sizeof(newVal), stdin); newVal[strcspn(newVal, "\n")] = 0;
            targetSensor->value = atof(newVal);
        }
    }

    printf("\tTechnician Name: "); fgets(tech, sizeof(tech), stdin); tech[strcspn(tech, "\n")] = 0;

    pushConfiguration(stack, code, typeStr, oldVal, newVal, tech);

    printf("\n\t" CLR_GREEN "[SUCCESS] Configuration change registered." CLR_RESET "\n");
    printf("\n\tPress Enter to return..."); getchar(); clearScreen();
}

void displayLastConfiguration(ConfigStack* stack) {
    if (stack == NULL || stack->top == NULL) {
        clearScreen();
        printf("\n\t" CLR_YELLOW "[INFO] No configuration changes logged yet." CLR_RESET "\n");
        return;
    }

    NetworkConfig* last = stack->top;

    clearScreen();
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "         " CLR_BOLD "LATEST CONFIGURATION" CLR_RESET "          " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    
    printf("\n\t  " CLR_BOLD "Asset Code:" CLR_RESET "  %s", last->equipmentCode);
    printf("\n\t  " CLR_BOLD "Type:" CLR_RESET "        %s", last->configType);
    printf("\n\t  " CLR_BOLD "Technician:" CLR_RESET "  %s", last->technician);
    printf("\n\t  " CLR_BOLD "Timestamp:" CLR_RESET "   %s", last->timestamp);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    
    printf("\n\t  " CLR_BOLD "FROM:" CLR_RESET " [%s]" CLR_RESET, last->oldValue);
    printf("\n\t  " CLR_BOLD "TO:  " CLR_RESET " " CLR_GREEN "[%s]" CLR_RESET, last->newValue);
    
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET "\n");

    printf("\n\tPress Enter to return...");
    getchar();
    clearScreen();
}

void displayNRecentConfigurations(ConfigStack* stack) {
    if (stack == NULL || stack->top == NULL) {
        clearScreen();
        printf("\n\t" CLR_YELLOW "[INFO] No configuration changes logged yet." CLR_RESET "\n");
        return;
    }

    int n, count = 0;
    printf("\n\tEnter number of logs to display: ");
    if (scanf("%d", &n) != 1) {
        while (getchar() != '\n');
        printf("\n\t" CLR_RED "[ERROR] Invalid input." CLR_RESET "\n");
        return;
    }
    while (getchar() != '\n');

    clearScreen();
    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "                      " CLR_BOLD "RECENT AUDIT TRAIL (Top %d)" CLR_RESET "                      " CLR_CYAN "|" CLR_RESET, n);
    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET);

    NetworkConfig* current = stack->top;
    
    while (current != NULL && count < n) {
        printf("\n\t  " CLR_BOLD "#%-2d" CLR_RESET " %-12s | Type: %-10s", 
               count + 1, current->equipmentCode, current->configType);
        
        printf("\n\t  " CLR_CYAN "-> " CLR_RESET "Change: " CLR_RED "[%s]" CLR_RESET " -> " CLR_GREEN "[%s]" CLR_RESET, 
               current->oldValue, current->newValue);
        
        printf("\n\t  " CLR_CYAN "-> " CLR_RESET "By: %-10s | At: %s", 
               current->technician, current->timestamp);
        
        printf("\n\t" CLR_CYAN "-------------------------------------------------------------------------" CLR_RESET);
        
        current = current->next;
        count++;
    }

    printf("\n\t" CLR_CYAN "+-------------------------------------------------------------------------+" CLR_RESET "\n");
    printf("\n\tPress Enter to return to management console...");
    getchar();
    clearScreen();
}

void saveConfigsToBinary(ConfigStack* stack) {
    if (stack == NULL || stack->top == NULL) {
        printf("\n\t" CLR_YELLOW "[INFO] Stack empty. No configurations to persist." CLR_RESET "\n");
        return;
    }

    FILE* file = fopen("configs.dat", "wb");
    if (file == NULL) {
        printf("\n\t" CLR_RED "[ERROR] Failed to open 'configs.dat' for binary writing." CLR_RESET "\n");
        return;
    }

    int count = 0;
    NetworkConfig* current = stack->top;

    while (current != NULL) {
        NetworkConfig temp = *current;
        temp.next = NULL; 

        fwrite(&temp, sizeof(NetworkConfig), 1, file);
        count++;
        current = current->next;
    }

    fclose(file);

    printf("\n\t" CLR_GREEN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_GREEN "|" CLR_RESET "        " CLR_BOLD "CONFIG BACKUP SUCCESS" CLR_RESET "        " CLR_GREEN "|" CLR_RESET);
    printf("\n\t" CLR_GREEN "+---------------------------------------+" CLR_RESET);
    printf("\n\t  [TOTAL] %d configurations serialized.", count);
    printf("\n\t  [FILE]  'configs.dat' updated.");
    printf("\n\t" CLR_GREEN "+---------------------------------------+" CLR_RESET "\n");
}

void loadConfigsFromBinary(ConfigStack* stack) {
    FILE* file = fopen("configs.dat", "rb");
    if (file == NULL) {
        printf("\n\t" CLR_YELLOW "[INFO] No configuration backup found. Starting fresh." CLR_RESET "\n");
        return;
    }

    NetworkConfig temp;
    ConfigStack tempStack;
    tempStack.top = NULL; 
    int count = 0;

    while (fread(&temp, sizeof(NetworkConfig), 1, file) == 1) {
        NetworkConfig* newNode = (NetworkConfig*)malloc(sizeof(NetworkConfig));
        if (newNode == NULL) continue;

        *newNode = temp;
        newNode->next = tempStack.top;
        tempStack.top = newNode;
        count++;
    }
    fclose(file);

    while (tempStack.top != NULL) {
        NetworkConfig* node = tempStack.top;
        tempStack.top = node->next;

        node->next = stack->top;
        stack->top = node;
    }

    printf("\n\t" CLR_GREEN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_GREEN "|" CLR_RESET "        " CLR_BOLD "SYSTEM CONFIG RESTORED" CLR_RESET "        " CLR_GREEN "|" CLR_RESET);
    printf("\n\t" CLR_GREEN "+---------------------------------------+" CLR_RESET);
    printf("\n\t  [SUCCESS] %d logs restored to audit stack.", count);
    printf("\n\t" CLR_GREEN "+---------------------------------------+" CLR_RESET "\n");
}

NetworkConfig* popConfig(ConfigStack* stack) {
    if (stack == NULL || stack->top == NULL) {
        return NULL;
    }

    NetworkConfig* temp = stack->top;
    stack->top = stack->top->next;
    return temp;
}

void revertLastConfiguration(ConfigStack* stack, Node* invHead, SensorStack* sensorStack) {
    if (stack == NULL || stack->top == NULL) {
        printf("\n\t" CLR_RED "[INFO] No configuration history available for rollback." CLR_RESET "\n");
        return;
    }

    NetworkConfig* lastLog = stack->top;
    
    Node* curr = invHead;
    int foundEquipment = 0, foundSensor = 0;
    SensorReading* currSensor = NULL;

    while (curr != NULL) {
        if (strcmp(curr->data.name, lastLog->equipmentCode) == 0) { foundEquipment = 1; break; }
        curr = curr->next;
    }

    if (!foundEquipment && sensorStack != NULL) {
        currSensor = sensorStack->top;
        while (currSensor != NULL) {
            if (strcmp(currSensor->code, lastLog->equipmentCode) == 0) { foundSensor = 1; break; }
            currSensor = currSensor->next;
        }
    }

    if (!foundEquipment && !foundSensor) {
        printf("\n\t" CLR_RED "[ERROR] Target asset '%s' not found in system." CLR_RESET "\n", lastLog->equipmentCode);
        return;
    }

    char currentVal[50], rollbackVal[50];
    strcpy(rollbackVal, lastLog->oldValue);

    if (foundEquipment) {
        if (strcmp(lastLog->configType, "IP Address") == 0) { strcpy(currentVal, curr->data.ip); strcpy(curr->data.ip, rollbackVal); }
        else if (strcmp(lastLog->configType, "Location") == 0) { strcpy(currentVal, curr->data.location); strcpy(curr->data.location, rollbackVal); }
        else { strcpy(currentVal, curr->data.status); strcpy(curr->data.status, rollbackVal); }
    } else {
        if (strcmp(lastLog->configType, "Value") == 0) { sprintf(currentVal, "%.2f", currSensor->value); currSensor->value = atof(rollbackVal); }
        else { strcpy(currentVal, currSensor->status); strcpy(currSensor->status, rollbackVal); }
    }

    stack->top = lastLog->next;
    free(lastLog); 

    pushConfiguration(stack, lastLog->equipmentCode, lastLog->configType, currentVal, rollbackVal, "SYSTEM_ROLLBACK");

    printf("\n\t" CLR_GREEN "[SUCCESS] Rollback applied for: %s" CLR_RESET, lastLog->equipmentCode);
    printf("\n\t" CLR_CYAN "-> Reverted %s from [%s] to [%s]" CLR_RESET "\n", lastLog->configType, currentVal, rollbackVal);
}

void displayAssetConfigHistory(ConfigStack* stack) {
    if (stack == NULL || stack->top == NULL) {
        clearScreen();
        printf("\n\t" CLR_YELLOW "[INFO] No configuration logs available." CLR_RESET "\n");
        return;
    }

    clearScreen();
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "         " CLR_BOLD "ASSET CONFIG HISTORY" CLR_RESET "          " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    
    char seenCodes[100][50]; 
    int seenCount = 0;
    NetworkConfig* curr = stack->top;

    printf("\n\t " CLR_BOLD "ASSETS WITH LOGS:" CLR_RESET);
    while (curr != NULL) {
        int alreadySeen = 0;
        for (int i = 0; i < seenCount; i++) {
            if (strcmp(seenCodes[i], curr->equipmentCode) == 0) { alreadySeen = 1; break; }
        }
        
        if (!alreadySeen && seenCount < 100) {
            printf("\n\t  • %s", curr->equipmentCode);
            strcpy(seenCodes[seenCount], curr->equipmentCode);
            seenCount++;
        }
        curr = curr->next;
    }

    char searchCode[50];
    printf("\n\n\tEnter Asset Code for full history: ");
    fgets(searchCode, sizeof(searchCode), stdin);
    searchCode[strcspn(searchCode, "\n")] = 0;

    clearScreen();
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "      " CLR_BOLD "HISTORY FOR: %-15s" CLR_RESET "      " CLR_CYAN "|" CLR_RESET, searchCode);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    
    curr = stack->top;
    int foundAny = 0;
    while (curr != NULL) {
        if (strcmp(curr->equipmentCode, searchCode) == 0) {
            foundAny = 1;
            printf("\n\t  " CLR_BOLD "Date:      " CLR_RESET "%s", curr->timestamp);
            printf("\n\t  " CLR_BOLD "Parameter: " CLR_RESET "%s", curr->configType);
            printf("\n\t  " CLR_RED "Old:" CLR_RESET " [%s] -> " CLR_GREEN "New:" CLR_RESET " [%s]", curr->oldValue, curr->newValue);
            printf("\n\t  " CLR_BOLD "Tech:      " CLR_RESET "%s", curr->technician);
            printf("\n\t" CLR_CYAN "---------------------------------------" CLR_RESET);
        }
        curr = curr->next;
    }

    if (!foundAny) {
        printf("\n\t" CLR_YELLOW "[INFO] No records found for asset '%s'." CLR_RESET "\n", searchCode);
    }

    printf("\n\tPress Enter to return...");
    getchar();
    clearScreen();
}

void clearConfigHistoryOptions(ConfigStack* stack) {
    if (stack == NULL || stack->top == NULL) {
        printf("\n\t" CLR_YELLOW "[INFO] Configuration log stack is already empty." CLR_RESET "\n");
        return;
    }

    int choice = 0;
    clearScreen();
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "         " CLR_BOLD "CLEAR HISTORY OPTIONS" CLR_RESET "         " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t  1. Clear ALL configuration logs");
    printf("\n\t  2. Clear logs for a SPECIFIC equipment");
    printf("\n\t  0. Cancel");
    printf("\n\t  Choice: ");
    
    if (scanf("%d", &choice) != 1) { while (getchar() != '\n'); return; }
    while (getchar() != '\n');

    if (choice == 0) return;

    if (choice == 1) {
        char confirm;
        printf("\n\t" CLR_RED "[WARNING] Clear ALL logs? (y/n): " CLR_RESET);
        scanf(" %c", &confirm);
        if (confirm == 'y' || confirm == 'Y') {
            while (stack->top != NULL) {
                NetworkConfig* toFree = popConfig(stack);
                free(toFree);
            }
            remove("configs.dat");
            printf("\n\t" CLR_GREEN "[SUCCESS] All logs purged." CLR_RESET "\n");
        }
    } 
    else if (choice == 2) {
        char code[50];
        printf("\n\tEnter Asset/Sensor Code to clear: ");
        fgets(code, sizeof(code), stdin);
        code[strcspn(code, "\n")] = 0;

        printf("\n\t" CLR_RED "[WARNING] Clear logs for '%s'? (y/n): " CLR_RESET, code);
        char confirm; scanf(" %c", &confirm); while (getchar() != '\n');

        if (confirm == 'y' || confirm == 'Y') {
            ConfigStack tempStack; tempStack.top = NULL;
            int deletedCount = 0;

            while (stack->top != NULL) {
                NetworkConfig* current = popConfig(stack);
                if (strcmp(current->equipmentCode, code) == 0) {
                    free(current);
                    deletedCount++;
                } else {
                    current->next = tempStack.top;
                    tempStack.top = current;
                }
            }

            while (tempStack.top != NULL) {
                NetworkConfig* node = popConfig(&tempStack);
                node->next = stack->top;
                stack->top = node;
            }

            if (deletedCount > 0) {
                saveConfigsToBinary(stack);
                printf("\n\t" CLR_GREEN "[SUCCESS] Deleted %d registries for '%s'." CLR_RESET "\n", deletedCount, code);
            } else {
                printf("\n\t" CLR_YELLOW "[INFO] No logs found for '%s'." CLR_RESET "\n", code);
            }
        }
    }
    printf("\n\tPress Enter to continue..."); getchar(); clearScreen();
}

void displayConfigAnalytics(ConfigStack* stack) {
    if (stack == NULL || stack->top == NULL) {
        clearScreen();
        printf("\n\t" CLR_YELLOW "[INFO] No data available for analytics." CLR_RESET "\n");
        return;
    }

    int ipChanges = 0, locChanges = 0, statusChanges = 0, total = 0;
    NetworkConfig* current = stack->top;

    while (current != NULL) {
        if (strcasecmp(current->configType, "IP Address") == 0 || strcasecmp(current->configType, "IP") == 0) ipChanges++;
        else if (strcasecmp(current->configType, "Location") == 0) locChanges++;
        else if (strcasecmp(current->configType, "Status") == 0) statusChanges++;

        total++;
        current = current->next;
    }

    clearScreen();
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "          " CLR_BOLD "CONFIGURATION ANALYTICS" CLR_RESET "        " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t  " CLR_BOLD "Total Operations Logged:" CLR_RESET " %d", total);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    
    printf("\n\t  IP Address Changes:  %d " CLR_GREEN "(%.1f%%)" CLR_RESET, ipChanges, (total > 0 ? (ipChanges * 100.0 / total) : 0));
    printf("\n\t  Location Changes:    %d " CLR_YELLOW "(%.1f%%)" CLR_RESET, locChanges, (total > 0 ? (locChanges * 100.0 / total) : 0));
    printf("\n\t  Status Changes:      %d " CLR_RED "(%.1f%%)" CLR_RESET, statusChanges, (total > 0 ? (statusChanges * 100.0 / total) : 0));
    
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET "\n");

    printf("\n\tPress Enter to return to management console...");
    getchar();
    clearScreen();
}

void checkStackHealth(ConfigStack* stack) {
    if (stack == NULL) {
        printf("\n\t" CLR_RED "[CRITICAL] Stack pointer is NULL." CLR_RESET "\n");
        return;
    }

    int count = 0;
    NetworkConfig* current = stack->top;
    while (current != NULL) {
        count++;
        current = current->next;
    }

    size_t memBytes = count * sizeof(NetworkConfig);

    clearScreen();
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t" CLR_CYAN "|" CLR_RESET "         " CLR_BOLD "STACK HEALTH STATUS" CLR_RESET "         " CLR_CYAN "|" CLR_RESET);
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
    printf("\n\t  Nodes in RAM:      %d", count);
    printf("\n\t  Memory Footprint:  %zu bytes", memBytes);
    
    if (count > 50) {
        printf("\n\t  [STATUS] " CLR_RED "CRITICAL: Stack heavy." CLR_RESET);
        printf("\n\t  Action: Recommend clearing old logs.");
    } else if (count > 30) {
        printf("\n\t  [STATUS] " CLR_YELLOW "WARNING: Stack growing." CLR_RESET);
        printf("\n\t  Action: Monitor memory usage.");
    } else {
        printf("\n\t  [STATUS] " CLR_GREEN "HEALTHY: Optimal." CLR_RESET);
    }
    printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET "\n");

    printf("\n\tPress Enter to return to management console...");
    getchar();
    clearScreen();
}

void menuConfigurations(ConfigStack* stack, Node* invHead, SensorStack* sensorStack) {
    int option = -1;

    do {
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "    " CLR_BOLD "MINI NOC SYSTEM - CONFIGURATIONS" CLR_RESET "   " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "                                       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "1." CLR_RESET " Register New Configuration        " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "2." CLR_RESET " View Latest Configuration Log     " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "3." CLR_RESET " View N Most Recent Configs        " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "4." CLR_RESET " Revert Last Configuration         " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "5." CLR_RESET " Display Asset's Config History    " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "6." CLR_RESET " Clear History Logs Registries     " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "7." CLR_RESET " Display Config Analytics          " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "8." CLR_RESET " Check Stack Memory Health         " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "                                       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_RED "0. Return to Main Menu" CLR_RESET "               " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t  Select an option " CLR_BOLD "-->" CLR_RESET " ");

        if (scanf("%d", &option) != 1) {
            printf("\n\t" CLR_RED "[ERROR] Invalid input. Please enter a number." CLR_RESET "\n");
            while (getchar() != '\n');
            printf("\n\tPress Enter to continue...");
            getchar();
            clearScreen();
            continue;
        }
        while (getchar() != '\n');

        switch (option) {
            case 1:
                clearScreen();
                registerNewConfiguration(stack, invHead, sensorStack);
                break;
            case 2:
                clearScreen();
                displayLastConfiguration(stack);
                break;
            case 3:
                clearScreen();
                displayNRecentConfigurations(stack);
                break;
            case 4:
                clearScreen();
                revertLastConfiguration(stack, invHead, sensorStack);
                break;
            case 5:
                clearScreen();
                displayAssetConfigHistory(stack);
                break;
            case 6:
                clearScreen();
                clearConfigHistoryOptions(stack);
                break;
            case 7:
                clearScreen();
                displayConfigAnalytics(stack);
                break;
            case 8:
                clearScreen();
                checkStackHealth(stack);
                break;
            case 0:
                clearScreen();
                printf("\n" CLR_CYAN "[SYSTEM] Returning to Main Menu..." CLR_RESET "\n");
                break;
            default:
                clearScreen();
                printf("\n\t" CLR_RED "[WARNING] Invalid option. Please try again." CLR_RESET "\n");
                printf("\n\tPress Enter to return to menu...");
                getchar();
                clearScreen();
                break;
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
    
    ConfigStack configStack;
    configStack.top = NULL;
    loadConfigsFromBinary(&configStack);

    SensorStack sensorStack;
    IncidentQueue incidentQueue;
    TechnicalIncident* completedHistory = loadHistoryFromBinary();
    initSensorStack(&sensorStack);
    
    incidentQueue.front = NULL;
    incidentQueue.rear = NULL;

    loadIncidentsFromBinary(&incidentQueue);
    fetchSensorDataFromAPI();
    importSensorReadings(&sensorStack, &incidentQueue);

    printf("\nPress Enter to start the application...");
    getchar();
    clearScreen();

    int mainOption = -1;

    do {
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "      " CLR_BOLD "MINI NOC SYSTEM - MAIN MENU" CLR_RESET "      " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "                                       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "1." CLR_RESET " Inventory Management              " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "2." CLR_RESET " Connectivity & Network Tests      " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "3." CLR_RESET " Sensor Data                       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "4." CLR_RESET " Incident Management               " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_GREEN "5." CLR_RESET " Configuration Logs                " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "                                       " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "|" CLR_RESET "  " CLR_RED "0. Exit Program" CLR_RESET "                      " CLR_CYAN "|" CLR_RESET);
        printf("\n\t" CLR_CYAN "+---------------------------------------+" CLR_RESET);
        printf("\n\t  Select an option " CLR_BOLD "-->" CLR_RESET " ");
        
        if(scanf("%d", &mainOption) != 1) {
            printf("\n\t" CLR_RED "[ERROR] Invalid input. Please enter a number." CLR_RESET "\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            printf("\n\tPress Enter to continue...");
            getchar();
            clearScreen();
            continue;
        }
        getchar();

        switch (mainOption) {
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
                menuIncidents(&incidentQueue, equipmentList, &sensorStack, &completedHistory);
                clearScreen();
                break;
            case 5:
                clearScreen();
                menuConfigurations(&configStack, equipmentList, &sensorStack);
                clearScreen();
                break;
            case 0:
                clearScreen();
                printf("\n" CLR_CYAN "[SYSTEM] Saving session data to backup file..." CLR_RESET "\n"); 
                saveInventoryToBinary(equipmentList);
                saveIncidentsToBinary(&incidentQueue);
                saveHistoryToBinary(completedHistory);
                saveConfigsToBinary(&configStack);
                printf("\n" CLR_GREEN "[SUCCESS] Exiting program. Goodbye!" CLR_RESET "\n");
                break;
            default:
                clearScreen();
                printf("\n\t" CLR_RED "[WARNING] Invalid option. Please try again." CLR_RESET "\n");
                printf("\n\tPress Enter to return to menu...");
                getchar();
                clearScreen();
                break;
        }
    } while (mainOption != 0);

    return 0;
}