#include "../include/actor.hpp"
#include "../include/handler.hpp"


// ======================
//         ACTOR
// ======================



std::string Actor::get_table(){
    return table;
}


std::string Actor::get_id(){
    return id;
}


uint32_t Actor::get_num_cols(std::string db, std::string table){
    if (db == "")
        db = AIMS_DB;
    if (table == "")
        table = this->table;
    int num;
    rset = EXECQ("SELECT count(*) FROM information_schema.columns where table_schema='" + db + "' and table_name='" + table + "'");
    if(rset->next())
        num = rset->getInt(1);
    else    // if table is not present, return 0
        num = 0;

    return num;
}


std::vector<std::string> Actor::get_cols(std::string db, std::string table){
    std::vector<std::string> cols;
    sql::ResultSet *rset = EXECQ("SELECT COLUMN_NAME FROM information_schema.columns where table_schema='" + db + "' and table_name='" + table + "' order by ordinal_position");
    while (rset->next()){
        cols.push_back(rset->getString(1));
    }

    return cols;
}


std::vector<std::vector<std::string>> Actor::get_all_data(std::string db, std::string table, std::string condition){
    if (table == "")
        table = this->table;
    if (db == "")
        db = AIMS_DB;

    std::vector<std::vector<std::string>> all_data;
    int num_cols = get_num_cols(db, table);

    USE_DB(db);
    rset = EXECQ("SELECT * FROM " + table + (condition != "" ? " where " + condition : ""));

    while (rset->next()){
        std::vector<std::string> data;  // a row in table

        for (int i=0; i<num_cols; i++){
            data.push_back(rset->getString(i+1));
        }
        all_data.push_back(data);       // add each row to whole table data
    }

    return all_data;
}


std::vector<std::string> Actor::get_data(std::string db, std::string table, std::string col, std::string val, std::string condition){
    std::vector<std::string> data;
    int num_cols = get_num_cols(db, table);

    USE_DB(db);
    rset = EXECQ("SELECT * FROM " + table + " where " + col + "='" + val + (condition != "" ? "' and " + condition : "'" ) );
    rset->next();
    for (int i=0; i<num_cols; i++){
        data.push_back(rset->getString(i+1));
    }

    return data;
}


void Actor::change_passwd(std::string new_pass){
    EXEC("update " AIMS_DB "." + table + " set Passwd='" + passwd_to_SHA256(get_val(stmt, AIMS_DB, table, "Salt", "ID", id), new_pass) + "' where ID='" + id + "'");
}


void Actor::exit(){
    // free resources
    delete stmt;
    delete conn;
}



// ======================
//         ADMIN
// ======================


Admin::Admin(){
    table = ADMIN;
    driver = get_driver_instance();

    // The sql user password, not the admin password
    std::vector<std::string> user_and_passwd = get_user_and_passwd();
    conn = driver->connect("localhost", user_and_passwd[0], user_and_passwd[1]);
    stmt = conn->createStatement();
}


Admin::Admin(const std::string &id){
    table = ADMIN;
    this->id = id;
    driver = get_driver_instance();

    // The sql user password, not the admin password
    std::vector<std::string> user_and_passwd = get_user_and_passwd();
    conn = driver->connect("localhost", user_and_passwd[0], user_and_passwd[1]);
    stmt = conn->createStatement();

    get_data_in_vars();
}


Admin::Admin(sql::Statement *stmt){
    this->stmt = stmt;
    table = ADMIN;
}


Admin::Admin(sql::Statement *stmt, const std::string &id){
    table = ADMIN;
    this->id = id;
    this->stmt = stmt;
    get_data_in_vars();
}


sql::Statement* Admin::get_stmt(){
    return stmt;
}


bool Admin::check_databases(){
    rset = EXECQ("SHOW DATABASES");
    std::vector<std::string> dbs;
    while (rset->next())
        dbs.push_back(rset->getString(1));

    // if any database is missing then returns false
    if (std::find(dbs.begin(), dbs.end(), AIMS_DB) == dbs.end() ||
        std::find(dbs.begin(), dbs.end(), AIMS_STU) == dbs.end() ||
        std::find(dbs.begin(), dbs.end(), AIMS_COURSE) == dbs.end()){
            return false;
    }
    return true;
}


