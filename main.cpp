#include <iostream>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <iomanip>

using namespace std;

// Colors
#define GREEN "\033[38;5;120m"
#define RED "\033[38;5;210m"
#define ROSE "\033[38;5;211m"
#define SKY "\033[38;5;153m"
#define LILAC "\033[38;5;177m"
#define SAND "\033[38;5;223m"
#define RESET "\033[0m"

//  Typing
void type(string s, int speed = 20000) {
    for (char c : s) {
        cout << c;
        cout.flush();
        usleep(speed);
    }
    cout << endl;
}

void intro() {
    cout << SKY;
    type(" Welcome to the Vehicle Rental System 🚗", 20000);
    cout << RESET;
}

void clearScreen() { system("clear"); }

void loading() {
    cout << SAND << "Processing";
    for (int i = 0; i < 3; i++) {
        cout << ".";
        cout.flush();
        sleep(1);
    }
    cout << RESET << endl;
}

// Exception
class MyException {
public:
    string msg;
    MyException(string m) : msg(m) {}
};

// Base class
class Vehicle {
protected:
    int id;
    string name;
    string model;
    double price;
    bool available;

public:
    Vehicle(int i, string n, string m, double p)
        : id(i), name(n), model(m), price(p), available(true) {}

    virtual string getType() = 0;

    int getId() { return id; }
    string getName() { return name; }
    string getModel() { return model; }
    double getPrice() { return price; }
    bool isAvailable() { return available; }

    void rent() {
        if (!available) throw MyException("Vehicle already rented!");
        available = false;
    }

    void giveBack() {
        if (available) throw MyException("Vehicle was not rented!");
        available = true;
    }
};

// Types
class Car : public Vehicle {
public:
    Car(int i, string n, string m, double p) : Vehicle(i,n,m,p) {}
    string getType() override { return "Car"; }
};

class Bike : public Vehicle {
public:
    Bike(int i, string n, string m, double p) : Vehicle(i,n,m,p) {}
    string getType() override { return "Bike"; }
};

class Scooter : public Vehicle {
public:
    Scooter(int i, string n, string m, double p) : Vehicle(i,n,m,p) {}
    string getType() override { return "Scooter"; }
};

//Customer class
class Customer {
private:
    int id;
    string name;

public:
    Customer(int i, string n) : id(i), name(n) {}

    int getId() { return id; }
    string getName() { return name; }
};

void printReceipt(Vehicle* v, int days, double bill);

class RentalSystem {
private:
    vector<Vehicle*> vehicles;
    vector<Customer*> customers; // 🔥 NEW

public:
    bool idExists(int id) {
        for (auto v : vehicles)
            if (v->getId() == id) return true;
        return false;
    }

    Vehicle* find(int id) {
        for (auto v : vehicles)
            if (v->getId() == id) return v;
        return nullptr;
    }

    Customer* findCustomer(int id) {
        for (auto c : customers)
            if (c->getId() == id) return c;
        return nullptr;
    }

    void addCustomer() {
        int id;
        string name;

        cout << "Enter Customer ID: ";
        cin >> id;

        for (auto c : customers)
            if (c->getId() == id)
                throw MyException("Customer ID exists!");

        cout << "Enter Name: ";
        cin >> name;

        customers.push_back(new Customer(id, name));
        cout << GREEN << "Customer Added!\n" << RESET;
    }

    void addVehicle() {
        int id, type;
        string name, model;
        double price;

        cout << "Select Type (1.Car 2.Bike 3.Scooter): ";
        cin >> type;

        if (type == 1) {
            cout << "Select Model (1.Sedan 2.SUV): ";
            int m; cin >> m;
            model = (m == 1 ? "Sedan" : m == 2 ? "SUV" : throw MyException("Invalid model!"));
        }
        else if (type == 2) {
            cout << "Select Model (1.Sports 2.Cruiser): ";
            int m; cin >> m;
            model = (m == 1 ? "Sports" : m == 2 ? "Cruiser" : throw MyException("Invalid model!"));
        }
        else if (type == 3) {
            cout << "Select Model (1.Electric 2.Petrol): ";
            int m; cin >> m;
            model = (m == 1 ? "Electric" : m == 2 ? "Petrol" : throw MyException("Invalid model!"));
        }
        else throw MyException("Invalid type!");

        cout << "Enter ID: ";
        cin >> id;

        if (idExists(id)) throw MyException("Duplicate ID!");

        cout << "Enter Name: ";
        cin >> name;

        cout << "Enter Price: ";
        cin >> price;

        Vehicle* v;
        if (type == 1) v = new Car(id, name, model, price);
        else if (type == 2) v = new Bike(id, name, model, price);
        else v = new Scooter(id, name, model, price);

        vehicles.push_back(v);
        cout << GREEN << "Vehicle Added!\n" << RESET;
    }

