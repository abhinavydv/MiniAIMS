#include "../include/student.hpp"


using namespace std;


bool handle_student(sql::Statement* stmt, std::string id){
    Student student(stmt, id);
    std::string msg = "1. Register for course\n"
                    "2. Deregister from course\n"
                    "3. View Courses\n"
                    "4. Get CGPA\n\n";

    cout << msg;
    int op = get_choice(1, 4);

    if (op == 1){
        string code;
        while (code == ""){
            code = input("Enter Course code: ");
            if (get_val(stmt, AIMS_STU, "stu_" + id, "CourseCode", "CourseCode", code) != ""){
                cout << YELLOW "You are already registered in this course!!" << "\n" NO_COLOR;
            }
            else if (get_val(stmt, AIMS_DB, COURSE, "CourseCode", "CourseCode", code) == ""){
                cout << YELLOW "Course does not exist!!" << "\n" NO_COLOR;
            }
            code = "";
        }
        student.reg_course(code);
    }
    else if (op == 2){
        string code = input("Enter Course code: ");
        while (get_val(stmt, AIMS_STU, "stu_" + id, "CourseCode", "CourseCode", code) == ""){
            cout << YELLOW "You are not registered in this course!!" << "\n" NO_COLOR;
            code = input("Enter Course code: ");
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
            cout << sem << ": " << gpa << "\n";
        }
        cout << "Your CGPA till now is: " << student.calc_CGPA() << "\n";
    }

    return true;
}