void Admin::get_data_in_vars(){
    USE_DB(AIMS_DB);
    rset = EXECQ("SELECT * from " ADMIN " where id='" + id + "'");
    rset->next();

    name = rset->getString("Name");
    passwd = rset->getString("Passwd");
    salt = rset->getString("Salt");
}


void Admin::init(){
    EXEC(CREATE_DB AIMS_DB);
    EXEC(CREATE_DB AIMS_COURSE);
    EXEC(CREATE_DB AIMS_STU);
    EXEC("USE " AIMS_DB);
    // stmt->execute("CREATE TABLE Actor (ID varchar(32) PRIMARY KEY, Name varchar(100),"
    //         "Role char(10), Passwd char(" HASH_LENGTH "), Salt char(32))");

    EXEC("CREATE TABLE " ADMIN " (ID varchar(32) PRIMARY KEY, Name varchar(100), "
            "Passwd char(" HASH_LENGTH "), Salt char(" SALT_SIZE "))");

    EXEC("CREATE TABLE " STUDENT " (ID varchar(32) PRIMARY KEY, Name varchar(100), DateOfBirth date, AdmissionDate date, "
            "FatherName varchar(100), MotherName varchar(100), Passwd char(" HASH_LENGTH "), Salt char(" SALT_SIZE "))");

    EXEC("CREATE TABLE " FACULTY " (ID varchar(32) PRIMARY KEY, Name varchar(100), JoiningDate date, "
            "Passwd char(" HASH_LENGTH "), Salt char(" SALT_SIZE "))");

    EXEC("CREATE TABLE " SEMESTER " (Name varchar(50) PRIMARY KEY, Start date, End date, Type char(4))");

    EXEC("CREATE TABLE " COURSE " (Code varchar(32), Name varchar(100), Credits char(2), InstructorId varchar(32), "
            "Semester varchar(50), Segment char(10), isFloating char(1), FOREIGN KEY(InstructorId) REFERENCES " FACULTY "(ID), "
            " FOREIGN KEY(Semester) REFERENCES Semester(Name), PRIMARY KEY(Code, Semester))");
}


void Admin::clear_all(){
    EXEC(DROP_DB AIMS_COURSE);
    EXEC(DROP_DB AIMS_STU);
    EXEC(DROP_DB AIMS_DB);
}


void Admin::reset(){
    clear_all();
    init();
}


// void Admin::insert_default_data(){
//     // if (id == ""){
//     //     id = "adm0001.iith";
//     // }

//     // EXEC("INSERT INTO " ADMIN " VALUES ('" + id + "', 'The Admin'," 
//     //         "'7299f3488ded4e34277fd96afcdd911449b69b111aa45f6a2bd25d168f7a87f0', 'abcd')");
//     // default password is 'admin'

//     std::vector<std::vector<std::string>> stu_data = {
//         {"cs21btech11001", "Abhay Shankar", "2003-06-14", NULL_STR, NULL_STR, NULL_STR, PASSWORD, SALT},
//         {"cs21btech11002", "Abhinav Yadav", "2003-06-14", NULL_STR, NULL_STR, NULL_STR, PASSWORD, SALT},
//         {"cs21btech11030", "Karthik Kotikalapudi", "2003-06-14", NULL_STR, NULL_STR, NULL_STR, PASSWORD, SALT},
//         {"cs21btech11049", "Rahul Ramchandran", "2003-06-14", NULL_STR, NULL_STR, NULL_STR, PASSWORD, SALT},
//         {"cs21btech11055", "Sadineni Abhinay", "2003-06-14", NULL_STR, NULL_STR, NULL_STR, PASSWORD, SALT},
//         {"cs21btech11060", "Varun Gupta", "2003-06-14", NULL_STR, NULL_STR, NULL_STR, PASSWORD, SALT},
//     };
//     // insert_all(stmt, STUDENT, stu_data, {});
//     add_students_to_IITH(stu_data);

