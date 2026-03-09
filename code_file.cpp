#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <algorithm>
#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <cmath>

using namespace std;

// ==============================
// FORWARD DECLARATIONS
// ==============================
class Driver;
class Rider;
class Ride;

// ==============================
// CLASS DEFINITIONS
// ==============================

class Driver {
private:
    int driverID;
    string name;
    string contact;
    string vehicleType;
    string location;
    double rating;
    int totalRides;
    bool isAvailable;
    
public:
    Driver(int id, string n, string c, string v, string loc) {
        driverID = id;
        name = n;
        contact = c;
        vehicleType = v;
        location = loc;
        rating = 4.5;
        totalRides = 0;
        isAvailable = true;
    }
    
    // Getters
    int getID() const { return driverID; }
    string getName() const { return name; }
    string getContact() const { return contact; }
    string getVehicle() const { return vehicleType; }
    string getLocation() const { return location; }
    double getRating() const { return rating; }
    int getTotalRides() const { return totalRides; }
    bool getAvailability() const { return isAvailable; }
    
    // Setters
    void setAvailability(bool avail) { isAvailable = avail; }
    void completeRide() { 
        totalRides++; 
        isAvailable = true;
    }
    void updateRating(double newRating) {
        rating = (rating * totalRides + newRating) / (totalRides + 1);
    }
    
    void display() const {
        cout << "ID: " << driverID << " | Name: " << name 
             << " | Contact: " << contact << " | Vehicle: " << vehicleType
             << " | Location: " << location << " | Rating: " << fixed << setprecision(1) << rating
             << " | Rides: " << totalRides << " | Available: " << (isAvailable ? "Yes" : "No") << endl;
    }
    
    // For file saving
    string toString() const {
        stringstream ss;
        ss << driverID << "," << name << "," << contact << "," << vehicleType << ","
           << location << "," << fixed << setprecision(2) << rating << ","
           << totalRides << "," << (isAvailable ? "1" : "0");
        return ss.str();
    }
};

class Rider {
private:
    int riderID;
    string name;
    string contact;
    int totalRides;
    string memberSince;
    
public:
    Rider(int id, string n, string c, string joinDate) {
        riderID = id;
        name = n;
        contact = c;
        totalRides = 0;
        memberSince = joinDate;
    }
    
    // Getters
    int getID() const { return riderID; }
    string getName() const { return name; }
    string getContact() const { return contact; }
    int getTotalRides() const { return totalRides; }
    string getJoinDate() const { return memberSince; }
    
    void completeRide() { totalRides++; }
    
    void display() const {
        cout << "ID: " << riderID << " | Name: " << name 
             << " | Contact: " << contact << " | Total Rides: " << totalRides
             << " | Member Since: " << memberSince << endl;
    }
    
    // For file saving
    string toString() const {
        stringstream ss;
        ss << riderID << "," << name << "," << contact << ","
           << totalRides << "," << memberSince;
        return ss.str();
    }
};

class Ride {
private:
    static int nextID;
    int rideID;
    int driverID;
    int riderID;
    string pickup;
    string destination;
    double distance;
    double fare;
    string status;
    string timestamp;
    double driverRating;
    
public:
    Ride(int dID, int rID, string pick, string dest, double dist) {
        rideID = nextID++;
        driverID = dID;
        riderID = rID;
        pickup = pick;
        destination = dest;
        distance = dist;
        status = "requested";
        driverRating = 0.0;
        
        time_t now = time(0);
        char* dt = ctime(&now);
        timestamp = dt;
        timestamp = timestamp.substr(0, timestamp.length()-1);
        calculateFare();
    }
    
    Ride() : rideID(0), driverID(0), riderID(0), distance(0), fare(0), driverRating(0) {}
    
    // Getters
    int getRideID() const { return rideID; }
    int getDriverID() const { return driverID; }
    int getRiderID() const { return riderID; }
    string getPickup() const { return pickup; }
    string getDestination() const { return destination; }
    double getDistance() const { return distance; }
    double getFare() const { return fare; }
    string getStatus() const { return status; }
    string getTimestamp() const { return timestamp; }
    double getDriverRating() const { return driverRating; }
    
    void calculateFare() {
        // Base fare: $2 + $1.5 per km
        fare = 2.0 + (distance * 1.5);
    }
    
    void completeRide(double rating) {
        status = "completed";
        driverRating = rating;
    }
    
    void cancelRide() {
        status = "cancelled";
    }
    
    void assignDriver(int dID) {
        driverID = dID;
        status = "assigned";
    }
    
    void display() const {
        cout << "Ride ID: " << rideID << " | Driver ID: " << driverID 
             << " | Rider ID: " << riderID << " | From: " << pickup
             << " | To: " << destination << " | Distance: " << distance << " km"
             << " | Fare: $" << fixed << setprecision(2) << fare
             << " | Status: " << status << " | Time: " << timestamp;
        if (status == "completed") {
            cout << " | Rating: " << driverRating << "/5";
        }
        cout << endl;
    }
    
    // For file saving
    string toString() const {
        stringstream ss;
        ss << rideID << "," << driverID << "," << riderID << ","
           << pickup << "," << destination << "," << fixed << setprecision(2) << distance << ","
           << fare << "," << status << "," << timestamp << "," << driverRating;
        return ss.str();
    }
};

int Ride::nextID = 1000;

// ==============================
// RIDE MANAGER CLASS FOR HANDLING RIDE OPERATIONS
// ==============================

class RideManager {
private:
    queue<Ride> rideRequests;  // For storing new ride requests
    vector<Ride> completedRides;  // For storing completed rides
    vector<Ride> cancelledRides;  // For storing cancelled rides
    vector<Ride> allRides;  // For storing all rides
    
public:
    // Add new ride request to queue
    void addRideRequest(const Ride& ride) {
        rideRequests.push(ride);
        allRides.push_back(ride);
    }
    
    // Get next ride request from queue
    Ride getNextRideRequest() {
        if (rideRequests.empty()) {
            return Ride();  // Return empty ride if queue is empty
        }
        Ride ride = rideRequests.front();
        rideRequests.pop();
        return ride;
    }
    
