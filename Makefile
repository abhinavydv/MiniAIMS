all: compile-files create-executable clean-up run

executable = aims

compile-files:
	@echo "\ncompiling files"
	g++ -c main.cpp src/actor.cpp src/handler.cpp src/admin.cpp src/faculty.cpp src/student.cpp

create-executable:
	@echo "\nLinking and Creating Executable"
	g++ handler.o actor.o admin.o faculty.o student.o main.o -o $(executable) -lmysqlcppconn -lcrypto

clean-up:
	@echo "\nCleaning up"
	rm *.o

run:
	@echo "\n\nRunning...\n"
	@./$(executable)

exit:
	@echo "\n\nrun" ./$(executable) in terminal