//     std::vector<std::vector<std::string>> faculty_data = {
//         {"11111cs.iith", "Dr. M V Panduranga Rao", NULL_STR, PASSWORD, SALT},
//         {"11112cs.iith", "Dr. Srijith PK", NULL_STR, PASSWORD, SALT},
//         {"11113cs.iith", "Dr. Subramanyam Kalyansundaram", NULL_STR, PASSWORD, SALT},
//     };
//     // insert_all(stmt, FACULTY, faculty_data, {});
//     add_faculties(faculty_data);
// }


void Admin::insert_default_data(){
    // read the data from CSV files
    auto faculties = read_csv(FACULTY_CSV);
    auto students = read_csv(STUDENT_CSV);
    auto semesters = read_csv(SEMESTER_CSV);
    auto courses = read_csv(COURSE_CSV);

    add_faculties(faculties);
    add_students_to_IITH(students);
    
    for (auto sem: semesters){
        add_semester(sem);
    }

    add_courses(courses);

    for (auto stu: students){
        for (auto course: courses){
            add_student_to_course(stu.at(0), course.at(0));
        }
        // Update the grade in aims_stu database
        EXEC("UPDATE " AIMS_STU ".stu_" + stu.at(0) + " set Grade='A' where Semester='UG21_NOV21_MAR22'");
    }
    for (auto course: courses){
        if (course.at(4) == "UG21_NOV21_MAR22")
            // Update the grade in aims_course database
            EXEC("UPDATE " AIMS_COURSE ".course_" + course.at(0) + "_" + course.at(4) + " set Grade='A'");
    }
}


void Admin::add_semester(const std::vector<std::string>& sem){
    USE_DB(AIMS_DB);
    insert_val(stmt, SEMESTER, sem, {});
}


void Admin::remove_semester(const std::string& sem){
    delete_val(stmt, AIMS_DB, SEMESTER, "Name", sem);
}


void Admin::add_course(const std::vector<std::string> &course){

    // Add course to Course table
    USE_DB(AIMS_DB)
    insert_val(stmt, COURSE, course, {});

    // Create the table for this course
    USE_DB(AIMS_COURSE);
    // EXEC("CREATE TABLE course_" + course[0] + "_" + course[3] + " (StudentId varchar(32) PRIMARY KEY, "
    // "Grade char(2), FOREIGN KEY(StudentId) REFERENCES " AIMS_DB "." STUDENT "(ID))");
    EXEC("CREATE TABLE course_" + course[0] + "_" + course[4] + " (StudentId varchar(32) PRIMARY KEY, "
    "Grade char(2))");
}


void Admin::add_courses(const std::vector<std::vector<std::string>> &courses){
    for (auto course: courses){
        add_course(course);
    }
}


void Admin::remove_course(const std::string& code, const std::string& sem){
    // delete value from aims_db.Course table
    USE_DB(AIMS_DB);
    EXEC("DELETE FROM " COURSE " where Code='" + code + "' and Semester='" + sem + "'");

    // Delete the aims_course.course_<code>_<sem> table
    USE_DB(AIMS_COURSE);
    EXEC("DROP TABLE course_" + code + "_" + sem);
}


// void Admin::remove_courses(const std::vector<std::string> &courses){
//     for (std::string course: courses){
//         remove_course(course);
//     }
// }


void Admin::add_student_to_IITH(const std::vector<std::string> &student){
    // insert value in aims_db.Student table
    USE_DB(AIMS_DB);
    insert_val(stmt, STUDENT, student, {});

    // Create aims_stu.stu_<id> table
    USE_DB(AIMS_STU);
    EXEC("CREATE TABLE stu_" + student[0] + " (CourseCode varchar(32), "
    "Semester varchar(50), RegistrationDate date, grade char(2), PRIMARY KEY(CourseCode, Semester))");
}


void Admin::add_students_to_IITH(const std::vector<std::vector<std::string>> &students){
    for (auto student: students){
        add_student_to_IITH(student);
    }
}


void Admin::remove_student_from_IITH(const std::string& id){
    // delete value from aims_db.Student table
    USE_DB(AIMS_DB);
    EXEC("DELETE FROM " STUDENT " where id='" + id + "'");

    // delete aims_stu.stu_<id> table
    USE_DB(AIMS_STU);
    EXEC("DROP TABLE stu_" + id);
}


