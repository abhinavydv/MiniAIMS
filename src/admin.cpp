#include "../include/admin.hpp"


using namespace std;


void handle_admin(sql::Statement* stmt, string id){
    Admin admin(stmt, id);
    string msg = "\n1. Semesters\n"
                "2. Courses\n"
                "3. Students\n"
                "4. Faculty\n\n";

    cout << msg;

    int op = get_choice(1, 4);
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
    }
    catch (BackException){}
}


void update_sem(Admin& admin){
    string msg = "\n0. View Semesters\n"
                "1. Add Semester\n"
                "2. Remove semester\n\n";
    cout << msg;
    
    int op = get_choice(0, 2);

    try {
        if (op == 0){
            print_data_table(admin.get_cols(AIMS_DB, SEMESTER), admin.get_all_data(AIMS_DB, SEMESTER));
        }
        else if (op == 1){
            string name = input("Enter Semester name: ");
            while (get_val(admin.get_stmt(), AIMS_DB, SEMESTER, "Name", "Name", name) != "" || !no_spcl_ch(name)){
                if (no_spcl_ch(name))
                    cout << RED "Semester already exists" << "\n" NO_COLOR;
                else
                    cout << RED "No special characters allowed (except '_')\n" NO_COLOR;
                name = input("Enter Semester name: ");
            }
            vector<string> sem = {name};

            sem.push_back(input_date("Enter start date (yyyy-mm-dd): "));
            sem.push_back(input_date("Enter end date (yyyy-mm-dd): "));
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
                cout << RED "Semester does not exists!!" << "\n" NO_COLOR;
                name = input("Enter Semester name: ");
            }
            admin.remove_semester(name);
        }
        if (op != 0){
            cout << GREEN "DONE!\n" NO_COLOR;
        }
    }
    catch (BackException){}

}


// FIXME: Course IDs with semester should be unique
void update_courses(Admin& admin){
    std::string msg = "\n0. View Courses\n"
                    "1. Add course\n"
                    "2. Remove course\n"
                    "3. Update course\n\n";
    cout << msg;

    sql::Statement *stmt = admin.get_stmt();

    int op = get_choice(0, 3);

    try {
        if (op == 0){
            print_data_table(admin.get_cols(AIMS_DB, COURSE), admin.get_all_data(AIMS_DB, COURSE));
        }
        else if (op == 1){
            string code;
            string sem;

            while (code == "" || sem == ""){
                code = input("Enter course code: ");
                sem = input("Enter Semester name: ");
                if (get_val(stmt, AIMS_DB, COURSE, "Code", "Code", code, " Semester='" + sem + "'") != ""){
                    cout << RED "Course-Semester pair already exists!\n" NO_COLOR;
                    code = "";
                    sem = "";
                }
                else if (!no_spcl_ch(code+sem)){
                    cout << RED "No special character allowed (except '_')\n" NO_COLOR;
                    code = "";
                    sem = "";
                }
                else if (get_val(stmt, AIMS_DB, SEMESTER, "Name", "Name", sem) == ""){
                    cout << RED "Semester does not exist!\n" NO_COLOR;
                    code = "";
                    sem = "";
                }
            }

            vector<string> course = {code};

            course.push_back(input("Enter course name: "));
            course.push_back(input("Enter no. of credits: "));

            string faculty = input("Enter Faculty ID: ");
            while (get_val(stmt, AIMS_DB, FACULTY, "ID", "ID", faculty) == ""){
                cout << RED "Faculty does not exist!\n" NO_COLOR;
                faculty = input("Enter Faculty ID: ");
            }    
            course.push_back(faculty);
            course.push_back(sem);
            string seg = input("Enter segments for course (start-end (ex: 1-4)): ");
            while (str_split(seg, '-').size() != 2 || seg.size()>3){
                cout << RED "Wrong Segement format\n" NO_COLOR;
                seg = input("Enter segments for course (start-end (ex: 1-4)): ");
            }
            course.push_back(seg);
            course.push_back(confirm("Enter floating status") ? "Y" : "N");
            admin.add_course(course);
        }
        else if(op == 2){
            // auto data = admin.get_all_data(AIMS_DB, COURSE);
            string code;
            string sem;

            while (code == "" || sem == ""){
                if (code == "")
                    code = input("Enter course code: ");
                if (get_val(stmt, AIMS_DB, COURSE, "Code", "Code", code) == ""){
                    cout << RED "Course does not exist!\n" NO_COLOR;
                    code = "";
                    continue;
                }
                if (sem == "")
                    sem = input("Enter Semester name: ");
                if (get_val(stmt, AIMS_DB, SEMESTER, "Name", "Name", sem) == ""){
                    cout << RED "Semester does not exist!\n" NO_COLOR;
                    sem = "";
                }
                else if (get_val(stmt, AIMS_DB, COURSE, "Code", "Code", code, " Semester='" + sem + "'") == ""){
                    cout << RED "Course-Semester pair does not exist!\n" NO_COLOR;
                    code = "";
                    sem = "";
                }
            }
            admin.remove_course(code, sem);
        }
        else if (op == 3){
            string code;
            string sem;

            while (code == "" || sem == ""){
                if (code == "")
                    code = input("Enter course code: ");
                if (get_val(stmt, AIMS_DB, COURSE, "Code", "Code", code) == ""){
                    cout << RED "Course does not exist!\n" NO_COLOR;
                    code = "";
                    continue;
                }
                if (sem == "")
                    sem = input("Enter Semester name: ");
                if (get_val(stmt, AIMS_DB, SEMESTER, "Name", "Name", sem) == ""){
                    cout << RED "Semester does not exist!\n" NO_COLOR;
                    code = "";
                    sem = "";
                }
                else if (get_val(stmt, AIMS_DB, COURSE, "Code", "Code", code, " Semester='" + sem + "'") == ""){
                    cout << RED "Course-Semester pair does not exist!\n" NO_COLOR;
                    code = "";
                    sem = "";
                }
            }
            auto data = admin.get_data(AIMS_DB, COURSE, "Code", code, "Semester='" + sem + "'");
            auto cols = admin.get_cols(AIMS_DB, COURSE);

            auto new_data = update_data(cols, data, {0, 4});

            // USE_DB(AIMS_DB);
            // EXEC("DELETE FROM " COURSE " where Code='" + data.at(0) + "' and Semester='" + data.at(4) + "'");
            // insert_val(stmt, COURSE, new_data, {});
            update_val(stmt, AIMS_DB, COURSE, cols, new_data, "Code", code, "Semester='" + sem + "'");
        }
        if (op != 0){
            cout << GREEN "DONE!\n" NO_COLOR;
        }
    }
    catch (BackException){}
}


