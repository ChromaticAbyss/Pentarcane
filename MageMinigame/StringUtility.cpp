#include <string>
#include <iostream>
#include <vector>
using namespace std;

bool canBeConvertedToNumber(string input){
	static bool initialized=false;
	static vector<char> legitSymbols;
	static int lSym=0;
	if(initialized==false){
		legitSymbols.push_back('0');
		legitSymbols.push_back('1');
		legitSymbols.push_back('2');
		legitSymbols.push_back('3');
		legitSymbols.push_back('4');
		legitSymbols.push_back('5');
		legitSymbols.push_back('6');
		legitSymbols.push_back('7');
		legitSymbols.push_back('8');
		legitSymbols.push_back('9');
		legitSymbols.push_back('.');
		legitSymbols.push_back(',');
		legitSymbols.push_back(' ');
		legitSymbols.push_back('\t');
		legitSymbols.push_back('-');
		initialized=true;
		lSym=legitSymbols.size();
	}

	int len=input.size();

	for(int i=0;i<len;i++){
		char cha=input[i];
		bool legit=false;
		for(int l=0;l<lSym;l++){
			if(legitSymbols[l]==cha){
				legit=true;
				break;
			}
		}

		if(legit==false){
			//cout<<cha<<" is not a legit symbol for conversion into float!";  
			return false;
		}
	}

	return true;
}


string cullString(string input){
	if(input.size()==0){return "";}
	string working=input;

	bool foundSomethingToCull=true;

	while(foundSomethingToCull){
		foundSomethingToCull=false;
		

		if(working[0]==' ' || working[0]=='\t'){
			working=working.substr(1);
			if(working.size()==0){return "";}//positive size?
			foundSomethingToCull=true;
		}

		int len=working.size();
		if(working.size()==0){return "";}//positive size?

			if(working[len-1]==' ' || working[len-1]=='\t'){
				len=len-1;
				if(len>0){
					working=working.substr(0,working.size()-1);
				};
				foundSomethingToCull=true;
			}

	}//cull as long as there are things left to cull

	return working;
}





string everythingLeftOfButNot(string input,string toFind, bool returnFullStringOnNothingFound=true){
	//Returns full string if nothing is found
	if(input.size()==0){return "";}
	string working=input;

	int pos=working.find(toFind);
	if (pos==std::string::npos){
		cout<<" everything**** did not find "<<toFind<<" in its input: "<<input<<endl;
		if(returnFullStringOnNothingFound){return working;}else{return "";}
	}


	working=working.substr(0,pos);

	//cout<<"|"<<working<<"|"<<endl;
	return working;
}

string everythingLeftOfIncluding(string input,string toFind, bool returnFullStringOnNothingFound=true){
	//returns empty string if nothing is found
	if(input.size()==0){return "";}
	string working=input;

	int pos=working.find(toFind);
	if (pos==std::string::npos){
		cout<<" everything**** did not find "<<toFind<<" in its input: "<<input<<endl;
		if(returnFullStringOnNothingFound){return working;}else{return "";}
	}

	working=working.substr(0,pos+toFind.size());

	//cout<<"|"<<working<<"|"<<endl;
	return working;
}

string everythingRightOfButNot(string input,string toFind, bool returnFullStringOnNothingFound=true){
	//Returns whole string if nothing is found
	if(input.size()==0){return "";}
	string working=input;

	int pos=working.find(toFind);
	if (pos==std::string::npos){
		cout<<" everything**** did not find "<<toFind<<" in its input: "<<input<<endl;
		if(returnFullStringOnNothingFound){return working;}else{return "";}
	}

	working=working.substr(pos+toFind.size());

	//cout<<"|"<<working<<"|"<<endl;
	return working;
}

string everythingRightOfIncluding(string input,string toFind, bool returnFullStringOnNothingFound=true){
	//Returns whole string if nothing is found
	if(input.size()==0){return "";}
	string working=input;

	int pos=working.find(toFind);
	if (pos==std::string::npos){
		cout<<" everything**** did not find "<<toFind<<" in its input: "<<input<<endl;
		if(returnFullStringOnNothingFound){return working;}else{return "";}
	}

	working=working.substr(pos);

	//cout<<"|"<<working<<"|"<<endl;
	return working;
}


bool stringFindBool(string input,string toFind){
	int pos=input.find(toFind);
	if (pos==std::string::npos){
		return false;
	}
	return true;
}

int getEndOfThisTag(vector<string>& data,int lineToStartIn){
	//might be inefficient
	unsigned int position=lineToStartIn;
	int depth=1;

	string tagToClose=everythingRightOfButNot(data[position],"<");
	tagToClose=everythingLeftOfButNot(tagToClose,">");
	if(stringFindBool(tagToClose," ")){
		tagToClose=everythingLeftOfButNot(tagToClose," ");
	}
	
	//cout<<"Looking for closure of: "<<tagToClose<<endl;

	position++;
	while(position<data.size()){
		string line=data[position];
		//cout<<"L "<<line<<endl;
		if (stringFindBool(line,"<"+tagToClose+">")){
			depth++; //We find other instances of this tag nested within it!
			//cout<<"Another opener found: Depth is now: "<<depth<<endl;
		}

		

		if (stringFindBool(line,"</"+tagToClose+">")){
			depth--;

			//cout<<"Closure found: Depth is now: "<<depth<<endl;
			if(depth==0){return position;}
		}

		position=position+1;
	}


	cout<<"ERROR: Tag does not close: "<<data[lineToStartIn]<<endl;
	return -1; //
}

string intToString(int a){
	return to_string(static_cast<long long>(a));
};


string replace(string base,string find,string replaceWith){
	int pos=base.find(find);
	if (pos==std::string::npos){
		return base;
	}

	string leftPart=everythingLeftOfButNot(base,find);
	string rightPart=everythingRightOfButNot(base,find);
	rightPart=replace(rightPart,find,replaceWith);

	return leftPart+replaceWith+rightPart;

}

vector<int> findAllOccurences(string base, string find){
 string str,sub; // str is string to search, sub is the substring to search for

 vector<int> positions; // holds all the positions that sub occurs within str

 size_t pos = base.find(find, 0);
 while(pos != string::npos)
 {
     positions.push_back(pos);
     pos = base.find(find,pos+1);
 }

 return positions;
}