

const API_URL = "http://localhost:8080/api/students";

const form = document.getElementById("studentForm");
const list = document.getElementById("studentList");
const search = document.getElementById("searchInput");
const total = document.getElementById("totalStudents");
const totalCourses = document.getElementById("totalCourses");
const averageAge = document.getElementById("averageAge");
const latestStudent = document.getElementById("latestStudent");
const recordCount = document.getElementById("recordCount");
const message = document.getElementById("message");
const submitButton = document.getElementById("submitButton");
const themeToggle = document.getElementById("themeToggle");

let students = [];
let editingId = null;
let isLoading = false;

async function apiRequest(url, options = {}) {
    const response = await fetch(url, {
        ...options,
        headers: {
            "Content-Type": "application/json",
            ...options.headers
        }
    });

    const data = await response.json().catch(() => ({}));

    if (!response.ok) {
        throw new Error(
            data.error || `Request failed: ${response.status}`
        );
    }

    return data;
}

function showMessage(text, isError = false) {
    message.textContent = text;
    message.classList.toggle("error", isError);
}

function updateStatistics() {
    total.textContent = students.length;

    recordCount.textContent =
        `${students.length} ${students.length === 1 ? "record" : "records"}`;

    const courses = new Set(
        students
            .map(s => String(s.course).trim().toLowerCase())
            .filter(Boolean)
    );

    totalCourses.textContent = courses.size;

    if (students.length) {
        const sum = students.reduce(
            (result, student) => result + Number(student.age), 0
        );

        averageAge.textContent = (sum / students.length).toFixed(1);

        latestStudent.textContent =
            students[students.length - 1].name || "—";
        latestStudent.title = latestStudent.textContent;
    } else {
        averageAge.textContent = "—";
        latestStudent.textContent = "—";
        latestStudent.title = "";
    }
}

function createElement(tag, className, text = "") {
    const element = document.createElement(tag);
    element.className = className;
    element.textContent = text;
    return element;
}

function renderStudents() {
    updateStatistics();
    list.replaceChildren();

    const query = search.value.trim().toLowerCase();

    const filtered = students.filter(student =>
        [student.id, student.name, student.age, student.course]
            .some(value => String(value).toLowerCase().includes(query))
    );

    if (!filtered.length) {
        const empty = createElement("div", "empty-state");
        empty.append(
            createElement("span", "empty-icon", "✧"),
            createElement(
                "h3",
                "",
                students.length ? "No matches found" : "Your student space awaits"
            ),
            createElement(
                "p",
                "",
                students.length
                    ? "Try another name, ID or course."
                    : "Add your first student using the form."
            )
        );
        list.appendChild(empty);
        return;
    }

    filtered.forEach(student => {
        const card = createElement("article", "student-card");

        const initials = String(student.name || "?")
            .trim()
            .split(/\s+/)
            .slice(0, 2)
            .map(word => word[0])
            .join("")
            .toUpperCase();

        const avatar = createElement("div", "student-avatar", initials);
        const details = createElement("div", "student-details");
        const nameRow = createElement("div", "student-name-row");

        nameRow.append(
            createElement("h3", "student-name", student.name),
            createElement("span", "student-id", `ID: ${student.id}`)
        );

        const meta = createElement("div", "student-meta");
        meta.append(
            createElement("span", "", `◷ Age: ${student.age}`),
            createElement("span", "", `▤ ${student.course}`)
        );

        details.append(nameRow, meta);

        const actions = createElement("div", "student-actions");

        const editButton = createElement("button", "edit-button", "Edit");
        editButton.type = "button";
        editButton.addEventListener("click", () => startEditing(student));

        const deleteButton = createElement("button", "delete-button", "Delete");
        deleteButton.type = "button";
        deleteButton.addEventListener("click", () => deleteStudent(student, deleteButton));

        actions.append(editButton, deleteButton);
        card.append(avatar, details, actions);
        list.appendChild(card);
    });
}

async function loadStudents() {
    if (isLoading) return;
    isLoading = true;

    try {
        const data = await apiRequest(API_URL);

        if (!Array.isArray(data)) {
            throw new Error("The server did not return a student list.");
        }

        students = data;
        renderStudents();
    } catch (error) {
        console.error("Loading students failed:", error);

        total.textContent = "—";
        totalCourses.textContent = "—";
        averageAge.textContent = "—";
        latestStudent.textContent = "—";
        recordCount.textContent = "Unavailable";

        list.replaceChildren();

        const errorBox = createElement("div", "empty-state error-state");
        errorBox.append(
            createElement("span", "empty-icon", "!"),
            createElement("h3", "", "Unable to load records"),
            createElement(
                "p", "",
                "Start server.exe in your project folder, then refresh this page."
            )
        );

        list.appendChild(errorBox);
        showMessage(error.message, true);
    } finally {
        isLoading = false;
    }
}