    void showVehicles() {
    if (vehicles.empty())
        throw MyException("No vehicles available!");

    cout << ROSE;
    cout << "\n--------------------------------------------------------------------------\n";

    cout << left
         << setw(6)  << "ID"
         << setw(10) << "Type"
         << setw(12) << "Model"
         << setw(12) << "Name"
         << setw(10) << "Price"
         << setw(12) << "Status" << endl;

    cout << "--------------------------------------------------------------------------\n";
    cout << RESET;

    for (auto v : vehicles) {
        cout << left
             << setw(6)  << v->getId()
             << setw(10) << v->getType()
             << setw(12) << v->getModel()
             << setw(12) << v->getName()
             << setw(10) << ("₹" + to_string((int)v->getPrice()))
             << setw(12) << (v->isAvailable() ? "Available" : "Rented")
             << endl;
    }
}

    void rentVehicle() {
        int id, cid, days;

        cout << "Enter Vehicle ID: ";
        cin >> id;

        Vehicle* v = find(id);
        if (!v) throw MyException("Invalid Vehicle ID!");

        cout << "Enter Customer ID: ";
        cin >> cid;

        Customer* c = findCustomer(cid);
        if (!c) throw MyException("Customer not found!");

        cout << "Enter days: ";
        cin >> days;

        v->rent();

        double bill = v->getPrice() * days;

        loading();
        cout << GREEN << "Rented successfully!\n" << RESET;

        cout << SKY << "Customer: " << c->getName() << RESET << endl;

        printReceipt(v, days, bill);
    }

    void returnVehicle() {
        int id;
        cout << "Enter ID: ";
        cin >> id;

        Vehicle* v = find(id);
        if (!v) throw MyException("Invalid ID!");

        v->giveBack();

        loading();
        cout << GREEN << "Returned successfully!\n" << RESET;
    }
};

void printReceipt(Vehicle* v, int days, double bill) {
    cout << GREEN;
    const int width = 38;

    cout << "\n+" << string(width, '-') << "+\n";
    cout << "| " << setw(width - 2) << left << "RENT RECEIPT" << " |\n";
    cout << "+" << string(width, '-') << "+\n";

    auto line = [&](string label, string value) {
        cout << "| " << setw(14) << left << label
             << ": " << setw(width - 18) << left << value << " |\n";
    };

    line("Vehicle ID", to_string(v->getId()));
    line("Type", v->getType());
    line("Model", v->getModel());
    line("Name", v->getName());
    line("Days", to_string(days));
    line("Price/Day", "₹" + to_string((int)v->getPrice()));

    cout << "+" << string(width, '-') << "+\n";
    line("TOTAL BILL", "₹" + to_string((int)bill));
    cout << "+" << string(width, '-') << "+\n";

    cout << RESET;
}

// MAIN
int main() {
    RentalSystem system;
    int choice;

    clearScreen();
    intro();

    do {
        cout << SKY << "\n===== MENU =====" << RESET;
        cout << "\n1. Add Vehicle\n2. Show Vehicles\n3. Rent\n4. Return\n5. Exit\n6. Add Customer\n";
        cout << "Choice: ";
        cin >> choice;

        try {
            if (choice == 1) system.addVehicle();
            else if (choice == 2) system.showVehicles();
            else if (choice == 3) system.rentVehicle();
            else if (choice == 4) system.returnVehicle();
            else if (choice == 6) system.addCustomer();
        }
        catch (MyException &e) {
            cout << RED << "Error: " << e.msg << RESET << endl;
        }

    } while (choice != 5);

    cout << "Thank you!\n";
}