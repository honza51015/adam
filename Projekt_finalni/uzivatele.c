#include "knihovna.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_GRADES 10
#define FILE_PATH "data.txt"
#define MAX_USERS 100
#define MAX_NAME_LEN 50
#define MAX_PASSWORD_LEN 50
#define MAX_SUBJECTS_LEN 100
#define MAX_FIXED_SUBJECTS 5

typedef struct {
    char username[MAX_NAME_LEN];
    char password[MAX_PASSWORD_LEN];
    char role; // 'S' = spravce, 'T' = student, 'U' = ucitel
    char subjects[MAX_SUBJECTS_LEN]; // Predmety u ucitele, oddelene carkami
} User;

User users[MAX_USERS];
int userCount = 0;

typedef struct {
    char subject[MAX_NAME_LEN];
    int exercisePoints;
    int testPoints;
    int finalExamPoints;
    char grade;
} Grade;

Grade grades[MAX_USERS][MAX_GRADES];
int gradesCount[MAX_USERS] = { 0 };

const char* fixedSubjects[MAX_FIXED_SUBJECTS] = {
    "PCT",
    "FY",
    "MPE",
    "MA",
    "EL"
};

void oddelovac() {
    printf("-------------------------------------------------------------------------\n");
}

void clc() {
    system("cls"); 
}

void oddelovac();
void clc();

void loadUsers();
void saveUsers();
void printUsersInline(char role);
void addUser(char role);
void editGroup();
void removeUser(char role);
void createGroup();
void displayGroups();
void addSubject(int studentIndex);
int getValidChoice(int min, int max);

void gradeStudents(const char* teacherUsername);
void viewGroupGrades(const char* teacherUsername);
void viewStudentGrades(const char* studentUsername);

//nacteni uzivatele
void loadUsers() {
    FILE* file = fopen(FILE_PATH, "r");
    if (!file) {
        printf("Soubor nenalezen, vytvarim novy.\n");
        return;
    }

    userCount = 0;
    while (fscanf(file, "%49s %49s %c %99[^\n]", users[userCount].username, users[userCount].password, &users[userCount].role, users[userCount].subjects) == 4) {
        userCount++;
        if (userCount >= MAX_USERS) break;
    }
    fclose(file);

    // P�id�n� Admina, pokud neexistuje
    int adminExists = 0;
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, "Admin") == 0) {
            adminExists = 1;
            break;
        }
    }
    if (!adminExists) {
        strcpy(users[userCount].username, "Admin");
        strcpy(users[userCount].password, "1234"); // V�choz� heslo, p��padn� zm��te
        users[userCount].role = 'S';
        strcpy(users[userCount].subjects, "-");
        userCount++;
        //printf("V�choz� Admin p�id�n do pam�ti.\n");
    }
}

//ulozeni uzivatele
void saveUsers() {
    FILE* file = fopen(FILE_PATH, "w");
    if (!file) {
        printf("Chyba pri ukladani uzivatelu.\n");
        return;
    }

    for (int i = 0; i < userCount; i++) {
        // P�esko�te Admina
        if (strcmp(users[i].username, "Admin") == 0) {
            continue;
        }
        fprintf(file, "%s %s %c %s\n", users[i].username, users[i].password, users[i].role, users[i].subjects);
    }
    fclose(file);
}

//zobrazeni uzivatelu
void printUsersInline(char role) {
    printf("Seznam %s: ", role == 'S' ? "spravcu" : (role == 'T' ? "studentu" : "ucitelu"));
    int first = 1;
    for (int i = 0; i < userCount; i++) {
        if (users[i].role == role) {
            if (!first) printf(", ");
            first = 0;
            printf("%s", users[i].username);
        }
    }
    if (first) {
        printf("zadni nejsou zaregistrovani.");
    }
    printf("\n");
}

