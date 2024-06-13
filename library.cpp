#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <conio.h>
#include <exception>
#include <stdexcept>

using namespace std;

class Course;
class Grade;

// Base class for Person
class Person {
protected:
    string name;
    int id;
    string password;

public:
    Person(string n, int i, string pwd) : name(n), id(i), password(pwd) {}
    virtual ~Person() {}

    string getName() const { return name; }
    int getId() const { return id; }
    bool checkPassword(const string& pwd) const { return password == pwd; }
    void setPassword(const string& pwd) { password = pwd; }
    string getPassword() const { return password; }
};

// Grade class
class Grade {
private:
    Course* course;
    char grade;
    int marks;

public:
    Grade(Course* c, char g, int m) : course(c), grade(g), marks(m) {}

    Course* getCourse() const;
    char getGrade() const { return grade; }
    int getMarks() const { return marks; }

    void save(ofstream& out) const;
    static Grade load(ifstream& in, const vector<Course>& courses);
};

// Forward declaration of Student and Course classes
class Student;
class Instructor;

// Course class
class Course {
private:
    string courseName;
    string courseCode;
    vector<Student*> students;
    Instructor* instructor;

public:
    Course(string name, string code) : courseName(name), courseCode(code), instructor(nullptr) {}

    void enrollStudent(Student* student);
    void assignInstructor(Instructor* instr);

    string getCourseName() const { return courseName; }
    string getCourseCode() const { return courseCode; }
    vector<Student*> getStudents() const { return students; }
    Instructor* getInstructor() const { return instructor; }
};

// Student class
class Student : public Person {
private:
    vector<Grade> grades;
    vector<Course*> enrolledCourses;

public:
    Student(string n, int i, string pwd) : Person(n, i, pwd) {}

    void enrollCourse(Course* course);
    void addGrade(const Grade& grade);
    vector<Grade> getGrades() const { return grades; }
    vector<Course*> getEnrolledCourses() const { return enrolledCourses; }

    double getGPA() const;

    void saveGrades(ofstream& out) const;
    void loadGrades(ifstream& in, const vector<Course>& courses);
};

// Instructor class
class Instructor : public Person {
private:
    string department;
    vector<Course*> assignedCourses;

public:
    Instructor(string n, int i, string dept, string pwd) : Person(n, i, pwd), department(dept) {}

    void assignCourse(Course* course);
    void addGradeToStudent(Student* student, Course* course, char grade, int marks);

    string getDepartment() const { return department; }
    vector<Course*> getAssignedCourses() const { return assignedCourses; }
};

// Method implementations for Grade class
Course* Grade::getCourse() const {
    return course;
}

void Grade::save(ofstream& out) const {
    out << course->getCourseCode() << " " << grade << " " << marks << endl;
}

Grade Grade::load(ifstream& in, const vector<Course>& courses) {
    string courseCode;
    char grade;
    int marks;
    in >> courseCode >> grade >> marks;

    auto it = find_if(courses.begin(), courses.end(), [&courseCode](const Course& c) {
        return c.getCourseCode() == courseCode;
    });

    // if (it != courses.end()) {
    //     return Grade(&(*it), grade, marks);
    // } else {
    //     throw runtime_error("Course not found during grade loading.");
    // }
}

// Method implementations for Student class
void Student::enrollCourse(Course* course) {
    enrolledCourses.push_back(course);
}

void Student::addGrade(const Grade& grade) {
    grades.push_back(grade);
}

double Student::getGPA() const {
    double total = 0;
    for (const Grade& grade : grades) {
        switch (grade.getGrade()) {
            case 'A': total += 4.0; break;
            case 'B': total += 3.0; break;
            case 'C': total += 2.0; break;
            case 'D': total += 1.0; break;
            case 'F': total += 0.0; break;
        }
    }
    return grades.empty() ? 0 : total / grades.size();
}

void Student::saveGrades(ofstream& out) const {
    for (const Grade& grade : grades) {
        grade.save(out);
    }
}

void Student::loadGrades(ifstream& in, const vector<Course>& courses) {
    while (in) {
        try {
            grades.push_back(Grade::load(in, courses));
        } catch (const runtime_error& e) {
            break;
        }
    }
}

// Method implementations for Instructor class
void Instructor::assignCourse(Course* course) {
    assignedCourses.push_back(course);
}

void Instructor::addGradeToStudent(Student* student, Course* course, char grade, int marks) {
    Grade newGrade(course, grade, marks);
    student->addGrade(newGrade);
}

// Method implementations for Course class
void Course::enrollStudent(Student* student) {
    students.push_back(student);
    student->enrollCourse(this);
}

