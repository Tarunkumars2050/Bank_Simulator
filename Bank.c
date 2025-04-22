#include "Bank.h"
#include <math.h>
void initializeSystem(Account accounts[], int *numAccounts) {
    loadData(accounts, numAccounts);
    
    if(*numAccounts == 0) {
        // Create admin account with account number 1000
        Account adminAccount = {
            .accountNo = 1000,
            .balance = 0.0,
            .history.front = 0,
            .history.rear = -1,
            .history.count = 0
        };
        strcpy(adminAccount.name, "Admin_Account");
        strcpy(adminAccount.pin, "admin123");
        
        accounts[0] = adminAccount;
        (*numAccounts)++;
        saveData(accounts, *numAccounts);
    }
}

int login(Account accounts[], int numAccounts, int *currentUser) {
    int accountNo;
    char pin[PIN_LENGTH];
    
    printf("\nEnter Account Number: ");
    scanf("%d", &accountNo);
    printf("Enter PIN: ");
    scanf("%s", pin);
    
    for(int i = 0; i < numAccounts; i++) {
        if(accounts[i].accountNo == accountNo && strcmp(accounts[i].pin, pin) == 0) {
            *currentUser = i;
            return 1;
        }
    }
    return 0;
}

// Add at the top of the file after includes
Account *g_accounts = NULL;
int g_numAccounts = 0;

void performTransaction(Account *account, TransactionType type) {
    float amount;
    
    switch(type) {
        case DEPOSIT:
            printf("\nEnter amount to deposit: ");
            scanf("%f", &amount);
            if(amount > 0) {
                account->balance += amount;
                addTransaction(account, DEPOSIT, amount);
                saveData(g_accounts, g_numAccounts);  // Use global variables
                printf("Deposit successful. New balance: %.2f\n", account->balance);
            }
            break;
            
        case WITHDRAWAL:
            printf("\nEnter amount to withdraw: ");
            scanf("%f", &amount);
            if(amount > 0 && amount <= account->balance) {
                account->balance -= amount;
                addTransaction(account, WITHDRAWAL, amount);
                saveData(g_accounts, g_numAccounts);  // Use global variables
                printf("Withdrawal successful. New balance: %.2f\n", account->balance);
            } else {
                printf("Invalid amount or insufficient funds!\n");
            }
            break;
            
        case BALANCE_INQUIRY:
            printf("\nCurrent balance: %.2f\n", account->balance);
            addTransaction(account, BALANCE_INQUIRY, 0);
            break;
    }
}

void addTransaction(Account *account, TransactionType type, float amount) {
    TransactionHistory *history = &account->history;
    
    if(history->count == MAX_TRANSACTIONS) {
        history->front = (history->front + 1) % MAX_TRANSACTIONS;
    } else {
        history->count++;
    }
    
    history->rear = (history->rear + 1) % MAX_TRANSACTIONS;
    
    Transaction newTransaction = {
        .type = type,
        .amount = amount,
        .balance = account->balance,
        .timestamp = time(NULL)
    };
    
    history->transactions[history->rear] = newTransaction;
}


void createAccount(Account accounts[], int *numAccounts) {
    if (*numAccounts >= MAX_USERS) {
        printf("\nError: Maximum number of users reached!\n");
        return;
    }

    // Create temporary account
    Account newAccount;
    char tempName[50];
    char tempPin[PIN_LENGTH];
    
    // Find highest existing account number, starting from 1001
    int maxAccountNo = 1001;  // Start from 1001 to reserve 1000 for admin
    for(int i = 0; i < *numAccounts; i++) {
        if(accounts[i].accountNo > maxAccountNo && accounts[i].accountNo != 1000) {
            maxAccountNo = accounts[i].accountNo;
        }
    }

    printf("\nEnter your name: ");
    scanf(" %[^\n]s", tempName);
    
    printf("Create your PIN (4 digits): ");
    scanf("%s", tempPin);

    newAccount.accountNo = maxAccountNo + 1;
    newAccount.balance = 0.0;
    newAccount.history.front = 0;
    newAccount.history.rear = -1;
    newAccount.history.count = 0;

    // Replace spaces with underscores in name
    strcpy(newAccount.name, tempName);
    for(int i = 0; newAccount.name[i]; i++) {
        if(newAccount.name[i] == ' ') {
            newAccount.name[i] = '_';
        }
    }
    
    strcpy(newAccount.pin, tempPin);

    // Save the new account
    accounts[*numAccounts] = newAccount;
    (*numAccounts)++;
    g_numAccounts = *numAccounts;
    saveData(accounts, *numAccounts);

    printf("\nAccount created successfully!\n");
    printf("Your account number is: %d\n", newAccount.accountNo);
}