//pridani uzivatele
void addUser(char role) {
    if (userCount >= MAX_USERS) {
        printf("Nelze pridat dalsiho uzivatele. Kapacita je plna.\n");
        return;
    }

    char username[MAX_NAME_LEN], password[MAX_PASSWORD_LEN];
    printf("Zadejte jmeno uzivatele: ");
    scanf("%49s", username);
    printf("Zadejte heslo uzivatele: ");
    scanf("%49s", password);
 
    // Inicializace p�edm�t�
    char subjects[MAX_SUBJECTS_LEN] = "-";

    // Pokud je role u�itel, nab�dne se v�b�r v�ce p�edm�t�
    if (role == 'U') {
        printf("Vyberte predmety, ktere ucitel vyucuje (zadavejte cisla oddelena carkou):\n");
        for (int i = 0; i < MAX_FIXED_SUBJECTS; i++) {
            printf("%d) %s\n", i + 1, fixedSubjects[i]);
        }

        char input[MAX_SUBJECTS_LEN];
        printf("Zadejte volbu: ");
        scanf(" %99[^\n]", input);

        // Zpracov�n� ��sel na n�zvy p�edm�t�
        char* token = strtok(input, ",");
        subjects[0] = '\0'; // Vynulov�n� pro sestaven� seznamu p�edm�t�
        while (token != NULL) {
            int subjectIndex = atoi(token) - 1; // P�evod na index
            if (subjectIndex >= 0 && subjectIndex < MAX_FIXED_SUBJECTS) {
                if (subjects[0] != '\0') {
                    strcat(subjects, ","); // P�id�n� ��rky mezi p�edm�ty
                }
                strcat(subjects, fixedSubjects[subjectIndex]);
            }
            else {
                printf("Neplatna volba: %s\n", token);
            }
            token = strtok(NULL, ",");
        }

        if (subjects[0] == '\0') {
            strcpy(subjects, "-"); // Pokud nebyly zad�ny platn� p�edm�ty
        }
    }

    // Pokud je role student, nab�dne se v�b�r jednoho p�edm�tu
    if (role == 'T') {
        const char* studentSubjects[] = {
            "BPC-TLI1A",
            "BPC-TLI1B",
            "BPC-TLI2A",
            "BPC-TLI2B",
            "BPC-TLI3A",
            "BPC-TLI3B"
        };
        const int numStudentSubjects = sizeof(studentSubjects) / sizeof(studentSubjects[0]);

        printf("Vyberte predmet, ktery bude student studovat (maximalne jeden):\n");
        for (int i = 0; i < numStudentSubjects; i++) {
            printf("%d) %s\n", i + 1, studentSubjects[i]);
        }

        int choice = getValidChoice(1, numStudentSubjects);
        strcpy(subjects, studentSubjects[choice - 1]); // Zaps�n� vybran�ho p�edm�tu
    }

    // Ulo�en� nov�ho u�ivatele
    strcpy(users[userCount].username, username);
    strcpy(users[userCount].password, password);
    users[userCount].role = role;
    strcpy(users[userCount].subjects, subjects);
    userCount++;

    printf("Uzivatel pridan.\n");
    saveUsers();
}