    // Check if there are pending ride requests
    bool hasPendingRequests() const {
        return !rideRequests.empty();
    }
    
    // Get number of pending requests
    int getPendingRequestsCount() const {
        return rideRequests.size();
    }
    
    // Complete a ride and move to completed list
    void completeRide(const Ride& ride) {
        completedRides.push_back(ride);
        // Update in allRides
        for (auto& r : allRides) {
            if (r.getRideID() == ride.getRideID()) {
                r = ride;
                break;
            }
        }
    }
    
    // Cancel a ride and move to cancelled list
    void cancelRide(const Ride& ride) {
        cancelledRides.push_back(ride);
        // Update in allRides
        for (auto& r : allRides) {
            if (r.getRideID() == ride.getRideID()) {
                r = ride;
                break;
            }
        }
    }
    
    // Get completed rides
    const vector<Ride>& getCompletedRides() const {
        return completedRides;
    }
    
    // Get cancelled rides
    const vector<Ride>& getCancelledRides() const {
        return cancelledRides;
    }
    
    // Get all rides
    const vector<Ride>& getAllRides() const {
        return allRides;
    }
    
    // Display ride queue
    void displayRideQueue() const {
        cout << "\n===== RIDE REQUEST QUEUE =====\n";
        cout << "Pending Requests: " << rideRequests.size() << "\n\n";
        
        if (rideRequests.empty()) {
            cout << "No pending ride requests.\n";
            return;
        }
        
        // To display queue contents without modifying it
        queue<Ride> tempQueue = rideRequests;
        int count = 1;
        while (!tempQueue.empty()) {
            cout << "Request #" << count++ << ":\n";
            tempQueue.front().display();
            tempQueue.pop();
            cout << endl;
        }
    }
    
    // Display completed rides
    void displayCompletedRides() const {
        cout << "\n===== COMPLETED RIDES =====\n";
        cout << "Total Completed: " << completedRides.size() << "\n\n";
        
        if (completedRides.empty()) {
            cout << "No completed rides.\n";
            return;
        }
        
        for (const auto& ride : completedRides) {
            ride.display();
        }
    }
    
    // Display cancelled rides
    void displayCancelledRides() const {
        cout << "\n===== CANCELLED RIDES =====\n";
        cout << "Total Cancelled: " << cancelledRides.size() << "\n\n";
        
        if (cancelledRides.empty()) {
            cout << "No cancelled rides.\n";
            return;
        }
        
        for (const auto& ride : cancelledRides) {
            ride.display();
        }
    }
    
    // Display summary statistics
    void displayStatistics() const {
        cout << "\n===== RIDE STATISTICS =====\n";
        cout << "Total Rides Requested: " << allRides.size() << "\n";
        cout << "Pending Requests: " << rideRequests.size() << "\n";
        cout << "Completed Rides: " << completedRides.size() << "\n";
        cout << "Cancelled Rides: " << cancelledRides.size() << "\n";
        
        if (!completedRides.empty()) {
            double totalEarnings = 0;
            double totalDistance = 0;
            double totalRating = 0;
            
            for (const auto& ride : completedRides) {
                totalEarnings += ride.getFare();
                totalDistance += ride.getDistance();
                totalRating += ride.getDriverRating();
            }
            
            double avgDistance = totalDistance / completedRides.size();
            double avgRating = totalRating / completedRides.size();
            
            cout << "\n--- Financial Summary ---\n";
            cout << "Total Earnings: $" << fixed << setprecision(2) << totalEarnings << "\n";
            cout << "Average Earnings per Ride: $" << totalEarnings / completedRides.size() << "\n";
            
            cout << "\n--- Ride Metrics ---\n";
            cout << "Total Distance: " << totalDistance << " km\n";
            cout << "Average Distance: " << avgDistance << " km\n";
            cout << "Average Driver Rating: " << avgRating << "/5\n";
            
            cout << "\n--- Success Rates ---\n";
            double completionRate = (completedRides.size() * 100.0) / allRides.size();
            double cancellationRate = (cancelledRides.size() * 100.0) / allRides.size();
            cout << "Completion Rate: " << fixed << setprecision(1) << completionRate << "%\n";
            cout << "Cancellation Rate: " << cancellationRate << "%\n";
        } else {
            cout << "\nNo completed rides to calculate statistics.\n";
        }
    }
    
    // Find ride by ID
    Ride* findRideById(int rideId) {
        for (auto& ride : allRides) {
            if (ride.getRideID() == rideId) {
                return &ride;
            }
        }
        return nullptr;
    }
};

// ==============================
// FILE HANDLER CLASS
// ==============================

class FileHandler {
public:
    // Save drivers to CSV file
    static bool saveDrivers(const vector<Driver>& drivers, const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "✗ Error: Cannot open file " << filename << " for writing\n";
            return false;
        }
        
        // Write header
        file << "ID,Name,Contact,Vehicle,Location,Rating,TotalRides,Available\n";
        
        // Write driver data
        for (const auto& driver : drivers) {
            file << driver.getID() << ","
                 << driver.getName() << ","
                 << driver.getContact() << ","
                 << driver.getVehicle() << ","
                 << driver.getLocation() << ","
                 << fixed << setprecision(2) << driver.getRating() << ","
                 << driver.getTotalRides() << ","
                 << (driver.getAvailability() ? "1" : "0") << "\n";
        }
        
