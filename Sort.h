//
// Created by cc on 2021/5/6.
//

#ifndef PDNET2_0_SORT_H
#define PDNET2_0_SORT_H


#include <string>
#include <memory>
#include "v_table.h"

using namespace std;

typedef unsigned short SORTNUM_t;

typedef unsigned short SORTID;
typedef int Integer_t;
typedef double Real_t;
typedef string String_t;
class SortValue;
typedef pair<unique_ptr<shared_ptr<SortValue>>,int> Product_t;//use unique_ptr to avoid memory leak
//typedef SortValue** Product_t;

class SortTable;
extern SortTable sorttable;

enum type{dot,productsort,Integer,Real,String};

/*===========================================================*/
typedef struct mapsort_info
{
    //its index in corresponding vector
    SORTID sid;
    type tid;
} MSI;

struct MemberInfo{
    string membername;
    SizeList msl;
    MemberInfo(string name,SizeList sizeList):membername(name),msl(sizeList){}
};

class ProductSort
{
public:
    string id;
    SORTNUM_t sortnum;
    vector<string> sortname;
    vector<MemberInfo> memberInfos;//used for 'struct,union' in program, mapping member name to member size
    vector<mapsort_info> sortid;
//    bool hastid;
//    bool hasindex;
    ProductSort(){/*hastid=hasindex=false;*/}

};

class SortTable
{
public:
    vector<ProductSort> productsort;
    bool hasdot;

    static SORTID psptr;

public:
    SortTable(){hasdot = false;}
    int get_memberid(SORTID sid,string member) {
        int pos = 0;
        for (unsigned int i = 0; i < sorttable.productsort[sid].memberInfos.size(); i++)
            if (member == sorttable.productsort[sid].memberInfos[i].membername)
                return i;
    }
    SizeList get_membersize(SORTID sid,string member){
        for (unsigned int i = 0; i < sorttable.productsort[sid].memberInfos.size(); i++)
            if (member == sorttable.productsort[sid].memberInfos[i].membername)
                return sorttable.productsort[sid].memberInfos[i].msl;
    }
    //first string is the sortname,the second MSI is this sort's information;
    map<string,MSI> mapSort;
    friend class CPN;
};


#endif //PDNET2_0_SORT_H
