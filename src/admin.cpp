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
                cout << YELLOW "Semester already exists" << "\n";
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
                cout << YELLOW "Semester does not exists!!" << "\n";
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

    int op = get_choice(1, 3);

    if (op == 1){
        string code = input("Enter course code: ");
        while (get_val(admin.get_stmt(), AIMS_DB, COURSE, "Code", "Code", code) != ""){
            cout << YELLOW "Course already exists" << "\n";
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
        while (get_val(admin.get_stmt(), AIMS_DB, COURSE, "Name", "Name", code) == ""){
            cout << YELLOW "Course does not exists!!" << "\n";
            code = input("Enter Course code: ");
        }
        admin.remove_course(code);
    }
    else if (op == 3){
        string code = input("Enter Course code: ");
        while (get_val(admin.get_stmt(), AIMS_DB, COURSE, "Name", "Name", code) == ""){
            cout << YELLOW "Course does not exists!!\n";
            code = input("Enter Course code: ");
        }
        auto data = admin.get_data(AIMS_DB, COURSE, "Code", code);
        
        cout << "Data of this course: \n";
        auto cols = admin.get_cols(AIMS_DB, COURSE);
        print_data_table(cols, {data});
        cout << "\n";

        cout << "Enter values for columns (Enter \"*\" to keep previous value): \n";
        cout << YELLOW "If you update code or Semester, the process might take some time\n\n";
        update:
        auto new_data = update_data(cols, data);
        if (new_data.at(0) != data.at(0) || new_data.at(4) != data.at(4)){
            sql::Statement *stmt = admin.get_stmt();
            USE_DB(AIMS_DB);
            sql::ResultSet *rset = EXECQ("SELECT Code, Semester FROM " COURSE " where Code='" + new_data.at(0) + "' and Semester=" + new_data.at(4) + "'");
            if (rset->next()){
                cout << "(Code, Semester) pair already exists. Enter again\n";
                goto update;
            }
            string old_tbl = "course_" + data.at(0) + "_" + data.at(4);
            string new_tbl = "course_" + new_data.at(0) + "_" + new_data.at(4);
            auto students = admin.get_all_data(AIMS_COURSE, old_tbl);
            for (auto stu: students){
                string table = "stu_" + stu.at(0);
                
            }
        }


    }
}


void update_students(Admin& admin){
    
}


void update_faculty(Admin& admin){

}


