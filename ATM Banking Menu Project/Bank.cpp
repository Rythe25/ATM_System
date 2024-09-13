/*
User accounts are stored in a file. 
User need to login using account number+PIN code. 
User can check balance, deposit, withdraw, transfer, change PIN code, and exit. 
(user accounts and other information are stored in file).
*/

#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <fstream>

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;
using namespace std::chrono_literals;

const int MAX_USERS = 10;

struct User
{
    string name;
    int id;
    int pin;
    double balance;
};

User users[MAX_USERS];
int userCount = 0;

bool validatePIN(int inputPIN, int actualPIN)
{
    return inputPIN == actualPIN;
}

void saveUserData()
{
    ofstream file("userdata.txt"); // Writing files
    if (file.is_open()) // If the file is open sucessfully 
    {
        file << userCount << endl;
        for (int i = 0; i < userCount; i++)
        {
            file << users[i].name << endl;
            file << users[i].id << endl;
            file << users[i].pin << endl;
            file << users[i].balance << endl;
        }
        file.close();
    }
}

bool loadUserData()
{
    ifstream file("userdata.txt");// Rading files
    if (file.is_open())
    {
        file >> userCount;
        file.ignore();
        for (int i = 0; i < userCount; i++)
        {
            getline(file, users[i].name);
            file >> users[i].id;
            file >> users[i].pin;
            file >> users[i].balance;
            file.ignore();
        }
        file.close();
        return true;
    }
    return false;
}

User *findUserByID(int id) // Transfering
{
    for (int i = 0; i < userCount; i++)
    {
        if (users[i].id == id)
        {
            return &users[i]; // Returning the address of the user
        }
    }
    return nullptr;
}

bool isUniqueID(int id)
{
    return findUserByID(id) == nullptr;
}

bool isUniqueUsername(const string &username)
{
    for (int i = 0; i < userCount; i++)
    {
        if (users[i].name == username)
        {
            return false;
        }
    }
    return true;
}

void changePIN(User &user)// Reference
{
    int oldPIN, newPIN;
    cout << "\t\t\t\t\t\t|| Enter old security PIN: ";
    cin >> oldPIN;

    if (validatePIN(oldPIN, user.pin))
    {
        cout << "\t\t\t\t\t\t|| Enter new security PIN: ";
        cin >> newPIN;
        user.pin = newPIN;
        saveUserData();
        cout << "\033[33m" << "\t\t\t\t\t\t>> PIN changed successfully." << "\033[0m" << endl;
    }
    else
    {
        cout << "\033[31m" << "\t\t\t\t\t\t!! Incorrect old PIN." << "\033[0m" << endl;
    }
}

void transferBalance(User &user)
{
    int destID;
    double transferAmount;
    cout << "\t\t\t\t\t\t|| Enter destination account ID: ";
    cin >> destID;
    cout << "\t\t\t\t\t\t|| Enter transfering amount: ";
    cin >> transferAmount;

    User *destUser = findUserByID(destID);
    if (destUser && user.balance >= transferAmount)
    {
        user.balance -= transferAmount;// Main User
        destUser->balance += transferAmount; 
        /*Destination user balance + new tranfered balance*/
        saveUserData();
        cout << "\033[33m" << "\t\t\t\t\t\t>> Transferred " << transferAmount << " to account " << destID << "." << "\033[0m" << endl;
    }
    else
    {
        cout << "\033[31m" << "\t\t\t\t\t\t!! Transfer failed. Either the destination account was not found or you have insufficient balance." << "\033[0m" << endl;
    }
}

void withdrawBalance(User &user)
{
    double withdrawAmount;
    cout << "\t\t\t\t\t\t|| Enter an amount to withdraw: ";
    cin >> withdrawAmount;

    if (user.balance >= withdrawAmount)
    {
        user.balance -= withdrawAmount;
        saveUserData();
        cout << "\033[33m" << "\t\t\t\t\t\t>> Withdrew " << withdrawAmount << "." << "\033[0m" << endl;
    }
    else
    {
        cout << "\033[31m" << "\t\t\t\t\t\t!! Insufficient balance." << "\033[0m" << endl;
    }
}

void depositBalance(User &user)
{
    double depositAmount;
    cout << "\t\t\t\t\t\t|| Enter an amount to deposit: ";
    cin >> depositAmount;

    user.balance += depositAmount;
    saveUserData();
    cout << "\033[33m" << "\t\t\t\t\t\t>> Deposited " << depositAmount << "." << "\033[0m" << endl;
}

