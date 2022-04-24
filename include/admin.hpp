#include "handler.hpp"
#include "actor.hpp"


bool handle_admin(sql::Statement*, std::string);
void update_sem(Admin&);
void update_courses(Admin&);
void update_students(Admin&);
void update_faculty(Admin&);