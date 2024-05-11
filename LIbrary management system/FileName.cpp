#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

class Book {
public:
    string title;
    string author;
    string category;

    Book(const string& t, const string& a, const string& c)
        : title(t), author(a), category(c) {}
};

class User {
public:
    string username;
    string name;
    double fineAmount;

    User(const string& u, const string& n, double f)
        : username(u), name(n), fineAmount(f) {}
};

class Librarian : public User {
public:
    string encryptedPassword;

    Librarian(const string& u, const string& p, const string& n)
        : User(u, n, 0.0), encryptedPassword(encryptPassword(p)) {}

private:
    string encryptPassword(const string& password) {
        const char key = 0x1F;
        string encrypted = password;
        for (char& c : encrypted) {
            c ^= key;
        }
        return encrypted;
    }
};

class Transaction {
public:
    string librarianUsername;
    string username;
    double fineAmount;

    Transaction(const string& l, const string& u, double f)
        : librarianUsername(l), username(u), fineAmount(f) {}
};

class FileHandler {
public:
    void saveToFile(const vector<Book>& books, const vector<User>& users,
        const vector<Librarian>& librarians, const vector<Transaction>& transactions) {
        ofstream file("lms.txt");

        if (file.is_open()) {
            saveItems(file, books);
            saveItems(file, users);
            saveItems(file, librarians);
            saveItems(file, transactions);
            file.close();
        }
        else {
            cout << "Error opening file for writing.\n";
        }
    }

    void loadFromFile(vector<Book>& books, vector<User>& users,
        vector<Librarian>& librarians, vector<Transaction>& transactions) {
        ifstream file("lms.txt");

        if (!file.is_open()) {
            cout << "File not found. Starting with empty records.\n";
            return;
        }

        string type;
        while (file >> type) {
            if (type == "Book") {
                string title, author, category;
                file >> title >> author >> category;
                books.push_back(Book(title, author, category));
            }
            else if (type == "User") {
                string username, name;
                double fineAmount;
                file >> username >> name >> fineAmount;
                users.push_back(User(username, name, fineAmount));
            }
            else if (type == "Librarian") {
                string username, password, name;
                file >> username >> password >> name;
                librarians.push_back(Librarian(username, password, name));
            }
            else if (type == "Transaction") {
                string librarianUsername, username;
                double fineAmount;
                file >> librarianUsername >> username >> fineAmount;
                transactions.push_back(Transaction(librarianUsername, username, fineAmount));
            }
        }

        file.close();
    }

private:
    template <typename T>
    void saveItems(ofstream& file, const vector<T>& items) {
        for (const auto& item : items) {
            file << itemToFileString(item) << "\n";
        }
    }

    template <typename T>
    string itemToFileString(const T& item) {
        string result;
        if constexpr (is_same_v<T, Book>) {
            result = "Book ";
            result += item.title + " " + item.author + " " + item.category;
        }
        else if constexpr (is_same_v<T, User>) {
            result = "User ";
            result += item.username + " " + item.name + " " + to_string(item.fineAmount);
        }
        else if constexpr (is_same_v<T, Librarian>) {
            result = "Librarian ";
            result += item.username + " " + item.encryptedPassword + " " + item.name;
        }
        else if constexpr (is_same_v<T, Transaction>) {
            result = "Transaction ";
            result += item.librarianUsername + " " + item.username + " " + to_string(item.fineAmount);
        }
        return result;
    }
};

class LibraryManagementSystem {
private:
    vector<Book> books;
    vector<User> users;
    vector<Librarian> librarians;
    vector<Transaction> transactions;
    FileHandler fileHandler;

public:
    LibraryManagementSystem() {
        loadFromFile();
    }

    ~LibraryManagementSystem() {
        saveToFile();
    }

    void loadFromFile() {
        fileHandler.loadFromFile(books, users, librarians, transactions);
    }

    void saveToFile() {
        fileHandler.saveToFile(books, users, librarians, transactions);
    }

    void addBook(const Book& book) {
        books.push_back(book);
    }

    void addUser(const User& user) {
        users.push_back(user);
    }

    void addLibrarian(const Librarian& librarian) {
        librarians.push_back(librarian);
    }

    void addTransaction(const Transaction& transaction) {
        transactions.push_back(transaction);
    }

    void displayBooks() {
        cout << "Books in the library:\n";
        for (const auto& book : books) {
            cout << "Title: " << book.title << ", Author: " << book.author << ", Category: " << book.category << endl;
        }
    }

    void displayUsers() {
        cout << "Library users:\n";
        for (const auto& user : users) {
            cout << "Username: " << user.username << ", Name: " << user.name
                << ", Fine Amount: $" << user.fineAmount << endl;
        }
    }

    void displayLibrarianLoginPortal() {
        cout << "==== Librarian Login Portal ====\n";
        string username, password;
        cout << "Enter librarian username: ";
        cin >> username;
        cout << "Enter password: ";
        cin >> password;

        if (loginLibrarian(username, password)) {
            cout << "Login successful! Welcome, " << username << ".\n";
            displayLibrarianActions(username);
        }
        else {
            cout << "Invalid login credentials for librarian.\n";
        }
    }