User *login()// (Function) Pointer to Structure
{
    char choice;
    string username;
    int userID, userPIN;

    do
    {
        cout << "\n";
        cout << "\t\t\t\t\t\t||=================================================" << endl;        
        cout << "\t\t\t\t\t\t|| Do you want to log in? (y/n): ";
        cin >> choice;

        if (choice != 'y' && choice != 'Y')
        {
            cout << "\033[33m" << "\t\t\t\t\t\t>> Returning to main menu." << "\033[0m" << endl;
            return nullptr;
        }

        cout << "\t\t\t\t\t\t|| Enter your username: ";
        cin.ignore();
        getline(cin, username);
        cout << "\t\t\t\t\t\t|| Enter your user ID: ";
        cin >> userID;
        cout << "\t\t\t\t\t\t|| Enter your security PIN: ";
        cin >> userPIN;

        for (int i = 0; i < userCount; i++)// Condition
        {
            if (users[i].name == username && users[i].id == userID && validatePIN(userPIN, users[i].pin))
            {
                cout << "\033[33m" << "\t\t\t\t\t\t>> Login successful." << "\033[0m" << endl;
                return &users[i];
            }
        }
        cout << "\033[31m" << "\t\t\t\t\t\t!! Login failed. Incorrect username, ID, or PIN." << "\033[0m" << endl;

    } while (true);

    return nullptr;
}

void signUp()
{
    char choice;

    do
    {
        cout << "\n";
        cout << "\t\t\t\t\t\t||=================================================" << endl;
        cout << "\t\t\t\t\t\t|| Do you want to sign up a new user? (y/n): ";
        cin >> choice;

        if (choice != 'y' && choice != 'Y')
        {
            cout << "\033[33m" << "\t\t\t\t\t\t>> Returning to main menu." << "\033[0m" << endl;
            return;
        }

        if (userCount >= MAX_USERS)
        {
            cout << "\033[31m" << "\t\t\t\t\t\t!! User limit reached." << endl
                 << "\t\t\t\t\t\t!! Cannot sign up new users." << "\033[0m" << endl;
            return;
        }

        User newUser;
        cout << "\t\t\t\t\t\t|| Enter your username: ";
        cin.ignore();
        getline(cin, newUser.name);

        while (!isUniqueUsername(newUser.name))
        {
            cout << "\033[31m" << "\t\t\t\t\t\t!! Username has been taken. " << "\033[0m" << endl
                 << "\t\t\t\t\t\t|| Enter a different username: ";
            getline(cin, newUser.name);
        }

        do
        {
            cout << "\t\t\t\t\t\t|| Create an ID (6 digits): ";
            cin >> newUser.id;

            if (newUser.id < 100000 || newUser.id > 999999)
            {
                cout << "\033[31m" << "\t\t\t\t\t\t!! ID must be 6 digits!" << "\033[0m" << endl;
            }
            else if (!isUniqueID(newUser.id))
            {
                cout << "\033[31m" << "\t\t\t\t\t\t!! ID has been taken. " << "\033[0m" << endl;
            }

        } while (newUser.id < 100000 || newUser.id >= 999999 || !isUniqueID(newUser.id));

        do
        {
            cout << "\t\t\t\t\t\t|| Create a security PIN (4 digits): ";
            cin >> newUser.pin;

            if (newUser.pin < 1000 || newUser.pin > 9999)
            {
                cout << "\033[31m" << "\t\t\t\t\t\t!! Security PIN must be 4 digits!" << "\033[0m" << endl;
            }
        } while (newUser.pin < 1000 || newUser.pin >= 9999);

        newUser.balance = 0;
        users[userCount++] = newUser;
        saveUserData();
        cout << "\033[33m" << "\t\t\t\t\t\t>> Sign up successful. Please log in." << "\033[0m" << endl;

        return;

    } while (choice == 'y' || choice == 'Y');
}

