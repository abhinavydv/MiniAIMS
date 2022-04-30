#ifndef _ACTOR
#define _ACTOR


#include <iostream>
#include <string>
#include <vector>
#include <map>

// MySQL Connection headers
#include <mysql_connection.h>
#include <cppconn/resultset.h>
#include <cppconn/driver.h>
#include <cppconn/statement.h>



class Actor {
    protected:
    std::string id;
    std::string name;       // Display Name
    std::string passwd;        // hashed password
    std::string salt;
    std::string table;
    sql::Driver *driver;
    sql::Connection *conn;
    sql::Statement *stmt;
    sql::ResultSet *rset;

    public:
    std::string get_table();
    // returns the number of columns from `db.table`
    uint32_t get_num_cols(std::string db="", std::string table="");
    // returns a vector of column names in `db.table`
    std::vector<std::string> get_cols(std::string db, std::string table);
    // returns all data from a `db.table` matching the `condition`
    std::vector<std::vector<std::string>> get_all_data(std::string db="", std::string table="", std::string condition="");
    // Get a single entry from `db.table` where `col`=`val`
    std::vector<std::string> get_data(std::string db, std::string table, std::string col, std::string val, std::string condition="");
    // populate class variables with data from database
    void get_data_in_vars();
    std::string get_id();
    std::string get_name();
    void exit();
};


class Admin: public Actor {
    public:
    Admin();
    Admin(sql::Statement* stmt);
    Admin(const std::string& id);
    Admin(sql::Statement* stmt, const std::string& id);
    sql::Statement* get_stmt();
    // Check if all databases related to this software are present
    bool check_databases();
    void get_data_in_vars();
    // create required databases and tables
    void init();
    // Insert default data for testing in the databases
    void insert_default_data();
    // delete all databases
    void clear_all();
    // calls clear_all() and init()
    void reset();
    void add_semester(const std::vector<std::string>& sem);
    void remove_semester(const std::string& sem);
    void add_course(const std::vector<std::string>& course);
    void add_courses(const std::vector<std::vector<std::string>>& courses);
    void remove_course(const std::string& code, const std::string& sem);
    void add_student_to_IITH(const std::vector<std::string>& student);
    void add_students_to_IITH(const std::vector<std::vector<std::string>>& students);
    void remove_student_from_IITH(const std::string& id);
    void remove_students_from_IITH(const std::vector<std::string>& ids);
    void add_student_to_course(const std::string& stu_id, const std::string& course_code);
    // void add_students_to_course(const std::vector<std::string>&, const std::string&);
    void remove_student_from_course(const std::string& stu_id, const std::string& course_code);
    // void remove_students_from_course(const std::vector<std::string>&, const std::string&);
    void add_faculty(const std::vector<std::string>& faculty);
    void add_faculties(const std::vector<std::vector<std::string>>& faculties);
    void remove_faculty(const std::string& id);
    void remove_faculties(const std::vector<std::string>& ids);
};


class Student: public Actor {
    private:
    std::string f_name;
    std::string m_name;
    std::string dob;

    public:
    Student(const std::string& id);
    Student(sql::Statement* stmt, const std::string& id);
    void get_data_in_vars();
    void reg_course(const std::string& code);
    void dereg_course(const std::string& code);
    // returns a pair: pair.first = columns of table;  pair.second = all data
    std::pair<std::vector<std::string>, std::vector<std::vector<std::string>>> get_courses();
    double gpa_in_sem(const std::string& sem);
    // returns a vector of semesters in which student has even registered a course
    std::vector<std::string> get_sems();
    double calc_CGPA();
};


class Faculty: public Actor {
    public:
    Faculty(const std::string& id);
    Faculty(sql::Statement* stmt, const std::string& id);
    void get_data_in_vars();
    // return courses coordinated by the faculty
    std::vector<std::vector<std::string>> get_courses();
    // returns no. of students enrolled in a course
    int student_count(const std::string& code);
    // returns a vector of students registered for the course
    std::vector<std::vector<std::string>> get_reg_students(const std::string&, const std::string&);
    // // Can submit from csv file or from terminal one by one
    // bool submit_grades();
};


#endif
