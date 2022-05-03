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

// returns if `str` contains only digits
bool is_digit(std::string str);
/*
split the `str` with `delim` as the delimiter. Remove white spaces if strip=true. 
keep empty splitted strings while splitting if allow_empty=true
*/
std::vector<std::string> str_split(std::string str, char delim, bool strip=false, bool allow_empty=false);
// Remove trailing and leading `target` characters from str
std::string str_strip(std::string str, char target=' ');
// split `str` using `delim` as a delimiter. Remove white spaces if strip=true.
std::pair<std::string, std::string> split_pair(std::string str, char delim, bool strip=false);
// returns contents of the file named `file_name`
std::string read_file(std::string file_name);
// returns a vector containing each line of `file`
std::vector<std::string> readlines_file(std::string file_name);
// load environment data from .env file
std::map<std::string, std::string> loadenv();
// extract user name and password from .env file
std::vector<std::string> get_user_and_passwd();
// prints a table with headings from `cols` and values from `values`
void print_data_table(const std::vector<std::string>& cols, const std::vector<std::vector<std::string>>& values);
// generates a random salt of length `size`
std::string generate_salt(size_t size=32);
// returns hash of `salt` + `password`
std::string passwd_to_SHA256(const std::string& salt, const std::string& passwd);
// check if an entered value conatins forbidden symbols
bool check_value(std::string value);
// insert `values` in corresponding `cols` of `table`
void insert_val(sql::Statement* stmt, std::string table, std::vector<std::string> values, std::vector<std::string> cols);
// insert all `values` in corresponding `cols` of `table`
void insert_all(sql::Statement* stmt, std::string table, std::vector<std::vector<std::string>> values, std::vector<std::string> cols);
// return value from `db.table` from `col_to_get` where `pk_col`=`val` and `condition` (if not empty) is satisfied
std::string get_val(sql::Statement* stmt, std::string db, std::string table, std::string col_to_get, std::string pk_col, std::string val, std::string condition="");
// delete value from `db.table` where `col`=`val`
void delete_val(sql::Statement* stmt, std::string db, std::string table, std::string col, std::string val);
// upadte each column of `cols` of `db.table` with value from `values` where `col` = `val` and condition (if not empty) is satisfied
void update_val(sql::Statement*, std::string db, std::string table, std::vector<std::string> cols, std::vector<std::string> vals, std::string col, std::string val, std::string condition="");
// returns all data present in rset
std::vector<std::vector<std::string>> extract(sql::ResultSet* rset, int num_cols);
// returns current date
std::string get_current_date();
// returns if user is admin, faculty or student
std::string get_user_type(sql::Statement* stmt, std::string id);
/*
takes input from user after showing `prompt`
handles some commands.
validates entered values for forbidden characters.
allows user to enter empty string if allow_empty=true
*/
std::string input(const std::string prompt="", bool allow_empty=false);
// takes an int as input from user
int input_int(const std::string prompt="");
// check if the date is valid
bool check_date(std::string date);
// take a date as input from user
std::string input_date(std::string prompt="", bool allow_empty=false);
// get an integer `a` from user : `low` <= `a` <= `high`
int get_choice(int low, int high);
// prompt user to enter new data for each col in `cols` except cols at index from `non_updatables`
std::vector<std::string> update_data(std::vector<std::string> cols, std::vector<std::string> values, std::vector<int> non_updatables);
// Ask user for a confirmation after showing the `prompt`.
bool confirm(std::string prompt="");
// check if file named `file` exists
bool file_exists(std::string file);
// read CSV data from `file`
std::vector<std::vector<std::string>> read_csv(std::string file);
// returns true if str contains no special characters except '_'
bool no_spcl_ch(std::string str);
// check if str contains only digits after ignoring `ignore`
bool isdigit(std::string str, char ignore=0);

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
