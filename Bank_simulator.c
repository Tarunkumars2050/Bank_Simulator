#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "accounts.txt"
#define LAST_ACC_FILE "last_acc_no.txt"

typedef struct {
    int acc_no;
    char name[100];
    int age;
    char address[100];
    char acc_type[20];
    float balance;
} Account;

void create_account();
void view_account();
void update_account();
void delete_account();
void save_account(Account acc);
void list_all_accounts();
int generate_unique_account_no();

int main() {
    int choice;

    while (1) {
        printf("\n--- Bank Management System ---\n");
        printf("1. Create Account\n");
        printf("2. View Account\n");
        printf("3. Update Account\n");
        printf("4. Delete Account\n");
        printf("5. List All Accounts\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // consume newline

        switch (choice) {
            case 1: create_account(); break;
            case 2: view_account(); break;
            case 3: update_account(); break;
            case 4: delete_account(); break;
            case 5: list_all_accounts(); break;
            case 6: printf("Exiting...\n"); exit(0);
            default: printf("Invalid choice!\n");
        }
    }
}

int generate_unique_account_no() {
    FILE *fp = fopen(LAST_ACC_FILE, "r");
    int last_acc_no = 1000000;
    if (fp != NULL) {
        fscanf(fp, "%d", &last_acc_no);
        fclose(fp);
    }
    last_acc_no++;
    fp = fopen(LAST_ACC_FILE, "w");
    if (fp != NULL) {
        fprintf(fp, "%d", last_acc_no);
        fclose(fp);
    }
    return last_acc_no;
}

void create_account() {
    Account acc;
    acc.acc_no = generate_unique_account_no();
    printf("\nGenerated Account Number: %d\n", acc.acc_no);
    printf("Enter Name: ");
    fgets(acc.name, sizeof(acc.name), stdin);
    acc.name[strcspn(acc.name, "\n")] = 0;
    printf("Enter Age: ");
    scanf("%d", &acc.age);
    getchar();
    printf("Enter Address: ");
    fgets(acc.address, sizeof(acc.address), stdin);
    acc.address[strcspn(acc.address, "\n")] = 0;
    printf("Enter Account Type (Saving/Current/etc.): ");
    fgets(acc.acc_type, sizeof(acc.acc_type), stdin);
    acc.acc_type[strcspn(acc.acc_type, "\n")] = 0;
    printf("Enter Initial Deposit: ");
    scanf("%f", &acc.balance);

    save_account(acc);
    printf("Account created successfully!\n");
}

void save_account(Account acc) {
    FILE *fp = fopen(FILE_NAME, "a");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }
    fprintf(fp, "%d,%s,%d,%s,%s,%.2f\n", acc.acc_no, acc.name, acc.age, acc.address, acc.acc_type, acc.balance);
    fclose(fp);
}