        file.close();
        cout << "✓ Drivers data saved to " << filename << " (" << drivers.size() << " records)\n";
        return true;
    }
    
    // Load drivers from CSV file
    static vector<Driver> loadDrivers(const string& filename) {
        vector<Driver> drivers;
        ifstream file(filename);
        
        if (!file.is_open()) {
            cout << "✗ Warning: File " << filename << " not found. Starting with empty driver list.\n";
            return drivers;
        }
        
        string line;
        getline(file, line); // Skip header
        
        while (getline(file, line)) {
            stringstream ss(line);
            vector<string> tokens;
            string token;
            
            while (getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            
            if (tokens.size() >= 8) {
                try {
                    int id = stoi(tokens[0]);
                    string name = tokens[1];
                    string contact = tokens[2];
                    string vehicle = tokens[3];
                    string location = tokens[4];
                    double rating = stod(tokens[5]);
                    int totalRides = stoi(tokens[6]);
                    bool available = (tokens[7] == "1");
                    
                    // Create driver
                    Driver driver(id, name, contact, vehicle, location);
                    // Update rating and rides
                    for (int i = 0; i < totalRides; i++) {
                        driver.completeRide();
                    }
                    driver.setAvailability(available);
                    
                    drivers.push_back(driver);
                } catch (const exception& e) {
                    cout << "✗ Error parsing driver data: " << line << endl;
                }
            }
        }
        
        file.close();
        cout << "✓ Drivers data loaded from " << filename << " (" << drivers.size() << " records)\n";
        return drivers;
    }
    
    // Save riders to CSV file
    static bool saveRiders(const vector<Rider>& riders, const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "✗ Error: Cannot open file " << filename << " for writing\n";
            return false;
        }
        
        file << "ID,Name,Contact,TotalRides,MemberSince\n";
        
        for (const auto& rider : riders) {
            file << rider.getID() << ","
                 << rider.getName() << ","
                 << rider.getContact() << ","
                 << rider.getTotalRides() << ","
                 << rider.getJoinDate() << "\n";
        }
        
        file.close();
        cout << "✓ Riders data saved to " << filename << " (" << riders.size() << " records)\n";
        return true;
    }
    
    // Load riders from CSV file
    static vector<Rider> loadRiders(const string& filename) {
        vector<Rider> riders;
        ifstream file(filename);
        
        if (!file.is_open()) {
            cout << "✗ Warning: File " << filename << " not found. Starting with empty rider list.\n";
            return riders;
        }
        
        string line;
        getline(file, line); // Skip header
        
        while (getline(file, line)) {
            stringstream ss(line);
            vector<string> tokens;
            string token;
            
            while (getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            
            if (tokens.size() >= 5) {
                try {
                    int id = stoi(tokens[0]);
                    string name = tokens[1];
                    string contact = tokens[2];
                    int totalRides = stoi(tokens[3]);
                    string joinDate = tokens[4];
                    
                    Rider rider(id, name, contact, joinDate);
                    // Update total rides
                    for (int i = 0; i < totalRides; i++) {
                        rider.completeRide();
                    }
                    
                    riders.push_back(rider);
                } catch (const exception& e) {
                    cout << "✗ Error parsing rider data: " << line << endl;
                }
            }
        }
        
        file.close();
        cout << "✓ Riders data loaded from " << filename << " (" << riders.size() << " records)\n";
        return riders;
    }
    
    // Save rides to CSV file
    static bool saveRides(const vector<Ride>& rides, const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "✗ Error: Cannot open file " << filename << " for writing\n";
            return false;
        }
        
        file << "RideID,DriverID,RiderID,Pickup,Destination,Distance,Fare,Status,Timestamp,Rating\n";
        
        for (const auto& ride : rides) {
            file << ride.getRideID() << ","
                 << ride.getDriverID() << ","
                 << ride.getRiderID() << ","
                 << ride.getPickup() << ","
                 << ride.getDestination() << ","
                 << fixed << setprecision(2) << ride.getDistance() << ","
                 << ride.getFare() << ","
                 << ride.getStatus() << ","
                 << ride.getTimestamp() << ","
                 << ride.getDriverRating() << "\n";
        }
        
        file.close();
        cout << "✓ Rides data saved to " << filename << " (" << rides.size() << " records)\n";
        return true;
    }
    
    // Save report to text file
    static bool saveReport(const string& reportContent, const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cout << "✗ Error: Cannot create report file " << filename << endl;
            return false;
        }
        
        file << "===== MINI-UBER SYSTEM REPORT =====\n";
        file << "Generated on: " << getCurrentDateTime() << "\n\n";
        file << reportContent;
        
        file.close();
        cout << "✓ Report saved to " << filename << endl;
        return true;
    }
    
    // Export all data to backup directory
    static bool exportAllData(const vector<Driver>& drivers,
                             const vector<Rider>& riders,
                             const vector<Ride>& rides) {
        cout << "\n=== EXPORTING ALL DATA ===\n";
        
        bool success = true;
        success &= saveDrivers(drivers, "drivers_backup.csv");
        success &= saveRiders(riders, "riders_backup.csv");
        success &= saveRides(rides, "rides_backup.csv");
        
        if (success) {
            cout << "✓ All data exported successfully!\n";
        } else {
            cout << "✗ Some errors occurred during export\n";
        }
        
        return success;
    }
    
private:
    static string getCurrentDateTime() {
        time_t now = time(0);
        char buffer[100];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));
        return string(buffer);
    }
};

// ==============================
// RIDE SHARING SYSTEM CLASS
// ==============================

class RideSharingSystem {
private:
    vector<Driver> drivers;
    vector<Rider> riders;
    RideManager rideManager;
    map<int, Driver*> driverMap;
    map<int, Rider*> riderMap;
    
    // File names
    const string DRIVERS_FILE = "drivers.csv";
    const string RIDERS_FILE = "riders.csv";
    const string RIDES_FILE = "rides.csv";
    
public:
    RideSharingSystem() {
        loadAllData();
        if (drivers.empty() || riders.empty()) {
            createSampleData();
        }
    }
    
    ~RideSharingSystem() {
        saveAllData();
    }
    
    void loadAllData() {
        cout << "\n=== LOADING DATA FROM FILES ===\n";
        drivers = FileHandler::loadDrivers(DRIVERS_FILE);
        riders = FileHandler::loadRiders(RIDERS_FILE);
        
        // Update maps
        for (int i = 0; i < drivers.size(); i++) {
            driverMap[drivers[i].getID()] = &drivers[i];
        }
        for (int i = 0; i < riders.size(); i++) {
            riderMap[riders[i].getID()] = &riders[i];
        }
        
        cout << "✓ Data loaded successfully!\n";
        cout << "  Drivers: " << drivers.size() << endl;
        cout << "  Riders: " << riders.size() << endl;
    }
    
