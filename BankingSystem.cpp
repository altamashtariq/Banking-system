#include <iostream>
#include <string>
#include <vector>
using namespace std;

// ══════════════════════════════════════════════
//  CLASS 1 : Transaction
//  Stores a single transaction record
// ══════════════════════════════════════════════
class Transaction {
public:
    string type;
    double amount;
    double balanceAfter;

    Transaction(string t, double amt, double bal) {
        type         = t;
        amount       = amt;
        balanceAfter = bal;
    }

    void display() {
        cout << "  | " << type;
        // pad spacing
        for (int i = type.length(); i < 20; i++) cout << " ";
        cout << "| Rs." << amount;
        for (int i = to_string((int)amount).length(); i < 12; i++) cout << " ";
        cout << "| Balance: Rs." << balanceAfter << "\n";
    }
};

// ══════════════════════════════════════════════
//  CLASS 2 : Account  (Base Class)
//  Holds account data and basic operations
// ══════════════════════════════════════════════
class Account {
protected:
    int    accountNo;
    string ownerName;
    string pin;
    double balance;
    vector<Transaction> history;   // list of all transactions

    static int nextAccountNo;      // shared counter for all accounts

public:
    // Constructor
    Account(string name, string userPin, double openingBalance) {
        accountNo = nextAccountNo;
        nextAccountNo++;
        ownerName = name;
        pin       = userPin;
        balance   = openingBalance;
        history.push_back(Transaction("Account Opened", openingBalance, balance));
    }

    // Destructor
    virtual ~Account() {}

    // ── Getters ───────────────────────────────
    int    getAccountNo() { return accountNo; }
    string getName()      { return ownerName; }
    double getBalance()   { return balance; }

    bool checkPin(string p) { return pin == p; }

    // ── Deposit ───────────────────────────────
    void deposit(double amount) {
        if (amount <= 0) {
            cout << "\n  [!] Deposit amount must be greater than 0.\n";
            return;
        }
        balance += amount;
        history.push_back(Transaction("Deposit", amount, balance));
        cout << "\n  [✔] Rs." << amount << " deposited."
             << "  New balance: Rs." << balance << "\n";
    }

    // ── Withdraw (virtual so child classes can override) ──
    virtual void withdraw(double amount) {
        if (amount <= 0) {
            cout << "\n  [!] Amount must be greater than 0.\n";
            return;
        }
        if (amount > balance) {
            cout << "\n  [!] Not enough balance. Current balance: Rs." << balance << "\n";
            return;
        }
        balance -= amount;
        history.push_back(Transaction("Withdrawal", amount, balance));
        cout << "\n  [✔] Rs." << amount << " withdrawn."
             << "  New balance: Rs." << balance << "\n";
    }

    // ── Change PIN ────────────────────────────
    void changePin(string oldPin, string newPin) {
        if (!checkPin(oldPin)) {
            cout << "\n  [!] Wrong current PIN.\n";
            return;
        }
        if (newPin.length() < 4) {
            cout << "\n  [!] New PIN must be at least 4 digits.\n";
            return;
        }
        pin = newPin;
        cout << "\n  [✔] PIN changed successfully.\n";
    }

    // ── Mini Statement ────────────────────────
    void printStatement() {
        cout << "\n  ╔══════════════════════════════════════════╗\n";
        cout << "  ║           ACCOUNT STATEMENT              ║\n";
        cout << "  ╚══════════════════════════════════════════╝\n";
        cout << "  Account No  : " << accountNo   << "\n";
        cout << "  Name        : " << ownerName   << "\n";
        cout << "  Type        : " << getType()   << "\n";
        cout << "  Balance     : Rs." << balance  << "\n";
        cout << "  ──────────────────────────────────────────\n";
        cout << "  | Type                | Amount      | Running Balance\n";
        cout << "  ──────────────────────────────────────────\n";
        for (int i = 0; i < history.size(); i++) {
            history[i].display();
        }
        cout << "  ──────────────────────────────────────────\n";
    }

    // ── Pure virtual: every child must say what type it is ──
    virtual string getType() = 0;
};

int Account::nextAccountNo = 1001;   // accounts start from 1001