void update_students(Admin& admin){
    std::string msg = "\n0. View Students\n"
                    "1. Add student to IITH\n"
                    "2. Remove student from IITH\n"
                    "3. Add student to course\n"
                    "4. Remove student from course\n"
                    "5. Update student data\n\n";
    
    cout << msg;
    sql::Statement* stmt = admin.get_stmt();
    int op = get_choice(0, 5);

    try {
        if (op == 0){
            sql::ResultSet* rset = EXECQ("SELECT ID, NAME, DateOfBirth, AdmissionDate, FatherName, MotherName from " AIMS_DB "." STUDENT);
            print_data_table({"ID", "NAME", "Date Of Birth", "Admission Date", "Father Name", "Mother Name"}, extract(rset, 6));
        }
        else if (op == 1){
            string id = input("Enter Student ID: ");
            while (get_val(stmt, AIMS_DB, STUDENT, "ID", "ID", id) != "" || !no_spcl_ch(id)){
                if (no_spcl_ch(id))
                    cout << RED "This student already exists!\n" NO_COLOR;
                else 
                    cout << RED "No special characters allowed (except '_')" NO_COLOR;
                id = input("Enter Student ID: ");
            }
            vector<string> student = {id};
            student.push_back(input("Enter Name: "));
            student.push_back(input_date("Enter Date of birth (yyyy-mm-dd): "));
            student.push_back(get_current_date());
            student.push_back(input("Enter Father's name: "));
            student.push_back(input("Enter Mother's name: "));
            string passwd = input("Enter password: ");
            string salt = generate_salt();
            student.push_back(passwd_to_SHA256(salt, passwd));
            student.push_back(salt);
            admin.add_student_to_IITH(student);
        }
        else {
            string id = input("Enter Student ID: ");
            while (get_val(stmt, AIMS_DB, STUDENT, "ID", "ID", id) == ""){
                cout << RED "This student does not exist!\n" NO_COLOR;
                id = input("Enter Student ID: ");
            }
            if (op == 2)
                admin.remove_student_from_IITH(id);
            else if (op == 3){
                string code = input("Enter Course code: ");
                while (get_val(stmt, AIMS_DB, COURSE, "Code", "Code", code) == ""){
                    cout << RED "Course does not exists!!" << "\n" NO_COLOR;
                    code = input("Enter Course code: ");
                }
                admin.add_student_to_course(id, code);
            }
            else if (op == 4){
                string code = input("Enter Course code: ");
                while (get_val(stmt, AIMS_STU, "stu_" + id, "CourseCode", "CourseCode", code) == ""){
                    cout << RED "Student isn't registered in this course!!" << "\n" NO_COLOR;
                    code = input("Enter Course code: ");
                }
                admin.remove_student_from_course(id, code);
            }
            else if (op == 5){
                auto data = admin.get_data(AIMS_DB, STUDENT, "ID", id);
                auto cols = admin.get_cols(AIMS_DB, STUDENT);
                auto new_data = update_data(cols, data, {0, 2, 3, 6, 7});
                new_data[2] = input_date("DateOfBirth (" + data.at(2) + "): ", true);
                if (new_data[2] == ""){
                    new_data[2] = data.at(2);
                }
                new_data[3] = input_date("AdmissionDate (" + data.at(3) + "): ", true);
                if (new_data[3] == ""){
                    new_data[3] = data.at(3);
                }
                // delete_val(stmt, AIMS_DB, STUDENT, "ID", id);
                // USE_DB(AIMS_DB);
                // insert_val(stmt, STUDENT, new_data, {});
                update_val(stmt, AIMS_DB, STUDENT, admin.get_cols(AIMS_DB, STUDENT), new_data, "ID", id);
            }
        }
        if (op != 0){
            cout << GREEN "DONE!\n" NO_COLOR;
        }
    }
    catch (BackException){}
}


