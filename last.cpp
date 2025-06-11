#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
using namespace std;

struct User {
    string username;
    string password;
    double walletBalance;
    bool isAdmin;
};

unordered_map<string, User> userDatabase;

void initializeUsers() {
    userDatabase["normalUser1"] = {"normalUser1", "pass123", 100.0, false};
    userDatabase["normalUser2"] = {"normalUser2", "pass456", 150.0, false};
    userDatabase["normalUser3"] = {"normalUser3", "pass456", 350.0, false};
    userDatabase["adminUser1"] = {"adminUser1", "admin123", 200.0, true};
    userDatabase["adminUser2"] = {"adminUser2", "admin456", 250.0, true};
}

User authenticateUser(const string &username, const string &password) {
    if (username.empty() || password.empty()) {
        cout << "Username or password cannot be empty!" << endl;
        return {"", "", 0.0, false};
    }
    if (userDatabase.find(username) != userDatabase.end() && userDatabase[username].password == password) {
        return userDatabase[username];
    }
    return {"", "", 0.0, false};
}  

bool isInputValid(const string &input) {
    return !input.empty();
}

bool isNumeric(const string &str) {
    for (char const &c : str) {
        if (!isdigit(c) && c != '.' && c != '-') 
            return false;
    }
    return true;
}

void displayBalance(const User &user) {
    cout << "Current wallet balance: $" << user.walletBalance << endl;
}

void purchaseItem(User &user, double itemPrice) {
    if (user.walletBalance >= itemPrice) {
        user.walletBalance -= itemPrice;
        cout << "Purchase successful! New balance: $" << user.walletBalance << endl;
    } else {
        cout << "Insufficient funds for this purchase!" << endl;
    }
}

struct Item {
    string name;
    double price;
    int quantity;
};

unordered_map<string, Item> inventory;

void loadInventoryFromFile() {
    ifstream inFile("inventory.txt");
    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        string name;
        double price;
        int quantity;
        getline(ss, name, ',');
        ss >> price;
        ss.ignore();
        ss >> quantity;
        inventory[name] = {name, price, quantity};
    }
    inFile.close();
}

void saveInventoryToFile() {
    ofstream outFile("inventory.txt");
    for (const auto& pair : inventory) {
        Item item = pair.second;
        outFile << item.name << "," << item.price << "," << item.quantity << "\n";
    }
    outFile.close();
}

void showItem(const string &itemName) {
    if (inventory.find(itemName) != inventory.end()) {
        Item item = inventory[itemName];
        cout << "Item: " << item.name << ", Price: $" << item.price << ", Quantity: " << item.quantity << endl;
    } else {
        cout << "Item not found in inventory!" << endl;
    }
}

void buyItem(User &user, const string &itemName) {
    if (inventory.find(itemName) != inventory.end()) {
        Item &item = inventory[itemName];
        if (item.quantity > 0) {
            purchaseItem(user, item.price);
            item.quantity--;
            saveInventoryToFile();  
        } else {
            cout << "Sold out!" << endl;
        }
    } else {
        cout << "Item not found!" << endl;
    }
}

void addItem(const string& itemName, double price, int quantity) {
    inventory[itemName] = {itemName, price, quantity};
    cout << "Item added: " << itemName << ", Price: $" << price << ", Quantity: " << quantity << endl;
    saveInventoryToFile(); 
}

void removeItem(const string& itemName) {
    if (inventory.find(itemName) != inventory.end()) {
        inventory.erase(itemName); 
        cout << "Item removed: " << itemName << endl;
        saveInventoryToFile();  
    } else {
        cout << "Item not found!" << endl;
    }
}

void renameItem(const string& oldName, const string& newName) {
    if (inventory.find(oldName) != inventory.end()) {
        Item item = inventory[oldName];
        item.name = newName;
        inventory.erase(oldName);
        inventory[newName] = item;
        cout << "Item renamed from " << oldName << " to " << newName << endl;
        saveInventoryToFile();  
    } else {
        cout << "Item to rename not found!" << endl;
    }
}

void changePrice(const string& itemName, double newPrice) {
    if (newPrice <= 0) {
        cout << "Price must be greater than zero!" << endl;
        return;
    }
    if (inventory.find(itemName) != inventory.end()) {
        inventory[itemName].price = newPrice; 
        cout << "Price for " << itemName << " set to $" << newPrice << endl;
        saveInventoryToFile();  
    } else {
        cout << "Item not found!" << endl;
    }
}

int main() {
    initializeUsers();
    loadInventoryFromFile(); 

    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    User currentUser = authenticateUser(username, password);
    if (currentUser.username == "") {
        cout << "Authentication failed!" << endl;
        return 0;
    }

    cout << "Welcome, " << currentUser.username << "!" << endl;
    string command;

    while (true) {
        cout << "\nEnter command: ";
        cin >> command;
        if (command == "exit") {
            break;
        }
        else if (command == "show") {
            string itemName;
            cin >> itemName;
            showItem(itemName);
        }
        else if (command == "buy") {
            string itemName;
            cin >> itemName;
            buyItem(currentUser, itemName);
        }
        else if (command == "balance") {
            displayBalance(currentUser);
        }
        else if (command == "add" && currentUser.isAdmin) {
            string itemName;
            double price;
            int quantity;
            cin >> itemName >> price >> quantity;
            if (isNumeric(to_string(price)) && price > 0 && quantity > 0) {
                addItem(itemName, price, quantity);
            } else {
                cout << "Invalid price or quantity!" << endl;
                break;
            }
        }
        else if (command == "remove" && currentUser.isAdmin) {
            string itemName;
            cin >> itemName;
            removeItem(itemName);
        }
        else if (command == "rename" && currentUser.isAdmin) {
            string oldName, newName;
            cin >> oldName >> newName;
            renameItem(oldName, newName);
        }
        else if (command == "price" && currentUser.isAdmin) {
            string itemName;
            double newPrice;
            cin >> itemName >> newPrice;
            if (isNumeric(to_string(newPrice)) && newPrice > 0) {
                changePrice(itemName, newPrice);
            } else {
                cout << "Invalid price!" << endl;
            }
        }
        else if (command == "help") {
            cout << "Available commands:\n";
            cout << "show <item>\n";
            cout << "buy <item>\n";
            cout << "balance\n";
            if (currentUser.isAdmin) {
                cout << "add <item> <price> <quantity>\n";
                cout << "remove <item>\n";
                cout << "rename <oldname> <newname>\n";
                cout << "price <item> <newprice>\n";
            }
            cout << "exit\n";
        }
        else {
            cout << "Unknown command!" << endl;
        }
    }

    return 0;
}
