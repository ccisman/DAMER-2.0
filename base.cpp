//
// Created by cc on 2020/10/10.
//

#include "base.h"

int string_replace(std::string &s1, const std::string &s2, const std::string &s3)//replace s2 in s1 to s3
{
    std::string::size_type pos = 0;
    std::string::size_type a = s2.size();
    std::string::size_type b = s3.size();
    while ((pos = s1.find(s2, pos)) != std::string::npos)
    {
        s1.replace(pos, a, s3);
        pos += b;
    }
    return 0;
}

void readGraph(std::string input, std::string output) //transfer .txt to .dot
{

    const char* in = input.data();
    const char* ou = output.data();

    std::ifstream fin;
    fin.open(in, std::ios::in);

    std::ofstream fout;
    fout.open(ou, std::ios::out);

    fout << "digraph G{" << std::endl << "rankdir = LR" << std::endl;
    std::string s;
    while (getline(fin, s))
    {
        if (s[0] != '{') {
            fout << s << '\n';
            continue;
        }

        std::string u, v, lable;
        int n = int(s.length());
        int i = 1;
        //cout << s << n << std::endl;
        while (s[i] != ',') i++;
        //cout << i << std::endl;
        u += s.substr(1, i - 1);
        //cout << u << std::endl;
        int j = n - 2;
        while (s[j] != ',') j--;
        //cout << j << std::endl;
        v += s.substr(j + 1, n - 1 - j - 1);
        //cout << v << std::endl;
        //lable = s.substr(i + 1, j - i - 1);

        std::string edge = "";
        edge += u;
        edge += "->";
        edge += v;
        //edge += "[label=\"";
        //edge += lable;
        //edge += "\"];";
        fout << edge << std::endl;
    }
    fout << "}" << std::endl;
    fin.close();
    fout.close();
}

void makeGraph(std::string inputname, std::string outputname) //generate .png
{
    std::string s = "";
    s += "dot -Tpng ";
    s += inputname;
    s += " -o ";
    s += outputname;
    const char* cmd = s.data();
    const char* iname = inputname.data();
    system(cmd);
}

//template <class T>
void Bubble_sort(vector<string> &change_P)
{
    for (unsigned int i = 0; i < change_P.size(); i++)
    {
        for (unsigned int j = 0; j < change_P.size() - 1 - i; j++)
        {
            int num1 = atoi(change_P[j].substr(1).c_str());
            int num2 = atoi(change_P[j + 1].substr(1).c_str());
            if (num1 > num2)
            {
                string temp = change_P[j];
                change_P[j] = change_P[j + 1];
                change_P[j + 1] = temp;
            }
        }
    }
}


bool booloperate(string s1,string s2,string Operator){
    string v1=s1,v2=s2;
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
}