void displayLogo()
{
    cout << "\n";
    cout << "\033[93;44m" << "\t\t\t\t\t\t                                                                                                          " << "\033[0m" << endl;
    cout << "\033[93;44m" << "\t\t\t\t\t\t  "<< "\033[0m" << "\033[1;97m" <<"        ______   _________  ________  _______    ______        _       ____  _____  ___  ____         "<< "\033[0m" << "\033[34;44m" <<"  " << "\033[0m" << endl;
    cout << "\033[93;44m" << "\t\t\t\t\t\t  "<< "\033[0m" << "\033[1;97m" <<"      .' ____ \\ |  _   _  ||_   __  ||_   __ \\  |_   _ \\      / \\     |_   \\|_   _||_  ||_  _|        "<< "\033[0m" << "\033[34;44m" <<"  " << "\033[0m" << endl;
    cout << "\033[93;44m" << "\t\t\t\t\t\t  "<< "\033[0m" << "\033[1;97m" <<"      | (___ \\_||_/ | | \\_|  | |_ \\_|  | |__) |   | |_) |    / _ \\      |   \\ | |    | |_/ /          "<< "\033[0m" << "\033[34;44m" <<"  " << "\033[0m" << endl;
    cout << "\033[93;44m" << "\t\t\t\t\t\t  "<< "\033[0m" << "\033[1;97m" <<"       _.____`.     | |      |  _| _   |  ___/    |  __ \\   / ___ \\     | |\\ \\| |    |  __ \\          "<< "\033[0m" << "\033[34;44m" <<"  " << "\033[0m" << endl;
    cout << "\033[93;44m" << "\t\t\t\t\t\t  "<< "\033[0m" << "\033[1;97m" <<"      | \\____) |   _| |_    _| |__/ | _| |_      _| |__) |_/ /   \\ \\_  _| |_\\   |_  _| |  \\ \\_        "<< "\033[0m" << "\033[34;44m" <<"  " << "\033[0m" << endl;
    cout << "\033[93;44m" << "\t\t\t\t\t\t  "<< "\033[0m" << "\033[1;97m" <<"       \\______.'  |_____|  |________||_____|    |_______/|____| |____||_____|\\____||____||____|       "<< "\033[0m" << "\033[34;44m" <<"  " << "\033[0m" << endl;
    cout << "\033[93;44m" << "\t\t\t\t\t\t  "<< "\033[0m" << "\033[1;97m" <<"                                                                                                      "<< "\033[0m" << "\033[34;44m" <<"  " << "\033[0m" << endl;
    cout << "\033[93;44m" << "\t\t\t\t\t\t                                                                                                          " << "\033[0m" << endl; 
}

void clearScreen()
{
#ifdef _WIN32 // Executable code in Window (Code can be compile in window)
    system("CLS");
#else
    system("clear"); // For MacOS and Linux
#endif
}

void displayMainMenu()
{
    clearScreen();
    displayLogo();
    cout << "\n\n";
    cout << "\t\t\t\t\t\t||=================================================" << endl;
    cout << "\t\t\t\t\t\t|| 1. Sign Up" << endl;
    cout << "\t\t\t\t\t\t|| 2. Log In" << endl;
    cout << "\t\t\t\t\t\t|| 3. Quit" << endl;
    cout << "\t\t\t\t\t\t|| Choose an option: ";
}

int main()
{
    loadUserData();
    int option;

    do
    {
        displayMainMenu();
        cin >> option;

        switch (option)
        {
        case 1:
            clearScreen();
            displayLogo();
            signUp();
            break;
        case 2:
        {
            clearScreen();
            displayLogo();
            User *user = login();
            if (user)
            {
                int userOption;
                do
                {
                    clearScreen();
                    displayLogo();
                    cout << "\n";
                    cout << "\t\t\t\t\t\t||=================================================" << endl;
                    cout << "\t\t\t\t\t\t|| Username : " << "\033[33m" << user->name << "\033[0m"<< endl;
                    cout << "\t\t\t\t\t\t|| 1. Check Balance" << endl;
                    cout << "\t\t\t\t\t\t|| 2. Deposit Balance" << endl;
                    cout << "\t\t\t\t\t\t|| 3. Withdraw Balance" << endl;
                    cout << "\t\t\t\t\t\t|| 4. Transfer Balance" << endl;
                    cout << "\t\t\t\t\t\t|| 5. Change PIN" << endl;
                    cout << "\t\t\t\t\t\t|| 6. Log Out" << endl;
                    cout << "\t\t\t\t\t\t|| Choose an option: ";
                    cin >> userOption;

                    switch (userOption)
                    {
                    case 1:
                        cout << "\033[33m" << "\t\t\t\t\t\t>> Your current balance is: " << user->balance << "\033[0m" << endl;
                        break;
                    case 2:
                        depositBalance(*user);
                        break;
                    case 3:
                        withdrawBalance(*user);
                        break;
                    case 4:
                        transferBalance(*user);
                        break;
                    case 5:
                        changePIN(*user);
                        break;
                    case 6:
                        cout << "\033[33m" << "\t\t\t\t\t\t>> Logging out." << "\033[0m" << endl;
                        break;
                    default:
                        cout << "\033[31m" << "\t\t\t\t\t\t!! Invalid option." << "\033[0m" << endl;
                        break;
                    }
                    if (userOption != 6)
                    {
                        cout << "\n\t\t\t\t\t\t>> Press any key to continue...";
                        cin.ignore();
                        cin.get();
                    }
                } while (userOption != 6);
            }
            break;
        }
        case 3:
            cout << "\033[33m" << "\t\t\t\t\t\t>> Exiting the application." << "\033[0m" << endl;
            break;
        default:
            cout << "\033[31m" << "\t\t\t\t\t\t!! Invalid option." << "\033[0m" << endl;
            break;
        }

        if (option != 3)
        {
            cout << "\n\t\t\t\t\t\t>> Press any key to return to the main menu...";
            cin.ignore();
            cin.get();
        }

    } while (option != 3);

    return 0;
}