// ══════════════════════════════════════════════
//  CLASS 3 : SavingsAccount  (Child of Account)
//  Normal savings with 4% interest
// ══════════════════════════════════════════════
class SavingsAccount : public Account {
public:
    SavingsAccount(string name, string pin, double balance)
        : Account(name, pin, balance) {}

    string getType() { return "Savings Account"; }

    void addInterest() {
        double interest = balance * 0.04;   // 4%
        balance += interest;
        history.push_back(Transaction("Interest (4%)", interest, balance));
        cout << "\n  [✔] Interest of Rs." << interest << " added.\n";
    }
};

// ══════════════════════════════════════════════
//  CLASS 4 : CurrentAccount  (Child of Account)
//  Business account with overdraft facility
// ══════════════════════════════════════════════
class CurrentAccount : public Account {
private:
    double overdraftLimit;   // can go below 0 up to this limit

public:
    CurrentAccount(string name, string pin, double balance, double overdraft = 5000)
        : Account(name, pin, balance) {
        overdraftLimit = overdraft;
    }

    string getType() { return "Current Account"; }

    // Override withdraw to allow overdraft
    void withdraw(double amount) {
        if (amount <= 0) {
            cout << "\n  [!] Amount must be greater than 0.\n";
            return;
        }
        if (amount > balance + overdraftLimit) {
            cout << "\n  [!] Exceeds overdraft limit. Max you can withdraw: Rs."
                 << (balance + overdraftLimit) << "\n";
            return;
        }
        balance -= amount;
        history.push_back(Transaction("Withdrawal", amount, balance));
        cout << "\n  [✔] Rs." << amount << " withdrawn."
             << "  New balance: Rs." << balance << "\n";
    }
};

// ══════════════════════════════════════════════
//  CLASS 5 : Bank
//  Manages all accounts — create, find, operate
// ══════════════════════════════════════════════
class Bank {
private:
    string bankName;
    vector<Account*> accounts;   // list of all accounts (any type)

    // Find account by account number, returns nullptr if not found
    Account* findAccount(int accNo) {
        for (int i = 0; i < accounts.size(); i++) {
            if (accounts[i]->getAccountNo() == accNo)
                return accounts[i];
        }
        return nullptr;
    }

    // Find account + verify PIN
    Account* getVerifiedAccount(int accNo, string pin) {
        Account* acc = findAccount(accNo);
        if (acc == nullptr) {
            cout << "\n  [!] Account " << accNo << " not found.\n";
            return nullptr;
        }
        if (!acc->checkPin(pin)) {
            cout << "\n  [!] Wrong PIN. Try again.\n";
            return nullptr;
        }
        return acc;
    }

public:
    Bank(string name) {
        bankName = name;
    }

    // Free memory when Bank is destroyed
    ~Bank() {
        for (int i = 0; i < accounts.size(); i++)
            delete accounts[i];
    }

    // ── Create Accounts ───────────────────────
    void createSavings(string name, string pin, double deposit) {
        SavingsAccount* acc = new SavingsAccount(name, pin, deposit);
        accounts.push_back(acc);
        cout << "\n  [✔] Savings Account created!"
             << "  Account No: " << acc->getAccountNo() << "\n";
    }

    void createCurrent(string name, string pin, double deposit) {
        CurrentAccount* acc = new CurrentAccount(name, pin, deposit);
        accounts.push_back(acc);
        cout << "\n  [✔] Current Account created!"
             << "  Account No: " << acc->getAccountNo() << "\n";
    }

    // ── Deposit ───────────────────────────────
    void deposit(int accNo, string pin, double amount) {
        Account* acc = getVerifiedAccount(accNo, pin);
        if (acc) acc->deposit(amount);
    }

    // ── Withdraw ──────────────────────────────
    void withdraw(int accNo, string pin, double amount) {
        Account* acc = getVerifiedAccount(accNo, pin);
        if (acc) acc->withdraw(amount);
    }

