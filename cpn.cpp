//
// Created by cc on 2020/10/10.
//
#include "cpn.h"
#include "v_table.h"
#include<cmath>
#include<stack>

SortTable sorttable;
SORTID SortTable::psptr = 0;

vector<V_Table *> v_tables;//variable tables
vector<Triple> readpointer;
vector<Triple> writepointer;
vector<Triple> strcpypointer;
vector<Triple> threadcreatearr;
map<string,string> join_create;
map<string,unsigned short> place_size;//first is place,second is size. place(1)——address(n)
map<string,bool> place_isglobal;//first is place,second is isglobal. isglobal and size should be together(can be improved)

int gen_P_num=0,gen_T_num=0;
ID_t id_ptr = 1000;
TID_t init_tid = "\"main\"";
TID_t pthread_P = "pthread";
type TID_colorset = String;
//TID_t tid_increment = 1000;
//string tid_store_P;
//string tid_store_type = "tid_store";
//string oldtid_str = "oldtid";
string alloc_store_P;
string alloc_store_type = "alloc_store";
string malloc_str = "malloc";
string calloc_str = "calloc";
ID_t init_alloc = 10000;
unsigned int malloc_maxsize = 100;
unsigned short thread_max = 20;

//map<string,string> map_address;//first is address(string),second is variable. address(1)——variable(1)


map<string,type> map_build_in_type {
        {"void",dot},
        {"char",String},
        {"short",Integer},
        {"int",Integer},
        {"long",Integer},
        {"float",Real},
        {"double",Real}
};
//map<string,MSI> usrdf_type;
string controlflag = "control";
string arr_suffix = "_arr",var_suffix = "_var",begin_suffix = " begin",
        id_suffix = "_id",tid_str = "__tid",
        end_suffix = " end",return_suffix = "_v",
        call_suffix = "()",global_suffix = "_global",
        call_statement_suffix = "_call",join_suffix = " join";
string executed_P_name = "executed_P";
vector<string> pthread_type = {"pthread_t","pthread_mutex_t","pthread_cond_t"};
vector<string> pthread_func_type = {"pthread_create","pthread_join","pthread_exit",
                                    "pthread_mutex_init","pthread_mutex_lock","pthread_mutex_unlock",
                                    "pthread_cond_init","pthread_cond_signal","pthread_cond_wait","pthread_mutex_destroy"};
vector<string> lib_func_type = {"printf","malloc","calloc","strcpy"};



void CPN::init() {

    sorttable.psptr = 0;
    gen_P_num = gen_T_num = 0;
    placecount = transitioncount = arccount = varcount = 0;

    place = new CPlace[400];
    transition = new CTransition[400];
    arc = new CArc[2000];

    vartable = new Variable[500];

}

void process_stack(stack<int> &Stack_n,stack<string> &Stack_o)
{
    string top = Stack_o.top();
    int top1,top2;
    string op = Stack_o.top();
    Stack_o.pop();
    if(!is_unary(top))
    {
        top1 = Stack_n.top();
        Stack_n.pop();
        top2 = Stack_n.top();
        Stack_n.pop();

        Stack_n.push(operate(top1,top2,op));
    }
    else
    {
        top1 = Stack_n.top();
        Stack_n.pop();

        Stack_n.push(operate(top1,0,op));
    }
}

