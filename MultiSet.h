//
// Created by cc on 2021/5/6.
//

#ifndef PDNET2_0_MULTISET_H
#define PDNET2_0_MULTISET_H

#include "Sort.h"
#include "expression_tree.h"
#include<unordered_map>

typedef unsigned short token_count_t;
typedef unsigned short color_count_t;
typedef unsigned long hash_t;
class SortValue;
typedef shared_ptr<SortValue> token;


struct Bucket{
    type tid;
    Integer_t integer;
    Real_t real;
    String_t str;
    Product_t pro;
    size_t hash();
    bool operator==(const Bucket &bkt);
};



/*==========================SortValue=========================*/
/*=====================Abstract Base Class====================*/
class SortValue
{
public:
//    virtual ~SortValue() = 0;
    virtual type getmytype() = 0;
    virtual SORTID getsid() = 0;
    virtual void setcolor(const Bucket &bucket) = 0;
    virtual void getcolor(Bucket &bucket) = 0;
};

class ProductSortValue:public SortValue
{
private:
    //this is a index;
    Product_t valueindex;
    SORTID sortid;
public:
    explicit ProductSortValue(SORTID sid);
    virtual type getmytype(){return productsort;}
    virtual SORTID getsid(){return sortid;}
    virtual void setcolor(const Bucket &bucket);
    virtual void getcolor(Bucket &bucket);
};

class IntegerSortValue:public SortValue
{
private:
    Integer_t value;
public:
    virtual type getmytype(){return Integer;}
    virtual SORTID getsid(){cerr<<"IntegerSortValue doesn't have sid!"<<endl;exit(-1);};
    virtual void setcolor(const Bucket &bucket);
    virtual void getcolor(Bucket &bucket);
};

class RealSortValue:public SortValue
{
private:
    Real_t value;
public:
    virtual type getmytype(){return Real;}
    virtual SORTID getsid(){cerr<<"RealSortValue doesn't have sid!"<<endl;exit(-1);};
    virtual void setcolor(const Bucket &bucket);
    virtual void getcolor(Bucket &bucket);
};

class StringSortValue:public SortValue
{
private:
    String_t value;
public:
    virtual type getmytype(){return String;}
    virtual SORTID getsid(){cerr<<"StringSortValue doesn't have sid!"<<endl;exit(-1);};
    virtual void setcolor(const Bucket &bucket);
    virtual void getcolor(Bucket &bucket);
};


/*==========================Factory=========================*/
/*=====================Abstract Base Class====================*/

class Factory{
public:
    virtual shared_ptr<SortValue> generateSortValue() = 0;
};

class IntegerFactory{
public:
    shared_ptr<SortValue> generateSortValue(){
        shared_ptr<SortValue> p(new IntegerSortValue);
        return p;
    }
};


class RealFactory{
public:
    shared_ptr<SortValue> generateSortValue(){
        shared_ptr<SortValue> p(new RealSortValue);
        return p;
    }
};

class StringFactory{
public:
    shared_ptr<SortValue> generateSortValue(){
        shared_ptr<SortValue> p(new StringSortValue);
        return p;
    }
};

class ProductFactory{
public:
    shared_ptr<SortValue> generateSortValue(SORTID sid){
        shared_ptr<SortValue> p(new ProductSortValue(sid));
        return p;
    }
};

//class Tokens
//{
//public:
//    SortValue *color;
//    token_count_t tokencount;
//    Tokens *next;
//
//    Tokens() {color = NULL;tokencount=0;next=NULL;}
//    ~Tokens() {
//        if(color!=NULL)
//            delete color;
//    }
//    void initiate(token_count_t tc,type sort,SORTID sid);
//
//};

//**Multiset contains tokens with the same type**//

//Hasher

struct tokenHasher{
public:
    size_t operator()(const token &tk) const{
        Bucket bkt;
        tk->getcolor(bkt);
        return bkt.hash();
    }
};

struct tokenEqualto{
public:
    bool operator()(const token &tk1,const token &tk2) const{
        Bucket bkt1,bkt2;
        if(tk1->getmytype() != tk2->getmytype())
            return false;
        tk1->getcolor(bkt1);
        tk2->getcolor(bkt2);
        return bkt1 == bkt2;
    }
};

class MultiSet
{
private:
    type tid;
    SORTID sid;
//    Tokens *tokenQ;//queue contains head node
    unordered_map<token,token_count_t,tokenHasher,tokenEqualto> mapTokens;
    color_count_t color_count;
//    hash_t hash_value;
public:
    MultiSet(){color_count = 0;}
    MultiSet(type tid,SORTID sid):tid(tid),sid(sid){color_count = 0;}
    bool generateFromToken(const token &tk);

    bool operator>=(const MultiSet &ms1);
    bool operator==(const MultiSet &ms);
    void operator=(const MultiSet &ms);
    void MINUS(const MultiSet &ms);
    void PLUS(const MultiSet &ms);
    void clear();
    hash_t Hash();

};


extern IntegerFactory fint;
extern RealFactory freal;
extern StringFactory fstr;
extern ProductFactory fpro;

#endif //PDNET2_0_MULTISET_H
