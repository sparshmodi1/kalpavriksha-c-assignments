#include <stdio.h>
#include <ctype.h>

const int STANDARD = 150;
const int PREMIUM = 250;
const int RECLINER = 400;

float calculateSubtotal(int tickets_count, int ticket_price)
{
    return tickets_count * ticket_price;
}

float calculateDiscount(float subtotal_amount, int ticket_count)
{
    if (ticket_count >= 5)
    {
        return subtotal_amount * 0.10f;
    }
    else
    {
        return 0.0f;
    }
}

int main()
{
    char customer_name[50];

    if (scanf("%49s", customer_name) != 1) {
        printf("Invalid customer name.\n");
        return 1;
    }

    int tickets_count;

    if (scanf("%d", &tickets_count) != 1) {
        printf("Invalid input. Please enter an integer.\n");
        return 1;
    }

    if (tickets_count < 1 || tickets_count > 20) {
        printf("Number of tickets must be between 1 and 20.\n");
        return 1;
    }

    int ticket_category;

    if (scanf("%d", &ticket_category) != 1) {
        printf("Invalid input. Please enter an integer.\n");
        return 1;
    }

    float subtotal;

    switch (ticket_category)
    {
        case 1:
            subtotal = calculateSubtotal(tickets_count, STANDARD);
            printf("Ticket Category: Standard\n");
            break;

        case 2:
            subtotal = calculateSubtotal(tickets_count, PREMIUM);
            printf("Ticket Category: Premium\n");
            break;

        case 3:
            subtotal = calculateSubtotal(tickets_count, RECLINER);
            printf("Ticket Category: Recliner\n");
            break;

        default:

            printf("Invalid category selected\n");
            return 1;
    }
    
    printf("Customer: %s\n", customer_name);
    printf("Tickets: %d\n", tickets_count);
    printf("Subtotal: %.2f\n", subtotal);

    float discount = calculateDiscount(subtotal, tickets_count);
    printf("Discount: %.2f\n", discount);

    float finalbill = subtotal - discount;
    printf("Final Bill: %.2f\n", finalbill);

    return 0;
}