void update_faculty(Admin& admin){
    std::string msg = "\n0. View Faculty\n"
                    "1. Add Faculty\n"
                    "2. Remove Faculty\n"
                    "3. Update Faculty\n\n";

    cout << msg;
    sql::Statement* stmt = admin.get_stmt();
    int op = get_choice(0, 3);

    try {
        if (op == 0){
            sql::ResultSet* rset = EXECQ("SELECT ID, Name, JoiningDate from " AIMS_DB "." FACULTY);
            print_data_table({"ID", "Name", "Joining Date"}, extract(rset, 3));
        }
        else if (op == 1){
            string id = input("Enter Faculty ID: ");
            while (get_val(stmt, AIMS_DB, FACULTY, "ID", "ID", id) != ""){
                cout << RED "This Faculty already exists!\n" NO_COLOR;
                id = input("Enter Faculty ID: ");
            }
            vector<string> faculty = {id};
            faculty.push_back(input("Enter Name: "));
            faculty.push_back(get_current_date());
            string passwd = input("Enter password: ");
            string salt = generate_salt();
            faculty.push_back(passwd_to_SHA256(salt, passwd));
            faculty.push_back(salt);
            admin.add_faculty(faculty);
        }
        else {
            string id = input("Enter Faculty ID: ");
            while (get_val(stmt, AIMS_DB, FACULTY, "ID", "ID", id) == ""){
                cout << RED "This Faculty does not exist!\n" NO_COLOR;
                id = input("Enter Faculty ID: ");
            }
            if (op == 2){
                admin.remove_faculty(id);
            }
            else if (op == 3) {
                auto data = admin.get_data(AIMS_DB, FACULTY, "ID", id);
                // auto cols = admin.get_cols(AIMS_DB, FACULTY);
                std::cout << "Enter values for columns (Press enter without any value to keep previous value):\n";
                string val = input("Name (" + data.at(1) + "): ", true);
                if (val != ""){
                    data[1] = val;
                }
                val = input_date("JoiningDate (" + data.at(2) + "): ", true);
                if (val != ""){
                    data[2] = val;
                }
                // USE_DB(AIMS_DB);
                // delete_val(stmt, AIMS_DB, FACULTY, "ID", id);
                // insert_val(stmt, FACULTY, data, {});
                update_val(stmt, AIMS_DB, FACULTY, admin.get_cols(AIMS_DB, FACULTY), data, "id", id);
            }
        }
        if (op != 0){
            cout << GREEN "DONE!\n" NO_COLOR;
        }
    }
    catch (BackException){}
}

