﻿#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_USERS 100
#define MAX_NAME_LEN 50
#define MAX_PASSWORD_LEN 50
#define MAX_SUBJECTS_LEN 100
#define FILE_PATH "C:\\Users\\user\\Desktop\\škola\\1. Výška\\1\\2. Zimní semestr\\Počítače a programování 1\\projekt\\data.txt"

typedef struct {
    char username[MAX_NAME_LEN];
    char password[MAX_PASSWORD_LEN];
    char role; // 'S' = správce, 'T' = student, 'U' = učitel
    char subjects[MAX_SUBJECTS_LEN]; // Předměty u učitele, oddělené čárkami
} User;

User users[MAX_USERS];
int userCount = 0;

// Deklarace dostupných předmětů
const char* subjectsHint = "Dostupne predmety: MA, MPE, FY, PRG, TDE, EL";

// Načte uživatele ze souboru
void loadUsers() {
    FILE* file = fopen(FILE_PATH, "r");
    if (file == NULL) {
        printf("Soubor s daty nenalezen, vytvarim novy.\n");
        userCount = 0;
        return;
    }

    userCount = 0;
    while (fscanf(file, "%49s %49s %c %99[^\n]", users[userCount].username, users[userCount].password, &users[userCount].role, users[userCount].subjects) == 4 ||
        fscanf(file, "%49s %49s %c", users[userCount].username, users[userCount].password, &users[userCount].role) == 3) {
        if (users[userCount].role != 'U') {
            strcpy(users[userCount].subjects, "-");
        }
        userCount++;
    }
    fclose(file);
}

// Uloží uživatele do souboru (bez hlavního správce)
void saveUsers() {
    FILE* file = fopen(FILE_PATH, "w");
    if (file == NULL) {
        printf("Chyba pri ukladani uzivatelu do souboru.\n");
        return;
    }

    for (int i = 0; i < userCount; ++i) {
        if (strcmp(users[i].username, "Admin") == 0 && users[i].role == 'S') {
            continue; // Nepřidáváme hlavního správce do souboru
        }
        fprintf(file, "%s %s %c %s\n", users[i].username, users[i].password, users[i].role, users[i].subjects);
    }
    fclose(file);
}

// Výpis uživatelů podle role na jednom řádku
void printUsersInline(char role) {
    printf("Seznam %s: ", role == 'S' ? "spravcu" : (role == 'T' ? "studentu" : "ucitelu"));
    int first = 1;
    for (int i = 0; i < userCount; i++) {
        if (users[i].role == role) {
            if (!first) printf(", ");
            first = 0;
            if (role == 'U') {
                printf("%s/%s/%s", users[i].username, users[i].password, users[i].subjects);
            }
            else {
                printf("%s/%s", users[i].username, users[i].password);
            }
        }
    }
    if (first) {
        printf("zadni nejsou zaregistrovani.");
    }
    printf("\n");
}

// Přidání uživatele
void addUser(char role) {
    if (userCount >= MAX_USERS) {
        printf("Neni mozne pridat dalsiho uzivatele. Kapacita je plna.\n");
        return;
    }

    char username[MAX_NAME_LEN], password[MAX_PASSWORD_LEN], subjects[MAX_SUBJECTS_LEN] = "-";
    printf("Zadej jmeno noveho %s: ", role == 'S' ? "spravce" : (role == 'T' ? "studenta" : "ucitele"));
    scanf("%49s", username);
    printf("Zadej heslo noveho %s: ", role == 'S' ? "spravce" : (role == 'T' ? "studenta" : "ucitele"));
    scanf("%49s", password);

    if (role == 'U') {
        printf("%s\n", subjectsHint);
        printf("Zadejte predmety oddelene carkou (napr. MA,FY): ");
        scanf(" %99[^\n]", subjects);
    }

    int suffix = 1;
    char originalUsername[MAX_NAME_LEN];
    strcpy(originalUsername, username);

    for (int i = 0; i < userCount; ++i) {
        if (strcmp(users[i].username, username) == 0) {
            sprintf(username, "%s_%d", originalUsername, suffix++);
            i = -1; // Restart kontrolu od začátku
        }
    }

    strcpy(users[userCount].username, username);
    strcpy(users[userCount].password, password);
    users[userCount].role = role;
    strcpy(users[userCount].subjects, subjects);
    userCount++;

    printf("%s %s byl pridan.\n", role == 'S' ? "Spravce" : (role == 'T' ? "Student" : "Ucitel"), username);
    saveUsers();
}

