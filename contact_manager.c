#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_CONTACTS 100
#define NAME_LEN 50
#define PHONE_LEN 20
#define EMAIL_LEN 50

typedef struct {
    int id;
    char name[NAME_LEN];
    char phone[PHONE_LEN];
    char email[EMAIL_LEN];
} Contact;

Contact contacts[MAX_CONTACTS];
int contactCount = 0;
int nextId = 1;

char filename[100] = "contacts.csv";
int isDirty = 0;
int hasSavedOnce = 0;

// ================= UTIL =================
void trimNewline(char *str) {
    str[strcspn(str, "\n")] = 0;
}

int getIntInput() {
    char buffer[20];
    int value;

    while (1) {
        fgets(buffer, sizeof(buffer), stdin);
        if (sscanf(buffer, "%d", &value) == 1)
            return value;

        printf("Invalid input. Enter a number: ");
    }
}

int isDuplicate(char *name, char *phone) {
    for (int i = 0; i < contactCount; i++) {
        if (strcmp(contacts[i].name, name) == 0 &&
            strcmp(contacts[i].phone, phone) == 0) {
            return 1;
        }
    }
    return 0;
}

// ================= SEARCH =================
void searchContact() {
    if (contactCount == 0) {
        printf("No contacts to search.\n");
        return;
    }

    char query[NAME_LEN];

    printf("Enter name or phone to search: ");
    fgets(query, sizeof(query), stdin);
    trimNewline(query);

    if (strlen(query) == 0) {
        printf("Search query cannot be empty.\n");
        return;
    }

    int found = 0;

    for (int i = 0; i < contactCount; i++) {

        char nameLower[NAME_LEN];
        char phoneLower[PHONE_LEN];
        char queryLower[NAME_LEN];

        strcpy(nameLower, contacts[i].name);
        strcpy(phoneLower, contacts[i].phone);
        strcpy(queryLower, query);

        for (int j = 0; nameLower[j]; j++) nameLower[j] = tolower(nameLower[j]);
        for (int j = 0; phoneLower[j]; j++) phoneLower[j] = tolower(phoneLower[j]);
        for (int j = 0; queryLower[j]; j++) queryLower[j] = tolower(queryLower[j]);

        if (strstr(nameLower, queryLower) || strstr(phoneLower, queryLower)) {
            printf("\nID: %d\n", contacts[i].id);
            printf("Name: %s\n", contacts[i].name);
            printf("Phone: %s\n", contacts[i].phone);
            printf("Email: %s\n", contacts[i].email);
            found = 1;
        }
    }

    if (!found) {
        printf("No matching contacts found.\n");
    }
}

// ================= CORE =================
void addContact() {
    if (contactCount >= MAX_CONTACTS) {
        printf("Contact list full.\n");
        return;
    }

    Contact c;
    c.id = nextId++;

    printf("Enter name: ");
    fgets(c.name, NAME_LEN, stdin);
    trimNewline(c.name);

    printf("Enter phone: ");
    fgets(c.phone, PHONE_LEN, stdin);
    trimNewline(c.phone);

    if (isDuplicate(c.name, c.phone)) {
        printf("Duplicate contact!\n");
        return;
    }

    printf("Enter email (optional): ");
    fgets(c.email, EMAIL_LEN, stdin);
    trimNewline(c.email);

    contacts[contactCount++] = c;
    isDirty = 1;

    printf("Contact added.\n");
}

