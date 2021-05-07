//
// Created by cc on 2020/8/4.
//

#ifndef PDNET_CHECKER_BASE_H
#define PDNET_CHECKER_BASE_H

#include<string>
#include<vector>
#include <fstream>
#include<iostream>
using namespace std;

extern int string_replace(std::string &s1, const std::string &s2, const std::string &s3);
extern void readGraph(std::string input, std::string output);
extern void makeGraph(std::string inputname, std::string outputname);

extern vector<string> pthread_type;
extern vector<string> pthread_func_type;

template <class T>
bool exist_in(vector<T> &vec,T element)
{
    for(unsigned int i=0;i<vec.size();i++)
        if(element == vec[i])
            return true;

    return false;
}


template<class T>
T operate(T s1,T s2,string Operator) {
    T v1, v2;
    v1 = s1;
    v2 = s2;
    if (Operator == "+")
        return v1 + v2;
    else if (Operator == "-")
        return v1 - v2;
    else if (Operator == "*")
        return v1 * v2;
    else if (Operator == "/") {
        if (v2 != 0)
            return v1 / v2;
        else {
            cerr << "divide 0 ERROR!" << endl;
            exit(-1);
        }
    } else if (Operator == "&")
        return int(v1) & int(v2);
    else if (Operator == "|")
        return int(v1) | int(v2);
    else if (Operator == "^")
        return int(v1) ^ int(v2);
    else if (Operator == "%")
        return int(v1) % int(v2);
    else if (Operator == "!")
        return !(int(v1));
    else if (Operator == "@")
        return -v1;
}

template<class T>
bool booloperate(T s1,T s2,string Operator) {
    T v1, v2;
    v1 = s1;
    v2 = s2;
    if (Operator == ">")
        return v1 > v2;
    else if (Operator == ">=")
        return v1 >= v2;
    else if (Operator == "<")
        return v1 < v2;
    else if (Operator == "<=")
        return v1 <= v2;
    else if (Operator == "==")
        return v1 == v2;
    else if (Operator == "!=")
        return v1 != v2;
    else if (Operator == "||")
        return v1 || v2;
    else if (Operator == "&&")
        return v1 && v2;

}

extern bool booloperate(string s1,string s2,string Operator);


#endif //PDNET_CHECKER_BASE_H