void displayTransactionHistory(Account *account) {
    TransactionHistory *history = &account->history;
    
    if (history->count == 0) {
        printf("\nNo transactions to display!\n");
        return;
    }

    printf("\n=== Transaction History ===\n");
    int current = history->front;
    
    for (int i = 0; i < history->count; i++) {
        Transaction *trans = &history->transactions[current];
        char *type;
        
        switch(trans->type) {
            case DEPOSIT:
                type = "Deposit";
                printf("Type: %s, Amount: +%.2f, Balance: %.2f\n", 
                       type, trans->amount, trans->balance);
                break;
            case WITHDRAWAL:
                type = "Withdrawal";
                printf("Type: %s, Amount: -%.2f, Balance: %.2f\n", 
                       type, trans->amount, trans->balance);
                break;
            case BALANCE_INQUIRY:
                type = "Balance Inquiry";
                printf("Type: %s, Balance: %.2f\n", 
                       type, trans->balance);
                break;
        }
        
        current = (current + 1) % MAX_TRANSACTIONS;
    }
}


int adminLogin(void) {
    char username[50];
    char password[50];
    
    printf("\nEnter Admin Username: ");
    scanf("%s", username);
    printf("Enter Admin Password: ");
    scanf("%s", password);
    
    return (strcmp(username, "Room_14") == 0 && strcmp(password, "admin1234") == 0);
}

void adminMenu(Account accounts[], int *numAccounts) {
    int choice;
    time_t lastCheck = time(NULL);
    
    while(1) {
        // Check maturity every second
        time_t currentTime = time(NULL);
        if(difftime(currentTime, lastCheck) >= 1) {
            checkAndProcessMaturity(accounts, *numAccounts);
            lastCheck = currentTime;
        }
        
        printf("\n=== ADMIN MENU ===\n");
        printf("1. Create New Account\n");
        printf("2. View All Users\n");
        printf("3. Process Loan\n");
        printf("4. Process FD\n");
        printf("5. Delete Account\n");
        printf("6. Admin Bank Operations\n");  // New option
        printf("7. Logout\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:
                createAccount(accounts, numAccounts);
                break;
            case 2:
                displayAllUsers(accounts, *numAccounts);
                break;
            case 3:
                processLoan(accounts, *numAccounts);
                break;
            case 4:
                processFD(accounts, *numAccounts);
                break;
            case 5:
                adminDeleteAccount(accounts, numAccounts);
                break;
            case 6:
                adminBankOperations(accounts, *numAccounts);
                break;
            case 7:
                printf("\nAdmin logged out successfully!\n");
                return;
            default:
                printf("\nInvalid choice!\n");
        }
    }
}

// Add this new function
void adminBankOperations(Account accounts[], int numAccounts) {
    int choice;
    
    while(1) {
        printf("\n=== ADMIN BANK OPERATIONS ===\n");
        printf("1. Deposit\n");
        printf("2. Withdraw\n");
        printf("3. Check Balance\n");
        printf("4. Back to Admin Menu\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:
                performTransaction(&accounts[0], DEPOSIT);
                break;
            case 2:
                performTransaction(&accounts[0], WITHDRAWAL);
                break;
            case 3:
                performTransaction(&accounts[0], BALANCE_INQUIRY);
                break;
            case 4:
                return;
            default:
                printf("\nInvalid choice!\n");
        }
    }
}