void Admin::remove_students_from_IITH(const std::vector<std::string> &ids){
    for(std::string id: ids){
        remove_student_from_IITH(id);
    }
}


void Admin::add_student_to_course(const std::string& stu_id, const std::string& course_code){
    Student stu(stmt, stu_id);
    stu.reg_course(course_code);
}


// void Admin::add_students_to_course(const std::vector<std::string> &stu_ids, const std::string& course_code){
//     for (std::string stu_id: stu_ids){
//         add_student_to_course(stu_id, course_code);
//     }
// }


void Admin::remove_student_from_course(const std::string& stu_id, const std::string& course_code){
    Student stu(stmt, stu_id);
    stu.dereg_course(course_code);
}


// void Admin::remove_students_from_course(const std::vector<std::string> &stu_ids, const std::string& course_code){
//     for (std::string stu_id: stu_ids){
//         remove_student_from_course(stu_id, course_code);
//     }
// }


void Admin::add_faculty(const std::vector<std::string> &faculty){
    USE_DB(AIMS_DB);
    insert_val(stmt, FACULTY, faculty, {});
}


void Admin::add_faculties(const std::vector<std::vector<std::string>> &faculties){
    for (auto faculty: faculties){
        add_faculty(faculty);
    }
}


void Admin::remove_faculty(const std::string& id){
    delete_val(stmt, AIMS_DB, FACULTY, "ID", id);
}


void Admin::remove_faculties(const std::vector<std::string> &ids){
    for (std::string id: ids){
        remove_faculty(id);
    }
}
 



// ======================
//         STUDENT
// ======================


Student::Student(const std::string &id){
    this->id = id;
    table = STUDENT;

    driver = get_driver_instance();

    // The sql user password, not the admin password
    std::vector<std::string> user_and_passwd = get_user_and_passwd();
    conn = driver->connect("localhost", user_and_passwd[0], user_and_passwd[1]);
    stmt = conn->createStatement();
}


Student::Student(sql::Statement* stmt, const std::string& id){
    this->stmt = stmt;
    this->id = id;
    table = STUDENT;
}


void Student::get_data_in_vars(){
    USE_DB(AIMS_DB);
    EXECQ("SELECT * FROM " STUDENT " where id='" + id + "'");

    name = rset->getString("Name");
    f_name = rset->getString("FatherName");
    m_name = rset->getString("MotherName");
    dob = rset->getString("DateOfBirth");
    passwd = rset->getString("Passwd");
    salt = rset->getString("Salt");
}


void Student::reg_course(const std::string& code){
    // Insert course in Stu_id table in aims_stu db
    USE_DB(AIMS_STU);
    rset = EXECQ("SELECT * FROM " AIMS_DB "." COURSE " as c," AIMS_DB "." SEMESTER " as s where c.Semester=s.Name and code='" + code + "' ORDER BY s.Start DESC");
    rset->next();
    std::vector<std::string> course = {
        rset->getString("Code"),
        rset->getString("Semester"),
        get_current_date(),
        NULL_STR
    };

    insert_val(stmt, "stu_" + id, course, {});

    // Insert Student in Course_code_sem table of aims_course db
    USE_DB(AIMS_COURSE);
    std::vector<std::string> student = {id, NULL_STR};
    insert_val(stmt, "course_" + code + "_" + rset->getString("Semester"), student, {});
}


void Student::dereg_course(const std::string& code){
    // delete from stu_id table of aims_stu db
    USE_DB(AIMS_STU);
    rset = EXECQ("SELECT * FROM stu_" + id + " where CourseCode='" + code + "'");
    rset->next();
    delete_val(stmt, AIMS_STU, "stu_" + id, "CourseCode", code);

    // delete from course_code_sem table of aims_course db
    delete_val(stmt, AIMS_COURSE, "course_" + code + "_" + rset->getString("Semester"), "StudentId", id);
}