    void saveAllData() {
        cout << "\n=== SAVING DATA TO FILES ===\n";
        FileHandler::saveDrivers(drivers, DRIVERS_FILE);
        FileHandler::saveRiders(riders, RIDERS_FILE);
        
        // Combine all rides for saving
        vector<Ride> allRidesForSave = rideManager.getAllRides();
        FileHandler::saveRides(allRidesForSave, RIDES_FILE);
        
        cout << "✓ All data saved successfully!\n";
    }
    
    void createSampleData() {
        cout << "\n=== CREATING SAMPLE DATA ===\n";
        
        if (drivers.empty()) {
            drivers.push_back(Driver(1, "John Doe", "03001234567", "Sedan", "Gulberg"));
            drivers.push_back(Driver(2, "Jane Smith", "03111223344", "SUV", "DHA"));
            drivers.push_back(Driver(3, "Ahmed Khan", "03214455667", "Compact", "Model Town"));
            drivers.push_back(Driver(4, "Sara Ahmed", "03331112233", "Sedan", "Faisal Town"));
            drivers.push_back(Driver(5, "Ali Raza", "03445556677", "Motorcycle", "Liberty"));
        }
        
        if (riders.empty()) {
            riders.push_back(Rider(101, "Bilal Malik", "03551234567", "2024-01-15"));
            riders.push_back(Rider(102, "Fatima Khan", "03661112233", "2024-02-20"));
            riders.push_back(Rider(103, "Usman Ali", "03771223344", "2024-03-10"));
            riders.push_back(Rider(104, "Zainab Riaz", "03881334455", "2024-01-25"));
        }
        
        // Update maps
        for (int i = 0; i < drivers.size(); i++) {
            driverMap[drivers[i].getID()] = &drivers[i];
        }
        for (int i = 0; i < riders.size(); i++) {
            riderMap[riders[i].getID()] = &riders[i];
        }
        
        cout << "✓ Sample data created successfully!\n";
    }
    
    void showMainMenu() {
        int choice;
        do {
            cout << "\n===== MINI-UBER RIDE SHARING SYSTEM =====\n";
            cout << "1. Admin Menu\n";
            cout << "2. Book a Ride (Rider)\n";
            cout << "3. Driver Menu\n";
            cout << "4. Ride Management Menu\n";
            cout << "5. View All Data\n";
            cout << "6. Generate Reports\n";
            cout << "7. File Operations\n";
            cout << "8. Exit\n";
            cout << "Enter your choice: ";
            cin >> choice;
            
            switch(choice) {
                case 1: adminMenu(); break;
                case 2: bookRideMenu(); break;
                case 3: driverMenu(); break;
                case 4: rideManagementMenu(); break;
                case 5: viewAllData(); break;
                case 6: generateReports(); break;
                case 7: fileOperationsMenu(); break;
                case 8: 
                    cout << "Saving data and exiting...\n";
                    saveAllData();
                    cout << "Thank you for using Mini-Uber!\n";
                    break;
                default: cout << "Invalid choice! Please try again.\n";
            }
        } while(choice != 8);
    }
    
    void rideManagementMenu() {
        int choice;
        do {
            cout << "\n----- RIDE MANAGEMENT MENU -----\n";
            cout << "1. View Ride Queue\n";
            cout << "2. Process Next Ride Request\n";
            cout << "3. View All Completed Rides\n";
            cout << "4. View All Cancelled Rides\n";
            cout << "5. Cancel Specific Ride\n";
            cout << "6. View Ride Statistics\n";
            cout << "7. Back to Main Menu\n";
            cout << "Enter choice: ";
            cin >> choice;
            
            switch(choice) {
                case 1: rideManager.displayRideQueue(); break;
                case 2: processSingleRideRequest(); break;
                case 3: rideManager.displayCompletedRides(); break;
                case 4: rideManager.displayCancelledRides(); break;
                case 5: cancelSpecificRide(); break;
                case 6: rideManager.displayStatistics(); break;
                case 7: break;
                default: cout << "Invalid choice!\n";
            }
        } while(choice != 7);
    }
    
    void processSingleRideRequest() {
        if (!rideManager.hasPendingRequests()) {
            cout << "No pending ride requests to process.\n";
            return;
        }
        
        Ride ride = rideManager.getNextRideRequest();
        cout << "\nProcessing Ride ID: " << ride.getRideID() << endl;
        cout << "From: " << ride.getPickup() << " to " << ride.getDestination() << endl;
        cout << "Driver ID: " << ride.getDriverID() << " | Rider ID: " << ride.getRiderID() << endl;
        cout << "Distance: " << ride.getDistance() << " km | Fare: $" << ride.getFare() << endl;
        
        int action;
        cout << "\n1. Complete Ride\n";
        cout << "2. Cancel Ride\n";
        cout << "3. Skip for now (return to queue)\n";
        cout << "Choose action: ";
        cin >> action;
        
        if (action == 1) {
            double rating;
            cout << "Enter driver rating (1-5): ";
            cin >> rating;
            
            if (rating < 1 || rating > 5) {
                cout << "Invalid rating! Using default 4.0\n";
                rating = 4.0;
            }
            
            ride.completeRide(rating);
            rideManager.completeRide(ride);
            
            // Update driver
            Driver* driver = driverMap[ride.getDriverID()];
            if (driver) {
                driver->completeRide();
                driver->updateRating(rating);
            }
            
            // Update rider
            Rider* rider = riderMap[ride.getRiderID()];
            if (rider) {
                rider->completeRide();
            }
            
            cout << "✓ Ride completed successfully!\n";
        }
        else if (action == 2) {
            ride.cancelRide();
            rideManager.cancelRide(ride);
            
            // Make driver available again
            Driver* driver = driverMap[ride.getDriverID()];
            if (driver) {
                driver->setAvailability(true);
            }
            
            cout << "✗ Ride cancelled.\n";
        }
        else {
            // Put back in queue
            rideManager.addRideRequest(ride);
            cout << "Ride returned to queue.\n";
        }
    }
    