//upraven� skupiny
void editGroup() {
    char groups[MAX_USERS][MAX_NAME_LEN];
    int groupCount = 0;

    // Na�ten� v�ech existuj�c�ch skupin
    for (int i = 0; i < userCount; i++) {
        if (users[i].role == 'U' || users[i].role == 'T') {
            char tempSubjects[MAX_SUBJECTS_LEN];
            strcpy(tempSubjects, users[i].subjects);
            char* token = strtok(tempSubjects, ",");
            while (token != NULL) {
                if (strncmp(token, "sk-", 3) == 0) { // Pouze skupiny
                    int exists = 0;
                    for (int j = 0; j < groupCount; j++) {
                        if (strcmp(groups[j], token) == 0) {
                            exists = 1;
                            break;
                        }
                    }
                    if (!exists) {
                        strcpy(groups[groupCount++], token);
                    }
                }
                token = strtok(NULL, ",");
            }
        }
    }

    if (groupCount == 0) {
        printf("Zadne skupiny nejsou vytvoreny.\n");
        return;
    }

    // V�pis skupin
    printf("Aktualni skupiny:\n");
    for (int i = 0; i < groupCount; i++) {
        printf("%d) %s\n", i + 1, groups[i]);
    }

    // V�b�r skupiny
    int groupChoice = getValidChoice(1, groupCount);
    const char* selectedGroup = groups[groupChoice - 1];

    // V�pis student� ve skupin�
    printf("Studenti ve skupine %s:\n", selectedGroup);
    int studentIndexes[MAX_USERS];
    int studentCount = 0;

    for (int i = 0; i < userCount; i++) {
        if (users[i].role == 'T' && strstr(users[i].subjects, selectedGroup)) {
            printf("%d) %s\n", studentCount + 1, users[i].username);
            studentIndexes[studentCount++] = i;
        }
    }

    if (studentCount == 0) {
        printf("Ve skupine nejsou zadni studenti.\n");
    }

    // V�b�r akce
    printf("Vyberte akci:\n");
    printf("1) Pridat studenta\n");
    printf("2) Smazat studenta\n");
    int action = getValidChoice(1, 2);

    if (action == 1) {
        // P�id�n� studenta
        printf("Zadejte jmeno studenta k pridani: ");
        char studentName[MAX_NAME_LEN];
        scanf(" %49[^\n]", studentName);

        int studentIndex = -1;
        for (int i = 0; i < userCount; i++) {
            if (users[i].role == 'T' && strcmp(users[i].username, studentName) == 0) {
                studentIndex = i;
                break;
            }
        }

        if (studentIndex == -1) {
            printf("Student nenalezen.\n");
        }
        else if (strstr(users[studentIndex].subjects, selectedGroup)) {
            printf("Student je jiz ve skupine.\n");
        }
        else {
            strcat(users[studentIndex].subjects, ",");
            strcat(users[studentIndex].subjects, selectedGroup);
            printf("Student %s byl pridan do skupiny %s.\n", studentName, selectedGroup);
        }
    }
    else if (action == 2) {
        // Smaz�n� studenta
        printf("Zadejte cislo studenta k smazani: ");
        int studentChoice = getValidChoice(1, studentCount);
        int studentIndex = studentIndexes[studentChoice - 1];

        // Odebr�n� skupiny a zn�mek
        char tempSubjects[MAX_SUBJECTS_LEN];
        strcpy(tempSubjects, users[studentIndex].subjects);
        char updatedSubjects[MAX_SUBJECTS_LEN] = "";
        char* token = strtok(tempSubjects, ",");

        while (token != NULL) {
            if (strcmp(token, selectedGroup) != 0 &&
                !(strlen(token) > strlen(selectedGroup) && strstr(token, selectedGroup))) {
                if (strlen(updatedSubjects) > 0) strcat(updatedSubjects, ",");
                strcat(updatedSubjects, token);
            }
            token = strtok(NULL, ",");
        }

        strcpy(users[studentIndex].subjects, updatedSubjects);
        printf("Student %s byl odebran ze skupiny %s.\n", users[studentIndex].username, selectedGroup);
    }

    saveUsers();
}

//odtran�n� u�ivatele
void removeUser(char role) {
    printf("Aktualni %s (jmena): ", role == 'S' ? "spravci" : (role == 'T' ? "studenti" : "ucitele"));
    printUsersInline(role);

    printf("\nZadejte jmeno k odstraneni: ");
    char username[MAX_NAME_LEN];
    scanf("%49s", username);

    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0 && users[i].role == role) {
            for (int j = i; j < userCount - 1; j++) {
                users[j] = users[j + 1]; // Posun
            }
            userCount--;
            saveUsers();
            printf("Uzivatel %s byl odstranen.\n", username);
            return;
        }
    }

    printf("Uzivatel nenalezen.\n");
}

