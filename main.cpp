#include "include/handler.hpp"
#include "include/admin.hpp"
#include "include/faculty.hpp"
#include "include/student.hpp"

using namespace std;


void first_run();
void main_prompt(sql::Statement*);
pair<string, string> login(sql::Statement*);


int main(){
    try{
        first_run();

        // Create connection
        sql::Driver *driver = get_driver_instance();
        std::vector<std::string> user_and_passwd = get_user_and_passwd();
        sql::Connection *conn = driver->connect("localhost", user_and_passwd[0], user_and_passwd[1]);
        sql::Statement *stmt = conn->createStatement();

        cout << "\nWELCOME TO AIMS\n===============\n" << 
        "Enter .b anywhere to go one step back, " << 
        ".l to logout, .c to clear terminal and .q to quit.\n";

        while (true){
            try{
                main_prompt(stmt);
            }
            catch (BackException){}
            catch (LogoutException){}
        }
    }
    catch (EOFException){}
    catch (QuitException){}

    cout << "\nBye!\n";

    return 0;
}


// checks whether the software is run for the first time and asks for
// MySQL UserID and password if not present in .env file and for new admin and password
// if databases are not present
void first_run(){
    auto uap = get_user_and_passwd();
    bool fl = !file_exists(".env");
    std::ofstream f(ENV_FILE, ios::out);
    do {
        if (uap.at(0) == ""){
            uap[0] = input("MySQL Username: ");
            // string write = "\n" USER_KEY "=\"" + uap.at(0) + "\"\n";
            // f << write;
        }
        if (uap.at(1) == ""){
            uap[1] = input("MySQL Password: ");
            // string write = "\n" PASSWD_KEY "=\"" + uap.at(1) + "\"\n";
            // f << write;
        }
        try {
            sql::Driver* d = get_driver_instance();
            sql::Connection* conn = d->connect("localhost", uap.at(0), uap.at(1));
            delete conn;
        }
        catch (sql::SQLException){
            if (fl)
                cout << RED "MySQL username or password is wrong! or MySQL server is not running!\n" NO_COLOR;
            uap[0] = "";
            uap[1] = "";
            fl = true;
        }

    } while (uap.at(0) == "" || uap.at(1) == "");
    f << "\n" USER_KEY << "=\"" << uap.at(0) << "\"\n";
    f << "\n" PASSWD_KEY << "=\"" << uap.at(1) << "\"\n";
    f.close();


    Admin admin;
    sql::Statement* stmt = admin.get_stmt();
    if (!admin.check_databases()){  // check if all databases exist
        admin.reset();
        cout << "The application is being run for the first time. You are the admin now.\n";
        string user;
        while (user == ""){
            user = input("Enter your username: ");
            if (!no_spcl_ch(user)){
                cout << RED "Username cannot contain any special character (except '_')\n";
                user = "";
            }
        }
        string name = input("Enter your Name: ");
        string passwd = input("Enter your password: ");
        string salt = generate_salt(SALT_SIZE_INT);
        string passwd_hash = passwd_to_SHA256(salt, passwd);
        USE_DB(AIMS_DB);
        insert_val(stmt, ADMIN, {user, name, passwd_hash, salt}, {});

        if (confirm("Should we enter some default data for testing?"))
            admin.insert_default_data();

        cout << GREEN "All set!\n\n" NO_COLOR;
    }
    admin.exit();
}


// The main prompt that lets user login
void main_prompt(sql::Statement *stmt){
    pair<string, string> iu;
    try {
        iu = login(stmt);
    }
    catch (BackException){
        throw QuitException();
    }

    while (true){
        if (iu.second == ADMIN)
            handle_admin(stmt, iu.first);
        else if (iu.second == STUDENT)
            handle_student(stmt, iu.first);
        else if (iu.second == FACULTY)
            handle_faculty(stmt, iu.first);
    }
}


// retuns username and usertype after user is logged in
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

