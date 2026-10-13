#include<stdio.h>

int main(){
    const int STANDARD = 150;
    const int PREMIUM = 250;
    const int RECLINER = 400;

    char c_name[50];
    printf("Enter Customer Name: ");
    scanf("%s", c_name);

    int tickets;
    printf("Enter number of tickets between 1 and 20 inclusive: ");
    scanf("%d", &tickets);
    if(tickets < 1 || tickets > 20){
        printf("Invalid number of tickets. Please enter a number between 1 and 20.\n");
        return 1;
    }

    int ticket_category;
    printf("Choose Ticket Category \n 1 = Standard: 150 \n 2 = Premium: 250 \n 3 = Recliner: 400 \n Enter your choice: ");
    scanf("%d", &ticket_category);

    printf("Customer: %s\n", c_name);

    float subtotal;
    switch(ticket_category)
    {
    case 1:
        subtotal = tickets * STANDARD;
        printf("Ticket Category: Standard\n");
        break;
    case 2:
        subtotal = tickets * PREMIUM;
        printf("Ticket Category: Premium\n");
        break;
    case 3:
        subtotal = tickets * RECLINER;
        printf("Ticket Category: Recliner\n");
        break;
    default:
        printf("Invalid category selected\n");
        return 1;
    }

    printf("Tickets: %d\n", tickets);
    printf("Subtotal: %.2f\n", subtotal);

    float discount;
    if(tickets>=5){
        discount = subtotal*0.1;
    }
    else{
        discount = 0;
    }
    printf("Discount: %.2f\n", discount);

    float finalbill = subtotal - discount;
    printf("Final Bill: %.2f\n", finalbill);

    return 0;
}