//vytvo�en� skupin
void createGroup() {
    char groupName[MAX_NAME_LEN];
    char chosenSubject[MAX_NAME_LEN];
    char chosenClass[MAX_NAME_LEN];
    char chosenTeacher[MAX_NAME_LEN];

    // Krok 1: Zad�n� n�zvu skupiny
    printf("Zadejte nazev skupiny: ");
    scanf(" %49[^\n]", groupName);
    char prefixedGroupName[MAX_NAME_LEN];
    snprintf(prefixedGroupName, sizeof(prefixedGroupName), "sk-%s", groupName);

    // Krok 2: V�b�r p�edm�tu
    printf("Vyberte predmet ze seznamu:\n");
    for (int i = 0; i < MAX_FIXED_SUBJECTS; i++) {
        printf("%d) %s\n", i + 1, fixedSubjects[i]);
    }
    int subjectChoice = getValidChoice(1, MAX_FIXED_SUBJECTS);
    strcpy(chosenSubject, fixedSubjects[subjectChoice - 1]);

    // Krok 3: Vyps�n� dostupn�ch u�itel�
    printf("Dostupni ucitele pro predmet '%s':\n", chosenSubject);
    int availableTeachers = 0;
    for (int i = 0; i < userCount; i++) {
        if (users[i].role == 'U' && strstr(users[i].subjects, chosenSubject)) {
            printf("- %s\n", users[i].username);
            availableTeachers++;
        }
    }

    if (availableTeachers == 0) {
        printf("Neni dostupny zadny vyucujici pro tento predmet.\n");
        return;
    }

    printf("Zadejte jmeno vybraneho ucitele: ");
    scanf(" %49s", chosenTeacher);

    // Krok 4: V�b�r t��dy bez omezen� na existenci skupin
    const char* studentClasses[] = {
        "BPC-TLI1A",
        "BPC-TLI1B",
        "BPC-TLI2A",
        "BPC-TLI2B",
        "BPC-TLI3A",
        "BPC-TLI3B"
    };
    const int numStudentClasses = sizeof(studentClasses) / sizeof(studentClasses[0]);

    printf("Dostupne tridy:\n");
    int availableClasses = 0;
    char availableClassMap[MAX_USERS][MAX_NAME_LEN]; // Mapov�n� pro v�b�r
    for (int i = 0; i < numStudentClasses; i++) {
        int hasStudents = 0;
        for (int j = 0; j < userCount; j++) {
            if (users[j].role == 'T' && strstr(users[j].subjects, studentClasses[i])) {
                hasStudents = 1;
                break;
            }
        }
        if (hasStudents) { // Zobrazit t��du, pokud ji m� alespo� jeden student
            printf("%d) %s\n", availableClasses + 1, studentClasses[i]);
            strcpy(availableClassMap[availableClasses], studentClasses[i]);
            availableClasses++;
        }
    }

    if (availableClasses == 0) {
        printf("Zadna dostupna trida.\n");
        return;
    }

    int classChoice = getValidChoice(1, availableClasses);
    strcpy(chosenClass, availableClassMap[classChoice - 1]);

    // Krok 5: P�id�n� skupiny k u�iteli a student�m
    for (int i = 0; i < userCount; i++) {
        if (users[i].role == 'U' && strcmp(users[i].username, chosenTeacher) == 0) {
            strcat(users[i].subjects, ",");
            strcat(users[i].subjects, prefixedGroupName);
        }
        else if (users[i].role == 'T' && strstr(users[i].subjects, chosenClass)) {
            strcat(users[i].subjects, ",");
            strcat(users[i].subjects, prefixedGroupName);
        }
    }

    printf("Skupina '%s' vytvorena a prirazena uciteli '%s' a studentum tridy '%s'.\n", prefixedGroupName, chosenTeacher, chosenClass);
    saveUsers();
}

