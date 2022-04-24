#include "../include/admin.hpp"


using namespace std;


bool handle_admin(sql::Statement* stmt, string id){
    Admin admin(stmt, id);
    string msg = "1. Update Semesters\n"
                "2. Update Courses\n"
                "3. Update Students\n"
                "4. Update faculty\n"
                "5. Logout\n"
                "6. Exit\n\n";

    cout << msg;

    int op = get_choice(1, 6);
    cout << "\n";
    try{
        if (op == 1)
            while (true)
                update_sem(admin);
        else if (op == 2)
            while (true)
                update_courses(admin);
        else if (op == 3)
            while (true)
                update_students(admin);
        else if (op == 4)
            while (true)
                update_faculty(admin);
        else if (op == 5)
            return false;
        else if (op == 6)
            return true;

        throw LineReachedException();
    }
    catch (EOFException){
        return false;
    }
}


void update_sem(Admin& admin){
    string msg = "1. Add Semester\n"
                "2. Remove semester\n\n";
    cout << msg;
    
    int op = get_choice(1, 3);

    try {
        if (op == 1){
            string name = input("Enter Semester name: ");
            while (get_val(admin.get_stmt(), AIMS_DB, SEMESTER, "Name", "Name", name) != ""){
                cout << YELLOW "Semester already exists" << "\n" NO_COLOR;
                name = input("Enter Semester name: ");
            }
            vector<string> sem = {name};

            sem.push_back(input("Enter start date (yyyy-mm-dd): "));
            sem.push_back(input("Enter end date (yyyy-mm-dd): "));
            string type = input("Enter type ('Even' or 'Odd'): ");
            while (type != "Even" && type != "Odd"){
                cout << "Invalid type!!" << "\n";
                type = input("Enter type ('Even' or 'Odd'): ");
            }
            sem.push_back(type);
            admin.add_semester(sem);
        }

        else if (op == 2){
            auto data = admin.get_all_data(AIMS_DB, SEMESTER);
            cout << "Semester data is:\n\n";
            print_data_table(admin.get_cols(AIMS_DB, SEMESTER), data);

            string name = input("Enter Semester name: ");
            while (get_val(admin.get_stmt(), AIMS_DB, SEMESTER, "Name", "Name", name) == ""){
                cout << YELLOW "Semester does not exists!!" << "\n" NO_COLOR;
                name = input("Enter Semester name: ");
            }
            admin.remove_semester(name);
        }
    }
    catch (EOFException){}

}


void update_courses(Admin& admin){
    std::string msg = "1. Add course\n"
                    "2. Remove course\n"
                    "3. Update course\n\n";
    cout << msg;

    sql::Statement *stmt = admin.get_stmt();

    int op = get_choice(1, 3);

    if (op == 1){
        string code = input("Enter course code: ");
        while (get_val(stmt, AIMS_DB, COURSE, "Code", "Code", code) != ""){
            cout << YELLOW "Course already exists" << "\n" NO_COLOR;
            code = input("Enter course code: ");
        }

        vector<string> course = {code};

        course.push_back(input("Enter course name: "));
        course.push_back(input("Enter no. of credits: "));
        course.push_back(input("Enter Instructor ID: "));
        course.push_back(input("Enter semester name: "));
        course.push_back(input("Enter segments for course (start-end (ex: 1-4))"));
        course.push_back(input("Enter floating status (Y or N)"));
        
        admin.add_course(course);
    }
    else if(op == 2){
        auto data = admin.get_all_data(AIMS_DB, COURSE);
        cout << "All course data is:\n\n";
        print_data_table(admin.get_cols(AIMS_DB, COURSE), data);

        string code = input("Enter Course code: ");
        while (get_val(stmt, AIMS_DB, COURSE, "Name", "Name", code) == ""){
            cout << YELLOW "Course does not exists!!" << "\n" NO_COLOR;
            code = input("Enter Course code: ");
        }
        admin.remove_course(code);
    }
    else if (op == 3){
        string code = input("Enter Course code: ");
        while (get_val(stmt, AIMS_DB, COURSE, "Name", "Name", code) == ""){
            cout << YELLOW "Course does not exists!!\n" NO_COLOR;
            code = input("Enter Course code: ");
        }
        auto data = admin.get_data(AIMS_DB, COURSE, "Code", code);
        
        cout << "Data of this course: \n";
        auto cols = admin.get_cols(AIMS_DB, COURSE);
        print_data_table(cols, {data});
        cout << "\n";

        cout << "Enter values for columns (Enter \"*\" to keep previous value): \n";
        cout << YELLOW "You cannot update code and semester. To change them, remove the course"
        " and add again (will result in loss of registered student data)\n\n" NO_COLOR;
        update:
        auto new_data = update_data(cols, data);
        if (new_data.at(0) != data.at(0) || new_data.at(4) != data.at(4)){
            cout << YELLOW "Cannot change code or semester. Enter again:\n" NO_COLOR;
            goto update;
        }
        USE_DB(AIMS_DB);
        EXEC("DELETE FROM " COURSE " where Code='" + data.at(0) + "' and Semester='" + data.at(4) + "'");
        insert_val(stmt, COURSE, new_data, {});
    }
}


void update_students(Admin& admin){
    std::string msg = "1. Add student to IITH\n"
                    "2. Remove student from IITH\n"
                    "3. Add student to course\n"
                    "4. Remove student from course\n"
                    "5. Update student data\n\n";
    
    cout << msg;
    
    sql::Statement* stmt = admin.get_stmt();

    int op = get_choice(1, 5);
}


void update_faculty(Admin& admin){

}


