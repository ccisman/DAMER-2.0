//
// Created by cc on 2020/10/10.
//

#include<sstream>
#include<iostream>
#include<fstream>
#include<regex>
#include<string>
#define TYPE_NUM 11
using namespace std;

string dec_type[TYPE_NUM]={"int","short","long","float","double","char","unsigned","signed","static","struct","pthread_t"};
int switchcount=0;

//pre-process
static string init_num = "0";

extern int string_replace(string &s1, const string &s2, const string &s3);

bool for_to_while(string s, string &oldtext, string &newtext)//true代表搜索到，false代表没搜索到，这里s不是引用
{
    regex pattern("for *\\((.*?);(.*?);(.*)\\)");
    smatch result;

    int position;
    int sum_num = 0, sum = 0,length;
    string text, res1, res2, res3;
    //string newtext, oldtext;

    if (regex_search(s, result, pattern)) {
        position = result.position();
        res1 = result[1];
        res2 = result[2];
        res3 = result[3];
        length = result.length();
    }
    else
        return false;
    newtext = "{\n" + res1 + ';' + "\nwhile(" + res2 + ')';
    for (unsigned int i = position + length; i < s.length(); i++)
    {
        if (s[i] == '{')
            sum_num++;
        else if (s[i] == '}')
        {
            sum_num--;
            if (sum_num == 0) {
                text = text + s[i];
                break;
            }
        }
        else if(s[i] == ';' && sum_num == 0){
            text = text + s[i];
            break;
        }
        text = text + s[i];
        sum++;
    }
    //cout << text << endl;
    //cout << sum;
    text = "{" + text + res3 + ";}\n}";
    newtext = newtext + text;
    oldtext = s.substr(position, sum + 1 + length);
    return true;
}

void for_to_while_all(string &s)
{
    string oldtext, newtext;
    while (1)
    {
        if (!for_to_while(s, oldtext, newtext))
            break;
        string_replace(s, oldtext, newtext);
    }
}

bool trans_plusplus(string s, string &newtext)//true代表搜索到，false代表没搜索到，这里s是引用
{
    regex pattern("\\+\\+");
    smatch result;

    int position, insert_position;
    int sum_num = 0, sum = 0;
    string text, res1, res2, res3;
    bool following_plus;
    //string newtext, oldtext;

    if (regex_search(s, result, pattern)) {
        position = result.position();
    }
    else
        return false;

    newtext = "";
    if (isalpha(s[position + 2]) || s[position + 2] == '_')
    {
        following_plus = false;
        for (unsigned int i = position + 2; i < s.length(); i++)
        {
            if (s[i] == '_' || isalpha(s[i])||isdigit(s[i]))
                text = s[i] + text;
            else
                break;
            sum++;
        }
    }
    else
    {
        following_plus = true;
        for (unsigned int i = position - 1; i > 0; i--)
        {
            if (s[i] == '_' || isalpha(s[i]) || isdigit(s[i]))
                text = s[i] + text;
            else
                break;
            sum++;
        }
    }
    //cout << text << endl;
    //cout << sum;
    text = text + "=" + text + "+1;\n";
    if (!following_plus)
    {
        for (unsigned int i = position; i >= 0; i--)
            if (s[i] == ';' || s[i] == '}')
            {
                insert_position = i + 1;
                break;
            }
        newtext = s.substr(0, insert_position) + text + s.substr(insert_position, position - insert_position) + s.substr(position + 2);
    }
    else
    {
        for (unsigned int i = position; i < s.length(); i++)
            if (s[i] == ';')
            {
                insert_position = i + 1;
                break;
            }
        newtext = s.substr(0, position) + s.substr(position + 2, insert_position - position - 2) + text + s.substr(insert_position);
    }

    return true;
}

void trans_plusplus_all(string &s)
{
    string newtext;
    while (1)
    {
        if (!trans_plusplus(s, newtext))
            break;
        s = newtext;
    }
}

