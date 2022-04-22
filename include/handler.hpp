#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <ctime>

// includes for MySQL
#include <mysql_connection.h>
#include <cppconn/resultset.h>
#include <cppconn/driver.h>
#include <cppconn/statement.h>


#define ENV_FILE "./.env"
#define USER_KEY "USER_NAME"
#define PASSWD_KEY "PASSWD"

// colors
#define NO_COLOR "\033[1;0m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"

// other constants
#define NULL_STR ""

// Frequent Functions
#define EXEC(sql) stmt->execute(sql);
#define EXECQ(sql) stmt->executeQuery(sql);
#define USE_DB(db) EXEC("use" + std::string(" ") + db)


std::vector<std::string> str_split(std::string, char);
std::string str_strip(std::string, char=' ');
std::pair<std::string, std::string> split_pair(std::string, char, bool=false);
std::string read_file(std::string);
std::vector<std::string> readlines_file(std::string);
std::map<std::string, std::string> loadenv();
std::vector<std::string> get_user_and_passwd();

std::vector<std::string> format_data(std::vector<std::string>, std::vector<std::vector<std::string>>); // return formatted data

std::string hex_to_string(const unsigned char*, size_t);
std::string passwd_to_SHA256(std::string);


bool check_value();
void validate_value(std::string);
// table_name, values, column_names
void insert_val(sql::Statement*, std::string, std::vector<std::string>, std::vector<std::string>);
void insert_all(sql::Statement*, std::string, std::vector<std::vector<std::string>>, std::vector<std::string>);
void use_db(sql::Statement*, std::string);

void delete_val(sql::Statement*, std::string, std::string, std::string);

bool check_integrity(sql::Statement*, std::string, std::string, std::string);

std::string get_current_date();



// Constant vars
const std::map<std::string, int> grade_to_gpa = {
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



// Classes

class MyException: public std::exception {
    public:
    MyException();
    MyException(std::string);
};
