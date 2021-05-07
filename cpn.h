//
// Created by cc on 2020/10/10.
//

#ifndef PDNET_CHECKER_CPN_H
#define PDNET_CHECKER_CPN_H

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <sstream>
#include <string.h>
#include "AST.h"
#include "y.tab.h"
#include "base.h"
#include "v_table.h"
#include "Sort.h"
#include "MultiSet.h"
//#include "condition_tree.h"
#include "expression_tree.h"
#include <gperftools/tcmalloc.h>
#include <gperftools/malloc_extension.h>
#define H1FACTOR 13//hash offset

using namespace std;

typedef unsigned short index_t;
typedef unsigned short VARID;
typedef unsigned int NUM_t;
typedef unsigned int ID_t;
typedef string TID_t;

enum Arc_Type{executed,control,call_enter,call_exit,data,write,call_connect,allocwrite,remain};//data is equal to read

struct Variable
{
    string id;
    SORTID sid;
    SortValue *value;//assume variable is not a productsort
    type tid;

    bool operator == (const Variable &var) const {

    }
    bool operator < (const Variable &var) const {
//        if(this->vid < var.vid)
//            return true;
//        else
//            return false;
    }
};

/*========================Net Element========================*/
typedef struct CPN_Small_Arc
{
    index_t idx;
    condition_tree arc_exp;
    Arc_Type arcType;
    bool onlydot;
} CSArc;

struct Triple{
    string first;
    string second;
    string third;
    string fourth;
    string fifth;
    bool flag;
};

typedef struct CPN_Place
{
    string id;
//    type tid;
//    SORTID sid;
    MultiSet initMarking;
    vector<CSArc>producer;
    vector<CSArc>consumer;

    //***PDNet added start***/
    bool control_P;
    bool is_mutex;
    bool is_cond;
    bool is_executed;
    bool is_pointer;
    bool is_global;
    bool is_structure;

    SizeList sizelist;
    string expression;
    vector<string> enter;
    vector<string> exit;
    vector<string> false_exit;
    vector<string> call_P;
    vector<string> correspond_P;
    vector<Triple> returns;//for func begin P
    vector<pair<string,string>> para_list;//first is identifier, second is place name
    //***PDNet added end***/

    CPN_Place(type tid,SORTID sid):initMarking(tid,sid){expression="";control_P=is_mutex=is_cond=is_executed=is_pointer=is_global=is_structure=false;}
    CPN_Place(){}
    void operator=(CPN_Place &plc){
        //copy except producer and consumer
        id = plc.id;
        initMarking = plc.initMarking;
        expression=plc.expression;
        enter = plc.enter;
        exit = plc.exit;
        false_exit = plc.false_exit;
        call_P = plc.call_P;
        correspond_P = plc.correspond_P;
        returns = plc.returns;
        para_list = plc.para_list;
        is_executed = plc.is_executed;
        control_P = plc.control_P;
        is_mutex = plc.is_mutex;
        is_cond = plc.is_cond;
    }

    void printTokens(string &str);
//    ~CPN_Place(){
//        delete initMarking;
//    }
} CPlace;

typedef struct CPN_Transition
{
    string id;
    condition_tree guard;
    bool hasguard;
    bool isreturn;
    bool is_writepointer;
    vector<CSArc> producer;
    vector<CSArc> consumer;
    CPN_Transition(){isreturn = false;is_writepointer=false;hasguard=false;}
    void operator=(CPN_Transition &trans){
        //copy except produce and consumer
        id = trans.id;
        guard = trans.guard;
        hasguard = trans.hasguard;
    }
//    set<string> relvars;
    //vector<Variable> relvararray;

    //***PDNet added start***/

    //***PDNet added end***/
} CTransition;

typedef struct CPN_Arc
{
    string id;
    bool isp2t;
    string source_id;
    string target_id;
    condition_tree arc_exp;
    Arc_Type arcType;
    bool onlydot;

    void operator=(CPN_Arc &arc){
        id = arc.id;
        isp2t = arc.isp2t;
        source_id = arc.source_id;
        target_id = arc.target_id;
        arc_exp = arc.arc_exp;
        arcType = arc.arcType;
        onlydot = arc.onlydot;
    }
//    MultiSet arc_MS;
//    ~CPN_Arc() {
//        arc_exp.destructor(arc_exp.root);
//    }
} CArc;

/*========================Color_Petri_Net=======================*/
class CPN
{
public:
    CPlace *place;
    CTransition *transition;
    CArc *arc;

    Variable *vartable;
    NUM_t placecount;
    NUM_t transitioncount;
    NUM_t arccount;
    NUM_t varcount;
    map<string,index_t> mapPlace;
    map<string,index_t> mapTransition;
    map<string,VARID> mapVariable;
    map<string,string> mapFunction;//map some unique function expression with their matched places , for example begin,end  and especially ret_tag
    map<string,string> mapPthread;//map some pthread information with their matched places
    map<string,string> mapJoin;//map pthread_create with join
    map<string,string> mapLabel;// Label ——> place_str
    map<string,string> mapGoto; // transition_str ——> Label
    //***PDNet added end***/
public:
    CPN();
    ~CPN();

    //***PDNet added start***//
    void getDecl(gtree *tree);
    void initDecl();
    void init();
    void init_alloc_func();

};



class var_type{
private:
    string tag;
    string name;
    bool pointer_flag;
//    int dimension;
    SizeList size_table;
public:
//    var_type(){size_table = NULL;}
//    ~var_type(){if(size_table)delete size_table;}
    void init(string t,string id,bool pointer_f,SizeList sizes){
        tag = t;
        name = id;
        pointer_flag = pointer_f;
//        dimension = dim;
        size_table.Set_sizes(sizes.getsizes());
//        size_table = new int[dim];
//        for(int i=0;i<dim;i++)
//            size_table[i] = size[i];
    }
    bool get_pointer_flag(){return pointer_flag;}
    SizeList get_size_table(){return size_table;}
    int get_dimension(){return size_table.getdim();}
    string get_tag(){return tag;}
    string get_name(){return name;}
};


#endif //PDNET_CHECKER_CPN_H