bool trans_subsub(string s, string &newtext)//true代表搜索到，false代表没搜索到，这里s是引用
{
    regex pattern("\\-\\-");
    smatch result;

    int position, insert_position;
    int sum_num = 0, sum = 0;
    string text, res1, res2, res3;
    bool following_plus;
    //string newtext, oldtext;

    if (regex_search(s, result, pattern)) {
        position = result.position();
    }
    else
        return false;

    newtext = "";
    if (isalpha(s[position - 1]) || s[position - 1] == '_')
    {
        following_plus = true;
        for (unsigned int i = position - 1; i > 0; i--)
        {
            if (s[i] == '_' || isalpha(s[i]))
                text = s[i] + text;
            else
                break;
            sum++;
        }
    }
    else
    {
        following_plus = false;
        for (unsigned int i = position + 2; i < s.length(); i++)
        {
            if (s[i] == '_' || isalpha(s[i]))
                text = s[i] + text;
            else
                break;
            sum++;
        }
    }
    //cout << text << endl;
    //cout << sum;
    text = text + "=" + text + "-1;\n";
    if (!following_plus)
    {
        for (unsigned int i = position; i >= 0; i--)
            if (s[i] == ';' || s[i] == '}')
            {
                insert_position = i + 1;
                break;
            }
        newtext = s.substr(0, insert_position) + text + s.substr(insert_position, position - insert_position) + s.substr(position + 2);
    }
    else
    {
        for (unsigned int i = position; i < s.length(); i++)
            if (s[i] == ';')
            {
                insert_position = i + 1;
                break;
            }
        newtext = s.substr(0, position) + s.substr(position + 2, insert_position - position - 2) + text + s.substr(insert_position);
    }

    return true;
}

void trans_subsub_all(string &s)
{
    string newtext;
    while (1)
    {
        if (!trans_subsub(s, newtext))
            break;
        s = newtext;
    }
}

bool trans_assign(string &s)
{
    vector<string> sign_list;
    sign_list.push_back("+");
    sign_list.push_back("-");
    sign_list.push_back("*");
    sign_list.push_back("/");
    for (unsigned int i = 0; i < sign_list.size(); i++)
    {
        string sign = sign_list[i];
        string p = "([a-zA-Z_0-9]+ *)\\" + sign + "=";
        regex pattern(p);
        smatch result;

        int position;
        int sum_num = 0, sum = 0;
        string text, res1, newtext;

        if (regex_search(s, result, pattern)) {
            position = result.position();
            text = result[0];
            res1 = result[1];
        }
        else
            continue;
        newtext = "=" + res1 + sign;
        text.replace(text.find(sign + "="), 2, newtext);
        s.replace(s.find(result[0]), result[0].length(), text);
        return true;
    }
    return false;
}

void trans_assign_all(string &s)
{
    while (1)
    {
        if (!trans_assign(s))
            break;
    }
}

bool trans_define(string &s)
{
    regex pattern("#define[ \t]+([a-zA-Z_0-9]*)?[ \t]+(.*)?\n");
    smatch result;

    int position;
    int sum_num = 0, sum = 0;
    string text, text1, res1, res2;

    if (regex_search(s, result, pattern)) {
        position = result.position();
        res1 = result[1];
        res2 = result[2];
    }
    else
        return false;

    s = s.replace(s.find(result[0]), result[0].length(), "");

    regex pattern1("[^a-zA-Z_]" + res1 + "[^a-zA-Z_]");
    while (1)
    {
        if (regex_search(s, result, pattern1))
        {
            text = result[0];
            text1 = text;
            text1 = text1.replace(text1.find(res1), res1.length(), res2);
            s = s.replace(s.find(text), text.length(), text1);
        }
        else
            break;
    }
    return true;
}

void trans_define_all(string &s)
{
    while (1)
    {
        if (trans_define(s) == false)
            return;

    }
}

