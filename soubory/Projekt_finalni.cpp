#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "knihovna.h"
#include "uzivatele.c"
#include "graficke_funkce.c"

int global_counter = 0;

void increment_counter() {
    global_counter++;
}


int main() {
    loadUsers();

    if (userCount == 0) {
        strcpy(users[0].username, "Admin");
        strcpy(users[0].password, "1234");
        users[0].role = 'S';
        strcpy(users[0].subjects, "-");
        userCount++;
        saveUsers();
    }

    while (1) {
        char username[MAX_NAME_LEN], password[MAX_PASSWORD_LEN];
        printf("Zadejte uzivatelske jmeno: ");
        scanf("%49s", username);
        printf("Zadejte heslo: ");
        scanf("%49s", password);

        int loggedIn = 0;
        for (int i = 0; i < userCount; i++) {
            if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
                loggedIn = 1;

                if (users[i].role == 'S') {
                    int choice;
                    do {
                        oddelovac();
                        printf("1) Pridat uzivatele\n");
                        printf("2) Odebrat uzivatele\n");
                        printf("3) Zobrazit uzivatele\n");
                        printf("4) Vytvorit skupinu\n");
                        printf("5) Upravit skupinu\n"); // Nová volba
                        printf("6) Zobrazit skupiny\n");
                        printf("7) Odhlasit se\n");
                        choice = getValidChoice(1, 7);

                        switch (choice) {
                        case 1:
                            printf("Zadejte roli (S - spravce/T - student/U - ucitel): ");
                            char role;
                            scanf(" %c", &role);
                            addUser(role);
                            break;
                        case 2:
                            printf("Zadejte roli (S - spravce/T - student/U - ucitel): ");
                            scanf(" %c", &role);
                            removeUser(role);
                            break;
                        case 3:
                            printf("Zadejte roli (S - spravce/T - student/U - ucitel): ");
                            scanf(" %c", &role);
                            printUsersInline(role);
                            break;
                        case 4:
                            createGroup();
                            break;
                        case 5:
                            editGroup(); // Volání nové funkce
                            break;
                        case 6:
                            displayGroups();
                            break;
                        case 7:
                            clc();
                            printf("Odhlaseni...\n");
                            break;
                        }
                    } while (choice != 7);
                }

                else if (users[i].role == 'U') {
                    int choice;
                    do {
                        oddelovac();
                        printf("1) Znamkovani studentu\n");
                        printf("2) Zobrazit znamky studentu\n");
                        printf("3) Odhlasit se\n");
                        choice = getValidChoice(1, 3);

                        switch (choice) {
                        case 1:
                            gradeStudents(users[i].username); // Přidán parametr učitele
                            break;
                        case 2:
                            viewGroupGrades(users[i].username);
                            break;
                        case 3:
                            clc();
                            printf("Odhlaseni...\n");
                            break;
                        }
                    } while (choice != 3);
                }

                else if (users[i].role == 'T') {
                    int choice;
                    do {
                        oddelovac();
                        printf("1) Zobrazit znamky\n");
                        printf("2) Odhlasit se\n");
                        choice = getValidChoice(1, 2);

                        switch (choice) {
                        case 1:
                            viewStudentGrades(users[i].username); // Nová funkce pro zobrazení známek studenta
                            break;
                        case 2:
                            clc();
                            printf("Odhlaseni...\n");
                            break;
                        }
                    } while (choice != 2);
                }
                break;
            }
        }

        if (!loggedIn) {
            printf("Nespravne jmeno nebo heslo.\n");
        }
    }

    return 0;
}