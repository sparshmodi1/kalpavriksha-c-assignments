#include <stdio.h>

#define FILENAME "users.txt"

typedef struct {
    int id;
    char name[50];
    int age;
} User;

void createUser() {
    User u, temp;
    int exists = 0;

    printf("Enter ID: ");
    scanf("%d", &u.id);

    FILE *fp = fopen(FILENAME, "r");

    while (fscanf(fp, "%d %s %d", &temp.id, temp.name, &temp.age) == 3) {
        if (temp.id == u.id) {
            exists = 1;
            break;
        }
    }

    fclose(fp);

    if (exists) {
        printf("ID already exists.\n");
        return;
    }

    printf("Enter Name: ");
    scanf("%s", u.name);

    printf("Enter Age: ");
    scanf("%d", &u.age);

    fp = fopen(FILENAME, "a");
    fprintf(fp, "%d %s %d\n", u.id, u.name, u.age);
    fclose(fp);

    printf("User added.\n");
}

void readUsers() {
    FILE *fp = fopen(FILENAME, "r");
    User u;

    printf("\nID    Name       Age\n");

    while (fscanf(fp, "%d %s %d", &u.id, u.name, &u.age) == 3) {
        printf("%-5d %-10s %d\n", u.id, u.name, u.age);
    }

    fclose(fp);
}

void updateUser() {
    int targetId;

    printf("Enter ID to update: ");
    scanf("%d", &targetId);

    FILE *fp = fopen(FILENAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    User u;

    while (fscanf(fp, "%d %s %d", &u.id, u.name, &u.age) == 3) {
        if (u.id == targetId) {
            printf("Enter new Name: ");
            scanf("%s", u.name);

            printf("Enter new Age: ");
            scanf("%d", &u.age);
        }

        fprintf(temp, "%d %s %d\n", u.id, u.name, u.age);
    }

    fclose(fp);
    fclose(temp);

    remove(FILENAME);
    rename("temp.txt", FILENAME);

    printf("User updated.\n");
}

void deleteUser() {
    int targetId;

    printf("Enter ID to delete: ");
    scanf("%d", &targetId);

    FILE *fp = fopen(FILENAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    User u;

    while (fscanf(fp, "%d %s %d", &u.id, u.name, &u.age) == 3) {
        if (u.id != targetId) {
            fprintf(temp, "%d %s %d\n", u.id, u.name, u.age);
        }
    }

    fclose(fp);
    fclose(temp);

    remove(FILENAME);
    rename("temp.txt", FILENAME);

    printf("User deleted.\n");
}

int main() {
    int choice;

    FILE *fp = fopen(FILENAME, "a");
    fclose(fp);

    while (1) {
        printf("\n1. Create\n");
        printf("2. Read\n");
        printf("3. Update\n");
        printf("4. Delete\n");
        printf("5. Exit\n");

        printf("Enter choice: ");
        scanf("%d", &choice);

        if (choice == 1)
            createUser();
        else if (choice == 2)
            readUsers();
        else if (choice == 3)
            updateUser();
        else if (choice == 4)
            deleteUser();
        else if (choice == 5)
            break;
    }

    return 0;
}