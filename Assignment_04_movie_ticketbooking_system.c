#include<stdio.h>
const int STANDARD = 150;
const int PREMIUM = 250;
const int RECLINER = 400;


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

int main(){
    char customer_name[50];
    printf("Enter Customer Name: ");
    scanf("%49s", customer_name);

    

    int tickets_count;
    printf("Enter number of tickets between 1 and 20 inclusive: ");

    if (scanf("%d", &tickets_count) != 1) {
        printf("Invalid input. Please enter an integer.\n");
        return 1;
    }

    if (tickets_count < 1 || tickets_count > 20) {
        printf("Number of tickets must be between 1 and 20.\n");
        return 1;
    }

    int ticket_category;
    printf("Choose Ticket Category \n 1 = Standard: 150 \n 2 = Premium: 250 \n 3 = Recliner: 400 \n Enter your choice: ");
    scanf("%d", &ticket_category);

    printf("Customer: %s\n", customer_name);

    float subtotal;
    switch(ticket_category)
    {
    case 1:
        subtotal = tickets_count * STANDARD;
        printf("Ticket Category: Standard\n");
        break;
    case 2:
        subtotal = tickets_count * PREMIUM;
        printf("Ticket Category: Premium\n");
        break;
    case 3:
        subtotal = tickets_count * RECLINER;
        printf("Ticket Category: Recliner\n");
        break;
    default:
        printf("Invalid category selected\n");
        return 1;
    }

    printf("Tickets: %d\n", tickets_count);
    printf("Subtotal: %.2f\n", subtotal);

    float discount = calculateDiscount(subtotal, tickets_count);
    printf("Discount: %.2f\n", discount);

    float finalbill = subtotal - discount;
    printf("Final Bill: %.2f\n", finalbill);

    return 0;
}