int cal_constant(string s){
    int count;
    string tmp;

    stack<string> Stack_o;
    stack<int> Stack_n;

    s = s + "#";
    Stack_o.push("#");

    for(unsigned int i=0;i<s.size();i++) {
        count = is_operator(s, i);
        if (count != 0) {
            string op = s.substr(i,count);
            if(tmp.size()==0 && s[i] == '-'){
                Stack_o.push("@");
                continue;
            }
            else if(tmp.size() != 0){
                auto tmp_i = atoi(tmp.c_str());
                if(tmp_i == 0 && tmp.c_str() != "0"){
                    cerr<<tmp<<" is not a constant!"<<endl;
                    exit(-1);
                }
                Stack_n.push(tmp_i);
                tmp.clear();
            }
            if(op == "("){
                Stack_o.push(op);
            }
            else if(op == ")"){
                while(Stack_o.top() != "("){
                    process_stack(Stack_n,Stack_o);
                }
                Stack_o.pop();
            }
            else{
                while(priority(op) <= priority(Stack_o.top())){
                    if (s[i] == '#' && Stack_o.top() == "#")
                    {
                        auto res = Stack_n.top();
                        Stack_n.pop();
                        return res;
                    }
                    process_stack(Stack_n, Stack_o);
                }
                Stack_o.push(op);
            }

            i += count - 1;
        }
        else if (s[i] == '=' && (s[i+1] == '_' || isalnum(s[i+1]) || isdigit(s[i+1]))
                 && (s[i+1] == '_' || isalnum(s[i+1]) || isdigit(s[i+1])) )//deal with assignment expression 'a=1' imply '1'
            tmp.clear();
        else
            tmp = tmp + s[i];
    }


}

void CPN::initDecl() {

    map<string,type>::iterator miter;

    MSI control,m_int,m_str;

    control.tid = String;
    control.sid = 0;
    sorttable.mapSort.insert(pair<string,MSI>(controlflag,control));

    m_int.tid = Integer;
    m_int.sid = 0;
    m_str.tid = String;
    m_str.sid = 0;

    for(miter=map_build_in_type.begin();miter!=map_build_in_type.end();miter++)
    {
        //normal build_in type
        MSI m,m1;
        m.tid = miter->second;
        m.sid = 0;

        sorttable.mapSort.insert(pair<string,MSI>(miter->first,m));

        //build_in type array
        ProductSort pp;
        pp.id = miter->first + arr_suffix;
        pp.sortnum = 4;
//        pp.mytype = productsort;
        pp.sortname.push_back(miter->first);
//        pp.hasindex = true;
//        pp.hastid = true;
        pp.sortname.push_back("index");
        pp.sortname.push_back("id");
        pp.sortname.push_back("tid");

        pp.sortid.push_back(m);
        pp.sortid.push_back(m_int);
        pp.sortid.push_back(m_int);
        pp.sortid.push_back(m_str);


        m1.sid = SortTable::psptr++;
        m1.tid = productsort;

        sorttable.mapSort.insert(pair<string,MSI>(pp.id,m1));
        sorttable.productsort.push_back(pp);

        //build_in type var
        ProductSort pp1;
        pp1.id = miter->first + var_suffix;
        pp1.sortnum = 3;
//        pp1.mytype = productsort;
        pp1.sortname.push_back(miter->first);
//        pp1.hastid = true;
        pp1.sortname.push_back("id");
        pp1.sortname.push_back("tid");

        pp1.sortid.push_back(m);
        pp1.sortid.push_back(m_int);
        pp1.sortid.push_back(m_str);


        m1.sid = SortTable::psptr++;
        m1.tid = productsort;

        sorttable.mapSort.insert(pair<string,MSI>(pp1.id,m1));
        sorttable.productsort.push_back(pp1);

        //build_in type array global
        ProductSort pp2;
        pp2.id = miter->first + arr_suffix + global_suffix;
        pp2.sortnum = 3;
//        pp2.mytype = productsort;
        pp2.sortname.push_back(miter->first);
//        pp2.hasindex = true;
        pp2.sortname.push_back("index");
        pp2.sortname.push_back("id");
//        pp.sortname.push_back("tid");

        pp2.sortid.push_back(m);
        pp2.sortid.push_back(m_int);
        pp2.sortid.push_back(m_int);
//        pp2.sortid.push_back(m_int);


        m1.sid = SortTable::psptr++;
        m1.tid = productsort;

        sorttable.mapSort.insert(pair<string,MSI>(pp2.id,m1));
        sorttable.productsort.push_back(pp2);

        //build_in type var global
        ProductSort pp3;
        pp3.id = miter->first + var_suffix + global_suffix;
        pp3.sortnum = 2;
//        pp3.mytype = productsort;
        pp3.sortname.push_back(miter->first);
        pp3.sortname.push_back("id");
//        pp3.sortname.push_back("tid");

        pp3.sortid.push_back(m);
        pp3.sortid.push_back(m_int);
//        pp1.sortid.push_back(m_str);


        m1.sid = SortTable::psptr++;
        m1.tid = productsort;

        sorttable.mapSort.insert(pair<string,MSI>(pp3.id,m1));
        sorttable.productsort.push_back(pp3);
    }
}

