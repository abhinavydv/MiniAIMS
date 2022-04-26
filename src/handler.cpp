#include "../include/handler.hpp"
#include "../include/actor.hpp"


MyException::MyException(){}


MyException::MyException(std::string msg){
    std::cout << RED << "error: " << NO_COLOR << msg << std::endl;
}


bool is_digit(std::string str){
    for (char c: str){
        if (c < 48 || c > 57)
            return false;
    }
    return true;
}


std::vector<std::string> str_split(std::string str, char delim){
    std::vector<std::string> result;
    int index = 0;
    result.push_back("");
    while (index<str.size()){
        if (str[index] == delim){
            if(result.back() != "")
                result.push_back("");
        }
        else {
            result.back().push_back(str.at(index));
        }
        index++;
    }
    if (result.back() == "")
        result.pop_back();

    return result;
}


std::string str_strip(std::string str, char target){
    return str.substr(str.find_first_not_of(target), str.find_last_not_of(target)+1-str.find_first_not_of(target));
}


std::pair<std::string, std::string> split_pair(std::string str, char delim, bool strip){
    std::pair<std::string, std::string> result;
    if (str.find(delim) == str.npos){
        if (strip)
            result.first = str_strip(str);
        else
            result.first = str;
        result.second = "";
    }
    else if (strip){
        result.first = str_strip(str.substr(0, str.find(delim)));
        result.second = str_strip(str.substr(str.find(delim)+1, str.size()-str.find(delim)));
    }
    else{
        result.first = str.substr(0, str.find(delim));
        result.second = str.substr(str.find(delim)+1, str.size()-str.find(delim));
    }
    return result;
}


std::string read_file(std::string file_name){
    std::string data;
    // char temp;
    std::fstream file;
    file.open(file_name, std::ios::in);

    std::stringstream buffer;

    buffer << file.rdbuf();
    data = buffer.str();

    // while(!file.eof()){
    //     file >> temp;
    //     data.push_back(temp);
    // }
    return data;
}


std::vector<std::string> readlines_file(std::string file_name){
    return str_split(read_file(file_name), '\n');
}


std::map<std::string, std::string> loadenv(){
    std::map<std::string, std::string> env;
    std::pair<std::string, std::string> temp;
    for (std::string str: readlines_file(ENV_FILE)){
        temp = split_pair(str, '=', true);
        temp.first = str_strip(temp.first, '"');
        temp.second = str_strip(temp.second, '"');
        env.insert(temp);
    }
    return env;
}


std::vector<std::string> get_user_and_passwd(){
    std::vector<std::string> uap;
    std::map<std::string, std::string> env = loadenv();
    try {
        uap.push_back(env.at(USER_KEY));
    }
    catch (std::out_of_range) {
        uap.push_back("");
    }
    try {
        uap.push_back(env.at(PASSWD_KEY));
    }
    catch (std::out_of_range) {
        uap.push_back("");
    }
    return uap;
}


void print_line(std::vector<int> sizes){
    for (int i: sizes){
        std::cout << "+";

        for (int j=0; j<i+2; j++){
            std::cout << "-";
        }
    }
    std::cout << "+\n";
}


void print_data_table(const std::vector<std::string>& cols, const std::vector<std::vector<std::string>>& values){
    if (values.size() == 0){
        std::cout << "No Data!!\n";
        return;
    }

    std::vector<int> sizes;
    if (cols.size() != 0){
        for (std::string col: cols){
            sizes.push_back(col.size());
        }
    }
    for (auto row: values){
        int i=0;
        for (auto val: row){
            sizes[i] = sizes.at(i) > val.size() ? sizes.at(i) : val.size();
            i++;
        }
    }

    print_line(sizes);

    if (cols.size() != 0){
        int i=0;
        for (auto col: cols){
            std::cout << "| " << col << " ";
            for (int j=0; j<sizes[i]-col.size(); j++){
                std::cout << " ";
            }
            i++;
        }
        std::cout << "|\n";
        print_line(sizes);
    }

    for (auto row: values){
        int i=0;
        for (auto val: row){
            std::cout << "| " << val << " ";
            for (int j=0; j<sizes[i]-val.size(); j++){
                std::cout << " ";
            }
            i++;
        }
        std::cout << "|\n";
    }
    print_line(sizes);

}


std::string generate_salt(size_t size){
    std::string salt;
    srand(time(NULL));
    char characters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890~!@#$%^&*()_-=_+{}[]|\\;:.,<>/?`~";
    for (size_t i=0; i<size; i++){
        salt.push_back(characters[rand()%sizeof(characters)]);
    }

    return salt;
}


std::string passwd_to_SHA256(const std::string& salt, const std::string& passwd){
    std::string sp = salt + passwd;

    unsigned char buf[HASH_LENGTH_INT];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, sp.c_str(), sp.size());
    SHA256_Final(buf, &sha256);

    std::stringstream ss;
    for(int i = 0; i < HASH_LENGTH_INT; i++)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)buf[i];

    return ss.str();
}


bool check_value(std::string value){
    if (value.find("'") != value.npos || value.find("\"") != value.npos)
        return false;
    return true;
}


void validate_value(std::string value){
    if (!check_value(value)){
        throw MyException("Symbols ' and \" are not allowed");
    }
}


