#ifndef _STUDENTDATABASE_
#define _STUDENTDATABASE_

#include "B.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <fstream>


using namespace std;

class StudentDatabase{
	private:
		BTree* studentTable;
		BTree* classTable;
		BTree* classAssignmentTable;

		fstream studentTableStream;
		fstream classTableStream;
		fstream classAssignmentStream;
		
		string* add(string tableName, string* args, int arg_size);
		string* remove(string tableName, string* args, int arg_size);
		string* modify(string tableName, string* args, int arg_size);
		string* searchByName(string tableName, string* args, int arg_size);
		string* searchByID(string tableName, string* args, int arg_size);

		

		string* unpackStudent(string str);
		string* unpackClass(string str);
		string* unpackClassAssignment(string str);
		string extend(string str, int length);
		string dextend(string str);
		string* executeCommand(string command, string tableName, string* args, int arg_size);
		string stringify(string tableName);
		void initialize();	

	public:
		string studentNameToID(string studentFirstName, string lastName);
		string classNameToID(string className);		
		string classAssignmentToID(string classID, string studentID);
		string ui(string command);
		StudentDatabase(string sFilePath, string cFilePath, string aFilePath);
		~StudentDatabase();
};



#endif