std::pair<std::vector<std::string>, std::vector<std::vector<std::string>>> Student::get_courses(){
    std::vector<std::vector<std::string>> courses;
    rset = EXECQ("SELECT s.CourseCode, c.Name, c.Credits, c.Semester, c.Segment, f.Name, s.RegistrationDate, s.Grade "
                    "FROM " AIMS_STU ".stu_" + id + " as s LEFT JOIN " AIMS_DB "." COURSE " as c ON s.CourseCode = c.Code AND s.Semester = c.Semester "
                    "LEFT JOIN " AIMS_DB "." FACULTY " as f ON c.InstructorId = f.ID LEFT JOIN " AIMS_DB "." SEMESTER " as sm ON s.Semester = sm.Name ORDER BY sm.Start");
    int num_cols = 8;
    std::vector<std::string> cols = {"Code", "Course", "Credits", "Semester", "Segment", "Instructor", "Reg. Date", "Grade"};
    while (rset->next()){
        std::vector<std::string> course;
        for (int i=0; i<num_cols; i++){
            course.push_back(rset->getString(i+1));
        }
        courses.push_back(course);
    }
    return std::make_pair(cols, courses);
}


double Student::gpa_in_sem(const std::string& sem){
    double gpa = 0;
    std::string grade;
    USE_DB(AIMS_STU);
    rset = EXECQ("SELECT c.Credits, s.Grade from stu_" + id + " as s, " AIMS_DB "." COURSE " as c where s.CourseCode=c.Code and s.Semester=c.Semester and s.Semester='" + sem + "'");
    rset->next();
    int num_courses = 0;

    for (int i=0; i<rset->rowsCount(); i++){
        grade = rset->getString("Grade");
        if (grade == ""){
            continue;
        }
        // std::cout << grade;
        gpa += (rset->getInt("Credits") * grade_to_gpa.at(grade));
        num_courses++;
    }
    if (num_courses == 0)
        return 0;
    return gpa / num_courses;
}


std::vector<std::string> Student::get_sems(){
    USE_DB(AIMS_STU);
    rset = EXECQ("SELECT DISTINCT Semester FROM stu_" + id);

    std::vector<std::string> sems;
    while (rset->next()){
        sems.push_back(rset->getString(1));
    }
    return sems;
}


double Student::calc_CGPA(){
    double cgpa = 0;
    double gpa;
    int sem_count = 0;
    auto sems = get_sems();
    for (auto sem: sems){
        gpa = gpa_in_sem(sem);
        if (gpa!=0){
            cgpa += gpa;
            sem_count++;
        }
    }
    if (sem_count == 0)
        return 0;

    return cgpa / sem_count;
}



// ======================
//         FACULTY
// ======================



Faculty::Faculty(const std::string& id){
    table = FACULTY;
    this->id = id;

    driver = get_driver_instance();

    // The sql user password, not the admin password
    std::vector<std::string> user_and_passwd = get_user_and_passwd();
    conn = driver->connect("localhost", user_and_passwd[0], user_and_passwd[1]);
    stmt = conn->createStatement();

    get_data_in_vars();
}


Faculty::Faculty(sql::Statement *stmt, const std::string& id){
    table = FACULTY;
    this->stmt = stmt;
    this->id = id;

    get_data_in_vars();
}


void Faculty::get_data_in_vars(){
    USE_DB(AIMS_DB);
    rset = EXECQ("SELECT * FROM " FACULTY " where id='" + id + "'");
    rset -> next();

    id = rset->getString("ID");
    name = rset->getString("Name");
    passwd = rset->getString("Passwd");
    salt = rset->getString("Salt");
}


std::vector<std::vector<std::string>> Faculty::get_courses(){
    std::vector<std::vector<std::string>> courses;
    return get_all_data(AIMS_DB, COURSE, "InstructorId=\"" + id + "\"");
}


std::vector<std::vector<std::string>> Faculty::get_reg_students(const std::string& code, const std::string& sem){
    rset = EXECQ("SELECT StudentId, Name, Grade from " AIMS_DB "." STUDENT ", " AIMS_COURSE ".course_" + code + "_" + sem + " where StudentId=ID");
    return extract(rset, 3);;
}


int Faculty::student_count(const std::string& code){
    std::string sem = get_val(stmt, AIMS_COURSE, COURSE, "Semester", "Code", code, "isFloating='Y'");
    // select no. of students
    rset = EXECQ("SELECT count(*) from " AIMS_COURSE ".course_" + code + "_" + sem);
    rset->next();
    return rset->getInt(1);
}

