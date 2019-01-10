#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib> 

#include "Log.h"

using namespace std;

void Log(string text) {
	Log("Debug", text);
}

bool Log(string type ,string text){
	//Types: Error,Error(Data),Warning,Warning(Data)
	static ofstream myfile("../Log.txt");
	static int calls=0;

	cout<<calls<<"|"<<type<<"\t: "<<text<<endl;


	  if (myfile.is_open())
	  {
		myfile<<calls<<"|"<<type<<"\t: "<<text<<endl;
	  }
	  else cout << "Unable to open file: DataFiles.txt"; 


	  if (type == "Error") {
		  int i = 7;
	  }

	  calls++;
	  return true;
};

bool Log(string type ,string text1, string text2){
	//Types: Error,Error(Data),Warning,Warning(Data)
	string a = text1 + " " + text2;
	return Log(type,a);
};