void displayAllUsers(Account accounts[], int numAccounts) {
    float totalBankWorth = 0;
    char displayName[50];  // Temporary buffer for display name
    
    printf("\n=== ALL USER ACCOUNTS ===\n");
    printf("%-15s %-30s %-15s\n", "Account No.", "Name", "Balance");
    printf("------------------------------------------------\n");
    
    for(int i = 0; i < numAccounts; i++) {
        // Copy name and replace underscores with spaces for display
        strcpy(displayName, accounts[i].name);
        for(int j = 0; displayName[j]; j++) {
            if(displayName[j] == '_') {
                displayName[j] = ' ';
            }
        }
        
        printf("%-15d %-30s %-15.2f\n", 
               accounts[i].accountNo, 
               displayName,   // Use display name with spaces
               accounts[i].balance);
        totalBankWorth += accounts[i].balance;
    }
    
    printf("\nTotal Bank Net Worth: %.2f\n", totalBankWorth);
}

// Add these functions at the end of the file

// Simple XOR encryption/decryption function
void encryptDecryptPin(char *pin) {
    const char key = 'K';  // Encryption key
    for(int i = 0; pin[i] != '\0'; i++) {
        pin[i] = pin[i] ^ key;
    }
}

void saveData(Account accounts[], int numAccounts) {
    FILE *file = fopen("c:\\Users\\TARUN\\Desktop\\DSA_PROJECT\\data.txt", "w");
    if (file == NULL) {
        printf("Error saving data!\n");
        return;
    }

    fprintf(file, "%d\n", numAccounts);
    
    for(int i = 0; i < numAccounts; i++) {
        Account acc = accounts[i];
        char encryptedPin[PIN_LENGTH];
        strcpy(encryptedPin, acc.pin);
        encryptDecryptPin(encryptedPin);
        
        fprintf(file, "%d %s %f %s\n", 
            acc.accountNo,
            acc.name,
            acc.balance,
            encryptedPin
        );
    }
    
    fclose(file);
    g_numAccounts = numAccounts;  // Update global count after save
}

void loadData(Account accounts[], int *numAccounts) {
    FILE *file = fopen("c:\\Users\\TARUN\\Desktop\\DSA_PROJECT\\data.txt", "r");
    if (file == NULL) {
        *numAccounts = 0;
        return;
    }

    fscanf(file, "%d\n", numAccounts);
    
    for(int i = 0; i < *numAccounts; i++) {
        Account *acc = &accounts[i];
        acc->history.front = 0;
        acc->history.rear = -1;
        acc->history.count = 0;
        
        fscanf(file, "%d %s %f %s\n",
            &acc->accountNo,
            acc->name,
            &acc->balance,
            acc->pin
        );
        
        encryptDecryptPin(acc->pin);  // Decrypt the PIN
    }
    
    fclose(file);
}

void deleteAccount(Account accounts[], int *numAccounts, int accountIndex) {
    char pin[PIN_LENGTH];
    printf("\nEnter PIN to confirm account deletion: ");
    scanf("%s", pin);
    
    if(strcmp(accounts[accountIndex].pin, pin) == 0) {
        // Shift remaining accounts
        for(int i = accountIndex; i < *numAccounts - 1; i++) {
            accounts[i] = accounts[i + 1];
        }
        (*numAccounts)--;
        saveData(accounts, *numAccounts);
        printf("\nAccount deleted successfully!\n");
    } else {
        printf("\nIncorrect PIN. Deletion cancelled.\n");
    }
}

