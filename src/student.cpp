#include "../include/student.hpp"


using namespace std;


void handle_student(sql::Statement* stmt, std::string id){
    Student student(stmt, id);
    std::string msg = "\n1. Register for course\n"
                    "2. Deregister from course\n"
                    "3. View Courses\n"
                    "4. Get CGPA\n"
                    "5. Change Password\n\n";

    cout << msg;
    int op = get_choice(1, 5);

    if (op == 1){
        string code;
        while (code == ""){
            code = input("Enter Course code: ");
            if (get_val(stmt, AIMS_STU, "stu_" + id, "CourseCode", "CourseCode", code) != ""){
                cout << RED "You are already registered in this course!!\n" NO_COLOR;
                code = "";
            }
            else if (get_val(stmt, AIMS_DB, COURSE, "isFloating", "Code", code) != "Y"){
                cout << RED "Course isn't floating!\n" NO_COLOR;
                code = "";
            }
            else if (get_val(stmt, AIMS_STU, "stu_" + id, "Grade", "CourseCode", code) != ""){
                cout << RED "Grades submitted. Cannot register!\n" << NO_COLOR;
                code = "";
            }
        }
        student.reg_course(code);
    }
    else if (op == 2){
        string code;
        while (code == ""){
            code = input("Enter Course code: ");
            if (get_val(stmt, AIMS_STU, "stu_" + id, "CourseCode", "CourseCode", code) == ""){
                cout << RED "You are not registered in this course!!" << "\n" NO_COLOR;
                code = "";
            }
            else if (get_val(stmt, AIMS_STU, "stu_" + id, "Grade", "CourseCode", code) != ""){
                cout << RED "Grades submitted. Cannot deregister!\n" << NO_COLOR;
                code = "";
            }
        }
        student.dereg_course(code);
    }
    else if (op == 3){
        auto courses = student.get_courses();
        print_data_table(courses.first, courses.second);
    }

    else if (op == 4){
        auto sems = student.get_sems();
        for (auto sem: sems){
            double gpa = student.gpa_in_sem(sem);
            if (gpa != 0)
                cout << sem << ": " << gpa << "\n";
        }
        cout << "Your CGPA till now is: " << student.calc_CGPA() << "\n";
    }
    else if (op == 5){
        student.change_passwd(get_new_passwd(stmt, STUDENT, id));
    }
    if (op == 1 || op == 2){
        cout << GREEN "Done!\n" NO_COLOR;
    }
}
