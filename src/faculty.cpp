#include "../include/faculty.hpp"

using namespace std;


void handle_faculty(sql::Statement* stmt, std::string id){
    Faculty faculty(stmt, id);
    string msg = "\n1. View Courses\n"
                "2. View Registered students\n"
                "3. Insert grades from csv\n"
                "4. Insert grades one by one\n"
                "5. Change Passsword\n\n";

    cout << msg;
    int op = get_choice(1, 5);

    if (op == 1){
        print_data_table(faculty.get_cols(AIMS_DB, COURSE), faculty.get_courses());
    }
    else if (op == 5){
        faculty.change_passwd(get_new_passwd(stmt, FACULTY, id));
    }
    else {
        string code;
        string sem;
 
        while (code == "" || sem == ""){
            if (code == "")
                code = input("Enter course code: ");
            if (get_val(stmt, AIMS_DB, COURSE, "Code", "Code", code, "InstructorId='" + faculty.get_id() + "'") == ""){
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
            else if (get_val(stmt, AIMS_DB, COURSE, "Code", "Code", code, " Semester='" + sem + "' and InstructorId='" + faculty.get_id() + "'") == ""){
                cout << RED "Course-Semester pair does not exist!\n" NO_COLOR;
                code = "";
                sem = "";
            }
        }
        if (op == 2)
            print_data_table({"ID", "Name", "GRADE"}, faculty.get_reg_students(code, sem));
        else if (op == 3){
            string file;
            std::vector<std::vector<std::string>> data;
            while (file == ""){
                file = input("Enter file name: ");
                if (!file_exists(file)){
                    cout << RED "File '" + file + "' does not exist!\n" NO_COLOR;
                    file = "";
                }
                else{
                    try{
                        data = read_csv(file);
                    }
                    catch (UnmatchingRowsException){
                        cout << RED "No. of rows in CSV file are not consistent!\n" NO_COLOR;
                        file = "";
                    }
                    if (file != ""){
                        if (data.size() == 0){
                            cout << RED "File is Empty!" NO_COLOR;
                            file = "";
                        }
                        else if (data.at(0).size() != 2){
                            cout << RED "File should have 2 columns!\n" NO_COLOR;
                            file = "";
                        }
                        for (auto gr: data){
                            if (gr.at(1).size() > 2 || gr.at(1).size() < 1){
                                cout << RED "Grade length should be 1 or 2\n" YELLOW << gr.at(0) << ", " << gr.at(1) << "\n" << NO_COLOR;
                                file = "";
                                break;
                            }
                        }
                    }
                }
            }
            // int count = faculty.student_count(code);
            auto students = faculty.get_reg_students(code, sem);
            int count = students.size();
            if (data.size() != count){
                cout << RED "No. of registered students is not equal to no. of students in csv!\n"
                            "No. of registered students = " + to_string(count) + "\nNo. of students in csv = " + to_string(data.size()) + "\n" NO_COLOR;
            }
            else {
                sort(data.begin(), data.end());
                bool fl = true;
                for (auto student: students){
                    if (search_n(data.begin(), data.end(), 1, student, [] (const vector<string>& row, const vector<string>& stu){
                                    return row.at(0) == stu.at(0);
                                }) == data.end()){
                        fl = false;
                        break;
                    }
                }
                if (!fl){
                    cout << RED "Some students are not present in csv file!\n" NO_COLOR;
                }
                else {
                    for (auto row: data){
                        EXEC("UPDATE " AIMS_COURSE ".course_" + code + "_" + sem + " set Grade='" + row.at(1) + "' where StudentId='" + row.at(0) + "'");
                        EXEC("UPDATE " AIMS_STU ".stu_" + row.at(0) + " set Grade='" + row.at(1) + "' where CourseCode='" + code + "'");
                    }
                }
            }
        }
        else if (op == 4){
            auto students = faculty.get_reg_students(code, sem);
            vector<vector<string>> grades;
            cout << "Enter grades:\n";
            // string sem = get_val(stmt, AIMS_COURSE, COURSE, "Semester", "Code", code, "isFloating='Y'");
            for (auto stu: students){
                string grade = input(stu.at(0) + " (" + stu.at(1) + "): ");
                while (search_n(grade_to_gpa.begin(), grade_to_gpa.end(), 1, grade, [](pair<string, int> p, const string& grd){
                                        return p.first == grd;
                                    }) == grade_to_gpa.end()){
                    cout << RED "Invalid grade!\n" NO_COLOR;
                    grade = input(stu.at(0) + " (" + stu.at(1) + "): ");
                }
                grades.push_back({stu.at(0), grade});
            }
            for (auto grade: grades){
                EXEC("UPDATE " AIMS_COURSE ".course_" + code + "_" + sem + " set Grade='" + grade.at(1) + "' where StudentId='" + grade.at(0) + "'");
                EXEC("UPDATE " AIMS_STU ".stu_" + grade.at(0) + " set Grade='" + grade.at(1) + "' where CourseCode='" + code + "'");
            }
        }
    }
    if (op == 3 || op == 4){
        cout << GREEN "DONE!\n" NO_COLOR;
    }
}