    void cancelSpecificRide() {
        int rideId;
        cout << "Enter Ride ID to cancel: ";
        cin >> rideId;
        
        Ride* ride = rideManager.findRideById(rideId);
        if (!ride) {
            cout << "Ride not found!\n";
            return;
        }
        
        cout << "Found Ride:\n";
        ride->display();
        
        char confirm;
        cout << "Are you sure you want to cancel this ride? (y/n): ";
        cin >> confirm;
        
        if (confirm == 'y' || confirm == 'Y') {
            ride->cancelRide();
            rideManager.cancelRide(*ride);
            
            // Make driver available again
            Driver* driver = driverMap[ride->getDriverID()];
            if (driver) {
                driver->setAvailability(true);
            }
            
            cout << "✓ Ride cancelled successfully!\n";
        } else {
            cout << "Cancellation aborted.\n";
        }
    }
    
    void bookRideMenu() {
        cout << "\n----- BOOK A RIDE -----\n";
        
        if (riders.empty()) {
            cout << "No riders available. Please add riders first.\n";
            return;
        }
        
        // Show available riders
        cout << "Available Riders:\n";
        for (int i = 0; i < riders.size(); i++) {
            cout << "ID: " << riders[i].getID() << " - " << riders[i].getName() << endl;
        }
        
        int riderID;
        cout << "Enter Rider ID: ";
        cin >> riderID;
        
        if (riderMap.find(riderID) == riderMap.end()) {
            cout << "Invalid Rider ID!\n";
            return;
        }
        
        cin.ignore(); // Clear buffer
        string pickup, destination;
        double distance;
        
        cout << "Enter Pickup Location: ";
        getline(cin, pickup);
        cout << "Enter Destination: ";
        getline(cin, destination);
        cout << "Enter Distance (km): ";
        cin >> distance;
        
        if (distance <= 0) {
            cout << "Invalid distance! Must be greater than 0.\n";
            return;
        }
        
        // Assign nearest available driver
        int driverID = findNearestAvailableDriver(pickup);
        
        if (driverID == -1) {
            cout << "No available drivers at the moment!\n";
            return;
        }
        
        // Create and add ride request
        Ride newRide(driverID, riderID, pickup, destination, distance);
        rideManager.addRideRequest(newRide);
        
        // Update driver availability
        driverMap[driverID]->setAvailability(false);
        
        cout << "\n✓ Ride booked successfully!\n";
        cout << "Ride ID: " << newRide.getRideID() << endl;
        cout << "Assigned Driver ID: " << driverID << endl;
        cout << "Driver Name: " << driverMap[driverID]->getName() << endl;
        cout << "Vehicle Type: " << driverMap[driverID]->getVehicle() << endl;
        cout << "Estimated Fare: $" << fixed << setprecision(2) << newRide.getFare() << endl;
        cout << "Status: Added to ride request queue (Position: " << rideManager.getPendingRequestsCount() << ")\n";
        
        cout << "\nRide Details:\n";
        newRide.display();
    }
    
    int findNearestAvailableDriver(const string& location) {
        // Simple implementation - find first available driver
        for (int i = 0; i < drivers.size(); i++) {
            if (drivers[i].getAvailability()) {
                return drivers[i].getID();
            }
        }
        return -1; // No available drivers
    }
    
    void adminMenu() {
        int choice;
        do {
            cout << "\n----- ADMIN MENU -----\n";
            cout << "1. View All Drivers\n";
            cout << "2. View All Riders\n";
            cout << "3. View All Rides\n";
            cout << "4. Process All Ride Requests\n";
            cout << "5. Add New Driver\n";
            cout << "6. Add New Rider\n";
            cout << "7. Save Data Now\n";
            cout << "8. Back to Main Menu\n";
            cout << "Enter choice: ";
            cin >> choice;
            
            switch(choice) {
                case 1: displayAllDrivers(); break;
                case 2: displayAllRiders(); break;
                case 3: displayAllRides(); break;
                case 4: processAllRideRequests(); break;
                case 5: addNewDriver(); break;
                case 6: addNewRider(); break;
                case 7: saveAllData(); break;
                case 8: break;
                default: cout << "Invalid choice!\n";
            }
        } while(choice != 8);
    }
    
    void processAllRideRequests() {
        cout << "\n----- PROCESS ALL RIDE REQUESTS -----\n";
        
        if (!rideManager.hasPendingRequests()) {
            cout << "No pending ride requests.\n";
            return;
        }
        
        int count = 0;
        while (rideManager.hasPendingRequests()) {
            Ride ride = rideManager.getNextRideRequest();
            
            // Auto-complete ride for demo purposes
            double rating = 4.0 + (rand() % 20) / 20.0; // Random rating 4.0-5.0
            
            ride.completeRide(rating);
            rideManager.completeRide(ride);
            
            // Update driver
            Driver* driver = driverMap[ride.getDriverID()];
            if (driver) {
                driver->completeRide();
                driver->updateRating(rating);
            }
            
            // Update rider
            Rider* rider = riderMap[ride.getRiderID()];
            if (rider) {
                rider->completeRide();
            }
            
            count++;
        }
        
        cout << "✓ Processed " << count << " ride(s) automatically.\n";
    }
    
    void displayAllRides() {
        cout << "\n----- ALL RIDES -----\n";
        cout << "Pending Requests: " << rideManager.getPendingRequestsCount() << endl;
        cout << "Completed Rides: " << rideManager.getCompletedRides().size() << endl;
        cout << "Cancelled Rides: " << rideManager.getCancelledRides().size() << endl;
        
        if (!rideManager.getCompletedRides().empty()) {
            cout << "\nRecent Completed Rides:\n";
            const vector<Ride>& completedRides = rideManager.getCompletedRides();
            int count = min(5, (int)completedRides.size());
            for (int i = 0; i < count; i++) {
                completedRides[completedRides.size() - 1 - i].display();
            }
        }
        
        if (!rideManager.getCancelledRides().empty()) {
            cout << "\nRecent Cancelled Rides:\n";
            const vector<Ride>& cancelledRides = rideManager.getCancelledRides();
            int count = min(3, (int)cancelledRides.size());
            for (int i = 0; i < count; i++) {
                cancelledRides[cancelledRides.size() - 1 - i].display();
            }
        }
    }
    
