#include <stdio.h>
#include <ctype.h>

int main() {
    char str[100];
    int values[50], n = 0;
    char operators[50];
    int m = 0;
    int i = 0, value, answer;

    printf("Enter expression: ");
    fgets(str, sizeof(str), stdin);

    while (str[i] != '\0') {

        while (isspace(str[i]))
            i++;

        if (!isdigit(str[i])) {
            printf("Error: Invalid expression.\n");
            return 0;
        }

        value = 0;

        while (isdigit(str[i])) {
            value = value * 10 + (str[i] - '0');
            i++;
        }

        values[n++] = value;

        while (isspace(str[i]))
            i++;

        if (str[i] == '\n' || str[i] == '\0')
            break;

        if (str[i] != '+' && str[i] != '-' &&
            str[i] != '*' && str[i] != '/') {
            printf("Error: Invalid expression.\n");
            return 0;
        }

        operators[m++] = str[i];
        i++;
    }

    if (n != m + 1) {
        printf("Error: Invalid expression.\n");
        return 0;
    }

    for (i = 0; i < m; i++) {
        if (operators[i] == '*' || operators[i] == '/') {

            if (operators[i] == '/') {
                if (values[i + 1] == 0) {
                    printf("Error: Division by zero.\n");
                    return 0;
                }

                values[i] = values[i] / values[i + 1];
            } else {
                values[i] = values[i] * values[i + 1];
            }

            int j;

            for (j = i + 1; j < n - 1; j++)
                values[j] = values[j + 1];

            for (j = i; j < m - 1; j++)
                operators[j] = operators[j + 1];

            n--;
            m--;
            i--;
        }
    }

    answer = values[0];

    for (i = 0; i < m; i++) {
        if (operators[i] == '+')
            answer += values[i + 1];
        else
            answer -= values[i + 1];
    }

    printf("%d\n", answer);

    return 0;
}