void adminDeleteAccount(Account accounts[], int *numAccounts) {
    int accountNo;
    printf("\nEnter Account Number to delete: ");
    scanf("%d", &accountNo);
    
    for(int i = 0; i < *numAccounts; i++) {
        if(accounts[i].accountNo == accountNo) {
            // Shift remaining accounts
            for(int j = i; j < *numAccounts - 1; j++) {
                accounts[j] = accounts[j + 1];
            }
            (*numAccounts)--;
            saveData(accounts, *numAccounts);
            printf("\nAccount deleted successfully!\n");
            return;
        }
    }
    printf("\nAccount not found!\n");
}

void transferMoney(Account accounts[], int numAccounts, int senderIndex) {
    int receiverAcc;
    float amount;
    char displayName[50];  // For displaying name with spaces
    
    printf("\nEnter recipient's account number: ");
    scanf("%d", &receiverAcc);
    
    // Find receiver's account
    int receiverIndex = -1;
    for(int i = 0; i < numAccounts; i++) {
        if(accounts[i].accountNo == receiverAcc) {
            receiverIndex = i;
            
            // Convert underscores to spaces for display
            strcpy(displayName, accounts[i].name);
            for(int j = 0; displayName[j]; j++) {
                if(displayName[j] == '_') {
                    displayName[j] = ' ';
                }
            }
            
            printf("Recipient Name: %s\n", displayName);
            break;
        }
    }
    
    if(receiverIndex == -1) {
        printf("\nRecipient account not found!\n");
        return;
    }
    
    if(receiverIndex == senderIndex) {
        printf("\nCannot transfer to same account!\n");
        return;
    }
    
    printf("Enter amount to transfer: ");
    scanf("%f", &amount);
    
    if(amount <= 0) {
        printf("\nInvalid amount!\n");
        return;
    }
    
    if(amount > accounts[senderIndex].balance) {
        printf("\nInsufficient funds!\n");
        return;
    }
    
    // Perform transfer
    accounts[senderIndex].balance -= amount;
    accounts[receiverIndex].balance += amount;
    
    // Add transactions to both accounts
    addTransaction(&accounts[senderIndex], WITHDRAWAL, amount);
    addTransaction(&accounts[receiverIndex], DEPOSIT, amount);
    
    saveData(accounts, numAccounts);
    
    printf("\nTransfer successful!\n");
    printf("Your new balance: %.2f\n", accounts[senderIndex].balance);
} // Added missing closing brace

// Add these global variables at the top
LoanRecord loans[MAX_LOANS];
FDRecord fds[MAX_FDS];
int numLoans = 0;
int numFDs = 0;

float calculateCompoundInterest(float principal, float rate, time_t startTime) {
    time_t currentTime = time(NULL);
    double timeElapsed = difftime(currentTime, startTime) / 60.0; // Convert to minutes
    if(timeElapsed > 5) timeElapsed = 5; // Cap at 5 minutes
    
    return principal * pow(1 + rate, timeElapsed) - principal;
}

void processLoan(Account accounts[], int numAccounts) {
    int accountNo;
    float amount;
    
    printf("\nEnter account number for loan: ");
    scanf("%d", &accountNo);
    
    // Find account
    int accIndex = -1;
    for(int i = 0; i < numAccounts; i++) {
        if(accounts[i].accountNo == accountNo) {
            accIndex = i;
            break;
        }
    }
    
    if(accIndex == -1) {
        printf("Account not found!\n");
        return;
    }
    
    printf("Enter loan amount (max %.2f): ", MAX_LOAN_AMOUNT);
    scanf("%f", &amount);
    
    if(amount <= 0 || amount > MAX_LOAN_AMOUNT) {
        printf("Invalid amount!\n");
        return;    }

    // Check if admin has sufficient balance
    if(amount > accounts[0].balance) {
        printf("Loan cannot be processed due to insufficient admin funds!\n");
        return;
    }
    
    // Deduct from admin account
    accounts[0].balance -= amount;
    addTransaction(&accounts[0], WITHDRAWAL, amount);
    
    // Add to user account
    accounts[accIndex].balance += amount;
    addTransaction(&accounts[accIndex], DEPOSIT, amount);
    
    // Add loan record
    loans[numLoans].accountNo = accountNo;
    loans[numLoans].amount = amount;
    loans[numLoans].startTime = time(NULL);
    loans[numLoans].interestRate = LOAN_INTEREST_RATE;
    numLoans++;
    
    saveData(accounts, numAccounts);
    
    printf("Loan processed successfully!\n");
    printf("Please note: Amount with interest will be automatically deducted after 5 minutes.\n");
}

