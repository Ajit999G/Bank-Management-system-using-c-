#include <iostream>
#include <fstream>
using namespace std;

char crypt(char c) {
    return c ^ 'K';   
}

int main() {
    ifstream in("accounts.txt");

    if (!in) {
        cout << "accounts.txt not found\n";
        return 1;
    }

    char c;
    cout << "---- Decrypted Account Data ----\n\n";
    while (in.get(c)) {
        cout << crypt(c);
    }

    in.close();
    return 0;
}