void getinfofromdeclarator(gtree *declarator,bool &pointer_flag,SizeList &arr_size,string &id){
    if(declarator->type != DECLARATOR){
        cerr << "ERROR!getinfofromdeclarator should be a declarator node!"<<endl;
        exit(-1);
    }

    gtree *direct_declarator;
    pointer_flag = false;
    arr_size.nonsize();
    gtree* identifier;
    if(declarator->child->type == POINTER){
        direct_declarator = declarator->child->next;
        pointer_flag = true;
    }
    else
        direct_declarator = declarator->child;
    if(direct_declarator->child && direct_declarator->child->type == DIRECT_DECLARATOR
    && direct_declarator->child->next && direct_declarator->child->next->place == "("){
        cerr << "ERROR!We don't support function in struct_declarator for now!"<<endl;
        exit(-1);
    }
    else if(direct_declarator->child && direct_declarator->child->type == DIRECT_DECLARATOR){
        //we just support 1 dimension array for now
        //multiple dimension just need some address translation
        gtree *identifier = direct_declarator;
        while(identifier->child)
            identifier = identifier->child;
        id = identifier->place;

        while(identifier->parent->next && identifier->parent->next->place == "["
        && identifier->parent->next->next){
            if(identifier->parent->next->next->type == CONSTANT_EXPRESSION) {
                string size_str = identifier->parent->next->next->place;
                int tmp_size = cal_constant(size_str);
                arr_size.Add_size(tmp_size);
                identifier = identifier->parent;
            }
            else{
                cerr << "we don't support array declaration like: int a[] for now!"<<endl;
                exit(-1);
            }
        }
    }
    else if(direct_declarator){
        // due to cut_tree, there may just have one IDENTIFIER below DIRECT_DECLARATOR
        id = direct_declarator->place;
    }
}

vector<var_type*> process_struct_declaration_list(gtree *p)
{
    vector<var_type*> var_list;
    gtree *struct_declaration_list = p;
    while(struct_declaration_list->type==STRUCT_DECLARATION_LIST)
        struct_declaration_list = struct_declaration_list->child;
    gtree *struct_declaration = struct_declaration_list;
    while(struct_declaration->type==STRUCT_DECLARATION)
    {
        gtree *specifier_qualifier_list = struct_declaration->child;
        gtree *struct_declarator_list = struct_declaration->child->next;
        string specifier = "";
        vector<var_type> declarators;

        while(specifier_qualifier_list != NULL)
        {
            if(specifier_qualifier_list->child->type == TYPE_SPECIFIER)
            {
                specifier = specifier_qualifier_list->child->child->place;
                break;
            }
            specifier_qualifier_list = specifier_qualifier_list->child->next;
        }

        if(specifier == "")
        {
            cout<<"error! declarator doesn't have a type!"<<endl;
            exit(-1);
        }

        while(struct_declarator_list->type == STRUCT_DECLARATOR_LIST)
            struct_declarator_list = struct_declarator_list->child;

        gtree *struct_declarator = struct_declarator_list;
        while(struct_declarator != NULL)
        {
            if(struct_declarator->child->type == DECLARATOR)
            {
                bool pointer_flag;
                SizeList arr_size;
                string id;
                getinfofromdeclarator(struct_declarator->child,pointer_flag,arr_size,id);
                var_type *v_type;
                v_type = new var_type;
                //now we just support dimension 1 or 0
                if(!arr_size.empty())
                    v_type->init(specifier,id,pointer_flag,arr_size);
                else
                    v_type->init(specifier,id,pointer_flag,arr_size);
                var_list.push_back(v_type);
//                if(struct_declarator->child->child->type == POINTER){
//                    cout<<"error! pointer doesn't not support for now."<<endl;
//                    exit(-1);
//                }
//                else{
//                    declarators.push_back(struct_declarator->child->child->child->place);
//                }
            }
            else{
                cerr << "ERROR!struct_declarator should have declarator for now!"<<endl;
                exit(-1);
            }
            struct_declarator = struct_declarator->parent->next->next;
        }
//        for(unsigned int i=0;i<declarators.size();i++) {
//            if(declarators[i].get_pointer_flag())
//                sortname.push_back(make_pair(specifier, declarators[i]));
//        }
        struct_declaration = struct_declaration->parent->next;
    }
    return var_list;
}