void trans_some_function(string &s)
{
    string err = "extern void __VERIFIER_error() __attribute__ ((__noreturn__));";

    string err1 = "extern void __VERIFIER_error();";
    string ass = "void __VERIFIER_assert(int cond) { if (!(cond)) { ERROR: __VERIFIER_error(); } return; }";
    string ass1 =
            "void __VERIFIER_assert(int cond) {\n\
		if (!(cond)) {\n\
			ERROR: __VERIFIER_error();\n\
		}\n\
		return;\n\
		}";

    string assum = "extern void __VERIFIER_assume(int expression);";

    string newassum=
            "void __VERIFIER_assume(int expression)\n\
		{\n\
			if (expression == 0)\n\
			{\n\
				__VERIFIER_error();\n\
			}\n\
		}\n";

    string newerr =
            "void __VERIFIER_error(){}\n";
    string newass =
            "void __VERIFIER_assert(int cond)\n\
		{\n\
			if (cond == 0)\n\
			{\n\
			__VERIFIER_error();\n\
			}\n\
		}\n";

    string nondet_int = "extern int __VERIFIER_nondet_int();";
    string nondet_int1 = "extern int __VERIFIER_nondet_int(void);";
    string newnondet_int =
            "int __VERIFIER_nondet_int()\n\
		{\n\
			nondet_num_int = nondet_num_int + 1;\n\
			return nondet_num_int;\n\
		}\n";
    string nondet_short = "extern short __VERIFIER_nondet_short();";
    string nondet_short1 = "extern short __VERIFIER_nondet_short(void);";
    string newnondet_short =
            "short __VERIFIER_nondet_short()\n\
		{\n\
			nondet_num_short = nondet_num_short + 1;\n\
			return nondet_num_short;\n\
		}\n";
    string nondet_long = "extern long __VERIFIER_nondet_long();";
    string newnondet_long =
            "long __VERIFIER_nondet_long()\n\
		{\n\
			nondet_num_long = nondet_num_long + 1;\n\
			return nondet_num_long;\n\
		}\n";
    string nondet_double = "extern double __VERIFIER_nondet_double();";
    string newnondet_double =
            "double __VERIFIER_nondet_double()\n\
		{\n\
			nondet_num_double = nondet_num_double + 1;\n\
			return nondet_num_double;\n\
		}\n";
    string nondet_uint = "unsigned int __VERIFIER_nondet_uint();";
    string newnondet_uint =
            "int __VERIFIER_nondet_uint()\n\
		{\n\
			nondet_num_uint = nondet_num_uint + 1;\n\
			return nondet_num_uint;\n\
		}\n";
    string_replace(s, err, newerr);
    string_replace(s, err1, newerr);
    //string_replace(s, ass, newass);
    //string_replace(s, ass1, newass);
    string_replace(s, "ERROR:", "");
    string_replace(s, assum, newassum);
    string_replace(s, nondet_int, newnondet_int);
    string_replace(s, nondet_int1, newnondet_int);
    string_replace(s, nondet_short, newnondet_short);
    string_replace(s, nondet_short1, newnondet_short);
    string_replace(s, nondet_double, newnondet_double);
    string_replace(s, nondet_uint, newnondet_uint);
    if (s.find("int nondet_num_int") == string::npos && s.find(newnondet_int) != string::npos )
        s = "int nondet_num_int=" + init_num + ";\n" + s;
    if (s.find("int nondet_num_short") == string::npos && s.find(newnondet_short) != string::npos)
        s = "int nondet_num_short=" + init_num + ";\n" + s;
    if (s.find("int nondet_num_long") == string::npos&&s.find(newnondet_long) != string::npos)
        s = "int nondet_num_long=" + init_num + ";\n" + s;
    if (s.find("int nondet_num_double") == string::npos&&s.find(newnondet_double) != string::npos)
        s = "int nondet_num_double=" + init_num + ";\n" + s;
    if (s.find("int nondet_num_uint") == string::npos&&s.find(newnondet_uint) != string::npos)
        s = "int nondet_num_uint=" + init_num + ";\n" + s;
    string pthread_library = "#include <pthread.h>";
    string stdio_library = "#include <stdio.h>";
    string assert_library = "#include <assert.h>";
    string stdlib_library = "#include <stdlib.h>";
    string string_library ="#include <string.h>";
    string_replace(s,  pthread_library, "");
    string_replace(s,  stdio_library, "");
    string_replace(s,  assert_library, "");
    string_replace(s,  stdlib_library, "");
    string_replace(s,  string_library, "");

    string atomic_begin = "extern void __VERIFIER_atomic_begin();";
    string atomic_begin_rp = "void __VERIFIER_atomic_begin(){}";
    string atomic_end = "extern void __VERIFIER_atomic_end();";
    string atomic_end_rp = "void __VERIFIER_atomic_end(){}";
    string abort = "extern void abort(void);";
    string abort_rp = "void abort(){}";
    string_replace(s,  atomic_begin, "");
    string_replace(s,  atomic_end, "");
    string_replace(s,  abort, abort_rp);

    string atomic_begin_call = "__VERIFIER_atomic_begin();";
    string atomic_end_call = "__VERIFIER_atomic_end();";
    string_replace(s,  atomic_begin_call, "");
    string_replace(s,  atomic_end_call, "");

    string gotoerror = "goto ERROR;";
    string_replace(s,gotoerror,"reach_error();");

}

void delete_space(string &s){
    while(auto siter = s.find(' ')){
        if(siter == string::npos)
            return;
        s.erase(siter,1);
    }
}