    // ── Transfer between two accounts ─────────
    void transfer(int fromAccNo, string pin, int toAccNo, double amount) {
        Account* from = getVerifiedAccount(fromAccNo, pin);
        if (!from) return;

        Account* to = findAccount(toAccNo);
        if (!to) {
            cout << "\n  [!] Destination account " << toAccNo << " not found.\n";
            return;
        }
        if (amount <= 0) {
            cout << "\n  [!] Transfer amount must be greater than 0.\n";
            return;
        }
        if (amount > from->getBalance()) {
            cout << "\n  [!] Not enough balance to transfer.\n";
            return;
        }

        // Manually move money
        from->withdraw(amount);
        to->deposit(amount);
        cout << "\n  [✔] Rs." << amount << " transferred to account "
             << toAccNo << " (" << to->getName() << ")\n";
    }

    // ── Check Balance ─────────────────────────
    void checkBalance(int accNo, string pin) {
        Account* acc = getVerifiedAccount(accNo, pin);
        if (acc) {
            cout << "\n  Account  : " << accNo << "\n";
            cout << "  Name     : " << acc->getName() << "\n";
            cout << "  Balance  : Rs." << acc->getBalance() << "\n";
        }
    }

    // ── Print Statement ───────────────────────
    void printStatement(int accNo, string pin) {
        Account* acc = getVerifiedAccount(accNo, pin);
        if (acc) acc->printStatement();
    }

    // ── Change PIN ────────────────────────────
    void changePin(int accNo, string oldPin, string newPin) {
        Account* acc = findAccount(accNo);
        if (!acc) { cout << "\n  [!] Account not found.\n"; return; }
        acc->changePin(oldPin, newPin);
    }

    // ── Add Interest to all Savings Accounts ──
    void applyInterestAll() {
        int count = 0;
        for (int i = 0; i < accounts.size(); i++) {
            // Dynamic cast checks if account is a SavingsAccount
            SavingsAccount* sa = dynamic_cast<SavingsAccount*>(accounts[i]);
            if (sa != nullptr) {
                sa->addInterest();
                count++;
            }
        }
        cout << "\n  [✔] Interest applied to " << count << " savings account(s).\n";
    }

    // ── Show All Accounts ─────────────────────
    void showAllAccounts() {
        if (accounts.empty()) {
            cout << "\n  No accounts found.\n";
            return;
        }
        cout << "\n  ╔══════════════════════════════════════════════════╗\n";
        cout << "  ║                 ALL ACCOUNTS                     ║\n";
        cout << "  ╚══════════════════════════════════════════════════╝\n";
        cout << "  No.   Acc No    Name                 Type             Balance\n";
        cout << "  ─────────────────────────────────────────────────────────────\n";
        for (int i = 0; i < accounts.size(); i++) {
            Account* a = accounts[i];
            cout << "  " << (i + 1) << ".    "
                 << a->getAccountNo() << "    ";
            // pad name
            cout << a->getName();
            for (int j = a->getName().length(); j < 21; j++) cout << " ";
            cout << a->getType();
            for (int j = a->getType().length(); j < 17; j++) cout << " ";
            cout << "Rs." << a->getBalance() << "\n";
        }
        cout << "  ─────────────────────────────────────────────────────────────\n";
        cout << "  Total accounts: " << accounts.size() << "\n";
    }

    string getName() { return bankName; }
};

// ══════════════════════════════════════════════
//  MENU + HELPER FUNCTIONS
// ══════════════════════════════════════════════
void showMenu() {
    cout << "\n  ┌───────────────────────────────────┐\n";
    cout << "  │           MAIN MENU               │\n";
    cout << "  ├───────────────────────────────────┤\n";
    cout << "  │  1.  Open Savings Account         │\n";
    cout << "  │  2.  Open Current Account         │\n";
    cout << "  │  3.  Deposit Money                │\n";
    cout << "  │  4.  Withdraw Money               │\n";
    cout << "  │  5.  Transfer Money               │\n";
    cout << "  │  6.  Check Balance                │\n";
    cout << "  │  7.  Account Statement            │\n";
    cout << "  │  8.  Change PIN                   │\n";
    cout << "  │  9.  Apply Interest (Savings)     │\n";
    cout << "  │  10. Show All Accounts            │\n";
    cout << "  │  0.  Exit                         │\n";
    cout << "  └───────────────────────────────────┘\n";
    cout << "  Your choice: ";
}

void pressEnter() {
    cout << "\n  Press Enter to go back to menu...";
    cin.ignore();
    cin.get();
}