void Course::assignInstructor(Instructor* instr) {
    instructor = instr;
    instr->assignCourse(this);
}

// University class
class University {
private:
    string name;
    vector<Student> students;
    vector<Instructor> instructors;
    vector<Course> courses;

    void loadStudents();
    void loadInstructors();
    void loadCourses();
    void saveStudents();
    void saveInstructors();
    void saveCourses();

public:
    University(string n) : name(n) {
        loadStudents();
        loadInstructors();
        loadCourses();
    }

    ~University() {
        saveStudents();
        saveInstructors();
        saveCourses();
    }

    void addStudent(const Student& student);
    void removeStudent(int studentId);
    void addInstructor(const Instructor& instructor);
    void removeInstructor(int instructorId);
    void addCourse(const Course& course);
    void removeCourse(const string& courseCode);
    vector<Student>& getStudents() { return students; }
    vector<Instructor>& getInstructors() { return instructors; }
    vector<Course>& getCourses() { return courses; }

    Course* findCourseByCode(const string& code);
    Student* findStudentById(int id);
    Instructor* findInstructorById(int id);
};

// University class methods for loading and saving data
void University::loadStudents() {
    ifstream file("students.txt");
    if (!file) return;
    string name, password;
    int id;
    while (file >> name >> id >> password) {
        Student student(name, id, password);
        student.loadGrades(file, courses);
        students.push_back(student);
    }
}

void University::loadInstructors() {
    ifstream file("instructors.txt");
    if (!file) return;
    string name, password, department;
    int id;
    while (file >> name >> id >> department >> password) {
        instructors.push_back(Instructor(name, id, department, password));
    }
}

void University::loadCourses() {
    ifstream file("courses.txt");
    if (!file) return;
    string name, code;
    while (file >> name >> code) {
        courses.push_back(Course(name, code));
    }
}

void University::saveStudents() {
    ofstream file("students.txt");
    for (const Student& student : students) {
        file << student.getName() << " " << student.getId() << " " << student.getPassword() << endl;
        student.saveGrades(file);
    }
}

void University::saveInstructors() {
    ofstream file("instructors.txt");
    for (const Instructor& instructor : instructors) {
        file << instructor.getName() << " " << instructor.getId() << " " << instructor.getDepartment() << " " << instructor.getPassword() << endl;
    }
}

void University::saveCourses() {
    ofstream file("courses.txt");
    for (const Course& course : courses) {
        file << course.getCourseName() << " " << course.getCourseCode() << endl;
    }
}

// University class methods for managing data
void University::addStudent(const Student& student) {
    students.push_back(student);
}

void University::removeStudent(int studentId) {
    students.erase(remove_if(students.begin(), students.end(), [&studentId](const Student& s) {
        return s.getId() == studentId;
    }), students.end());
}

void University::addInstructor(const Instructor& instructor) {
    instructors.push_back(instructor);
}

void University::removeInstructor(int instructorId) {
    instructors.erase(remove_if(instructors.begin(), instructors.end(), [&instructorId](const Instructor& i) {
        return i.getId() == instructorId;
    }), instructors.end());
}

void University::addCourse(const Course& course) {
    courses.push_back(course);
}

void University::removeCourse(const string& courseCode) {
    courses.erase(remove_if(courses.begin(), courses.end(), [&courseCode](const Course& c) {
        return c.getCourseCode() == courseCode;
    }), courses.end());
}

Course* University::findCourseByCode(const string& code) {
    auto it = find_if(courses.begin(), courses.end(), [&code](const Course& c) {
        return c.getCourseCode() == code;
    });
    return it != courses.end() ? &(*it) : nullptr;
}

Student* University::findStudentById(int id) {
    auto it = find_if(students.begin(), students.end(), [&id](const Student& s) {
        return s.getId() == id;
    });
    return it != students.end() ? &(*it) : nullptr;
}

Instructor* University::findInstructorById(int id) {
    auto it = find_if(instructors.begin(), instructors.end(), [&id](const Instructor& i) {
        return i.getId() == id;
    });
    return it != instructors.end() ? &(*it) : nullptr;
}

// Function to securely get password input
string getPassword() {
    string password;
    char ch;
    cout << endl << "Enter Password: ";
    while ((ch = _getch()) != '\r') {
        if (ch == '\b') {
            if (!password.empty()) {
                cout << "\b \b";
                password.pop_back();
            }
        } else {
            password.push_back(ch);
            cout << '*';
        }
    }
    cout << endl;
    return password;
}

