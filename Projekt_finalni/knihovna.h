#pragma once

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

void oddelovac();
void clc();