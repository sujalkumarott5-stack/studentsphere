# 🎓 StudentSphere — Student Record Management System

A student record management application built with **C++, HTML, CSS, and JavaScript**. StudentSphere provides a simple dashboard for managing student information, searching records, updating details, and exporting data to CSV.

## ✨ Features

- ➕ Add student records
- 📋 View all student records
- 🔍 Search students
- ✏️ Edit student details
- 🗑️ Delete student records
- 📊 Dashboard statistics
- 📥 Export student records to CSV
- 🌙 Dark mode
- 💾 File-based data storage
- 🔗 REST API for CRUD operations

## 🛠️ Tech Stack

| Technology | Purpose |
|---|---|
| C++ | Backend and server logic |
| HTML | Webpage structure |
| CSS | Styling and responsive layout |
| JavaScript | Frontend interactions |
| cpp-httplib | HTTP server |
| nlohmann/json | JSON data handling |
| Git and GitHub | Version control |

## 📁 Project Structure

```text
StudentSphere/
├── data/
│   └── students.txt
├── include/
│   ├── httplib.h
│   └── json.hpp
├── public/
│   ├── index.html
│   ├── style.css
│   └── script.js
├── src/
│   ├── main.cpp
│   └── server.cpp
├── .gitignore
└── README.md
```

## ⚙️ Requirements

- Windows operating system
- Visual Studio Code
- GCC C++ compiler (MinGW-w64 / MSYS2 UCRT64)
- Git
- Modern web browser

## 🚀 Installation and Setup

### 1. Clone the repository

```bash
git clone https://github.com/sujalkumarott5-stack/studentsphere.git
cd studentsphere
```

### 2. Compile the backend

Run this command from the project root:

```bash
g++ -std=c++17 -Wall -Wextra -I include src/server.cpp -o server.exe -lws2_32
```

### 3. Start the server

```powershell
.\server.exe
```

Keep the terminal running while using the application.

### 4. Open the frontend

Open `public/index.html` in your browser.

The backend API runs at:

`http://localhost:8080`

If the browser cannot connect to the API, check that the server is running and that the frontend origin is permitted by the server's CORS configuration.

## 🔌 API Endpoints

| HTTP Method | Endpoint | Description |
|---|---|---|
| GET | `/api/students` | Retrieve all student records |
| POST | `/api/students` | Add a student |
| PUT | `/api/students/{id}` | Update a student |
| DELETE | `/api/students/{id}` | Delete a student |

## 💾 Data Storage

Student records are stored locally in `data/students.txt`. Back up this file to preserve your records.

## 🎯 Project Objectives

- Practice C++ programming and file handling.
- Understand HTTP servers and REST APIs.
- Learn frontend and backend integration.
- Implement CRUD (Create, Read, Update, Delete) operations.
- Build a practical project for a programming portfolio.

## 🔮 Future Enhancements

- Integrate SQLite or MySQL.
- Add authentication and user accounts.
- Add pagination and improved validation.
- Deploy the application online.
- Add automated testing.

## 👨‍💻 Author

**Sujal Kumar**

GitHub: [@sujalkumarott5-stack](https://github.com/sujalkumarott5-stack)

## 📄 License

This project is intended for educational and learning purposes. A formal license can be added when one is selected.
