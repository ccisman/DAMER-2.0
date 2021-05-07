//
// Created by cc on 2020/10/10.
//

#ifndef CPN_PNML_PARSE_V_TABLE_H
#define CPN_PNML_PARSE_V_TABLE_H


#pragma once
#include<string>
#include<map>
#include<iostream>
#include<vector>
#include<list>

#define global_table_name "global"

//**** SizeList
//store size for array variables
//for example: an array variable's definition: int a[5][10]
//             matched SizeList will be: {5,10}
struct SizeList{
private:
    std::vector<int> sizes;
public:

    void Add_size(int s){
       sizes.push_back(s);
    }
    void Set_sizes(std::vector<int> s){
        sizes = s;
    }
    std::vector<int> next_size(){
        if(sizes.size() == 0){
            std::cerr<<"error in next_size  v_table.h!"<<std::endl;
            exit(-1);
        }
        std::vector<int> tmp(sizes);
        tmp.pop_back();
        return tmp;
    }
    int totalsize(){
        if(sizes.size() == 0)
            return 1;
        int s = 1;
        for(int i=0;i<sizes.size();i++){
            s *= sizes[i];
        }
        return s;
    }
    void nonsize(){}
    bool empty(){return sizes.size()==0;}
    int getdim(){return sizes.size();}
    std::vector<int> getsizes(){
        return sizes;
    }
};

struct Item{
//    std::string name;
    std::string place;
    SizeList sizelist;
    bool is_pointer;
};

class V_Table
{
private:
    std::string name;

    std::map<std::string,Item> v_map;

    V_Table *fa;

public:
    V_Table(std::string na) { name = na; fa = NULL;}
    V_Table(){fa = NULL;}
    void clear();
    void insert(std::string v, std::string place,SizeList sizelist,bool ispointer);
//    void Add_pointerrelated(std::string v,std::string place);
//    void erase(std::string v) {
//        v_map.erase(v);
//        v_sizemap.erase(v);
//        v_ispointer.erase(v);
//    }
    std::string get_name(){return name;}
    bool find_P(std::string s);
    void connect(V_Table *&father) { fa = father;}
    std::string get_place(std::string v);
    SizeList get_size(std::string v);
    bool get_ispointer(std::string v);
    bool get_isglobal(std::string v);
//    std::vector<std::string> get_pointerrelated(std::string v);
};

extern std::vector<V_Table *> v_tables;//variable tables
void init_v_table();
void release_v_table();

#endif //CPN_PNML_PARSE_V_TABLE_H