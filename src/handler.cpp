#include "../include/handler.hpp"


MyException::MyException(){}


MyException::MyException(std::string msg){
    std::cout << RED << "error: " << NO_COLOR << msg << std::endl;
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


void delete_val(sql::Statement* stmt, std::string table, std::string column, std::string value){
    validate_value(value);
    validate_value(column);
    EXEC("DELETE FROM " + table + "where " + column + "=" + value);
}


std::string get_current_date(){
    std::time_t t = std::time(0);
    std::tm *now = std::localtime(&t);
    
    char tm_now[11];
    strftime(tm_now, sizeof(tm_now), "%Y-%m-%d", now);

    return tm_now;
}