bool trans_switch(string &s)
{
    regex pattern("switch *\\((.*?)\\)");
    smatch result;
    int position;
    int sum_num = 0, sum = 0;
    string text, res1;

    if (regex_search(s, result, pattern)) {
        position = result.position();
        res1 = result[1];
        //res2 = result[2];
        //res3 = result[3];
    }
    else
        return false;

    for (unsigned int i = position; i < s.length(); i++)
    {
        if (sum_num > 0)
            text = text + s[i];
        if (s[i] == '{')
            sum_num++;
        else if (s[i] == '}')
        {
            sum_num--;
            text = text.substr(0, text.size() - 1);
            if (sum_num == 0)
                break;
        }

        sum++;
    }
    vector<string> label_list;
    vector<string> case_list;
    string default_label = "";

    regex pattern1("(case +)(.*?):");
    smatch result1;
    string temp_s = text;
    while (regex_search(temp_s, result1, pattern1))
    {
        string tmp,tmp1,case_str;
        case_str = result1[2];
        tmp = result1[1];
        tmp.append(case_str);
        tmp1 = tmp;

        delete_space(tmp);
        tmp += to_string(switchcount);
        text.replace(text.find(tmp1),tmp1.length(),tmp);

        label_list.push_back(tmp);
        case_list.push_back(case_str);

        temp_s = result1.suffix().str();
    }

    regex pattern2("(default *):");
    smatch result2;
    string temp_s1 = text;
    if (regex_search(temp_s1, result2, pattern2))
    {
        string tmp = result2[1],tmp1 = tmp;
        delete_space(tmp);
        tmp += to_string(switchcount);
        text.replace(text.find(tmp1),tmp1.length(),tmp);

        default_label = tmp;
    }

    string newtext;
    if (label_list.size() != 0)
        newtext = "\tif(" + res1 + "==" + case_list[0] + ")\n\t" + "goto " + label_list[0]+ ";\n";
    for (unsigned int i = 1; i < label_list.size(); i++)
        newtext += "\telse if(" + res1 + "==" + case_list[i] + ")\n\t" + "goto " + label_list[i]  + ";\n";
    if (default_label != "")
        newtext += "\telse\n\tgoto "+ default_label+ ";\n";

    newtext = "while(1){\n" + newtext + "\n" + text + "\nbreak;\n}\n";
    string oldtext = s.substr(position, sum + 1);
    string_replace(s, oldtext, newtext);
    switchcount++;
    /*cout << oldtext << endl;
    cout << newtext << endl;*/
}

void trans_switch_all(string &s)
{
    string oldtext, newtext;
    while (1)
    {
        if (!trans_switch(s))
            break;
    }
}

//** simplely judge **//
bool isnum(string s)
{
    stringstream sin(s);
    double t;
    char p;
    if(!(sin >> t))
        return false;
//    if(sin >> p)
//        return false;
//    else
    return true;
}
bool has_variable(string s)
{
    string temp = "";
    for(unsigned int i=0;i<s.length();i++) {
        if(s[i] == ' ')
            continue;
        if(s[i]!='_' && !isdigit(s[i]) && !isalpha(s[i]))
        {
            if(temp.size()>0 && !isnum(temp))
                return true;
            temp.clear();
        }
        else
            temp = temp + s[i];
    }
    if(temp.size() > 0 && !isnum(temp))
        return true;
    return false;
}

bool extract_dynamic_init(string &s,stack<string> &st)
{
    bool flag;
    int begin,end;
    string temp,right_exp;
    for(unsigned int i=0;i<s.length();i++)
    {
        if(s[i]=='=')
        {
            begin = end = i;
            while(begin >= 0 && s[begin] != ',')
                begin--;
            while(end<s.length() && s[end] != ',')
                end++;
            right_exp = s.substr(i+1,end-i-1);
            temp = s.substr(begin+1,end-begin-1);
            if(has_variable(right_exp)) {
                st.push(temp);
                s = s.substr(0,i) + s.substr(end,s.length());
                flag = true;
            }
            //cout<<"temp:"<<temp<<endl;
            //cout<<"right_exp:"<<right_exp<<endl;

        }
    }
    return flag;
}

bool judge_next_dec(string s,int position)
{
    int begin, end;

    begin = position;
    while (s[begin] == ' ' || s[begin] == '\n')
        begin++;
    end = begin;
    while (s[end] != ' ')
        end++;
    string temp = s.substr(begin, end - begin);


    for (unsigned int i = 0; i < TYPE_NUM; i++)
        if (dec_type[i] == temp)
            return true;
    return false;
}

bool judge_func_def(string s){
    //distinguish function definition and variable definition
    //we use the '=' out of () to judge
    int count=0;
    for(int i=0;i<s.length();i++){
        if(s[i] == '(')
            count++;
        else if(s[i] == ')')
            count--;
        else if(s[i] == '=' && count == 0)
            return false;
    }
    return true;
}

