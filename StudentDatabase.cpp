#include "StudentDatabase.h"

using namespace std;

//Constructor
StudentDatabase::StudentDatabase(string sFilePath, string cFilePath, string aFilePath){
	string studentTableAddress = sFilePath;
	string classTableAddress = cFilePath;
	string classAssignmentsTableAddress = aFilePath;
	
	studentTableStream.open(studentTableAddress,ios::in|ios::out|ios::binary);
	classTableStream.open(classTableAddress,ios::in|ios::out|ios::binary);
	classAssignmentStream.open(classAssignmentsTableAddress,ios::in|ios::out|ios::binary);
	
	studentTable = new BTree(&studentTableStream, 8, 50, false);
	classTable = new BTree(&classTableStream, 10, 50, false);
	classAssignmentTable = new BTree(&classAssignmentStream, 18, 2, false);
}

//Initializes/Creates a new the database (the default is to start with a prexisting database)
void StudentDatabase::initialize(){
	delete studentTable;
	delete classTable;
	delete classAssignmentTable;
	studentTable = new BTree(&studentTableStream, 8, 50, true);
	classTable = new BTree(&classTableStream, 10, 50, true);
	classAssignmentTable = new BTree(&classAssignmentStream, 18, 2, true);
}

//Destructor (also closes the streams)
StudentDatabase::~StudentDatabase(){
	delete studentTable;
	delete classTable;
	delete classAssignmentTable;
	studentTableStream.close();
	classTableStream.close();
	classAssignmentStream.close();
}

//Removes spaces from the end of a string
string StudentDatabase::dextend(string str){
	while(str[str.length()-1] == '.'){
		str = str.substr(0, str.length()-1);
	}
	return str;
}

//Adds spaces to the end of a string
string StudentDatabase::extend(string str, int length){
	while(str.length() < length){
		str += '.';
	}
	return str;
}

//Returns a students unique id (Id's are static for the lifetime of the database. If you make a new database and enter users in a different order students (and classes) might have different id's).
string StudentDatabase::studentNameToID(string firstName, string lastName){
	string new_firstName = extend(firstName, 20);
	string new_lastName = extend(lastName, 30);
	string id = new_firstName.substr(0, 4) + new_lastName.substr(0, 4);
	int i = 4;
	DRT* res = studentTable->search(id);
	while(i > 0 && res->getdata() != ""){
		string f_name = dextend(res->getdata().substr(0, 20));
		string l_name = dextend(res->getdata().substr(20, 30));
		id = new_firstName.substr(0, 4 + (4-i)) + new_lastName.substr(0, i);
		res = studentTable->search(id);
		i -= 1;
	}
	return id;
}

//Generates a unique Class ID
string StudentDatabase::classNameToID(string className){
	string e_className = extend(className, 50);
	string id = e_className.substr(0, 10);
	int i = 0;
	DRT* res = classTable->search(id);
	while(i < 41 && res->getdata() != ""){
		id = extend(e_className.substr(0 + i, className.length()) + e_className.substr(0, i), 10);
		res = classTable->search(id);
		i += 1;
	}
	return id;
}

//Generates a unique Class Assignment ID
string StudentDatabase::classAssignmentToID(string classID, string studentID){
	return classID + studentID;
}

//Turns the stored student data into a string
string* StudentDatabase::unpackStudent(string str){
	//I considered returning a string array, but chose not to
	return new string[1]{dextend(str.substr(0, 20)) + " " + dextend(str.substr(20, 30))};
}

//Turns the stored class data into a string
string* StudentDatabase::unpackClass(string str){
	return new string[1]{dextend(str.substr(0, 50))};
}

//Turns the stored data into a string
string* StudentDatabase::unpackClassAssignment(string str){
	return new string[1]{dextend(str.substr(0, 2))};
}

//Adds to a table in the database
string* StudentDatabase::add(string tableName, string* args, int arg_size){
	if(tableName == "Students" && arg_size == 3){
		string data = extend(args[1], 20) + extend(args[2], 30);
		if(data != ""){
			studentTable->modify(args[0], data);
			return new string[1]{""};
		}
		else{
			return new string[1]{"Invalid Argument Size"};
		}
	}
	else if(tableName == "Classes" && arg_size == 2){
		string data = extend(args[1], 50);
		if(data != ""){
			classTable->modify(args[0], data);
			return new string[1]{""};
		}
		else{
			return new string[1]{"Invalid Argument Size"};
		}
	}
	else if(tableName == "ClassAssignments" && arg_size == 2){
		string data = extend(args[1], 2);
		if(data != ""){
			classAssignmentTable->modify(args[0], data);
			return new string[1]{""};
		}
		else{
			return new string[1]{"Invalid Argument Size"};
		}
	}
	else{
		return new string[1]{"Invalid Argument Arity or Table Name"};
	}
}

//Modifies a reccord in a table
string* StudentDatabase::modify(string tableName, string* args, int arg_size){
	if(tableName == "ClassAssignments" && arg_size == 2){
		classAssignmentTable->modify(args[0], extend(args[1], 2));
		return new string[1]{""};
	}
	else if(tableName == "Students" && arg_size == 3){
		studentTable->modify(args[0], "");
		studentTable->modify(args[0], extend(args[1], 20) + extend(args[2], 30));
		return new string[1]{""};
	}
	else if(tableName == "Classes" && arg_size == 2){
		classTable->modify(args[0], "");
		classTable->modify(args[0], extend(args[1], 50));
		return new string[1]{""};
	}
	else{
		return new string[1]{"Invalid Argument Arity or Table Name"};
	}
}