    bool loginLibrarian(const string& username, const string& password) {
        auto librarianIt = find_if(librarians.begin(), librarians.end(),
            [username](const Librarian& librarian) { return librarian.username == username; });

        if (librarianIt != librarians.end()) {
            const char key = 0x1F;
            string enteredPassword = password;

            // Decrypt the stored encrypted password
            string storedPassword = librarianIt->encryptedPassword;
            for (char& c : storedPassword) {
                c ^= key;
            }

            // Encrypt the entered password for comparison
            for (char& c : enteredPassword) {
                c ^= key;
            }

            return enteredPassword == storedPassword;
        }

        return false;
    }

    void displayTransactions() {
        cout << "Transaction history:\n";
        for (const auto& transaction : transactions) {
            cout << "Librarian: " << transaction.librarianUsername
                << ", Username: " << transaction.username
                << ", Fine Amount: Rs." << transaction.fineAmount << endl;
        }
    }

    int displaySubMenu() {
        int subChoice;
        cout << "\n1. Go back\n";
        cout << "2. Exit\n";
        cout << "Enter your choice: ";
        cin >> subChoice;
        return subChoice;
    }

    int mainMenu() {
        int choice;
        do {
            cout << "\n==== Library Management System ====\n";
            cout << "1. Display Books\n";
            cout << "2. Display Users\n";
            cout << "3. Librarian Login Portal\n";
            cout << "4. Add User\n";
            cout << "5. Add Librarian\n";
            cout << "6. Add Book\n";
            cout << "7. Exit\n";
            cout << "Enter your choice: ";
            cin >> choice;

            switch (choice) {
            case 1:
                displayBooks();
                break;
            case 2:
                displayUsers();
                break;
            case 3:
                displayLibrarianLoginPortal();
                break;
            case 4:
                addUserToSystem();
                break;
            case 5:
                addLibrarianToSystem();
                break;
            case 6:
                addBookToSystem();
                break;
            case 7:
                cout << "Exiting the Library Management System. Goodbye!\n";
                break;
            default:
                cout << "Invalid choice. Please enter a valid option.\n";
            }

            if (choice != 7) {
                int subChoice = displaySubMenu();
                if (subChoice == 2) {
                    cout << "Exiting the Library Management System. Goodbye!\n";
                    return 0;
                }
            }

        } while (choice != 7);

        return 0;
    }

    void addUserToSystem() {
        string username, name;
        cout << "Enter new user's username: ";
        cin >> username;
        cout << "Enter user's name: ";
        cin.ignore(); // Ignore newline character left in the buffer
        getline(cin, name);

        User newUser(username, name, 0.0);
        addUser(newUser);
        cout << "User added successfully!\n";
    }

    void addLibrarianToSystem() {
        string username, password, name;
        cout << "Enter new librarian's username: ";
        cin >> username;
        cout << "Enter password: ";
        cin >> password;
        cout << "Enter librarian's name: ";
        cin.ignore();
        getline(cin, name);

        Librarian newLibrarian(username, password, name);
        addLibrarian(newLibrarian);
        cout << "Librarian added successfully!\n";
    }

    void addBookToSystem() {
        string title, author, category;
        cout << "Enter new book's title: ";
        cin.ignore();
        getline(cin, title);
        cout << "Enter book's author: ";
        getline(cin, author);
        cout << "Enter book's category: ";
        getline(cin, category);

        Book newBook(title, author, category);
        addBook(newBook);
        cout << "Book added successfully!\n";
    }

    void performTransaction(const string& librarianUsername) {
        cout << "\n==== Transaction Menu ====\n";
        cout << "1. Go back\n";
        cout << "2. Exit\n";
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        switch (choice) {
        case 1:
            cout << "Going back to the main menu.\n";
            break;
        case 2:
            cout << "Exiting the Library Management System. Goodbye!\n";
            exit(0);
        default:
            cout << "Invalid choice. Going back to the main menu.\n";
        }
    }

    void displayLibrarianActions(const string& librarianUsername) {
        int choice;
        do {
            cout << "\n==== Librarian Actions ====\n";
            cout << "1. Give Fine to User\n";
            cout << "2. View Transactions\n";
            cout << "3. Go back\n";
            cout << "4. Exit\n";
            cout << "Enter your choice: ";
            cin >> choice;

            switch (choice) {
            case 1:
                giveFineToUser(librarianUsername);
                break;
            case 2:
                displayTransactions();
                break;
            case 3:
                cout << "Going back to the main menu.\n";
                break;
            case 4:
                cout << "Exiting the Library Management System. Goodbye!\n";
                exit(0);
            default:
                cout << "Invalid choice. Please enter a valid option.\n";
            }

            if (choice != 3) {
                int subChoice = displaySubMenu();
                if (subChoice == 2) {
                    cout << "Exiting the Library Management System. Goodbye!\n";
                    exit(0);
                }
            }

        } while (choice != 3);
    }

    void giveFineToUser(const string& librarianUsername) {
        string username;
        double fineAmount;

        cout << "Enter username of the user to give a fine: ";
        cin >> username;

        auto userIt = find_if(users.begin(), users.end(),
            [username](const User& user) { return user.username == username; });

        if (userIt != users.end()) {
            cout << "Enter the fine amount: Rs.";
            cin >> fineAmount;

            Transaction transaction(librarianUsername, username, fineAmount);
            transactions.push_back(transaction);

            userIt->fineAmount += fineAmount;

            cout << "Fine successfully given to user " << username << ".\n";
        }
        else {
            cout << "User with username " << username << " not found.\n";
        }
    }
};

int main() {
    LibraryManagementSystem librarySystem;

    librarySystem.mainMenu();

    return 0;
}