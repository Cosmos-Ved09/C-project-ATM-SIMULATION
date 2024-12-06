#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>           // to get the time of doing transaction
#include <stdbool.h>

#define MAX_USERS 10
#define PIN_LENGTH 4
#define OTP_LENGTH 6

typedef struct {
    char username[50];
    char pin[PIN_LENGTH + 1];
    float balance;
} User;

User users[MAX_USERS];
int user_count = 0;

// Function to simulate OTP generation
void generate_otp(char* otp) {
    srand(time(0));
    for (int i = 0; i < OTP_LENGTH; i++) {
        otp[i] = '0' + rand() % 10;  // Generates a digit between '0' and '9'
    }
    otp[OTP_LENGTH] = '\0';  // Null-terminate the string
}

// Function to log transaction to file
void log_transaction(const char *username, const char *transaction_type, int amount, float balance) {
    FILE *log_file = fopen("transactions.txt", "a");
    if (log_file == NULL) {
        printf("Error opening transaction log file!\n");
        return;
    }
    
    time_t now;
    time(&now);

    // Write transaction details to the file
    fprintf(log_file, "%s | User: %s | Amount: Rs.%d | Balance: Rs.%.2f | %s", 
            transaction_type, username, amount, balance, ctime(&now));
    fclose(log_file);
}

// Function to show mini statement for a user
void show_mini_statement(const char *username) {
    FILE *log_file = fopen("transactions.txt", "r");
    if (log_file == NULL) {
        printf("\nNo transactions to display.\n");
        return;
    }
    
    char buffer[256];
    char last_5_transactions[5][256];  // To store the last 5 transactions
    int count = 0;

    printf("\n\t\tMini-Statement for %s (Last 5 Transactions):\n", username);
    printf("\t\t-----------------------------------\n");
    
    // Loop through the entire file and store only transactions for this user
    while (fgets(buffer, sizeof(buffer), log_file)) {
        if (strstr(buffer, username) != NULL) {  // Filter by username
            if (count < 5) {
                strcpy(last_5_transactions[count], buffer);  // Store last 5 transactions
                count++;
            } else {
                // Shift transactions to maintain only the last 5
                for (int i = 1; i < 5; i++) {
                    strcpy(last_5_transactions[i - 1], last_5_transactions[i]);
                }
                strcpy(last_5_transactions[4], buffer);
            }
        }
    }
    fclose(log_file);
    
    // Display the last 5 transactions
    for (int i = 0; i < count; i++) {
        printf("%s", last_5_transactions[i]);
    }
}

void initialize_users() {
    // Initializing users
    strcpy(users[0].username, "Ved Vyas");
    strcpy(users[0].pin, "1995");
    users[0].balance = 10000.00;

    strcpy(users[1].username, "Ravi Sharma"); // Placeholder for user 2
    strcpy(users[1].pin, "5612");
    users[1].balance = 5000.00;

    user_count = 2; // Update user count
}

int find_user_index(const char *username) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].username, username) == 0) {
            return i; // Return user index if found
        }
    }
    return -1; // User not found
}

int main() {
    initialize_users();

    char entered_username[50], entered_pin[PIN_LENGTH + 1];
    char otp[OTP_LENGTH + 1];  
    char entered_otp[OTP_LENGTH + 1];  // User entered OTP
    int amount = 1, option;
    float balance = 0;
    int continue_transaction = 1;

    printf("\t ******************WELCOME TO GalacticATM*******************");
    printf("\nEnter your username: ");
    scanf(" %[^\n]s", entered_username); // Read full line for username
    printf("Enter your PIN: ");
    scanf("%s", entered_pin);

    int user_index = find_user_index(entered_username);

    if (user_index == -1 || strcmp(users[user_index].pin, entered_pin) != 0) {
        printf("\n\t\tInvalid username or PIN.");
        return 1;
    }

    printf("\n\t\tLogin successful! Welcome, %s!", entered_username);
    
    // Store user's balance
    balance = users[user_index].balance;

    // Step 2: Generate and validate OTP for Two-Factor Authentication (2FA)
    generate_otp(otp);
    printf("\nYour OTP is: %s\n", otp);  // In a real system, OTP would be sent via SMS/Email
    
    printf("Please enter the OTP: ");
    scanf("%s", entered_otp);

    if (strcmp(otp, entered_otp) != 0) {
        printf("Invalid OTP! Exiting...\n");
        return 1;
    } else {
        printf("OTP Verified! You are logged in.\n");
    }

    // ATM transaction menu
    while (continue_transaction != 0) {
        printf("\n\t\t\t*************Available Transactions************");
        printf("\n\t\t1. Withdrawal");
        printf("\n\t\t2. Deposit");
        printf("\n\t\t3. Check Balance");
        printf("\n\t\t4. View Mini-Statement");
        printf("\n\n\t\tPlease select the option: ");
        scanf("%d", &option);

        switch (option) {
            case 1: // Withdrawal
                printf("\n\t\tEnter the amount to withdraw: ");
                scanf("%d", &amount);

                if (balance < amount) {
                    printf("\n\t\tSorry, insufficient balance!");
                } else {
                    balance -= amount;
                    printf("\n\t\tYou have withdrawn Rs.%d. Your new balance is Rs.%.2f", amount, balance);
                    log_transaction(users[user_index].username, "Withdrawal", amount, balance);  // Log the transaction
                }
                break;

            case 2: // Deposit
                printf("\n\t\tEnter the amount to deposit: ");
                scanf("%d", &amount);

                balance = balance + amount;
                printf("\n\t\tYou have deposited Rs.%d. Your new balance is Rs.%.2f", amount, balance);
                log_transaction(users[user_index].username, "Deposit", amount, balance);  // Log the transaction
                break;

            case 3: // Check Balance
                printf("\n\t\tYour current balance is Rs.%.2f", balance);
                break;

            case 4: // Mini-Statement
                show_mini_statement(users[user_index].username);
                break;

            default:
                printf("\n\t\tInvalid Option!!!");
        }

        // Update the user's balance in the user structure and file after every transaction
        users[user_index].balance = balance;
        FILE *file = fopen("balance.txt", "w");
        if (file != NULL) {
            for (int i = 0; i < user_count; i++) {
                fprintf(file, "%s %s %.2f\n", users[i].username, users[i].pin, users[i].balance);
            }
            fclose(file);
        } else {
            printf("\nError: Unable to save balance information!\n");
        }

        printf("\n\n\t\tDo you wish to perform another transaction? Press 1 for Yes, 0 for No: ");
        scanf("%d", &continue_transaction);
    }

    printf("\n\t\tThank you for banking with GalaticATM!\n");

    return 0;
}