//zobrazen� skupin
void displayGroups() {
    // Krok 1: Filtrov�n� skupin podle prefixu
    char groups[MAX_USERS][MAX_NAME_LEN];
    int groupCount = 0;

    for (int i = 0; i < userCount; i++) {
        if (users[i].role == 'U' || users[i].role == 'T') {
            char tempSubjects[MAX_SUBJECTS_LEN];
            strcpy(tempSubjects, users[i].subjects); // Kopie pro strtok
            char* token = strtok(tempSubjects, ",");
            while (token != NULL) {
                if (strncmp(token, "sk-", 3) == 0) { // Jen skupiny
                    int isNewGroup = 1;
                    for (int j = 0; j < groupCount; j++) {
                        if (strcmp(groups[j], token) == 0) {
                            isNewGroup = 0;
                            break;
                        }
                    }
                    if (isNewGroup) {
                        strcpy(groups[groupCount++], token);
                    }
                }
                token = strtok(NULL, ",");
            }
        }
    }

    if (groupCount == 0) {
        printf("Zadne skupiny nejsou vytvoreny.\n");
        return;
    }

    // Krok 2: V�b�r skupiny
    printf("Aktualni skupiny:\n");
    for (int i = 0; i < groupCount; i++) {
        printf("%d) %s\n", i + 1, groups[i]);
    }

    int choice = getValidChoice(1, groupCount);
    const char* selectedGroup = groups[choice - 1];

    // Krok 3: Zobrazen� detail� o skupin�
    printf("Skupina: %s\n", selectedGroup);

    printf("Ucitel: ");
    int teacherFound = 0;
    for (int i = 0; i < userCount; i++) {
        if (users[i].role == 'U' && strstr(users[i].subjects, selectedGroup)) {
            printf("%s\n", users[i].username);
            teacherFound = 1;
            break;
        }
    }
    if (!teacherFound) {
        printf("Zadny ucitel prirazen.\n");
    }

    printf("Studenti: ");
    int studentFound = 0;
    for (int i = 0; i < userCount; i++) {
        if (users[i].role == 'T' && strstr(users[i].subjects, selectedGroup)) {
            printf("%s ", users[i].username);
            studentFound = 1;
        }
    }
    if (!studentFound) {
        printf("Zadni studenti prirazeni.\n");
    }
    printf("\n");
}

//p�id�n� p�edm�tu
void addSubject(int studentIndex) {
    if (gradesCount[studentIndex] >= MAX_GRADES) {
        printf("Maximalni pocet predmetu dosazen.\n");
        return;
    }

    printf("Vyberte predmet:\n");
    for (int i = 0; i < MAX_FIXED_SUBJECTS; i++) {
        printf("%d) %s\n", i + 1, fixedSubjects[i]);
    }

    int choice = getValidChoice(1, MAX_FIXED_SUBJECTS);
    const char* selectedSubject = fixedSubjects[choice - 1];

    // Kontrola, jestli u� student tento p�edm�t nem�
    for (int i = 0; i < gradesCount[studentIndex]; i++) {
        if (strcmp(grades[studentIndex][i].subject, selectedSubject) == 0) {
            printf("Tento predmet je uz studentovi prirazen.\n");
            return;
        }
    }

    // P�id�n� p�edm�tu studentovi
    strcpy(grades[studentIndex][gradesCount[studentIndex]].subject, selectedSubject);
    grades[studentIndex][gradesCount[studentIndex]].exercisePoints = 0;
    grades[studentIndex][gradesCount[studentIndex]].testPoints = 0;
    grades[studentIndex][gradesCount[studentIndex]].finalExamPoints = 0;
    grades[studentIndex][gradesCount[studentIndex]].grade = 'F';

    gradesCount[studentIndex]++;
    printf("Predmet '%s' pridan.\n", selectedSubject);
}

int getValidChoice(int min, int max) {
    int choice;
    while (1) {
        printf("Vyberte moznost (%d-%d): ", min, max);
        if (scanf("%d", &choice) == 1 && choice >= min && choice <= max) {
            return choice;
        }
        printf("Neplatna volba. Zkuste znovu.\n");
        while (getchar() != '\n'); // Vycisteni bufferu
    }
}

