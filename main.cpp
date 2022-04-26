#include "include/admin.hpp"
#include "include/faculty.hpp"
#include "include/student.hpp"

using namespace std;


void first_run();
bool main_prompt(sql::Statement*);
pair<string, string> login(sql::Statement*);


int main(){
    first_run();

    // Create connection
    sql::Driver *driver = get_driver_instance();

    std::vector<std::string> user_and_passwd = get_user_and_passwd();
    sql::Connection *conn = driver->connect("localhost", user_and_passwd[0], user_and_passwd[1]);
    sql::Statement *stmt = conn->createStatement();


    cout << "WELCOME TO AIMS\n===============\n" << 
    "Enter .b (without quotes) anywhere to go one step back" << 
    ", .l anywhere to logout and .q anywhere to quit." << endl;

    bool done = false;
    while (!done){
        done = main_prompt(stmt);
    }

    cout << "\nBye\n";

    return 0;
}


void first_run(){
    Admin admin;
    admin.reset();
    admin.insert_default_data();
    admin.exit();
}


bool main_prompt(sql::Statement *stmt){
    pair<string, string> iu;
    try{
        iu = login(stmt);
    }
    catch (BackException){
        return true;
    }

    bool done;

    try {
        if (iu.second == ADMIN)
            done = handle_admin(stmt, iu.first);
        else if (iu.second == STUDENT)
            done = handle_student(stmt, iu.first);
        else if (iu.second == FACULTY)
            done = handle_faculty(stmt, iu.first);
    }
    catch (BackException){
        return false;
    }

    return done;
}


pair<string, string> login(sql::Statement *stmt){
    string id;
    string user;
    while (id == ""){
        id = input("Enter user ID: ");
        user = get_user_type(stmt, id);
        if (user == ""){
            cout << YELLOW "User does not exist" NO_COLOR << endl;
            id = "";
        }
    }
    cout << "Hello " << user << "\n";

    string passwd, passwd_hash, salt;
    while (passwd == ""){
        passwd = input("Enter Password: ");
        passwd_hash = get_val(stmt, AIMS_DB, user, "Passwd", "ID", id);
        salt = get_val(stmt, AIMS_DB, user, "Salt", "ID", id);
        if (passwd_hash != passwd_to_SHA256(salt, passwd)){
            cout << "Wrong password!!" << "\n";
            passwd = "";
        }
    }

    return {id, user};
}