void processFD(Account accounts[], int numAccounts) {
    int accountNo;
    float amount;
    
    printf("\nEnter account number for FD: ");
    scanf("%d", &accountNo);
    
    // Find account
    int accIndex = -1;
    for(int i = 0; i < numAccounts; i++) {
        if(accounts[i].accountNo == accountNo) {
            accIndex = i;
            break;
        }
    }
    
    if(accIndex == -1) {
        printf("Account not found!\n");
        return;
    }
    
    printf("Enter FD amount: ");
    scanf("%f", &amount);
    
    if(amount <= 0 || amount > accounts[accIndex].balance) {
        printf("Invalid amount or insufficient balance!\n");
        return;
    }
    
    // Deduct from user account
    accounts[accIndex].balance -= amount;
    addTransaction(&accounts[accIndex], WITHDRAWAL, amount);
    
    // Add to admin account
    accounts[0].balance += amount;
    addTransaction(&accounts[0], DEPOSIT, amount);
    
    // Add FD record
    fds[numFDs].accountNo = accountNo;
    fds[numFDs].amount = amount;
    fds[numFDs].startTime = time(NULL);
    fds[numFDs].interestRate = FD_INTEREST_RATE;
    numFDs++;
    
    saveData(accounts, numAccounts);
    
    printf("FD processed successfully!\n");
    printf("Amount with interest will be credited after 5 minutes.\n");
}

void checkAndProcessMaturity(Account accounts[], int numAccounts) {
    time_t currentTime = time(NULL);
    
    // Process loans
    for(int i = 0; i < numLoans; i++) {
        if(difftime(currentTime, loans[i].startTime) >= MAX_PERIOD) {
            float interest = calculateCompoundInterest(loans[i].amount, LOAN_INTEREST_RATE, loans[i].startTime);
            float totalAmount = loans[i].amount + interest;
            
            // Find account and deduct amount
            for(int j = 0; j < numAccounts; j++) {
                if(accounts[j].accountNo == loans[i].accountNo) {
                    accounts[j].balance -= totalAmount;
                    addTransaction(&accounts[j], WITHDRAWAL, totalAmount);
                    
                    // Credit principal and interest to admin
                    accounts[0].balance += totalAmount;
                    addTransaction(&accounts[0], DEPOSIT, totalAmount);
                    break;
                }
            }
            
            // Remove loan record
            for(int j = i; j < numLoans - 1; j++) {
                loans[j] = loans[j + 1];
            }
            numLoans--;
            i--;
        }
    }
    
    // Process FDs
    for(int i = 0; i < numFDs; i++) {
        if(difftime(currentTime, fds[i].startTime) >= MAX_PERIOD) {
            float interest = calculateCompoundInterest(fds[i].amount, FD_INTEREST_RATE, fds[i].startTime);
            float totalAmount = fds[i].amount + interest;
            
            // Deduct principal and interest from admin
            accounts[0].balance -= totalAmount;
            addTransaction(&accounts[0], WITHDRAWAL, totalAmount);
            
            // Find account and credit amount with interest
            for(int j = 0; j < numAccounts; j++) {
                if(accounts[j].accountNo == fds[i].accountNo) {
                    accounts[j].balance += totalAmount;
                    addTransaction(&accounts[j], DEPOSIT, totalAmount);
                    break;
                }
            }
            
            // Remove FD record
            for(int j = i; j < numFDs - 1; j++) {
                fds[j] = fds[j + 1];
            }
            numFDs--;
            i--;
        }
    }
    
    saveData(accounts, numAccounts);
}