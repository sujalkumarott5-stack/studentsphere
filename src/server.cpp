

#include "httplib.h"
#include "json.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <exception>

using json = nlohmann::json;
using namespace std;

struct Student {
    int id;
    string name;
    int age;
    string course;
};

const string DATA_FILE = "data/students.txt";

// Read all student records from the file.
vector<Student> loadStudents() {
    vector<Student> students;
    ifstream file(DATA_FILE);

    Student student;

    while (file >> student.id
                >> quoted(student.name)
                >> student.age
                >> quoted(student.course)) {
        students.push_back(student);
    }

    return students;
}

// Save all student records to the file.
bool saveStudents(const vector<Student>& students) {
    ofstream file(DATA_FILE);

    if (!file) {
        return false;
    }

    for (const auto& student : students) {
        file << student.id << " "
             << quoted(student.name) << " "
             << student.age << " "
             << quoted(student.course) << '\n';
    }

    return file.good();
}

// Convert a student to JSON.
json studentToJson(const Student& student) {
    return {
        {"id", student.id},
        {"name", student.name},
        {"age", student.age},
        {"course", student.course}
    };
}

// Send a JSON error response.
void sendError(
    httplib::Response& res,
    int status,
    const string& message
) {
    res.status = status;
    res.set_content(
        json{{"error", message}}.dump(),
        "application/json"
    );
}

// Validate student data received from the frontend.
bool parseStudent(
    const json& body,
    Student& student,
    string& error
) {
    if (!body.is_object() ||
        !body.contains("id") ||
        !body.contains("name") ||
        !body.contains("age") ||
        !body.contains("course")) {
        error = "Missing student fields.";
        return false;
    }

    if (!body["id"].is_number_integer() ||
        !body["name"].is_string() ||
        !body["age"].is_number_integer() ||
        !body["course"].is_string()) {
        error = "Invalid student field types.";
        return false;
    }

    student.id = body["id"].get<int>();
    student.name = body["name"].get<string>();
    student.age = body["age"].get<int>();
    student.course = body["course"].get<string>();

    if (student.id <= 0) {
        error = "Student ID must be positive.";
        return false;
    }

    if (student.name.empty() ||
        student.course.empty()) {
        error = "Name and course cannot be empty.";
        return false;
    }

    if (student.age < 1 || student.age > 120) {
        error = "Age must be between 1 and 120.";
        return false;
    }

    return true;
}

int main() {
    filesystem::create_directories("data");

    httplib::Server server;

    // Allow frontend requests from another local port.
    server.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods",
         "GET, POST, PUT, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });

    server.Options(
        R"(/api/students(?:/.*)?)",
        [](const httplib::Request&, httplib::Response& res) {
            res.status = 204;
        }
    );

    // GET /api/students
    // Retrieve all student records.
    server.Get(
        "/api/students",
        [](const httplib::Request&, httplib::Response& res) {
            json result = json::array();

            for (const auto& student : loadStudents()) {
                result.push_back(studentToJson(student));
            }

            res.set_content(result.dump(2), "application/json");
        }
    );

    // POST /api/students
    // Add a new student.
    server.Post(
        "/api/students",
        [](const httplib::Request& req, httplib::Response& res) {
            try {
                json body = json::parse(req.body);
                Student student{};
                string error;

                if (!parseStudent(body, student, error)) {
                    sendError(res, 400, error);
                    return;
                }

                auto students = loadStudents();

                auto existing = find_if(
                    students.begin(),
                    students.end(),
                    [&](const Student& item) {
                        return item.id == student.id;
                    }
                );

                if (existing != students.end()) {
                    sendError(res, 409, "Student ID already exists.");
                    return;
                }

                students.push_back(student);

                if (!saveStudents(students)) {
                    sendError(res, 500, "Could not save student records.");
                    return;
                }

                res.status = 201;
                res.set_content(
                    studentToJson(student).dump(),
                    "application/json"
                );
            }
            catch (const exception& e) {
                sendError(res, 400, e.what());
            }
        }
    );

    // PUT /api/students/{id}
    // Update an existing student's name, age, and course.
    server.Put(
        R"(/api/students/(\d+))",
        [](const httplib::Request& req, httplib::Response& res) {
            try {
                int id = stoi(req.matches[1].str());
                json body = json::parse(req.body);

                if (!body.is_object() ||
                    !body.contains("name") ||
                    !body.contains("age") ||
                    !body.contains("course") ||
                    !body["name"].is_string() ||
                    !body["age"].is_number_integer() ||
                    !body["course"].is_string()) {
                    sendError(res, 400, "Invalid student data.");
                    return;
                }

                string name = body["name"].get<string>();
                int age = body["age"].get<int>();
                string course = body["course"].get<string>();

                if (name.empty() || course.empty()) {
                    sendError(
                        res, 400,
                        "Name and course cannot be empty."
                    );
                    return;
                }

                if (age < 1 || age > 120) {
                    sendError(
                        res, 400,
                        "Age must be between 1 and 120."
                    );
                    return;
                }

                auto students = loadStudents();

                auto it = find_if(
                    students.begin(),
                    students.end(),
                    [id](const Student& item) {
                        return item.id == id;
                    }
                );

                if (it == students.end()) {
                    sendError(res, 404, "Student not found.");
                    return;
                }

                it->name = name;
                it->age = age;
                it->course = course;

                if (!saveStudents(students)) {
                    sendError(res, 500, "Could not save student records.");
                    return;
                }

                res.set_content(
                    studentToJson(*it).dump(),
                    "application/json"
                );
            }
            catch (const exception& e) {
                sendError(res, 400, e.what());
            }
        }
    );

    // DELETE /api/students/{id}
    // Delete a student by ID.
    server.Delete(
        R"(/api/students/(\d+))",
        [](const httplib::Request& req, httplib::Response& res) {
            try {
                int id = stoi(req.matches[1].str());

                auto students = loadStudents();
                size_t oldSize = students.size();

                students.erase(
                    remove_if(
                        students.begin(),
                        students.end(),
                        [id](const Student& student) {
                            return student.id == id;
                        }
                    ),
                    students.end()
                );

                if (students.size() == oldSize) {
                    sendError(res, 404, "Student not found.");
                    return;
                }

                if (!saveStudents(students)) {
                    sendError(
                        res, 500,
                        "Could not save student records."
                    );
                    return;
                }

                res.set_content(
                    json{
                        {"message", "Student deleted successfully."}
                    }.dump(),
                    "application/json"
                );
            }
            catch (const exception& e) {
                sendError(res, 400, e.what());
            }
        }
    );

    cout << "====================================\n";
    cout << "       StudentSphere Server\n";
    cout << "====================================\n";
    cout << "API: http://localhost:8080/api/students\n";
    cout << "Server is starting...\n";

    if (!server.listen("127.0.0.1", 8080)) {
        cerr << "Failed to start server.\n";
        cerr << "Check whether port 8080 is already in use.\n";
        return 1;
    }

    return 0;
}

