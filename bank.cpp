#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
using namespace std;

char crypt(char c) {
    return c ^ 'K';
}

string getTime() {
    time_t now = time(0);
    return ctime(&now);
}

class Account {
public:
    int accNo;
    string name;
    int pin;
    double balance;

    void display() {
        cout << "\nAcc No: " << accNo
             << "\nName: " << name
             << "\nBalance: ₹" << balance << endl;
    }
};

class Bank {
private:
    vector<Account> accounts;
    const string accFile = "accounts.txt";
    const string txnFile = "transactions.txt";

    void writeEncrypted(ofstream &out, string data) {
        for (char &c : data)
            out << crypt(c);
    }

    string readDecrypted(ifstream &in) {
        string data;
        char c;
        while (in.get(c))
            data += crypt(c);
        return data;
    }

public:
    Bank() {
        loadAccounts();
    }

    ~Bank() {
        saveAccounts();
    }

    void saveAccounts() {
        ofstream out(accFile);
        for (auto &a : accounts) {
            string line = to_string(a.accNo) + "|" + a.name + "|" +
                          to_string(a.pin) + "|" + to_string(a.balance) + "\n";
            writeEncrypted(out, line);
        }
        out.close();
    }

    void loadAccounts() {
        ifstream in(accFile);
        if (!in) return;

        string data = readDecrypted(in);
        in.close();

        size_t pos;
        while ((pos = data.find('\n')) != string::npos) {
            string line = data.substr(0, pos);
            data.erase(0, pos + 1);

            Account a;
            sscanf(line.c_str(), "%d|%[^|]|%d|%lf",
                   &a.accNo, &a.name[0], &a.pin, &a.balance);
            accounts.push_back(a);
        }
    }

    void logTransaction(string msg) {
        ofstream out(txnFile, ios::app);
        writeEncrypted(out, msg + "\n");
        out.close();
    }

    Account* authenticate(int accNo, int pin) {
        for (auto &a : accounts)
            if (a.accNo == accNo && a.pin == pin)
                return &a;
        return nullptr;
    }

    void createAccount() {
        Account a;
        cout << "Account No: "; cin >> a.accNo;
        cout << "Name: "; cin >> a.name;
        cout << "Set PIN: "; cin >> a.pin;
        cout << "Initial Balance: "; cin >> a.balance;
        accounts.push_back(a);
        cout << "Account created successfully.\n";
    }

    void userMenu(Account &a) {
        int ch;
        double amt;

        do {
            cout << "\n1.Deposit\n2.Withdraw\n3.View\n4.Exit\nChoice: ";
            cin >> ch;

            if (ch == 1) {
                cout << "Amount: "; cin >> amt;
                a.balance += amt;
                logTransaction("Deposit | Acc " + to_string(a.accNo) +
                               " | " + to_string(amt) + " | " + getTime());
            } 
            else if (ch == 2) {
                cout << "Amount: "; cin >> amt;
                if (amt <= a.balance) {
                    a.balance -= amt;
                    logTransaction("Withdraw | Acc " + to_string(a.accNo) +
                                   " | " + to_string(amt) + " | " + getTime());
                } else {
                    cout << "Insufficient balance\n";
                }
            }
            else if (ch == 3)
                a.display();

        } while (ch != 4);
    }

    // -------- ADMIN PANEL --------
    void adminPanel() {
        string pass;
        cout << "Admin Password: ";
        cin >> pass;

        if (pass != "admin123") {
            cout << "Access denied.\n";
            return;
        }

        int ch;
        do {
            cout << "\n--- ADMIN PANEL ---\n";
            cout << "1.View All Accounts\n2.Apply Interest\n3.Exit\n";
            cin >> ch;

            if (ch == 1) {
                for (auto &a : accounts)
                    a.display();
            } 
            else if (ch == 2) {
                for (auto &a : accounts)
                    a.balance += a.balance * 0.05;
                cout << "Interest applied successfully.\n";
            }
        } while (ch != 3);
    }

    void login() {
        int accNo, pin;
        cout << "Account No: "; cin >> accNo;
        cout << "PIN: "; cin >> pin;

        Account* a = authenticate(accNo, pin);
        if (!a) {
            cout << "Invalid credentials.\n";
            return;
        }

        userMenu(*a);
    }
};

int main() {
    Bank bank;
    int choice;

    do {
        cout << "\n==== BANK MANAGEMENT SYSTEM ====\n";
        cout << "1.Create Account\n2.Login\n3.Admin\n4.Exit\nChoice: ";
        cin >> choice;

        if (choice == 1) bank.createAccount();
        else if (choice == 2) bank.login();
        else if (choice == 3) bank.adminPanel();

    } while (choice != 4);

    return 0;
}