    void generateReports() {
        int choice;
        do {
            cout << "\n----- REPORTS MENU -----\n";
            cout << "1. Ride Summary Report\n";
            cout << "2. Driver Performance Report\n";
            cout << "3. Rider Activity Report\n";
            cout << "4. Financial Report\n";
            cout << "5. System Statistics\n";
            cout << "6. Back to Main Menu\n";
            cout << "Enter choice: ";
            cin >> choice;
            
            switch(choice) {
                case 1: rideSummaryReport(); break;
                case 2: driverPerformanceReport(); break;
                case 3: riderActivityReport(); break;
                case 4: financialReport(); break;
                case 5: systemStatistics(); break;
                case 6: break;
                default: cout << "Invalid choice!\n";
            }
        } while(choice != 6);
    }
    
    void financialReport() {
        cout << "\n===== FINANCIAL REPORT =====\n";
        
        const vector<Ride>& completedRides = rideManager.getCompletedRides();
        
        double totalEarnings = 0;
        double totalDistance = 0;
        double totalRating = 0;
        
        for (const auto& ride : completedRides) {
            totalEarnings += ride.getFare();
            totalDistance += ride.getDistance();
            totalRating += ride.getDriverRating();
        }
        
        cout << "Total Earnings: $" << fixed << setprecision(2) << totalEarnings << endl;
        cout << "Total Distance: " << totalDistance << " km" << endl;
        if (!completedRides.empty()) {
            cout << "Average Fare per Ride: $" << totalEarnings / completedRides.size() << endl;
            cout << "Average Distance per Ride: " << totalDistance / completedRides.size() << " km" << endl;
            cout << "Average Driver Rating: " << totalRating / completedRides.size() << "/5" << endl;
        }
        
        // Potential earnings from cancelled rides
        if (!rideManager.getCancelledRides().empty()) {
            cout << "\nPotential Lost Revenue from " << rideManager.getCancelledRides().size() 
                 << " cancelled rides: $" << fixed << setprecision(2) 
                 << (rideManager.getCancelledRides().size() * 10.0) << " (estimated)\n";
        }
    }
    
    void displayAllDrivers() {
        cout << "\n----- ALL DRIVERS -----\n";
        if (drivers.empty()) {
            cout << "No drivers available.\n";
            return;
        }
        
        int availableCount = 0;
        for (int i = 0; i < drivers.size(); i++) {
            drivers[i].display();
            if (drivers[i].getAvailability()) {
                availableCount++;
            }
        }
        cout << "\nTotal Drivers: " << drivers.size() << " | Available: " << availableCount << endl;
    }
    
    void displayAllRiders() {
        cout << "\n----- ALL RIDERS -----\n";
        if (riders.empty()) {
            cout << "No riders available.\n";
            return;
        }
        for (int i = 0; i < riders.size(); i++) {
            riders[i].display();
        }
        cout << "\nTotal Riders: " << riders.size() << endl;
    }
    
    void viewAllData() {
        cout << "\n===== SYSTEM DATA OVERVIEW =====\n";
        
        cout << "\n--- Drivers (" << drivers.size() << ") ---\n";
        if (drivers.empty()) {
            cout << "No drivers in system.\n";
        } else {
            for (int i = 0; i < drivers.size(); i++) {
                drivers[i].display();
            }
        }
        
        cout << "\n--- Riders (" << riders.size() << ") ---\n";
        if (riders.empty()) {
            cout << "No riders in system.\n";
        } else {
            for (int i = 0; i < riders.size(); i++) {
                riders[i].display();
            }
        }
        
        cout << "\n--- Ride Statistics ---\n";
        cout << "Pending Requests: " << rideManager.getPendingRequestsCount() << endl;
        cout << "Completed Rides: " << rideManager.getCompletedRides().size() << endl;
        cout << "Cancelled Rides: " << rideManager.getCancelledRides().size() << endl;
        
        if (!rideManager.getCompletedRides().empty()) {
            double totalEarnings = 0;
            double totalDistance = 0;
            for (const auto& ride : rideManager.getCompletedRides()) {
                totalEarnings += ride.getFare();
                totalDistance += ride.getDistance();
            }
            cout << "Total Earnings: $" << fixed << setprecision(2) << totalEarnings << endl;
            cout << "Total Distance: " << totalDistance << " km" << endl;
            cout << "Average Distance: " << totalDistance / rideManager.getCompletedRides().size() << " km" << endl;
        }
        
        cout << "\n--- File Status ---\n";
        cout << "Data will be saved to CSV files on exit\n";
        cout << "Files: drivers.csv, riders.csv, rides.csv\n";
    }
    
    void fileOperationsMenu() {
        int choice;
        do {
            cout << "\n----- FILE OPERATIONS MENU -----\n";
            cout << "1. Export All Data (Backup)\n";
            cout << "2. Export Driver Report\n";
            cout << "3. Export Rider Report\n";
            cout << "4. Export Financial Report\n";
            cout << "5. Load Data from Files\n";
            cout << "6. Save Data to Files\n";
            cout << "7. View File Statistics\n";
            cout << "8. Back to Main Menu\n";
            cout << "Enter choice: ";
            cin >> choice;
            
            switch(choice) {
                case 1: exportAllData(); break;
                case 2: exportDriverReport(); break;
                case 3: exportRiderReport(); break;
                case 4: exportFinancialReport(); break;
                case 5: loadAllData(); break;
                case 6: saveAllData(); break;
                case 7: viewFileStats(); break;
                case 8: break;
                default: cout << "Invalid choice!\n";
            }
        } while(choice != 8);
    }
    
    void driverMenu() {
        cout << "\n----- DRIVER MENU -----\n";
        displayAllDrivers();
        
        int driverID;
        cout << "Enter your Driver ID: ";
        cin >> driverID;
        
        if (driverMap.find(driverID) == driverMap.end()) {
            cout << "Invalid Driver ID!\n";
            return;
        }
        
        int choice;
        do {
            cout << "\nDriver: " << driverMap[driverID]->getName() << endl;
            cout << "1. View My Details\n";
            cout << "2. Change Availability\n";
            cout << "3. View My Completed Rides\n";
            cout << "4. View My Earnings\n";
            cout << "5. Back to Main Menu\n";
            cout << "Enter choice: ";
            cin >> choice;
            
            switch(choice) {
                case 1: 
                    driverMap[driverID]->display();
                    break;
                case 2:
                    {
                        int avail;
                        cout << "Set availability (1 for Available, 0 for Busy): ";
                        cin >> avail;
                        driverMap[driverID]->setAvailability(avail == 1);
                        cout << "Availability updated!\n";
                    }
                    break;
                case 3:
                    viewDriverRides(driverID);
                    break;
                case 4:
                    viewDriverEarnings(driverID);
                    break;
                case 5: break;
                default: cout << "Invalid choice!\n";
            }
        } while(choice != 5);
    }
    
