#ifndef STRUT_H
#define STRUT_H

#include <string>
#include <iostream>
#include <vector>


std::string cullString(std::string);

std::string everythingLeftOfButNot(std::string,std::string,bool returnFullStringOnNothingFound=true);
std::string everythingLeftOfIncluding(std::string,std::string,bool returnFullStringOnNothingFound=true);

std::string everythingRightOfButNot(std::string,std::string,bool returnFullStringOnNothingFound=true);
std::string everythingRightOfIncluding(std::string,std::string,bool returnFullStringOnNothingFound=true);

bool stringFindBool(std::string,std::string);
bool canBeConvertedToNumber(std::string);

int getEndOfThisTag(std::vector<std::string>&,int);

std::string intToString(int);

std::string replace(std::string,std::string,std::string);

std::vector<int> findAllOccurences(std::string base, std::string find);

#endif