void view_account() {
    int search_acc_no, found = 0;
    Account acc;
    FILE *fp = fopen(FILE_NAME, "r");
    if (fp == NULL) {
        printf("No accounts found!\n");
        return;
    }

    printf("Enter Account Number to view: ");
    scanf("%d", &search_acc_no);

    while (fscanf(fp, "%d,%[^,],%d,%[^,],%[^,],%f\n",
                  &acc.acc_no, acc.name, &acc.age, acc.address, acc.acc_type, &acc.balance) != EOF) {
        if (acc.acc_no == search_acc_no) {
            printf("\n--- Account Details ---\n");
            printf("Account No: %d\n", acc.acc_no);
            printf("Name: %s\n", acc.name);
            printf("Age: %d\n", acc.age);
            printf("Address: %s\n", acc.address);
            printf("Account Type: %s\n", acc.acc_type);
            printf("Balance: %.2f\n", acc.balance);
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (!found)
        printf("Account not found!\n");
}

void update_account() {
    int search_acc_no, found = 0;
    Account acc, accounts[100];
    int count = 0;

    FILE *fp = fopen(FILE_NAME, "r");
    if (fp == NULL) {
        printf("No accounts found!\n");
        return;
    }

    printf("Enter Account Number to update: ");
    scanf("%d", &search_acc_no);

    while (fscanf(fp, "%d,%[^,],%d,%[^,],%[^,],%f\n",
                  &acc.acc_no, acc.name, &acc.age, acc.address, acc.acc_type, &acc.balance) != EOF) {
        accounts[count++] = acc;
    }
    fclose(fp);

    for (int i = 0; i < count; i++) {
        if (accounts[i].acc_no == search_acc_no) {
            printf("\nWhat do you want to update?\n");
            printf("1. Address\n2. Account Type\n3. Deposit\n4. Withdraw\n");
            int choice;
            scanf("%d", &choice);
            getchar();
            if (choice == 1) {
                printf("Enter new Address: ");
                fgets(accounts[i].address, sizeof(accounts[i].address), stdin);
                accounts[i].address[strcspn(accounts[i].address, "\n")] = 0;
            } else if (choice == 2) {
                printf("Enter new Account Type: ");
                fgets(accounts[i].acc_type, sizeof(accounts[i].acc_type), stdin);
                accounts[i].acc_type[strcspn(accounts[i].acc_type, "\n")] = 0;
            } else if (choice == 3) {
                float amount;
                printf("Enter amount to Deposit: ");
                scanf("%f", &amount);
                accounts[i].balance += amount;
            } else if (choice == 4) {
                float amount;
                printf("Enter amount to Withdraw: ");
                scanf("%f", &amount);
                if (amount > accounts[i].balance)
                    printf("Insufficient balance!\n");
                else
                    accounts[i].balance -= amount;
            } else {
                printf("Invalid choice!\n");
                return;
            }
            found = 1;
            break;
        }
    }

    if (found) {
        fp = fopen(FILE_NAME, "w");
        for (int i = 0; i < count; i++) {
            fprintf(fp, "%d,%s,%d,%s,%s,%.2f\n",
                    accounts[i].acc_no, accounts[i].name, accounts[i].age,
                    accounts[i].address, accounts[i].acc_type, accounts[i].balance);
        }
        fclose(fp);
        printf("Account updated successfully!\n");
    } else {
        printf("Account not found!\n");
    }
}

void delete_account() {
    int search_acc_no, found = 0;
    Account acc, accounts[100];
    int count = 0;

    FILE *fp = fopen(FILE_NAME, "r");
    if (fp == NULL) {
        printf("No accounts found!\n");
        return;
    }

    printf("Enter Account Number to delete: ");
    scanf("%d", &search_acc_no);

    while (fscanf(fp, "%d,%[^,],%d,%[^,],%[^,],%f\n",
                  &acc.acc_no, acc.name, &acc.age, acc.address, acc.acc_type, &acc.balance) != EOF) {
        accounts[count++] = acc;
    }
    fclose(fp);

    fp = fopen(FILE_NAME, "w");
    for (int i = 0; i < count; i++) {
        if (accounts[i].acc_no != search_acc_no) {
            fprintf(fp, "%d,%s,%d,%s,%s,%.2f\n",
                    accounts[i].acc_no, accounts[i].name, accounts[i].age,
                    accounts[i].address, accounts[i].acc_type, accounts[i].balance);
        } else {
            found = 1;
        }
    }
    fclose(fp);

    if (found)
        printf("Account deleted successfully!\n");
    else
        printf("Account not found!\n");
}

void list_all_accounts() {
    Account acc;
    FILE *fp = fopen(FILE_NAME, "r");
    if (fp == NULL) {
        printf("No accounts to display!\n");
        return;
    }

    printf("\n%-15s %-30s %-40s %-15s %-10s\n", "Account No.", "Name", "Address", "Account Type", "Balance");
    printf("----------------------------------------------------------------------------------------------------------------\n");

    while (fscanf(fp, "%d,%[^,],%d,%[^,],%[^,],%f\n",
                  &acc.acc_no, acc.name, &acc.age, acc.address, acc.acc_type, &acc.balance) != EOF) {
        printf("%-15d %-30s %-40s %-15s %-10.2f\n",
               acc.acc_no, acc.name, acc.address, acc.acc_type, acc.balance);
    }
    fclose(fp);
}







// The code given bellow if for same work but it will not create last_acc_no.txt. this code will take last account created from account.txt
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

// #define FILE_NAME "accounts.txt"

// typedef struct {
//     int acc_no;
//     char name[100];
//     int age;
//     char address[100];
//     char acc_type[20];
//     float balance;
// } Account;

// void create_account();
// void view_account();
// void update_account();
// void delete_account();
// void save_account(Account acc);
// void list_all_accounts();
// int generate_unique_account_no();

// int main() {
//     int choice;

//     while (1) {
//         printf("\n--- Bank Management System ---\n");
//         printf("1. Create Account\n");
//         printf("2. View Account\n");
//         printf("3. Update Account\n");
//         printf("4. Delete Account\n");
//         printf("5. List All Accounts\n");
//         printf("6. Exit\n");
//         printf("Enter your choice: ");
//         scanf("%d", &choice);
//         getchar(); // consume newline

//         switch (choice) {
//             case 1: create_account(); break;
//             case 2: view_account(); break;
//             case 3: update_account(); break;
//             case 4: delete_account(); break;
//             case 5: list_all_accounts(); break;
//             case 6: printf("Exiting...\n"); exit(0);
//             default: printf("Invalid choice!\n");
//         }
//     }
// }

// int generate_unique_account_no() {
//     FILE *fp = fopen(FILE_NAME, "r");
//     int last_acc_no = 1000000;
//     Account acc;

//     if (fp != NULL) {
//         while (fscanf(fp, "%d,%[^,],%d,%[^,],%[^,],%f\n",
//                       &acc.acc_no, acc.name, &acc.age, acc.address, acc.acc_type, &acc.balance) != EOF) {
//             if (acc.acc_no > last_acc_no) {
//                 last_acc_no = acc.acc_no;
//             }
//         }
//         fclose(fp);
//     }
//     return last_acc_no + 1;
// }

// void create_account() {
//     Account acc;
//     acc.acc_no = generate_unique_account_no();
//     printf("\nGenerated Account Number: %d\n", acc.acc_no);
//     printf("Enter Name: ");
//     fgets(acc.name, sizeof(acc.name), stdin);
//     acc.name[strcspn(acc.name, "\n")] = 0;
//     printf("Enter Age: ");
//     scanf("%d", &acc.age);
//     getchar();
//     printf("Enter Address: ");
//     fgets(acc.address, sizeof(acc.address), stdin);
//     acc.address[strcspn(acc.address, "\n")] = 0;
//     printf("Enter Account Type (Saving/Current/etc.): ");
//     fgets(acc.acc_type, sizeof(acc.acc_type), stdin);
//     acc.acc_type[strcspn(acc.acc_type, "\n")] = 0;
//     printf("Enter Initial Deposit: ");
//     scanf("%f", &acc.balance);

//     save_account(acc);
//     printf("Account created successfully!\n");
// }

// void save_account(Account acc) {
//     FILE *fp = fopen(FILE_NAME, "a");
//     if (fp == NULL) {
//         printf("Error opening file!\n");
//         return;
//     }
//     fprintf(fp, "%d,%s,%d,%s,%s,%.2f\n", acc.acc_no, acc.name, acc.age, acc.address, acc.acc_type, acc.balance);
//     fclose(fp);
// }

// void view_account() {
//     int search_acc_no, found = 0;
//     Account acc;
//     FILE *fp = fopen(FILE_NAME, "r");
//     if (fp == NULL) {
//         printf("No accounts found!\n");
//         return;
//     }

//     printf("Enter Account Number to view: ");
//     scanf("%d", &search_acc_no);

//     while (fscanf(fp, "%d,%[^,],%d,%[^,],%[^,],%f\n",
//                   &acc.acc_no, acc.name, &acc.age, acc.address, acc.acc_type, &acc.balance) != EOF) {
//         if (acc.acc_no == search_acc_no) {
//             printf("\n--- Account Details ---\n");
//             printf("Account No: %d\n", acc.acc_no);
//             printf("Name: %s\n", acc.name);
//             printf("Age: %d\n", acc.age);
//             printf("Address: %s\n", acc.address);
//             printf("Account Type: %s\n", acc.acc_type);
//             printf("Balance: %.2f\n", acc.balance);
//             found = 1;
//             break;
//         }
//     }
//     fclose(fp);

//     if (!found)
//         printf("Account not found!\n");
// }

// void update_account() {
//     int search_acc_no, found = 0;
//     Account acc, accounts[100];
//     int count = 0;

//     FILE *fp = fopen(FILE_NAME, "r");
//     if (fp == NULL) {
//         printf("No accounts found!\n");
//         return;
//     }

//     printf("Enter Account Number to update: ");
//     scanf("%d", &search_acc_no);

//     while (fscanf(fp, "%d,%[^,],%d,%[^,],%[^,],%f\n",
//                   &acc.acc_no, acc.name, &acc.age, acc.address, acc.acc_type, &acc.balance) != EOF) {
//         accounts[count++] = acc;
//     }
//     fclose(fp);

//     for (int i = 0; i < count; i++) {
//         if (accounts[i].acc_no == search_acc_no) {
//             printf("\nWhat do you want to update?\n");
//             printf("1. Address\n2. Account Type\n3. Deposit\n4. Withdraw\n");
//             int choice;
//             scanf("%d", &choice);
//             getchar();
//             if (choice == 1) {
//                 printf("Enter new Address: ");
//                 fgets(accounts[i].address, sizeof(accounts[i].address), stdin);
//                 accounts[i].address[strcspn(accounts[i].address, "\n")] = 0;
//             } else if (choice == 2) {
//                 printf("Enter new Account Type: ");
//                 fgets(accounts[i].acc_type, sizeof(accounts[i].acc_type), stdin);
//                 accounts[i].acc_type[strcspn(accounts[i].acc_type, "\n")] = 0;
//             } else if (choice == 3) {
//                 float amount;
//                 printf("Enter amount to Deposit: ");
//                 scanf("%f", &amount);
//                 accounts[i].balance += amount;
//             } else if (choice == 4) {
//                 float amount;
//                 printf("Enter amount to Withdraw: ");
//                 scanf("%f", &amount);
//                 if (amount > accounts[i].balance)
//                     printf("Insufficient balance!\n");
//                 else
//                     accounts[i].balance -= amount;
//             } else {
//                 printf("Invalid choice!\n");
//                 return;
//             }
//             found = 1;
//             break;
//         }
//     }

//     if (found) {
//         fp = fopen(FILE_NAME, "w");
//         for (int i = 0; i < count; i++) {
//             fprintf(fp, "%d,%s,%d,%s,%s,%.2f\n",
//                     accounts[i].acc_no, accounts[i].name, accounts[i].age,
//                     accounts[i].address, accounts[i].acc_type, accounts[i].balance);
//         }
//         fclose(fp);
//         printf("Account updated successfully!\n");
//     } else {
//         printf("Account not found!\n");
//     }
// }

// void delete_account() {
//     int search_acc_no, found = 0;
//     Account acc, accounts[100];
//     int count = 0;

//     FILE *fp = fopen(FILE_NAME, "r");
//     if (fp == NULL) {
//         printf("No accounts found!\n");
//         return;
//     }

//     printf("Enter Account Number to delete: ");
//     scanf("%d", &search_acc_no);

//     while (fscanf(fp, "%d,%[^,],%d,%[^,],%[^,],%f\n",
//                   &acc.acc_no, acc.name, &acc.age, acc.address, acc.acc_type, &acc.balance) != EOF) {
//         accounts[count++] = acc;
//     }
//     fclose(fp);

//     fp = fopen(FILE_NAME, "w");
//     for (int i = 0; i < count; i++) {
//         if (accounts[i].acc_no != search_acc_no) {
//             fprintf(fp, "%d,%s,%d,%s,%s,%.2f\n",
//                     accounts[i].acc_no, accounts[i].name, accounts[i].age,
//                     accounts[i].address, accounts[i].acc_type, accounts[i].balance);
//         } else {
//             found = 1;
//         }
//     }
//     fclose(fp);

//     if (found)
//         printf("Account deleted successfully!\n");
//     else
//         printf("Account not found!\n");
// }

// void list_all_accounts() {
//     Account acc;
//     FILE *fp = fopen(FILE_NAME, "r");
//     if (fp == NULL) {
//         printf("No accounts to display!\n");
//         return;
//     }

//     printf("\n%-15s %-30s %-40s %-15s %-10s\n", "Account No.", "Name", "Address", "Account Type", "Balance");
//     printf("----------------------------------------------------------------------------------------------------------------\n");

//     while (fscanf(fp, "%d,%[^,],%d,%[^,],%[^,],%f\n",
//                   &acc.acc_no, acc.name, &acc.age, acc.address, acc.acc_type, &acc.balance) != EOF) {
//         printf("%-15d %-30s %-40s %-15s %-10.2f\n",
//                acc.acc_no, acc.name, acc.address, acc.acc_type, acc.balance);
//     }
//     fclose(fp);
// }