int trans_dynamic_init(string &s)
{
    stack<string> st;
    string str_pat = "\\W(";
    for(unsigned int i=0;i<TYPE_NUM;i++) {
        str_pat = str_pat + dec_type[i];
        if(i!=TYPE_NUM-1)
            str_pat = str_pat + "|";
    }
    str_pat = str_pat + ")(\\W.*?);";
    regex pattern(str_pat);
    smatch result;


    string text = s,res;
    string oldtext;

    while(regex_search(text, result, pattern)) {
//        cout<<"result = "<<result[2]<<endl;

        res = result[2];
        //cout<<"result[0].size:"<<result[0].length()<<endl;
        text = result.suffix();
        if(!judge_func_def(s)) {
            //cout<<"res:"<<result[0]<<endl;
            string old_res = res;
            if(extract_dynamic_init(res, st)) {
                s.replace(s.find(old_res), old_res.size(), res);
                if (!judge_next_dec(text, 0)) {
                    while (!st.empty()) {
                        string top = st.top();
                        top = "\n" + top + ";";
                        st.pop();
                        oldtext = text;
                        text.insert(0, top);
                        s.replace(s.find(oldtext), oldtext.size(), text);
//                    cout<<"text:"<<text<<endl;
//                    cout<<"s:"<<s<<endl;
                    }
                }
            }
        }

    }
}


int trans_advancedef(string &s){
    struct item{
        string content;
        long pos;
        bool is_insert;
        item(string c,long p,bool i):content(c),pos(p),is_insert(i){};
        bool operator<(item& item1){
            if(pos<item1.pos)
                return true;
            else
                return false;
        }
    };
//    stack<string> insert_content_arr;
//    stack<long> insert_pos_arr;
//    stack<long> delete_pos_arr;
    vector<item> items;

    string str_pat = "\\W(";
    for(unsigned int i=0;i<TYPE_NUM;i++) {
        str_pat = str_pat + dec_type[i];
        if(i!=TYPE_NUM-1)
            str_pat = str_pat + "|";
    }
    str_pat = str_pat + ")(\\W.*?;)";
    regex pattern(str_pat);
    smatch result;

    string text=s,insert_content;
    long pos=0,offset=0,insert_pos,delete_pos;
    int count=0;

    while(regex_search(text,result,pattern)){
        pos = result.position();
        int i= offset + pos;
        insert_pos = 0;
        count = 0;

        insert_content = result[1];
        insert_content += result[2];
        insert_content = "\n" + insert_content + "\n";

        text = result.suffix();

        int find_last_word_pos = pos + offset;
        while(find_last_word_pos>=0 && (
                s[find_last_word_pos] == '\t'
                || s[find_last_word_pos] == '\n'
                || s[find_last_word_pos] == ' '))
            find_last_word_pos--;
        if(s[find_last_word_pos] != ';' && s[find_last_word_pos] != '{'){}
        else {
            while (i >= 0) {

                if (s[i] == '{') {
                    count++;
                    if (count > 0) {
                        insert_pos = i + 1;
                        break;
                    }
                } else if (s[i] == '}')
                    count--;
                i--;
            }
            if (insert_pos != 0) {

                items.emplace_back(insert_content, offset + pos , false);
                items.emplace_back(insert_content, insert_pos, true);

            }
        }
        offset += pos + result.length();
    }

    sort(items.begin(),items.end());

    for(int i=items.size()-1;i>=0;i--){
        if(!items[i].is_insert){
            s.replace(items[i].pos,items[i].content.length(),"");
        }
        else{
            s.insert(items[i].pos,items[i].content);
        }
    }

}

void trans_annotation(string &s){
    string an_patten = "//.*?\n";

    regex pattern(an_patten);
    smatch result;
    string col;

    while(regex_search(s,result,pattern)){
        col = result[0];
        auto pos = result.position();
        s = s.replace(pos,col.length(),"");
    }
}

void trans_Bool2int(string &s){
    string boolpattern = "_Bool";
    regex pattern(boolpattern);
    smatch result;

    while(regex_search(s,result,pattern)){
        auto pos = result.position();
        s = s.replace(pos,boolpattern.length(),"int");
    }
}

void Add_NULL(string &s){
    regex pattern("[^a-zA-Z_]NULL[^a-zA-Z_]");
    smatch result;
    if(regex_search(s,result,pattern)){
        s = "#define NULL 0\n" + s;
    }

}

void pre_process(string &s)
{
    Add_NULL(s);
    trans_annotation(s);
    trans_Bool2int(s);
    trans_define_all(s);
    for_to_while_all(s);
//    trans_plusplus_all(s);
//    trans_subsub_all(s);
    trans_some_function(s);
    trans_assign_all(s);
    trans_switch_all(s);
    trans_dynamic_init(s);
    trans_advancedef(s);
    //cout << s << endl;
}