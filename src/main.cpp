
#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

using namespace std;

const string FILE_NAME = "data/students.txt";

class Student {
public:
    int id;
    string name;
    int age;
    string course;

    Student(int studentId, string studentName,
            int studentAge, string studentCourse)
        : id(studentId),
          name(studentName),
          age(studentAge),
          course(studentCourse) {}

    void display() const {
        cout << "\nStudent ID : " << id;
        cout << "\nName       : " << name;
        cout << "\nAge        : " << age;
        cout << "\nCourse     : " << course << "\n";
    }
};

vector<Student> students;

// Save all student records to a file
bool saveStudents() {
    ofstream file(FILE_NAME);

    if (!file) {
        cerr << "Error: Could not save student records.\n";
        return false;
    }

    for (const Student& s : students) {
        file << s.id << ' '
             << s.age << ' '
             << quoted(s.name) << ' '
             << quoted(s.course) << '\n';
    }

    if (!file) {
        cerr << "Error while writing student records.\n";
        return false;
    }

    return true;
}

// Load records when the program starts
void loadStudents() {
    ifstream file(FILE_NAME);

    if (!file) {
        cout << "No saved records found. Starting fresh.\n";
        return;
    }

    int id, age;
    string name, course;

    while (file >> id >> age
                >> quoted(name) >> quoted(course)) {
        if (id <= 0 || age <= 0) {
            cerr << "Skipping an invalid student record.\n";
            continue;
        }

        bool duplicate = false;

        for (const Student& s : students) {
            if (s.id == id) {
                duplicate = true;
                break;
            }
        }

        if (!duplicate) {
            students.emplace_back(id, name, age, course);
        }
    }

    cout << students.size()
         << " student record(s) loaded.\n";
}

// Add a new student
void addStudent() {
    int id, age;
    string name, course;

    cout << "\nEnter Student ID: ";

    if (!(cin >> id) || id <= 0) {
        cout << "Invalid ID. Enter a positive number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    for (const Student& s : students) {
        if (s.id == id) {
            cout << "This student ID already exists.\n";
            return;
        }
    }

    cout << "Enter Name: ";
    cin >> ws;
    getline(cin, name);

    if (name.empty()) {
        cout << "Name cannot be empty.\n";
        return;
    }

    cout << "Enter Age: ";

    if (!(cin >> age) || age <= 0) {
        cout << "Invalid age. Enter a positive number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }

    cout << "Enter Course: ";
    cin >> ws;
    getline(cin, course);

    if (course.empty()) {
        cout << "Course cannot be empty.\n";
        return;
    }

    students.emplace_back(id, name, age, course);

    if (saveStudents()) {
        cout << "\nStudent added and saved successfully!\n";
    } else {
        // Roll back the in-memory addition if saving fails.
        students.pop_back();
        cout << "Student was not added because saving failed.\n";
    }
}

// Display all students
void displayStudents() {
    if (students.empty()) {
        cout << "\nNo student records found.\n";
        return;
    }

    cout << "\n===== ALL STUDENT RECORDS =====\n";

    for (const Student& s : students) {
        s.display();
        cout << "------------------------------\n";
    }
}

// Convert text to lowercase for case-insensitive search
string toLowerCase(string text) {
    transform(text.begin(), text.end(), text.begin(),
        [](unsigned char ch) {
            return static_cast<char>(tolower(ch));
        });

    return text;
}

// Search by student ID or name
void searchStudent() {
    if (students.empty()) {
        cout << "\nNo student records found.\n";
        return;
    }

    string query;

    cout << "\nEnter Student ID or Name: ";
    cin >> ws;
    getline(cin, query);

    if (query.empty()) {
        cout << "Please enter an ID or name.\n";
        return;
    }

    string searchText = toLowerCase(query);
    bool found = false;

    cout << "\n===== SEARCH RESULTS =====\n";

    for (const Student& s : students) {
        bool idMatch = to_string(s.id) == query;

        bool nameMatch =
            toLowerCase(s.name).find(searchText) != string::npos;

        if (idMatch || nameMatch) {
            s.display();
            cout << "------------------------------\n";
            found = true;
        }
    }

    if (!found) {
        cout << "No matching student found.\n";
    }
}

// Main function
int main() {
    loadStudents();

    int choice = 0;

    do {
        cout << "\n===== STUDENT RECORD SYSTEM =====\n";
        cout << "1. Add Student\n";
        cout << "2. Display All Students\n";
        cout << "3. Search by ID or Name\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            cout << "Please enter a valid number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = 0;
            continue;
        }

        switch (choice) {
            case 1:
                addStudent();
                break;

            case 2:
                displayStudents();
                break;

            case 3:
                searchStudent();
                break;

            case 4:
                cout << "Exiting Student Record System.\n";
                break;

            default:
                cout << "Invalid choice. Try again.\n";
        }

    } while (choice != 4);

    return 0;
}