// ══════════════════════════════════════════════
//  MAIN
// ══════════════════════════════════════════════
int main() {
    Bank bank("MyBank");

    // --- Sample accounts so you can test right away ---
    bank.createSavings("Rahul Sharma",  "1111", 10000);
    bank.createSavings("Priya Singh",   "2222", 5000);
    bank.createCurrent("Rohan Traders", "3333", 20000);
    // --------------------------------------------------

    int choice;

    cout << "\n  ╔══════════════════════════════════════════╗\n";
    cout << "  ║     Welcome to MyBank Banking System     ║\n";
    cout << "  ╚══════════════════════════════════════════╝\n";

    do {
        showMenu();
        cin >> choice;

        if (choice == 1) {
            // ── Open Savings Account ──────────────
            string name, pin;
            double deposit;
            cout << "\n  --- Open Savings Account ---\n";
            cout << "  Name            : "; cin >> name;
            cout << "  Set PIN (4 dig) : "; cin >> pin;
            cout << "  Opening Deposit : Rs."; cin >> deposit;
            bank.createSavings(name, pin, deposit);
        }
        else if (choice == 2) {
            // ── Open Current Account ──────────────
            string name, pin;
            double deposit;
            cout << "\n  --- Open Current Account ---\n";
            cout << "  Name            : "; cin >> name;
            cout << "  Set PIN (4 dig) : "; cin >> pin;
            cout << "  Opening Deposit : Rs."; cin >> deposit;
            bank.createCurrent(name, pin, deposit);
        }
        else if (choice == 3) {
            // ── Deposit ───────────────────────────
            int accNo; string pin; double amount;
            cout << "\n  --- Deposit ---\n";
            cout << "  Account No : "; cin >> accNo;
            cout << "  PIN        : "; cin >> pin;
            cout << "  Amount     : Rs."; cin >> amount;
            bank.deposit(accNo, pin, amount);
        }
        else if (choice == 4) {
            // ── Withdraw ──────────────────────────
            int accNo; string pin; double amount;
            cout << "\n  --- Withdraw ---\n";
            cout << "  Account No : "; cin >> accNo;
            cout << "  PIN        : "; cin >> pin;
            cout << "  Amount     : Rs."; cin >> amount;
            bank.withdraw(accNo, pin, amount);
        }
        else if (choice == 5) {
            // ── Transfer ──────────────────────────
            int from, to; string pin; double amount;
            cout << "\n  --- Transfer Money ---\n";
            cout << "  From Account No : "; cin >> from;
            cout << "  PIN             : "; cin >> pin;
            cout << "  To Account No   : "; cin >> to;
            cout << "  Amount          : Rs."; cin >> amount;
            bank.transfer(from, pin, to, amount);
        }
        else if (choice == 6) {
            // ── Check Balance ─────────────────────
            int accNo; string pin;
            cout << "\n  --- Check Balance ---\n";
            cout << "  Account No : "; cin >> accNo;
            cout << "  PIN        : "; cin >> pin;
            bank.checkBalance(accNo, pin);
        }
        else if (choice == 7) {
            // ── Statement ─────────────────────────
            int accNo; string pin;
            cout << "\n  --- Account Statement ---\n";
            cout << "  Account No : "; cin >> accNo;
            cout << "  PIN        : "; cin >> pin;
            bank.printStatement(accNo, pin);
        }
        else if (choice == 8) {
            // ── Change PIN ────────────────────────
            int accNo; string oldPin, newPin;
            cout << "\n  --- Change PIN ---\n";
            cout << "  Account No  : "; cin >> accNo;
            cout << "  Current PIN : "; cin >> oldPin;
            cout << "  New PIN     : "; cin >> newPin;
            bank.changePin(accNo, oldPin, newPin);
        }
        else if (choice == 9) {
            // ── Apply Interest ────────────────────
            cout << "\n  --- Applying interest to all Savings accounts ---\n";
            bank.applyInterestAll();
        }
        else if (choice == 10) {
            // ── Show All ──────────────────────────
            bank.showAllAccounts();
        }
        else if (choice == 0) {
            cout << "\n  Thank you for using MyBank. Goodbye!\n\n";
        }
        else {
            cout << "\n  [!] Invalid choice. Please enter 0-10.\n";
        }

        if (choice != 0) pressEnter();

    } while (choice != 0);

    return 0;
}