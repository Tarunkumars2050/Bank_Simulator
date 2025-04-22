// Change the header guard
#ifndef BANK_H
#define BANK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_USERS 100
#define PIN_LENGTH 10
#define MAX_TRANSACTIONS 10
#define MAX_LOANS 100
#define MAX_FDS 100
#define LOAN_INTEREST_RATE 0.02
#define FD_INTEREST_RATE 0.01
#define MAX_PERIOD 300  // 5 minutes in seconds
#define MAX_LOAN_AMOUNT 100.0

// Transaction types
typedef enum {
    DEPOSIT,
    WITHDRAWAL,
    BALANCE_INQUIRY
} TransactionType;

// Structure for a single transaction
typedef struct {
    TransactionType type;
    float amount;
    float balance;
    time_t timestamp;
} Transaction;

// Structure for transaction history (circular queue)
typedef struct {
    Transaction transactions[MAX_TRANSACTIONS];
    int front;
    int rear;
    int count;
} TransactionHistory;

// Structure for user account
typedef struct {
    int accountNo;
    char name[50];
    char pin[PIN_LENGTH];
    float balance;
    TransactionHistory history;
} Account;

// Function prototypes
void initializeSystem(Account accounts[], int *numAccounts);
int login(Account accounts[], int numAccounts, int *currentUser);
void performTransaction(Account *account, TransactionType type);
void displayTransactionHistory(Account *account);
void addTransaction(Account *account, TransactionType type, float amount);
void createAccount(Account accounts[], int *numAccounts);
// Add these before #endif
void deleteAccount(Account accounts[], int *numAccounts, int accountIndex);
void adminDeleteAccount(Account accounts[], int *numAccounts);
void adminBankOperations(Account accounts[], int numAccounts);
void transferMoney(Account accounts[], int numAccounts, int senderIndex);

extern Account *g_accounts;  // Global accounts array
extern int g_numAccounts;   // Global number of accounts

void encryptDecryptPin(char *pin);
void saveData(Account accounts[], int numAccounts);
void loadData(Account accounts[], int *numAccounts);
void displayAllUsers(Account accounts[], int numAccounts);
int adminLogin(void);
void adminMenu(Account accounts[], int *numAccounts);

// Add these before the existing structures
typedef struct {
    int accountNo;
    float amount;
    time_t startTime;
    float interestRate;
} LoanRecord;

typedef struct {
    int accountNo;
    float amount;
    time_t startTime;
    float interestRate;
} FDRecord;

// Add these to the existing declarations
void processLoan(Account accounts[], int numAccounts);
void processFD(Account accounts[], int numAccounts);
void checkAndProcessMaturity(Account accounts[], int numAccounts);
float calculateCompoundInterest(float principal, float rate, time_t startTime);
#endif