void insert_val(sql::Statement *stmt, std::string table, std::vector<std::string> values, std::vector<std::string> cols){
    if (values.size() != cols.size() && cols.size()!=0){
        throw MyException("Column size does not match value size");
    }
    if (values.size() == 0){
        throw MyException("No Values provided");
    }
    std::string query = "INSERT INTO " + table;
    query.push_back(' ');

    if (cols.size() != 0){
        query.push_back('(');
        for (std::string col: cols){
            query.append(col);
            query.push_back(',');
        }
        query.pop_back();
        query.append(") ");
    }

    query.append("VALUES (");
    for (std::string value: values){
        if (value == NULL_STR){
            query.append("NULL,");
            continue;
        }
        validate_value(value);
        query.append("'" + value + "',");
    }
    query.pop_back();
    query.push_back(')');

    // std::cout << query << std::endl;

    EXEC(query);
}


void insert_all(sql::Statement *stmt, std::string table, std::vector<std::vector<std::string>> values, std::vector<std::string> cols){
    for (std::vector<std::string> value: values){
        insert_val(stmt, table, value, cols);
    }
}


std::string get_val(sql::Statement *stmt, std::string db, std::string table, std::string col_to_get, std::string pk_col, std::string val){
    validate_value(col_to_get);
    validate_value(val);
    validate_value(pk_col);
    sql::ResultSet *rset = EXECQ("SELECT " + col_to_get + " FROM " + db + "." + table + " WHERE " + pk_col + "='" + val + "'");
    if (rset->next()){
        return rset->getString(1);
    }
    else {
        return NULL_STR;
    }
}


void delete_val(sql::Statement* stmt, std::string db, std::string table, std::string column, std::string value){
    validate_value(value);
    validate_value(column);
    USE_DB(db);
    EXEC("DELETE FROM " + table + "where " + column + "='" + value + "'");
}


std::string get_current_date(){
    std::time_t t = std::time(0);
    std::tm *now = std::localtime(&t);

    char tm_now[11];
    strftime(tm_now, sizeof(tm_now), "%Y-%m-%d", now);

    return tm_now;
}


std::string get_user_type(sql::Statement *stmt, std::string id){
    USE_DB(AIMS_DB);
    sql::ResultSet *rset = EXECQ("SELECT * from " ADMIN " where id='" + id + "'");
    if (rset->next())
        return ADMIN;

    rset = EXECQ("SELECT * from " STUDENT " where id='" + id + "'");
    if (rset->next())
        return STUDENT;

    rset = EXECQ("SELECT * from " FACULTY " where id='" + id + "'");
    if (rset->next())
        return FACULTY;

    return NULL_STR;
}


std::string input(const std::string prompt, bool allow_empty){
    std::string temp;
    while (temp == ""){
        std::cout << prompt;
        std::getline(std::cin, temp);
        if (std::cin.eof())
            throw EOFException();
        if (temp == "" && allow_empty)
            break;

        else if (temp == ".b")
            throw BackException();
        else if (temp == ".l")
            if (confirm("Do you really want to logout?"))
                throw LogoutException();
            else
                temp = "";
        else if (temp == ".q")
            if (confirm("Do you really want to quit?"))
                throw QuitException();
            else
                temp = "";
        else if (temp == ".c"){
            temp = "";
            system("clear");
        }
    }

    return temp;
}


int input_int(const std::string prompt){
    return std::stol(input(prompt));
}


std::string input_date(std::string prompt){
    std::string date;
    std::vector<std::string> ymd;
    while (date == ""){
        date = input(prompt);
        ymd = str_split(date, '-');
        if (ymd.size() != 3)
            date = "";
        else {
            tm t;
            if (!strptime(date.c_str(), "%y%m%d", &t))
                date = "";
        }
        if (date == "")
            std::cout << YELLOW "Invalid date!!" NO_COLOR;
    }

    return date;
}


int get_choice(int low, int high){
    int op = input_int("Enter a choice: ");
    while (op < low || op > high){
        std::cout << "Invalid choice!!" << std::endl;
        op = input_int("Enter a choice: ");
    }
    return op;
}


std::vector<std::string> update_data(std::vector<std::string> cols, std::vector<std::string> values, std::vector<int> non_updatables){
    std::vector<std::string> new_data;
    std::sort(non_updatables.begin(), non_updatables.end());
    std::cout << "Enter values for columns (Press enter without any value to keep previous value): \n";
    for (int i=0; i<cols.size(); i++){
        if (std::binary_search(non_updatables.begin(), non_updatables.end(), i)){
            new_data.push_back(values.at(i));
            continue;
        }
        std::string data = input(cols.at(i) + " (" + values.at(i) + "): ", true);
        if (data == "")
            new_data.push_back(values.at(i));
        else
            new_data.push_back(data);
    }
    return new_data;
}


bool confirm(std::string prompt){
    if (prompt == "")
        prompt = "Are you sure? (Y or N): ";

    std::string yn;

    std::cout << prompt << " (Y or N): ";
    while (yn == ""){
        std::getline(std::cin, yn);
        if (std::cin.eof())
            throw EOFException();
        if (yn == "Y" || yn == "y")
            return true;
        else if (yn == "N" || yn == "n")
            return false;
        else {
            std::cout << "Wrong choice!\nEnter Y or N: ";
            yn = "";
        }
    }
    throw LineReachedException();

    
}