void updateContact() {
    printf("Enter contact ID to update: ");
    int id = getIntInput();

    int index = -1;

    for (int i = 0; i < contactCount; i++) {
        if (contacts[i].id == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("Contact not found.\n");
        return;
    }

    char input[NAME_LEN];
    int changed = 0;

    printf("Enter new name (leave blank to keep current): ");
    fgets(input, NAME_LEN, stdin);
    if (strcmp(input, "\n") != 0) {
        trimNewline(input);
        strcpy(contacts[index].name, input);
        changed = 1;
    }

    printf("Enter new phone (leave blank to keep current): ");
    fgets(input, PHONE_LEN, stdin);
    if (strcmp(input, "\n") != 0) {
        trimNewline(input);
        strcpy(contacts[index].phone, input);
        changed = 1;
    }

    printf("Enter new email (leave blank to keep current): ");
    fgets(input, EMAIL_LEN, stdin);
    if (strcmp(input, "\n") != 0) {
        trimNewline(input);
        strcpy(contacts[index].email, input);
        changed = 1;
    }

    if (changed) {
        isDirty = 1;
        printf("Contact updated.\n");
    } else {
        printf("No changes made.\n");
    }
}

void deleteContact() {
    printf("Enter ID to delete: ");
    int id = getIntInput();

    for (int i = 0; i < contactCount; i++) {
        if (contacts[i].id == id) {

            for (int j = i; j < contactCount - 1; j++) {
                contacts[j] = contacts[j + 1];
            }

            contactCount--;
            isDirty = 1;
            printf("Contact deleted.\n");
            return;
        }
    }

    printf("Contact not found.\n");
}

// ================= DISPLAY =================
void listContacts() {
    if (contactCount == 0) {
        printf("No contacts.\n");
        return;
    }

    for (int i = 0; i < contactCount; i++) {
        printf("\nID: %d\n", contacts[i].id);
        printf("Name: %s\n", contacts[i].name);
        printf("Phone: %s\n", contacts[i].phone);
        printf("Email: %s\n", contacts[i].email);
    }
}

// ================= FILE =================
void exportCSV() {
    FILE *file = fopen(filename, "w");

    if (!file) {
        printf("Error opening file.\n");
        return;
    }

    fprintf(file, "id,name,phone,email\n");

    for (int i = 0; i < contactCount; i++) {
        fprintf(file, "%d,\"%s\",\"%s\",\"%s\"\n",
                contacts[i].id,
                contacts[i].name,
                contacts[i].phone,
                contacts[i].email);
    }

    fclose(file);
    isDirty = 0;
    hasSavedOnce = 1;

    printf("Saved to %s\n", filename);
}

void importCSV() {
    FILE *file = fopen(filename, "r");
    if (!file) return;

    char line[200];

    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file)) {
        if (contactCount >= MAX_CONTACTS) break;

        Contact c;

        char *token = strtok(line, ",");
        if (!token) continue;
        c.id = atoi(token);

        token = strtok(NULL, ",");
        if (!token) continue;
        strcpy(c.name, token);

        token = strtok(NULL, ",");
        if (!token) continue;
        strcpy(c.phone, token);

        token = strtok(NULL, ",");
        if (token) strcpy(c.email, token);
        else strcpy(c.email, "");

        trimNewline(c.name);
        trimNewline(c.phone);
        trimNewline(c.email);

        if (c.name[0] == '"') memmove(c.name, c.name + 1, strlen(c.name));
        if (c.phone[0] == '"') memmove(c.phone, c.phone + 1, strlen(c.phone));
        if (c.email[0] == '"') memmove(c.email, c.email + 1, strlen(c.email));

        if (c.name[strlen(c.name)-1] == '"') c.name[strlen(c.name)-1] = '\0';
        if (c.phone[strlen(c.phone)-1] == '"') c.phone[strlen(c.phone)-1] = '\0';
        if (c.email[strlen(c.email)-1] == '"') c.email[strlen(c.email)-1] = '\0';

        if (!isDuplicate(c.name, c.phone)) {
            contacts[contactCount++] = c;

            if (c.id >= nextId)
                nextId = c.id + 1;
        }
    }

    fclose(file);
    isDirty = 0;
}

// ================= MENU =================
void menu() {
    printf("\n==============================\n");
    printf("      CONTACT MANAGER\n");
    printf("==============================\n");

    if (isDirty) printf("Status: Unsaved changes\n");
    else if (hasSavedOnce) printf("Status: All changes saved\n");

    printf("\n[1] Add Contact\n");
    printf("[2] List Contacts\n");
    printf("[3] Update Contact\n");
    printf("[4] Search Contact\n");
    printf("[5] Delete Contact\n");
    printf("[6] Save to File\n");
    printf("[7] Exit\n");

    printf("\nSelect option: ");
}

// ================= MAIN =================
int main() {
    importCSV();

    while (1) {
        menu();
        int choice = getIntInput();

        switch (choice) {
            case 1: addContact(); break;
            case 2: listContacts(); break;
            case 3: updateContact(); break;
            case 4: searchContact(); break;
            case 5: deleteContact(); break;
            case 6: exportCSV(); break;

            case 7:
                if (isDirty) {
                    printf("You have unsaved changes. Exit anyway? (y/n): ");
                    char confirm[10];
                    fgets(confirm, sizeof(confirm), stdin);

                    if (confirm[0] == 'y' || confirm[0] == 'Y') {
                        printf("Goodbye.\n");
                        return 0;
                    }
                } else {
                    printf("Goodbye.\n");
                    return 0;
                }
                break;

            default:
                printf("Invalid choice.\n");
        }
    }

    return 0;
}