    void viewDriverRides(int driverID) {
        cout << "\nCompleted Rides for Driver " << driverID << ":\n";
        bool found = false;
        double totalEarnings = 0;
        
        for (const auto& ride : rideManager.getCompletedRides()) {
            if (ride.getDriverID() == driverID) {
                ride.display();
                totalEarnings += ride.getFare();
                found = true;
            }
        }
        
        if (!found) {
            cout << "No completed rides found.\n";
        } else {
            cout << "\nTotal Earnings: $" << fixed << setprecision(2) << totalEarnings << endl;
        }
    }
    
    void viewDriverEarnings(int driverID) {
        double totalEarnings = 0;
        int rideCount = 0;
        
        for (const auto& ride : rideManager.getCompletedRides()) {
            if (ride.getDriverID() == driverID) {
                totalEarnings += ride.getFare();
                rideCount++;
            }
        }
        
        cout << "\n=== EARNINGS REPORT FOR DRIVER " << driverID << " ===\n";
        cout << "Total Rides Completed: " << rideCount << endl;
        cout << "Total Earnings: $" << fixed << setprecision(2) << totalEarnings << endl;
        if (rideCount > 0) {
            cout << "Average per Ride: $" << totalEarnings / rideCount << endl;
        }
    }
    
    void exportAllData() {
        FileHandler::exportAllData(drivers, riders, rideManager.getAllRides());
    }
    
    void exportDriverReport() {
        stringstream report;
        report << "DRIVER PERFORMANCE REPORT\n";
        report << "==========================\n";
        report << "Total Drivers: " << drivers.size() << "\n\n";
        
        for (int i = 0; i < drivers.size(); i++) {
            report << "Driver #" << (i+1) << ":\n";
            report << "  ID: " << drivers[i].getID() << "\n";
            report << "  Name: " << drivers[i].getName() << "\n";
            report << "  Rating: " << fixed << setprecision(2) << drivers[i].getRating() << "/5\n";
            report << "  Total Rides: " << drivers[i].getTotalRides() << "\n";
            report << "  Availability: " << (drivers[i].getAvailability() ? "Available" : "Busy") << "\n";
            report << "  --------------------\n";
        }
        
        FileHandler::saveReport(report.str(), "driver_report.txt");
    }
    
    void exportRiderReport() {
        stringstream report;
        report << "RIDER ACTIVITY REPORT\n";
        report << "=====================\n";
        report << "Total Riders: " << riders.size() << "\n\n";
        
        for (int i = 0; i < riders.size(); i++) {
            report << "Rider #" << (i+1) << ":\n";
            report << "  ID: " << riders[i].getID() << "\n";
            report << "  Name: " << riders[i].getName() << "\n";
            report << "  Total Rides: " << riders[i].getTotalRides() << "\n";
            report << "  Member Since: " << riders[i].getJoinDate() << "\n";
            report << "  --------------------\n";
        }
        
        FileHandler::saveReport(report.str(), "rider_report.txt");
    }
    
    void exportFinancialReport() {
        stringstream report;
        report << "FINANCIAL REPORT\n";
        report << "================\n";
        
        const vector<Ride>& completedRides = rideManager.getCompletedRides();
        const vector<Ride>& cancelledRides = rideManager.getCancelledRides();
        
        double totalEarnings = 0;
        double totalDistance = 0;
        double totalRating = 0;
        
        for (const auto& ride : completedRides) {
            totalEarnings += ride.getFare();
            totalDistance += ride.getDistance();
            totalRating += ride.getDriverRating();
        }
        
        report << "Total Completed Rides: " << completedRides.size() << "\n";
        report << "Total Earnings: $" << fixed << setprecision(2) << totalEarnings << "\n";
        report << "Total Distance: " << totalDistance << " km\n";
        if (!completedRides.empty()) {
            report << "Average Fare per Ride: $" << totalEarnings / completedRides.size() << "\n";
            report << "Average Distance per Ride: " << totalDistance / completedRides.size() << " km\n";
            report << "Average Driver Rating: " << totalRating / completedRides.size() << "/5\n";
        }
        
        report << "\nCANCELLED RIDES:\n";
        report << "Total Cancelled: " << cancelledRides.size() << "\n";
        
        FileHandler::saveReport(report.str(), "financial_report.txt");
    }
    
    void viewFileStats() {
        cout << "\n=== FILE STATISTICS ===\n";
        cout << "1. drivers.csv - " << drivers.size() << " driver records\n";
        cout << "2. riders.csv - " << riders.size() << " rider records\n";
        cout << "3. Current Ride Statistics:\n";
        cout << "   - Completed Rides: " << rideManager.getCompletedRides().size() << "\n";
        cout << "   - Cancelled Rides: " << rideManager.getCancelledRides().size() << "\n";
        cout << "   - Pending Requests: " << rideManager.getPendingRequestsCount() << "\n";
        cout << "4. Data will be saved on exit\n";
    }
    
    void addNewDriver() {
        cin.ignore();
        string name, contact, vehicle, location;
        
        cout << "\n----- ADD NEW DRIVER -----\n";
        cout << "Enter Name: ";
        getline(cin, name);
        
        if (name.empty()) {
            cout << "Name cannot be empty!\n";
            return;
        }
        
        cout << "Enter Contact: ";
        getline(cin, contact);
        
        cout << "Enter Vehicle Type (Sedan/SUV/Compact/Motorcycle): ";
        getline(cin, vehicle);
        
        cout << "Enter Location: ";
        getline(cin, location);
        
        int newID = drivers.empty() ? 1 : drivers.back().getID() + 1;
        drivers.push_back(Driver(newID, name, contact, vehicle, location));
        driverMap[newID] = &drivers.back();
        
        cout << "✓ New driver added successfully! ID: " << newID << endl;
        cout << "Note: Data will be saved when you exit or choose 'Save Data Now'\n";
    }
    