//zaps�n� zn�mek student�m u�itelem
void gradeStudents(const char* teacherUsername) {
    // Krok 1: V�pis skupin u�itele
    printf("Vase skupiny:\n");
    char teacherGroups[MAX_USERS][MAX_NAME_LEN];
    int groupCount = 0;

    // Na�ten� skupin u�itele
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, teacherUsername) == 0) {
            char tempSubjects[MAX_SUBJECTS_LEN];
            strcpy(tempSubjects, users[i].subjects);
            char* token = strtok(tempSubjects, ",");
            while (token != NULL) {
                if (strncmp(token, "sk-", 3) == 0) {
                    strcpy(teacherGroups[groupCount++], token);
                }
                token = strtok(NULL, ",");
            }
            break;
        }
    }

    if (groupCount == 0) {
        printf("Nemate zadne prirazene skupiny.\n");
        return;
    }

    // V�pis skupin u�itele
    for (int i = 0; i < groupCount; i++) {
        printf("%d) %s\n", i + 1, teacherGroups[i]);
    }

    // V�b�r skupiny
    int groupChoice = getValidChoice(1, groupCount);
    const char* selectedGroup = teacherGroups[groupChoice - 1];

    // Krok 2: V�b�r student�
    printf("Studenti ve skupine %s:\n", selectedGroup);
    int studentIndexes[MAX_USERS];
    int studentIndexCount = 0;

    for (int i = 0; i < userCount; i++) {
        if (users[i].role == 'T' && strstr(users[i].subjects, selectedGroup)) {
            printf("%d) %s\n", studentIndexCount + 1, users[i].username);
            studentIndexes[studentIndexCount++] = i;
        }
    }

    if (studentIndexCount == 0) {
        printf("Ve skupine nejsou zadni studenti.\n");
        return;
    }

    // V�b�r student�
    printf("Zadejte cisla studentu oddelena carkou: ");
    char studentInput[MAX_NAME_LEN];
    scanf(" %99[^\n]", studentInput);

    int selectedStudents[MAX_USERS];
    int selectedCount = 0;
    char* studentToken = strtok(studentInput, ",");
    while (studentToken != NULL) {
        int index = atoi(studentToken) - 1;
        if (index >= 0 && index < studentIndexCount) {
            selectedStudents[selectedCount++] = studentIndexes[index];
        }
        else {
            printf("Neplatne cislo studenta: %s\n", studentToken);
        }
        studentToken = strtok(NULL, ",");
    }

    if (selectedCount == 0) {
        printf("Nebyl vybran zadny platny student.\n");
        return;
    }

    // V�b�r typu hodnocen�
    printf("Vyberte typ hodnoceni:\n");
    printf("1) Cviceni\n");
    printf("2) Test\n");
    printf("3) Zaverecna zkouska\n");
    int evaluationType = getValidChoice(1, 3);

    const char* prefix;
    if (evaluationType == 1) {
        prefix = "cv";
    }
    else if (evaluationType == 2) {
        prefix = "t";
    }
    else {
        prefix = "zk";
    }

    // Zad�n� bod�
    int grade;
    do {
        printf("Zadejte body (0-100): ");
        if (scanf("%d", &grade) == 1 && grade >= 0 && grade <= 100) {
            break;
        }
        printf("Neplatne! Zadejte hodnotu mezi 0 a 100.\n");
        while (getchar() != '\n'); // Vy�istit buffer
    } while (1);

    // P�i�azen� zn�mky vybran�m student�m
    for (int i = 0; i < selectedCount; i++) {
        int studentIndex = selectedStudents[i];
        char gradeEntry[MAX_NAME_LEN];
        snprintf(gradeEntry, sizeof(gradeEntry), "%sz%d%s", prefix, grade, selectedGroup);

        // P�id�n� zn�mky do p�edm�t� studenta
        if (strlen(users[studentIndex].subjects) + strlen(gradeEntry) + 1 < MAX_SUBJECTS_LEN) {
            strcat(users[studentIndex].subjects, ",");
            strcat(users[studentIndex].subjects, gradeEntry);
        }
        else {
            printf("Nelze priradit znamku studentovi %s. Prekrocena kapacita predmetu.\n", users[studentIndex].username);
        }
    }

    printf("Znamka %s%d byla prirazena studentum ve skupine %s.\n", prefix, grade, selectedGroup);
    saveUsers();
}

