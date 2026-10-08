#include <stdio.h>

typedef struct {
    int rollno;
    char name[50];
    int marks[3];
} Student;

int calculateTotal(Student s) {
    int total = 0;

    for (int i = 0; i < 3; i++) {
        total += s.marks[i];
    }

    return total;
}

char calculategrade(float average) {
    if (average >= 85)
        return 'A';
    else if (average >= 70)
        return 'B';
    else if (average >= 50)
        return 'C';
    else if (average >= 35)
        return 'D';
    else
        return 'F';
}

void performancepattern(char grade) {
    int stars;

    if (grade == 'A')
        stars = 5;
    else if (grade == 'B')
        stars = 4;
    else if (grade == 'C')
        stars = 3;
    else if (grade == 'D')
        stars = 2;
    else
        return;

    for (int i = 0; i < stars; i++) {
        printf("*");
    }

    printf("\n");
}

void printRollNumbers(int n) {
    if (n == 0)
        return;

    printRollNumbers(n - 1);
    printf("%d ", n);
}

int main() {
    int n; // number of students
    int totalmarks;
    float avgmarks;
    char grade;

    Student S[100];

    printf("Enter No. of Students: ");
    scanf("%d", &n);

    if (n < 1 || n > 100)
        return 1;

    for (int i = 0; i < n; i++) {
        scanf("%d", &S[i].rollno);
        scanf(" %49[^0-9\n]", S[i].name);

        for (int j = 0; j < 3; j++) {
            scanf("%d", &S[i].marks[j]);
        }
    }

    for (int i = 0; i < n; i++) {
        totalmarks = calculateTotal(S[i]);
        avgmarks = totalmarks / 3.0;
        grade = calculategrade(avgmarks);

        printf("Roll: %d\n", S[i].rollno);
        printf("Name: %s\n", S[i].name);
        printf("Total: %d\n", totalmarks);
        printf("Average: %.2f\n", avgmarks);
        printf("Grade: %c\n", grade);

        if (avgmarks < 35) {
            continue;
        }

        printf("Performance: ");
        performancepattern(grade);
    }

    printf("List of Roll Numbers (via recursion): ");
    printRollNumbers(n);
    printf("\n");

    return 0;
}