// Odebrání uživatele (kromě hlavního správce)
void removeUser(char role) {
    printf("Aktualni %s (jmena): ", role == 'S' ? "spravci" : (role == 'T' ? "studenti" : "ucitele"));
    int first = 1;
    for (int i = 0; i < userCount; i++) {
        if (users[i].role == role && !(role == 'S' && strcmp(users[i].username, "Admin") == 0)) {
            if (!first) printf(", ");
            first = 0;
            printf("%s", users[i].username);
        }
    }
    if (first) {
        printf("zadni nejsou zaregistrovani.\n");
        return;
    }
    printf("\nZadej jmena k odstraneni (oddeleny carkou): ");

    char input[256];
    char* token;
    scanf(" %255[^\n]", input);

    token = strtok(input, ",");
    while (token != NULL) {
        for (int i = 0; i < userCount; ++i) {
            if (strcmp(users[i].username, token) == 0 && users[i].role == role && strcmp(users[i].username, "Admin") != 0) {
                printf("%s byl odstranen.\n", users[i].username);
                for (int j = i; j < userCount - 1; ++j) {
                    users[j] = users[j + 1];
                }
                userCount--;
                i--; // Kontrolovat znovu aktuální index
                break;
            }
        }
        token = strtok(NULL, ",");
    }
    saveUsers();
}

// Zajišťuje, že volba je platná
int getValidChoice(int min, int max) {
    int choice;
    while (1) {
        printf("Vyber moznost: ");
        if (scanf("%d", &choice) != 1 || choice < min || choice > max) {
            printf("Neplatna volba, zadejte hodnotu v rozsahu %d-%d.\n", min, max);
            while (getchar() != '\n'); // Vyčistí vstupní buffer
        }
        else {
            return choice;
        }
    }
}

//tlačítko zpět
void goBack() {
    printf("Navrat do hlavni nabidky...\n");
    return;  // Tímto se vrátíš zpět do hlavního menu
}

void oddelovac() {
    printf("----------------------------------\n");
}

int main() {
    loadUsers();

    // Přidání hlavního správce
    strcpy(users[userCount].username, "Admin");
    strcpy(users[userCount].password, "1234");
    users[userCount].role = 'S';
    strcpy(users[userCount].subjects, "-");
    userCount++;

    while (1) {
        char username[MAX_NAME_LEN], password[MAX_PASSWORD_LEN];
        printf("Zadej jmeno: ");
        scanf("%49s", username);
        printf("Zadej heslo: ");
        scanf("%49s", password);

        int loggedIn = 0;

        for (int i = 0; i < userCount; ++i) {
            if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
                printf("Prihlaseni jako %s.\n", users[i].username);
                loggedIn = 1;

                int choice;
                do {
                    oddelovac();
                    printf("1) Sprava spravcu\n");
                    printf("2) Sprava studentu\n");
                    printf("3) Sprava vyucujicich\n");
                    printf("4) Sprava skupin\n");
                    printf("5) Odhlaseni\n");
                    choice = getValidChoice(1, 5);

                    switch (choice) {
                    case 1:
                        oddelovac();
                        printUsersInline('S');                      
                        printf("Sprava spravcu\n");
                        printf("1) Pridat spravce\n");
                        printf("2) Odstranit spravce\n");
                        printf("3) Zpet\n");
                        choice = getValidChoice(1, 3);
                        if (choice == 1) {
                            addUser('S');
                        }
                        if (choice == 2) {
                            removeUser('S');
                        }
                        if (choice == 3) {
                            goBack();  
                        }
                        break;
                    case 2:
                        oddelovac();                      
                        printUsersInline('T');
                        printf("Sprava studentu\n");
                        printf("1) Pridat studenta\n");
                        printf("2) Odstranit studenta\n");
                        printf("3) Zpet\n");
                        choice = getValidChoice(1, 3);
                        if (choice == 1) {
                            addUser('T');
                        }
                        if (choice == 2) {
                            removeUser('T');
                        }
                        if (choice == 3) {
                            goBack();
                        }
                        break;
                    case 3:
                        oddelovac();
                        printUsersInline('U');                       
                        printf("Sprava vyucujicich\n");
                        printf("1) Pridat ucitele\n");
                        printf("2) Odstranit ucitele\n");
                        printf("3) Zpet\n");
                        choice = getValidChoice(1, 3);
                        if (choice == 1) {
                            addUser('U');
                        }
                        if (choice == 2) {
                            removeUser('U');
                        }
                        if (choice == 3) {
                            goBack();
                        }
                        break;
                    case 5:
                        printf("Odhlaseni %s...\n", users[i].username);
                        oddelovac();
                        break;
                    default:
                        printf("Neplatna volba.\n");
                    }
                } while (choice != 5);

                break;
            }
        }

        if (!loggedIn) {
            printf("Nespravne uzivatelske jmeno nebo heslo.\n");
        }
    }

    return 0;
}