void CPN::getDecl(gtree *tree){
    if (tree == NULL)return;
    else if(tree->type == STRUCT_OR_UNION_SPECIFIER)
    {
        if(tree->child->next->type == REMAIN || (tree->child->next->next != NULL &&
                                                 tree->child->next->next->type == REMAIN))
        {
            MSI m,m1,m2,m_int;
            m_int.tid = Integer;
            m_int.sid = 0;

            ProductSort pp,pp1,pp2,pp3,pp_base;
            gtree *struct_declare_list;
            string usrdefine_typename;
            vector<string> base_sortname;
            vector<MemberInfo> base_memberInfos;
            vector<MSI> base_sortid;
            int base_sortnum;
            if(tree->parent->parent->type == DECLARATION_SPECIFIERS)
                usrdefine_typename = tree->parent->parent->place;
            else{
                cerr<<"structure type error! "<<tree->place<<endl;
                exit(-1);
            }
            if(tree->child->next->type == REMAIN)
            {
//                usrdefine_typename = "";
                struct_declare_list = tree->child->next->next->child;
            }
            else {
//                usrdefine_typename = tree->child->next->place;
                struct_declare_list = tree->child->next->next->next->child;
            }

            vector<var_type*> var_list = process_struct_declaration_list(struct_declare_list);


            //var_list.size();
            for(SORTID i;i<var_list.size();i++) {
                if(var_list[i]->get_pointer_flag()) {
                    base_sortid.push_back(m_int);
                    base_memberInfos.emplace_back(var_list[i]->get_name(), var_list[i]->get_size_table());
                    base_sortname.push_back(var_list[i]->get_tag());
                }
                else {

                    auto iter = map_build_in_type.find(var_list[i]->get_tag());
                    MSI tmp_msi;
                    if (iter == map_build_in_type.end()) {
                        auto uiter = sorttable.mapSort.find(var_list[i]->get_tag());
                        if (uiter == sorttable.mapSort.end()) {
                            cerr << "ERROR about variable type when getDecl!" << endl;
                            exit(-1);
                        }
                        tmp_msi = uiter->second;
                    } else {
                        tmp_msi.tid = iter->second;
                        tmp_msi.sid = 0;
                    }

                    if (var_list[i]->get_dimension() == 0) {
                        base_sortid.push_back(tmp_msi);
                        base_memberInfos.emplace_back(var_list[i]->get_name(),var_list[i]->get_size_table());
                        base_sortname.push_back(var_list[i]->get_tag());
                    }
                    else {
                        auto totalsize = var_list[i]->get_size_table().totalsize();
                        for(int j=0;j<totalsize;j++) {
                            base_sortid.push_back(tmp_msi);

                            base_sortname.push_back(var_list[i]->get_tag());
                        }
                        base_memberInfos.emplace_back(var_list[i]->get_name() ,var_list[i]->get_size_table());
                    }
                }
            }
            base_sortnum = base_sortid.size();

            //base_type var
            pp_base.id = usrdefine_typename;
//            pp_base.mytype = productsort;
            pp_base.sortnum = base_sortnum;
            pp_base.sortid = base_sortid;
            pp_base.memberInfos = base_memberInfos;
            pp_base.sortname = base_sortname;

            sorttable.productsort.push_back(pp_base);

            m.sid = SortTable::psptr++;
            m.tid = productsort;

            sorttable.mapSort.insert(pair<string,MSI>(pp_base.id,m));

            //usrdefine_type var
            pp.id = usrdefine_typename + var_suffix;
//            pp.mytype = productsort;
            pp.sortnum = base_sortnum + 2;
            pp.sortid = base_sortid;
            pp.sortid.push_back(m_int);
            pp.sortid.push_back(m_int);
            pp.sortname = base_sortname;
            pp.sortname.push_back("id");
            pp.sortname.push_back("tid");
            pp.memberInfos = base_memberInfos;

            sorttable.productsort.push_back(pp);

            m.sid = SortTable::psptr++;
            m.tid = productsort;

            sorttable.mapSort.insert(pair<string,MSI>(pp.id,m));

            //usrdefine_type arr
            pp1.id = usrdefine_typename + arr_suffix;
//            pp1.mytype = productsort;
            pp1.sortnum = base_sortnum + 3;
            pp1.sortid = base_sortid;
            pp1.sortid.push_back(m_int);
            pp1.sortid.push_back(m_int);
            pp1.sortid.push_back(m_int);
            pp1.sortname = base_sortname;
            pp1.sortname.push_back("id");
            pp1.sortname.push_back("index");
            pp1.sortname.push_back("tid");
            pp1.memberInfos = base_memberInfos;

            sorttable.productsort.push_back(pp1);

            m.sid = SortTable::psptr++;
            m.tid = productsort;

            sorttable.mapSort.insert(pair<string,MSI>(pp1.id,m));

            //usrdefine_type var global
            pp2.id = usrdefine_typename + var_suffix + global_suffix;
//            pp2.mytype = productsort;
            pp2.sortnum = base_sortnum + 1;
            pp2.sortid = base_sortid;
            pp2.sortid.push_back(m_int);
//            pp.sortid.push_back(m_int);
            pp2.sortname = base_sortname;
            pp2.sortname.push_back("id");
//            pp.sortname.push_back("tid");
            pp2.memberInfos = base_memberInfos;

            sorttable.productsort.push_back(pp2);

            m.sid = SortTable::psptr++;
            m.tid = productsort;

            sorttable.mapSort.insert(pair<string,MSI>(pp2.id,m));

            //usrdefine_type var
            pp3.id = usrdefine_typename + arr_suffix + global_suffix;
//            pp3.mytype = productsort;
            pp3.sortnum = base_sortnum + 2;
            pp3.sortid = base_sortid;
            pp3.sortid.push_back(m_int);
            pp3.sortid.push_back(m_int);
            pp3.sortname = base_sortname;
            pp3.sortname.push_back("id");
            pp3.sortname.push_back("index");
            pp3.memberInfos = base_memberInfos;

            sorttable.productsort.push_back(pp3);

            m.sid = SortTable::psptr++;
            m.tid = productsort;

            sorttable.mapSort.insert(pair<string,MSI>(pp3.id,m));

            for(unsigned int i=0;i<var_list.size();i++){
                delete var_list[i];
            }
        }
    }
    getDecl(tree->child);
    getDecl(tree->next);

}

CPN::CPN() {
    placecount = 0;
    transitioncount = 0;
    arccount = 0;
    varcount = 0;
}

CPN::~CPN() {
    delete [] place;
    delete [] transition;
    delete [] arc;
    delete [] vartable;
}