//zobrazen� zn�mek student� u u�itele
void viewGroupGrades(const char* teacherUsername) {
    // Krok 1: V�pis skupin u�itele
    printf("Vase skupiny:\n");
    char teacherGroups[MAX_USERS][MAX_NAME_LEN];
    int groupCount = 0;

    // Na�ten� skupin u�itele
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, teacherUsername) == 0) { // Pouze skupiny p�ihl�en�ho u�itele
            char tempSubjects[MAX_SUBJECTS_LEN];
            strcpy(tempSubjects, users[i].subjects);
            char* token = strtok(tempSubjects, ",");
            while (token != NULL) {
                if (strncmp(token, "sk-", 3) == 0) { // Skupina
                    strcpy(teacherGroups[groupCount++], token);
                }
                token = strtok(NULL, ",");
            }
            break;
        }
    }

    if (groupCount == 0) {
        printf("Nemate zadne prirazene skupiny.\n");
        return;
    }

    // V�pis skupin u�itele
    for (int i = 0; i < groupCount; i++) {
        printf("%d) %s\n", i + 1, teacherGroups[i]);
    }

    // V�b�r skupiny
    int groupChoice = getValidChoice(1, groupCount);
    const char* selectedGroup = teacherGroups[groupChoice - 1];

    // Krok 2: V�pis student� a jejich zn�mek
    printf("Studenti a jejich znamky ve skupine %s:\n", selectedGroup);

    int studentCount = 0;

    // Iterace p�es v�echny studenty
    for (int i = 0; i < userCount; i++) {
        if (users[i].role == 'T' && strstr(users[i].subjects, selectedGroup)) { // Pouze studenti v dan� skupin�
            printf("%s-", users[i].username);

            // V�pis zn�mek pat��c�ch k vybran� skupin�
            char tempSubjects[MAX_SUBJECTS_LEN];
            strcpy(tempSubjects, users[i].subjects);
            char* token = strtok(tempSubjects, ",");
            int first = 1;
            int totalPoints = 0; // Celkov� sou�et bod�

            while (token != NULL) {
                // Kontrola, zda token kon�� vybranou skupinou
                size_t tokenLen = strlen(token);
                size_t groupLen = strlen(selectedGroup);
                if (tokenLen > groupLen && strcmp(&token[tokenLen - groupLen], selectedGroup) == 0) {
                    if (!first) printf(",");
                    printf("%.*s", (int)(tokenLen - groupLen), token); // Vypisujeme v�e p�ed skupinou
                    first = 0;

                    // Na��t�n� bod� z form�tu jako "zkz60v2"
                    int grade;
                    if (sscanf(token, "%*[^0-9]%d", &grade) == 1) { // Extrahuje ��slo (bodovou hodnotu)
                        totalPoints += grade;  // P�id�v�me zn�mky k celkov�mu sou�tu
                    }
                }
                token = strtok(NULL, ",");
            }

            if (first) { // Pokud student nem� ��dn� zn�mky pro skupinu
                printf("zadne znamky");
            }
            else {
                // Ur�en� v�sledn� zn�mky na z�klad� celkov�ho sou�tu bod�
                char finalGrade = 'F';
                if (totalPoints >= 91) finalGrade = 'A';
                else if (totalPoints >= 81) finalGrade = 'B';
                else if (totalPoints >= 71) finalGrade = 'C';
                else if (totalPoints >= 61) finalGrade = 'D';
                else if (totalPoints >= 51) finalGrade = 'E';

                printf(" | Celkovy soucet bodu: %d | Vysledna znamka: %c", totalPoints, finalGrade);
            }
            printf("\n");
            studentCount++;
        }
    }

    if (studentCount == 0) {
        printf("Ve skupine nejsou zadni studenti.\n");
    }
}