// Admin menu function
void adminMenu(University& university) {
    int choice;
    do {
        cout << endl << "\t\t\t\t\t\t**** Admin Menu ****" << endl;
        cout << "\t\t\t\t\t\t1. Add Student" << endl;
        cout << "\t\t\t\t\t\t2. Remove Student" << endl;
        cout << "\t\t\t\t\t\t3. Add Instructor" << endl;
        cout << "\t\t\t\t\t\t4. Remove Instructor" << endl;
        cout << "\t\t\t\t\t\t5. Add Course" << endl;
        cout << "\t\t\t\t\t\t6. Remove Course" << endl;
        cout << "\t\t\t\t\t\t0. Log Out" << endl;
        cout << "\t\t\t\t\t\tEnter your choice: ";
        try {
            cin >> choice;
            if(cin.fail()) throw invalid_argument("Invalid input");

            switch (choice) {
                case 1: {
                    string name, password;
                    int id;
                    cout << "Enter student name: ";
                    cin >> name;
                    cout << "Enter student ID: ";
                    cin >> id;
                    if(cin.fail()) throw invalid_argument("Invalid ID");
                    password = getPassword();
                    university.addStudent(Student(name, id, password));
                    cout << "Student added successfully." << endl;
                    break;
                }
                case 2: {
                    int id;
                    cout << "Enter student ID: ";
                    cin >> id;
                    if(cin.fail()) throw invalid_argument("Invalid ID");
                    university.removeStudent(id);
                    cout << "Student removed successfully." << endl;
                    break;
                }
                case 3: {
                    string name, password, department;
                    int id;
                    cout << "Enter instructor name: ";
                    cin >> name;
                    cout << "Enter instructor ID: ";
                    cin >> id;
                    if(cin.fail()) throw invalid_argument("Invalid ID");
                    cout << "Enter department: ";
                    cin >> department;
                    password = getPassword();
                    university.addInstructor(Instructor(name, id, department, password));
                    cout << "Instructor added successfully." << endl;
                    break;
                }
                case 4: {
                    int id;
                    cout << "Enter instructor ID: ";
                    cin >> id;
                    if(cin.fail()) throw invalid_argument("Invalid ID");
                    university.removeInstructor(id);
                    cout << "Instructor removed successfully." << endl;
                    break;
                }
                case 5: {
                    string name, code;
                    cout << "Enter course name: ";
                    cin >> name;
                    cout << "Enter course code: ";
                    cin >> code;
                    university.addCourse(Course(name, code));
                    cout << "Course added successfully." << endl;
                    break;
                }
                case 6: {
                    string code;
                    cout << "Enter course code: ";
                    cin >> code;
                    university.removeCourse(code);
                    cout << "Course removed successfully." << endl;
                    break;
                }
                case 0:
                    cout << "Logging out..." << endl;
                    break;
                default:
                    cout << "Invalid choice. Please try again." << endl;
            }
        } catch (const exception& e) {
            cout << "An error occurred: " << e.what() << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

    } while (choice != 0);
}

// Student menu function
void studentMenu(Student* student) {
    int choice;
    do {
        cout << endl << "\t\t\t\t\t\t**** Student Menu ****" << endl;
        cout << "\t\t\t\t\t\t1. View Enrolled Courses" << endl;
        cout << "\t\t\t\t\t\t2. View Grades" << endl;
        cout << "\t\t\t\t\t\t3. View GPA" << endl;
        cout << "\t\t\t\t\t\t0. Log Out" << endl;
        cout << "\t\t\t\t\t\tEnter your choice: ";
        try {
            cin >> choice;
            if(cin.fail()) throw invalid_argument("Invalid input");

            switch (choice) {
                case 1: {
                    cout << "Enrolled Courses:" << endl;
                    for (Course* course : student->getEnrolledCourses()) {
                        cout << course->getCourseName() << " (" << course->getCourseCode() << ")" << endl;
                    }
                    break;
                }
                case 2: {
                    cout << "Grades:" << endl;
                    for (const Grade& grade : student->getGrades()) {
                        cout << grade.getCourse()->getCourseName() << ": " << grade.getGrade() << " (" << grade.getMarks() << " marks)" << endl;
                    }
                    break;
                }
                case 3: {
                    cout << "GPA: " << student->getGPA() << endl;
                    break;
                }
                case 0:
                    cout << "Logging out..." << endl;
                    break;
                default:
                    cout << "Invalid choice. Please try again." << endl;
            }
        } catch (const exception& e) {
            cout << "An error occurred: " << e.what() << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

    } while (choice != 0);
}

// Instructor menu function
void instructorMenu(Instructor* instructor, University& university) {
    int choice;
    do {
        cout << endl << "\t\t\t\t\t\t**** Instructor Menu ****" << endl;
        cout << "\t\t\t\t\t\t1. View Assigned Courses" << endl;
        cout << "\t\t\t\t\t\t2. Add Grade for Student" << endl;
        cout << "\t\t\t\t\t\t0. Log Out" << endl;
        cout << "\t\t\t\t\t\tEnter your choice: ";
        try {
            cin >> choice;
            if(cin.fail()) throw invalid_argument("Invalid input");

            switch (choice) {
                case 1: {
                    cout << "Assigned Courses:" << endl;
                    for (Course* course : instructor->getAssignedCourses()) {
                        cout << course->getCourseName() << " (" << course->getCourseCode() << ")" << endl;
                    }
                    break;
                }
                case 2: {
                    string courseCode;
                    int studentId;
                    char grade;
                    int marks;

                    cout << "Enter course code: ";
                    cin >> courseCode;
                    if(cin.fail()) throw invalid_argument("Invalid course code");
                    Course* course = university.findCourseByCode(courseCode);
                    if (!course) {
                        cout << "Course not found." << endl;
                        break;
                    }

                    cout << "Enter student ID: ";
                    cin >> studentId;
                    if(cin.fail()) throw invalid_argument("Invalid student ID");
                    Student* student = university.findStudentById(studentId);
                    if (!student) {
                        cout << "Student not found." << endl;
                        break;
                    }

                    cout << "Enter grade (A-F): ";
                    cin >> grade;
                    if(cin.fail()) throw invalid_argument("Invalid grade");
                    cout << "Enter marks: ";
                    cin >> marks;
                    if(cin.fail()) throw invalid_argument("Invalid marks");

                    instructor->addGradeToStudent(student, course, grade, marks);
                    cout << "Grade added successfully." << endl;
                    break;
                }
                case 0:
                    cout << "Logging out..." << endl;
                    break;
                default:
                    cout << "Invalid choice. Please try again." << endl;
            }
        } catch (const exception& e) {
            cout << "An error occurred: " << e.what() << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

    } while (choice != 0);
}

// Login menu function
void loginMenu(University& university) {
    int choice;
    do {
        cout << endl << "\t\t\t\t\t\t**** University Management System ****" << endl;
        cout << "\t\t\t\t\t\t1. Admin Login" << endl;
        cout << "\t\t\t\t\t\t2. Student Login" << endl;
        cout << "\t\t\t\t\t\t3. Instructor Login" << endl;
        cout << "\t\t\t\t\t\t0. Exit" << endl;
        cout << "\t\t\t\t\t\tEnter your choice: ";
        try {
            cin >> choice;
            if(cin.fail()) throw invalid_argument("Invalid input");

            switch (choice) {
                case 1: {
                    string adminPassword = "admin";  // You can change this to a more secure method
                    string inputPassword = getPassword();
                    if (inputPassword == adminPassword) {
                        adminMenu(university);
                    } else {
                        cout << "Incorrect password. Access denied." << endl;
                    }
                    break;
                }
                case 2: {
                    int studentId;
                    cout << "Enter student ID: ";
                    cin >> studentId;
                    if(cin.fail()) throw invalid_argument("Invalid student ID");
                    Student* student = university.findStudentById(studentId);
                    if (student) {
                        string inputPassword = getPassword();
                        if (student->checkPassword(inputPassword)) {
                            studentMenu(student);
                        } else {
                            cout << "Incorrect password. Access denied." << endl;
                        }
                    } else {
                        cout << "Student not found." << endl;
                    }
                    break;
                }
                case 3: {
                    int instructorId;
                    cout << "Enter instructor ID: ";
                    cin >> instructorId;
                    if(cin.fail()) throw invalid_argument("Invalid instructor ID");
                    Instructor* instructor = university.findInstructorById(instructorId);
                    if (instructor) {
                        string inputPassword = getPassword();
                        if (instructor->checkPassword(inputPassword)) {
                            instructorMenu(instructor, university);
                        } else {
                            cout << "Incorrect password. Access denied." << endl;
                        }
                    } else {
                        cout << "Instructor not found." << endl;
                    }
                    break;
                }
                case 0:
                    cout << "Exiting..." << endl;
                    break;
                default:
                    cout << "Invalid choice. Please try again." << endl;
            }
        } catch (const exception& e) {
            cout << "An error occurred: " << e.what() << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

    } while (choice != 0);
}

int main() {
    University university("KLETECH");
    loginMenu(university);
    return 0;
}
