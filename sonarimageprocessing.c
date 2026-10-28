#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void matrixGeneration(int *ptr, int n)
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            *(ptr + i * n + j) = rand() % 256;
        }
    }
}

void printMatrix(int *ptr, int n)
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", *(ptr + i * n + j));
        }
        printf("\n");
    }
}

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void transposeMatrix(int *ptr, int n)
{
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            swap(ptr + i * n + j, ptr + j * n + i);
        }
    }
}

void reverseRows(int *ptr, int n)
{
    for (int i = 0; i < n; i++) {
        int left = 0;
        int right = n - 1;

        while (left < right) {
            swap(ptr + i * n + left, ptr + i * n + right);
            left++;
            right--;
        }
    }
}

void rotateMatrix(int *ptr, int n)
{
    transposeMatrix(ptr, n);
    reverseRows(ptr, n);
}

int calculateAverage(int *prev, int *curr, int *next,
               int n, int i, int j)
{
    int sum = 0;
    int count = 0;

    // Previous row
    if (i > 0) {
        for (int c = j - 1; c <= j + 1; c++) {
            if (c >= 0 && c < n) {
                sum += *(prev + c);
                count++;
            }
        }
    }

    // Current row
    for (int c = j - 1; c <= j + 1; c++) {
        if (c >= 0 && c < n) {
            sum += *(curr + c);
            count++;
        }
    }

    // Next row
    if (i < n - 1) {
        for (int c = j - 1; c <= j + 1; c++) {
            if (c >= 0 && c < n) {
                sum += *(next + c);
                count++;
            }
        }
    }

    return sum / count;
}

void applySmoothing(int *ptr, int n)
{
    int prev[n];
    int curr[n];
    int next[n];

    
    for (int j = 0; j < n; j++) {
        *(prev + j) = *(ptr + j);
    }

    
    for (int j = 0; j < n; j++) {
        *(next + j) = *(ptr + n + j);
    }

    for (int i = 0; i < n; i++) {

        // Save the original current row
        for (int j = 0; j < n; j++) {
            *(curr + j) = (i == 0)
                ? *(prev + j)
                : *(next + j);
        }

        // Preserve the next original row before updating the current row
        if (i + 2 < n) {
            for (int j = 0; j < n; j++) {
                *(next + j) = *(ptr + (i + 2) * n + j);
            }
        }

        
        for (int j = 0; j < n; j++) {
            int sum = 0;
            int count = 0;

            if (i > 0) {
                for (int c = j - 1; c <= j + 1; c++) {
                    if (c >= 0 && c < n) {
                        sum += *(prev + c);
                        count++;
                    }
                }
            }

            for (int c = j - 1; c <= j + 1; c++) {
                if (c >= 0 && c < n) {
                    sum += *(curr + c);
                    count++;
                }
            }

            if (i < n - 1) {
                for (int c = j - 1; c <= j + 1; c++) {
                    if (c >= 0 && c < n) {
                        sum += *(next + c);
                        count++;
                    }
                }
            }

            *(ptr + i * n + j) = sum / count;
        }

        
        for (int j = 0; j < n; j++) {
            *(prev + j) = *(curr + j);
        }
    }
}

int main(void)
{
    int n;

    printf("Enter matrix size (2-10): ");

    if (scanf("%d", &n) != 1 || n < 2 || n > 10) {
        printf("Matrix size is invalid.\n");
        return 1;
    }

    int matrix[n][n];
    int *ptr = &matrix[0][0];

    srand(time(NULL));

    matrixGeneration(ptr, n);

    printf("Original Randomly Generated Matrix:\n");
    printMatrix(ptr, n);

    transposeMatrix(ptr, n);

    printf("Matrix after 90 degree Clockwise Rotation:\n");
    printMatrix(ptr, n);

    applySmoothing(ptr, n);

    printf("Matrix after Applying 3 x 3 Smoothing Filter:\n");
    printMatrix(ptr, n);

    return 0;
}