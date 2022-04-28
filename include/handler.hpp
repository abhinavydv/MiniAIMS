#ifndef _HANDLER
#define _HANDLER


#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <openssl/sha.h>


// includes for MySQL
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

// CSV files
#define FACULTY_CSV "data/faculty.csv"
#define STUDENT_CSV "data/student.csv"
#define SEMESTER_CSV "data/semester.csv"
#define COURSE_CSV "data/course.csv"

// DEFAULTS
// The password is : '1234'
#define PASSWORD "e9cee71ab932fde863338d08be4de9dfe39ea049bdafb342ce659ec5450b69ae" 
#define SALT "abcd"
#define SALT_SIZE "32"
#define SALT_SIZE_INT 32


// colors
#define NO_COLOR "\033[1;0m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"

// Other constants
#define HASH_LENGTH "64"
#define HASH_LENGTH_INT 32
#define ENV_FILE "./.env"
#define USER_KEY "USER_NAME"
#define PASSWD_KEY "PASSWD"
#define NULL_STR ""

// Frequent Functions
#define EXEC(sql) stmt->execute(sql);
#define EXECQ(sql) stmt->executeQuery(sql);
#define USE_DB(db) EXEC("use" + std::string(" ") + db)


bool is_digit(std::string);
std::vector<std::string> str_split(std::string, char, bool=false, bool=false);
std::string str_strip(std::string, char=' ');
std::pair<std::string, std::string> split_pair(std::string, char, bool=false);
std::string read_file(std::string);
std::vector<std::string> readlines_file(std::string);
std::map<std::string, std::string> loadenv();
std::vector<std::string> get_user_and_passwd();
void print_data_table(const std::vector<std::string>&, const std::vector<std::vector<std::string>>&);
std::string generate_salt(size_t=32);
std::string passwd_to_SHA256(const std::string&, const std::string&);
bool check_value(std::string);
void validate_value(std::string);
void insert_val(sql::Statement*, std::string, std::vector<std::string>, std::vector<std::string>);
void insert_all(sql::Statement*, std::string, std::vector<std::vector<std::string>>, std::vector<std::string>);
std::string get_val(sql::Statement*, std::string, std::string, std::string, std::string, std::string, std::string="");
void delete_val(sql::Statement*, std::string, std::string, std::string, std::string);
std::vector<std::vector<std::string>> extract(sql::ResultSet*, int);
bool check_integrity(sql::Statement*, std::string, std::string, std::string);
std::string get_current_date();
std::string get_user_type(sql::Statement*, std::string);
std::string input(const std::string="", bool=false);
int input_int(const std::string="");
bool check_date(std::string);
std::string input_date(std::string="", bool=false);
int get_choice(int, int);
std::vector<std::string> update_data(std::vector<std::string>, std::vector<std::string>, std::vector<int>);
bool confirm(std::string="");
bool file_exists(std::string);
std::vector<std::vector<std::string>> read_csv(std::string);
void print_vec(std::vector<std::string>);
bool no_spcl_ch(std::string);
bool isdigit(std::string, char=0);

// Constant vars
const std::map<std::string, int> grade_to_gpa = {  // TODO: Verify these values
    {"A+", 10},
    {"A", 10},
    {"A-", 9},
    {"B", 8},
    {"B-", 7},
    {"C", 6},
    {"C-", 5},
    {"D", 4},
    {"F", 0},
    {"", 0}
};


// Exception Classes

class MyException: public std::exception {
    public:
    MyException();
    MyException(std::string);
};


class EOFException: public std::exception {};
class BackException: public std::exception {};
class LogoutException: public std::exception {};
class QuitException: public std::exception {};
class LineReachedException: public std::exception {};
class UnmatchingRowsException: public std::exception {};


#endif