//Removes a reccord from a table
string* StudentDatabase::remove(string tableName, string* args, int arg_size){
	if(tableName == "Students" && arg_size == 1){
		studentTable->modify(args[0], "");
		return new string[1]{""};
	}
	else if(tableName == "Classes" && arg_size == 1){
		classTable->modify(args[0], "");
		return new string[1]{""};
	}
	else if(tableName == "ClassAssignments" && arg_size == 1){
		classAssignmentTable->modify(args[0], "");
		return new string[1]{""};
	}
	else{
		return new string[1]{"Invalid Argument Arity or Table Name"};
	}
}

//Searches a table based on ID
string* StudentDatabase::searchByID(string tableName, string* args, int arg_size){
	if(tableName == "Students" && arg_size == 2){
		return unpackStudent(studentTable->search(args[1])->getdata());
	}
	else if(tableName == "Classes" && arg_size == 2){
		return unpackClass(classTable->search(args[1])->getdata());
	}
	else if(tableName == "ClassAssignments" && arg_size == 2){
		return unpackClassAssignment(classAssignmentTable->search(args[1])->getdata());
	}
	else{
		return new string[1]{"Invalid Argument Arity or Table Name"};
	}
}

//Searches a table based on name
string* StudentDatabase::searchByName(string tableName, string* args, int arg_size){
	if(tableName == "Students" && arg_size == 3){
		return unpackStudent(studentTable->search(studentNameToID(args[1], args[2]))->getdata());
	}
	else if(tableName == "Classes" && arg_size == 2){
		return unpackClass(classTable->search(classNameToID(args[1]))->getdata());
	}
	else if(tableName == "ClassAssignments" && arg_size == 4){
		return unpackClassAssignment(classAssignmentTable->search(classAssignmentToID(args[1], args[2]))->getdata());
	}
	else{
		return new string[1]{"Invalid Database Search Arguments"};
	}
}

//Turns a table into a string
string StudentDatabase::stringify(string tableName){
	string table = "";
	if(tableName == "Students"){
		DRT* s = studentTable->search("");
		string key = s->getnext();
		while(s->getnext() != ""){
			key = s->getnext();
			s = studentTable->search(key);
			string d = s->getdata();
			table += "ID: " + key + " " + "First_Name: " + dextend(d.substr(0, 20)) + " Last_Name: " + dextend(d.substr(20, 30)) + "\n";
		}
		delete s;
		return table;
	}
	else if(tableName == "Classes"){
		DRT* s = classTable->search("");
		string key = s->getnext();
		while(s->getnext() != ""){
			key = s->getnext();
			s = classTable->search(key);
			string d = s->getdata();
			table += "ID: " + key + " " + "Class_Name: " + dextend(d.substr(0, 50)) + "\n";
		}
		delete s;
		return table;
	}
	else if(tableName == "ClassAssignments"){
		DRT* s = classAssignmentTable->search("");
		string key = s->getnext();
		while(s->getnext() != ""){
			key = s->getnext();
			s = classAssignmentTable->search(key);
			string d = s->getdata();
			table += "ID: " + key + " " + "Grade: " + dextend(d.substr(0, 50)) + "\n";
		}
		delete s;
		return table;
	}

	return "Invalid Table Name";
}

//Executes commands; dispatches the user input to the correct methods.
string* StudentDatabase::executeCommand(string command, string tableName, string* args, int arg_size){
	if(command == "ADD"){
		return add(tableName, args, arg_size);
	}
	else if(command == "MODIFY"){
		return modify(tableName, args, arg_size);
	}
	else if(command == "REMOVE"){
		return remove(tableName, args, arg_size);
	}
	else if(command == "SEARCH"){
		if(args[0] == "ID"){
			return searchByID(tableName, args, arg_size);
		}
		else if(args[0] == "NAME"){
			return searchByName(tableName, args, arg_size);
		}
		else{
			return new string[1]{"Invalid Database Command"};
		}
	}
	else if(command == "OUT"){
		return new string[1]{stringify(tableName)};
	}
	else if(command == "INITIALIZE"){
		initialize();
		return new string[1]{"Database Initialized"};
	}
	else{
		return new string[1]{"Invalid Database Command"};
	}
}

//Parses the user input
string StudentDatabase::ui(string command){
	int wordCount = 1;
	for(int i = 0; i < command.length(); i++) (command[i] == ' ') ? wordCount += 1 : 0;
	string* words = new string[wordCount];
	for(int i = 0; i < wordCount; i++) words[i] = "";
	int i = 0;
	int j = 0;
	while(j < command.length()){
		if(command[j] == ' '){
			i += 1;
		}
		else{
			words[i] += command[j];
		}
		j += 1;
	}		
	if(wordCount < 2) {
		if(words[0] == "INITIALIZE"){
			initialize();
			return "Initialized";
		}
		return "Invalid User Input";
	}
	string* args = NULL;
	int argCount = 0;
	if(wordCount > 2){
		args = new string[wordCount-2];
		for(; argCount < wordCount-2; argCount++){
			args[argCount] = words[argCount+2];
		}
	}
	string* result = executeCommand(words[0], words[1], args, argCount);
	free(args);
	free(words);
	return result[0];
}