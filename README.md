# MiniAIMS
A menu driven miniature AIMS CLI application.

## Logistics
### Admin
- Add and remove students, courses and faculty to and from IITH, 
- Register and deregister a student to and from a course disregarding the course floating status and grade submission status.

### Semester
- Name (should be unique)
- Start date
- End date 
- Type (Even or Odd)

### Course
- Code  ((Code, Semester) pair should be unique*)
- Name
- Credits
- Instructor ID
- Semester (Value should match a Name from "Semester" Table)
- Segment (Following format: start-end  (ex: 1-4))
- Is Floating (Y or N)  
\* This pair is made unique (and not only code) so that same table can store the same course running in different semesters for different batches.

### Student
- ID (should be unique)
- Name
- Date Of Birth
- Admission date
- Father's Name
- Mother's Name

### Faculty
- ID (should be unique)
- Name
- Joining Date


## Getting Started (Linux Only)
### Install MySQL
`$ sudo apt install mysql-server`  
`$ sudo systemctl start mysql`

### Configure MySQL
`$ sudo mysql`    
`mysql> create user '<user-name>'@'localhost' identified by '<password>';`  
`mysql> grant all privileges on aims_db.* to '<user-name>'@'localhost';`  
`mysql> grant all privileges on aims_stu.* to '<user-name>'@'localhost';`  
`mysql> grant all privileges on aims_course.* to '<user-name>'@'localhost';`  
`mysql> flush privileges;`  
`mysql> quit;`


### Install Dependencies
`$ sudo apt install make g++ libssl-dev libmysqlcppconn-dev`
`$ sudo apt install libssl-dev`

### Running the application
This app can be run using make  
`$ make`  
OR  
Using the commands  
`$ g++ main.cpp src/* -lmysqlcppconn -lcrypto -o aims`  
`$ ./aims`

### Running first time
- Application would ask for MySQL username and password if optional configuration (see below) is not done.
- Will ask you admin username, Name and password.
- Will ask if you want to enter some default data (Recommended for testing purposes).

### Optional Configuration
Create a file `.env` in the root directory and enter your mysql username and password (as entered above while configuring MySQL):
```
USER_NAME="<user-name>"
PASSWD="<password>"
```


## References
- [MySQL Docs](https://dev.mysql.com/doc/connector-cpp/1.1/en) (For MySQL Connector)
- [Stack Overflow](https://stackoverflow.com) (For doubts about C++ Classes)