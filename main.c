#include "Bank.h"

int main() {
    Account accounts[MAX_USERS];
    int numAccounts = 0;
    int currentUser = -1;
    int choice;
    
    // Initialize global variables
    g_accounts = accounts;
    
    initializeSystem(accounts, &numAccounts);
    g_numAccounts = numAccounts;  // Update global count after initialization
    
    time_t lastCheck = time(NULL);
    
    while(1) {
        // Check maturity every second
        time_t currentTime = time(NULL);
        if(difftime(currentTime, lastCheck) >= 1) {
            checkAndProcessMaturity(accounts, numAccounts);
            lastCheck = currentTime;
        }
        
        if(currentUser == -1) {
            printf("\n=== BANK SIMULATOR ===\n");
            printf("1. Login\n");
            printf("2. Admin Login\n");
            printf("3. Exit\n");
            printf("Enter choice: ");
            scanf("%d", &choice);
            
            switch(choice) {
                case 1:
                    if(login(accounts, numAccounts, &currentUser)) {
                        printf("\nLogin successful!\n");
                    } else {
                        printf("\nInvalid credentials!\n");
                    }
                    break;
                case 2:
                    if(adminLogin()) {
                        adminMenu(accounts, &numAccounts);
                    } else {
                        printf("\nInvalid admin credentials!\n");
                    }
                    break;
                case 3:
                    printf("\nThank you for using ATM Simulator!\n");
                    return 0;
                default:
                    printf("\nInvalid choice!\n");
            }
        } else {
            printf("\n=== Welcome %s ===\n", accounts[currentUser].name);
            printf("1. Deposit\n");
            printf("2. Withdraw\n");
            printf("3. Check Balance\n");
            printf("4. Transaction History\n");
            printf("5. Transfer Money\n");
            printf("6. Delete Account\n");
            printf("7. Logout\n");
            printf("Enter choice: ");
            scanf("%d", &choice);
            
            switch(choice) {
                case 1:
                    performTransaction(&accounts[currentUser], DEPOSIT);
                    break;
                case 2:
                    performTransaction(&accounts[currentUser], WITHDRAWAL);
                    break;
                case 3:
                    performTransaction(&accounts[currentUser], BALANCE_INQUIRY);
                    break;
                case 4:
                    displayTransactionHistory(&accounts[currentUser]);
                    break;
                case 5:
                    transferMoney(accounts, numAccounts, currentUser);
                    break;
                case 6:
                    deleteAccount(accounts, &numAccounts, currentUser);
                    currentUser = -1;
                    break;
                case 7:
                    currentUser = -1;
                    printf("\nLogout successful!\n");
                    break;
                default:
                    printf("\nInvalid choice!\n");
            }
        }
    }
    
    return 0;
}