// Fill the form with the selected student's details.
function startEditing(student) {
    editingId = student.id;

    document.getElementById("studentId").value = student.id;
    document.getElementById("studentName").value = student.name;
    document.getElementById("studentAge").value = student.age;
    document.getElementById("studentCourse").value = student.course;

    // The ID is fixed during editing.
    document.getElementById("studentId").disabled = true;

    submitButton.innerHTML = "<span>✓</span> Save changes";

    showMessage(`Editing ${student.name}. Update the fields above.`);
    document.getElementById("add-student").scrollIntoView({
        behavior: "smooth",
        block: "start"
    });

    document.getElementById("studentName").focus();
}

// Return the form to Add Student mode.
function cancelEditing() {
    editingId = null;
    form.reset();

    document.getElementById("studentId").disabled = false;
    submitButton.innerHTML = "<span>＋</span> Add student";
}

// Add a new student or update an existing student.
form.addEventListener("submit", async event => {
    event.preventDefault();

    const student = {
        id: Number(document.getElementById("studentId").value),
        name: document.getElementById("studentName").value.trim(),
        age: Number(document.getElementById("studentAge").value),
        course: document.getElementById("studentCourse").value.trim()
    };

    if (
        !Number.isSafeInteger(student.id) ||
        student.id <= 0 ||
        !student.name ||
        !Number.isInteger(student.age) ||
        student.age < 1 ||
        student.age > 120 ||
        !student.course
    ) {
        showMessage("Please enter valid student details.", true);
        return;
    }

    submitButton.disabled = true;

    try {
        if (editingId !== null) {
            showMessage("Saving student changes...");

            await apiRequest(
                `${API_URL}/${encodeURIComponent(editingId)}`,
                {
                    method: "PUT",
                    body: JSON.stringify({
                        name: student.name,
                        age: student.age,
                        course: student.course
                    })
                }
            );

            const updatedName = student.name;
            cancelEditing();
            await loadStudents();
            showMessage(`${updatedName} updated successfully!`);
        } else {
            showMessage("Adding student...");

            await apiRequest(API_URL, {
                method: "POST",
                body: JSON.stringify(student)
            });

            form.reset();
            await loadStudents();
            showMessage(`${student.name} added successfully!`);
        }
    } catch (error) {
        console.error("Saving student failed:", error);
        showMessage(error.message, true);
    } finally {
        submitButton.disabled = false;
    }
});

async function deleteStudent(student, button) {
    if (!confirm(`Delete ${student.name} (ID: ${student.id}) permanently?`)) {
        return;
    }

    button.disabled = true;

    try {
        await apiRequest(
            `${API_URL}/${encodeURIComponent(student.id)}`,
            { method: "DELETE" }
        );

        if (editingId === student.id) {
            cancelEditing();
        }

        await loadStudents();
        showMessage(`${student.name} deleted successfully.`);
    } catch (error) {
        showMessage(error.message, true);
        button.disabled = false;
    }
}

search.addEventListener("input", renderStudents);

// Theme toggle
function setTheme(isDark) {
    document.body.classList.toggle("dark-mode", isDark);
    themeToggle.textContent = isDark ? "☀" : "☾";
    themeToggle.title = isDark ? "Light mode" : "Dark mode";

    try {
        localStorage.setItem(
            "studentSphereTheme",
            isDark ? "dark" : "light"
        );
    } catch (_) {
        // Theme works even when browser storage is unavailable.
    }
}

themeToggle.addEventListener("click", () => {
    setTheme(!document.body.classList.contains("dark-mode"));
});

try {
    setTheme(localStorage.getItem("studentSphereTheme") === "dark");
} catch (_) {
    setTheme(false);
}

// Sidebar navigation
document.querySelectorAll(".nav-link").forEach(link => {
    link.addEventListener("click", () => {
        document.querySelectorAll(".nav-link").forEach(item => {
            item.classList.remove("active");
        });

        link.classList.add("active");
    });
});

// Initial load
loadStudents();

const exportButton = document.getElementById("exportButton");

exportButton.addEventListener("click", () => {
    if (students.length === 0) {
        showMessage("There are no student records to export.", true);
        return;
    }

    const headers = ["ID", "Name", "Age", "Course"];

    const escapeCSV = value => {
        let text = String(value ?? "");

        // Protect spreadsheet applications from formula injection.
        if (/^[\s]*[=+\-@]/.test(text)) {
            text = "'" + text;
        }

        return `"${text.replace(/"/g, '""')}"`;
    };

    const rows = students.map(student => [
        student.id,
        student.name,
        student.age,
        student.course
    ]);

    const csv = [headers, ...rows]
        .map(row => row.map(escapeCSV).join(","))
        .join("\r\n");

    const blob = new Blob(
        ["\uFEFF" + csv],
        { type: "text/csv;charset=utf-8;" }
    );

    const url = URL.createObjectURL(blob);
    const link = document.createElement("a");

    link.href = url;
    link.download = "students.csv";

    document.body.appendChild(link);
    link.click();
    link.remove();

    URL.revokeObjectURL(url);
    showMessage("Student records exported successfully!");
});
#adding