//funkce pro studenty - zobrazen� zn�mek
void viewStudentGrades(const char* studentUsername) {
    // Krok 1: V�pis skupin studenta
    oddelovac();
    printf("Vase studijni skupiny:\n");
    char studentGroups[MAX_USERS][MAX_NAME_LEN];
    int groupCount = 0;

    // Na�ten� skupin studenta
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, studentUsername) == 0) { // Pouze skupiny p�ihl�en�ho studenta
            char tempSubjects[MAX_SUBJECTS_LEN];
            strcpy(tempSubjects, users[i].subjects);
            char* token = strtok(tempSubjects, ",");
            while (token != NULL) {
                if (strncmp(token, "sk-", 3) == 0) { // Skupina
                    strcpy(studentGroups[groupCount++], token);
                }
                token = strtok(NULL, ",");
            }
            break;
        }
    }

    if (groupCount == 0) {
        oddelovac();
        printf("Nemate zadne prirazene studijni skupiny.\n");
        return;
    }

    // V�pis skupin studenta
    for (int i = 0; i < groupCount; i++) {
        printf("%d) %s\n", i + 1, studentGroups[i]);
    }

    // V�b�r skupiny
    int groupChoice = getValidChoice(1, groupCount);
    const char* selectedGroup = studentGroups[groupChoice - 1];

    // Krok 2: V�pis zn�mek studenta ve vybran� skupin�
    oddelovac();
    printf("Znamky ve skupine %s:\n", selectedGroup);

    // Vyhled�n� studenta
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, studentUsername) == 0) { // Pouze p�ihl�en� student
            char tempSubjects[MAX_SUBJECTS_LEN];
            strcpy(tempSubjects, users[i].subjects);
            char* token = strtok(tempSubjects, ",");
            int hasGrades = 0;
            int totalPoints = 0;  // Celkov� sou�et bod�

            while (token != NULL) {
                // Kontrola, zda token kon�� vybranou skupinou
                size_t tokenLen = strlen(token);
                size_t groupLen = strlen(selectedGroup);
                if (tokenLen > groupLen && strcmp(&token[tokenLen - groupLen], selectedGroup) == 0) {
                    if (hasGrades) printf(","); // Odd�luje zn�mky ��rkou
                    printf("%.*s", (int)(tokenLen - groupLen), token); // Vypisuje ��st p�ed skupinou
                    hasGrades = 1;

                    // Na��t�n� bod� z form�tu jako "zkz60v2"
                    int grade;
                    if (sscanf(token, "%*[^0-9]%d", &grade) == 1) { // Extrahuje ��slo (bodovou hodnotu)
                        totalPoints += grade;  // P�id�v�me zn�mky k celkov�mu sou�tu
                    }
                }
                token = strtok(NULL, ",");
            }

            if (!hasGrades) { // Pokud student nem� ��dn� zn�mky pro skupinu
                printf("zadne znamky");
            }
            else {
                // P�i�azen� v�sledn� zn�mky na z�klad� celkov�ho sou�tu bod�
                char finalGrade = 'F';
                if (totalPoints >= 91) finalGrade = 'A';
                else if (totalPoints >= 81) finalGrade = 'B';
                else if (totalPoints >= 71) finalGrade = 'C';
                else if (totalPoints >= 61) finalGrade = 'D';
                else if (totalPoints >= 51) finalGrade = 'E';

                printf(" | Celkovy soucet bodu: %d | Vysledna znamka: %c", totalPoints, finalGrade);
            }
            printf("\n");
            return;
        }
    }

    printf("Student nenalezen.\n");
}