    void addNewRider() {
        cin.ignore();
        string name, contact;
        
        cout << "\n----- ADD NEW RIDER -----\n";
        cout << "Enter Name: ";
        getline(cin, name);
        
        if (name.empty()) {
            cout << "Name cannot be empty!\n";
            return;
        }
        
        cout << "Enter Contact: ";
        getline(cin, contact);
        
        // Current date as join date
        time_t now = time(0);
        tm* ltm = localtime(&now);
        stringstream ss;
        ss << (1900 + ltm->tm_year) << "-" 
           << setw(2) << setfill('0') << (1 + ltm->tm_mon) << "-" 
           << setw(2) << setfill('0') << ltm->tm_mday;
        string joinDate = ss.str();
        
        int newID = riders.empty() ? 101 : riders.back().getID() + 1;
        riders.push_back(Rider(newID, name, contact, joinDate));
        riderMap[newID] = &riders.back();
        
        cout << "✓ New rider added successfully! ID: " << newID << endl;
        cout << "Note: Data will be saved when you exit or choose 'Save Data Now'\n";
    }
    
    void rideSummaryReport() {
        cout << "\n===== RIDE SUMMARY REPORT =====\n";
        cout << "Total Rides Booked: " << rideManager.getAllRides().size() << endl;
        cout << "Completed Rides: " << rideManager.getCompletedRides().size() << endl;
        cout << "Cancelled Rides: " << rideManager.getCancelledRides().size() << endl;
        cout << "Pending Requests: " << rideManager.getPendingRequestsCount() << endl;
        
        if (!rideManager.getCompletedRides().empty()) {
            cout << "\n--- Last 5 Completed Rides ---\n";
            const vector<Ride>& completedRides = rideManager.getCompletedRides();
            int count = min(5, (int)completedRides.size());
            for (int i = 0; i < count; i++) {
                completedRides[completedRides.size() - 1 - i].display();
            }
        }
    }
    
    void driverPerformanceReport() {
        cout << "\n===== DRIVER PERFORMANCE REPORT =====\n";
        
        if (drivers.empty()) {
            cout << "No drivers available.\n";
            return;
        }
        
        // Sort drivers by rating
        vector<Driver> sortedDrivers = drivers;
        sort(sortedDrivers.begin(), sortedDrivers.end(), 
             [](const Driver& a, const Driver& b) {
                 return a.getRating() > b.getRating();
             });
        
        cout << "Top Performing Drivers (by rating):\n";
        for (int i = 0; i < min(3, (int)sortedDrivers.size()); i++) {
            cout << i+1 << ". ";
            sortedDrivers[i].display();
        }
        
        // Most active driver
        Driver* mostActive = &drivers[0];
        for (int i = 0; i < drivers.size(); i++) {
            if (drivers[i].getTotalRides() > mostActive->getTotalRides()) {
                mostActive = &drivers[i];
            }
        }
        cout << "\nMost Active Driver:\n";
        mostActive->display();
    }
    
    void riderActivityReport() {
        cout << "\n===== RIDER ACTIVITY REPORT =====\n";
        
        if (riders.empty()) {
            cout << "No riders available.\n";
            return;
        }
        
        // Find rider with most rides
        Rider* topRider = &riders[0];
        for (int i = 0; i < riders.size(); i++) {
            if (riders[i].getTotalRides() > topRider->getTotalRides()) {
                topRider = &riders[i];
            }
        }
        
        cout << "Top Rider (Most Rides):\n";
        topRider->display();
        
        cout << "\nAll Riders Activity:\n";
        for (int i = 0; i < riders.size(); i++) {
            cout << riders[i].getName() << ": " << riders[i].getTotalRides() << " rides\n";
        }
    }
    
    void systemStatistics() {
        cout << "\n===== SYSTEM STATISTICS =====\n";
        cout << "Total Drivers: " << drivers.size() << endl;
        cout << "Total Riders: " << riders.size() << endl;
        cout << "Total Rides: " << rideManager.getAllRides().size() << endl;
        cout << "Completion Rate: " 
             << (rideManager.getAllRides().empty() ? 0 : 
                (rideManager.getCompletedRides().size() * 100.0 / rideManager.getAllRides().size())) 
             << "%" << endl;
        cout << "Cancellation Rate: " 
             << (rideManager.getAllRides().empty() ? 0 : 
                (rideManager.getCancelledRides().size() * 100.0 / rideManager.getAllRides().size())) 
             << "%" << endl;
        
        // Available drivers
        int availableDrivers = 0;
        for (int i = 0; i < drivers.size(); i++) {
            if (drivers[i].getAvailability()) availableDrivers++;
        }
        cout << "Available Drivers: " << availableDrivers << "/" << drivers.size() << endl;
        
        // Active riders (with at least 1 ride)
        int activeRiders = 0;
        for (int i = 0; i < riders.size(); i++) {
            if (riders[i].getTotalRides() > 0) activeRiders++;
        }
        cout << "Active Riders: " << activeRiders << "/" << riders.size() << endl;
    }
};

// ==============================
// MAIN FUNCTION
// ==============================

int main() {
    cout << "=========================================\n";
    cout << "   MINI-UBER RIDE SHARING SYSTEM\n";
    cout << "   Data Structures Semester Project\n";
    cout << "   WITH ADVANCED RIDE MANAGEMENT\n";
    cout << "=========================================\n";
    cout << "\nFeatures Added:\n";
    cout << "1. Ride Request Queue System\n";
    cout << "2. Ride Management Menu\n";
    cout << "3. Cancellation List\n";
    cout << "4. Complete Ride Statistics\n";
    cout << "5. Individual Ride Processing\n";
    cout << "=========================================\n";
    
    // Create system instance
    RideSharingSystem system;
    
    // Show main menu
    system.showMainMenu();
    
    return 0;
}