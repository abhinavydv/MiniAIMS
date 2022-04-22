#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <openssl/sha.h>

// MySQL Connection headers
#include <mysql_connection.h>
#include <cppconn/resultset.h>
#include <cppconn/driver.h>
#include <cppconn/statement.h>


// Database names
#define AIMS_DB "aims_db"
#define AIMS_STU "aims_stu"
#define AIMS_COURSE "aims_course"

// Table Names
#define ADMIN "Admin"
#define FACULTY "Faculty"
#define STUDENT "Student"
#define COURSE "Course"
#define SEMESTER "Semester"

// SQL commands
#define CREATE_DB "CREATE DATABASE IF NOT EXISTS "
#define DROP_DB "DROP DATABASE IF EXISTS "

// # DEFAULTS

// The password is : '1234' and salt is 'abcd'
#define PASSWORD "e9cee71ab932fde863338d08be4de9dfe39ea049bdafb342ce659ec5450b69ae" 
#define SALT "abcd"

// Other constants
#define HASH_LENGTH "64"


class Actor {
    protected:
    std::string id;
    std::string name;       // Display Name
    std::string passwd;        // hashed password
    std::string salt;
    std::string table;
    std::string current_sem;
    sql::Driver *driver;
    sql::Connection *conn;
    sql::Statement *stmt;
    sql::ResultSet *rset;

    public:
    std::string get_table();
    uint32_t get_num_cols();
    std::vector<std::vector<std::string>> get_all_data();
    std::vector<std::vector<std::string>> get_multiple_data(std::string, std::string);
    std::vector<std::string> get_data(std::string, std::string);
    void get_data_in_vars();
    bool authenticate(std::string);
    std::string get_id();
    std::string get_name();
    void change_passwd(std::string);
    void exit();
};


class Admin: public Actor {

    public:
    Admin();
    Admin(sql::Statement*);
    Admin(const std::string&);
    Admin(sql::Statement*, const std::string&);
    void get_data_in_vars();
    void init();
    void insert_default_data();
    void clear_all();
    void reset();
    void add_semester(const std::vector<std::string>&);
    void add_course(const std::vector<std::string>&);
    void add_courses(const std::vector<std::vector<std::string>>&);
    void remove_course(const std::string&);
    void remove_courses(const std::vector<std::string>&);
    void add_student_to_IITH(const std::vector<std::string>&);
    void add_students_to_IITH(const std::vector<std::vector<std::string>>&);
    void remove_student_from_IITH(const std::string&);
    void remove_students_from_IITH(const std::vector<std::string>&);
    void add_student_to_course(const std::string&, const std::string&);
    void add_students_to_course(const std::vector<std::string>&, const std::string&);
    void remove_student_from_course(const std::string& , const std::string&);
    void remove_students_from_course(const std::vector<std::string>&, const std::string&);
    void add_faculty(const std::vector<std::string>&);
    void add_faculties(const std::vector<std::vector<std::string>>&);
    void remove_faculty(const std::string&);
    void remove_faculties(const std::vector<std::string>&);
};


class Student: public Actor {
    private:
    std::string f_name;
    std::string m_name;
    std::string dob;

    public:
    Student(const std::string&);
    Student(sql::Statement*, const std::string&);
    void get_data_in_vars();
    void reg_course(const std::string&);
    void dereg_course(const std::string&);
    std::string view_courses();
    double gpa_in_sem(const std::string&);
    double calc_CGPA();
};


class Faculty: public Actor {
    public:
    Faculty(const std::string&);
    Faculty(sql::Statement*, const std::string&);
    void get_data_in_vars();
    std::vector<std::string> get_student_ids();
    bool submit_grades();
    // Can submit from csv, edit a file or from terminal one by one
};
