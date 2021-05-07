//
// Created by cc on 2021/5/6.
//

#include "MultiSet.h"

IntegerFactory fint;
RealFactory freal;
StringFactory fstr;
ProductFactory fpro;

ProductSortValue::ProductSortValue(SORTID sid) {
    auto ps = sorttable.productsort[sid];
    auto sortnum = ps.sortnum;
    sortid = sid;
    auto tmp = new shared_ptr<SortValue>[sortnum];
    valueindex.first.reset(tmp);
    valueindex.second = sortnum;
    for(int i=0;i<sortnum;i++){
        if(ps.sortid[i].tid == Integer)
            valueindex.first.get()[i] = fint.generateSortValue();
        else if(ps.sortid[i].tid == Real)
            valueindex.first.get()[i] = freal.generateSortValue();
        else if(ps.sortid[i].tid == String)
            valueindex.first.get()[i] = fstr.generateSortValue();
        else
            valueindex.first.get()[i] = fpro.generateSortValue(ps.sortid[i].sid);
    }
}

void ProductSortValue::setcolor(const Bucket &bucket) {
    if(bucket.tid == productsort){
        auto ps = sorttable.productsort[sortid];
        auto sortnum = ps.sortnum;
        for(int i=0;i<sortnum;i++){
            Bucket bkt;
            bucket.pro.first.get()[i]->getcolor(bkt);
            valueindex.first.get()[i]->setcolor(bkt);
        }
    }
    else{
        cerr << "ProductSort's bucket doesn't match in MultiSet.cpp!"<<endl;
        exit(-1);
    }
}

void ProductSortValue::getcolor(Bucket &bucket) {
//    if(bucket.tid == productsort){
        auto ps = sorttable.productsort[sortid];
        auto sortnum = ps.sortnum;
        bucket.pro.second = sortnum;
        auto tmp = new shared_ptr<SortValue>[sortnum];
        bucket.pro.first.reset(tmp);
        for(int i=0;i<sortnum;i++){
            if(ps.sortid[i].tid == Integer)
                bucket.pro.first.get()[i] = fint.generateSortValue();
            else if(ps.sortid[i].tid == Real)
                bucket.pro.first.get()[i] = freal.generateSortValue();
            else if(ps.sortid[i].tid == String)
                bucket.pro.first.get()[i] = fstr.generateSortValue();
            else
                bucket.pro.first.get()[i] = fpro.generateSortValue(ps.sortid[i].sid);
        }
        bucket.tid = productsort;

        for(int i=0;i<sortnum;i++){
            Bucket bkt;
            valueindex.first.get()[i]->getcolor(bkt);
            bucket.pro.first.get()[i]->setcolor(bkt);
        }
//    }
//    else{
//        cerr << "ProductSort's bucket doesn't match in MultiSet.cpp!"<<endl;
//        exit(-1);
//    }
}

void IntegerSortValue::setcolor(const Bucket &bucket) {
    if(bucket.tid == Integer){
        value = bucket.integer;
    }
    else{
        cerr << "IntegerSort's bucket doesn't match in MultiSet.cpp!"<<endl;
        exit(-1);
    }
}

void IntegerSortValue::getcolor(Bucket &bucket) {
//    if(bucket.tid == Integer){
        bucket.integer = value;
        bucket.tid = Integer;
//    }
//    else{
//        cerr << "IntegerSort's bucket doesn't match in MultiSet.cpp!"<<endl;
//        exit(-1);
//    }
}

void RealSortValue::setcolor(const Bucket &bucket) {
    if(bucket.tid == Real){
        value = bucket.real;
    }
    else{
        cerr << "RealSort's bucket doesn't match in MultiSet.cpp!"<<endl;
        exit(-1);
    }
}

void RealSortValue::getcolor(Bucket &bucket) {
//    if(bucket.tid == Real){
        bucket.real = value;
        bucket.tid = Real;
//    }
//    else{
//        cerr << "RealSort's bucket doesn't match in MultiSet.cpp!"<<endl;
//        exit(-1);
//    }
}

void StringSortValue::setcolor(const Bucket &bucket) {
    if(bucket.tid == String){
        value = bucket.str;
    }
    else{
        cerr << "StringSort's bucket doesn't match in MultiSet.cpp!"<<endl;
        exit(-1);
    }
}

void StringSortValue::getcolor(Bucket &bucket) {
//    if(bucket.tid == String){
        bucket.str = value;
        bucket.tid = String;
//    }
//    else{
//        cerr << "StringSort's bucket doesn't match in MultiSet.cpp!"<<endl;
//        exit(-1);
//    }
}

size_t Bucket::hash() {
    switch(tid){
        case Integer:
            return integer;
        case Real:
            return static_cast<size_t >(real);
        case String: {
            size_t count = 0;
            for (int i = 0; i < str.length(); i++) {
                count += static_cast<size_t >(str[i]);
            }
            return count;
        }
        case productsort: {
            size_t count=0;
            for (int i = 0; i < pro.second; i++) {
                Bucket bkt;
                pro.first.get()[i]->getcolor(bkt);
                count += bkt.hash();
            }
            return count;
        }
    }
    cerr<<"error tid in hash() in MultiSet.cpp!"<<endl;
    exit(-1);

}

bool Bucket::operator==(const Bucket &bkt) {
    switch (tid) {
        case Integer:
            return integer == bkt.integer;
        case Real:
            return real == bkt.real;
        case String: {
            return str == bkt.str;
            case productsort:
                return pro == bkt.pro;
        }
            return false;
    }
    cerr<<"error tid in operator== in MultiSet.cpp!"<<endl;
    exit(-1);
}

bool operator==(Product_t p1,Product_t p2){
    if(p1.second != p2.second)
        return false;
    for(int i=0;i<p1.second;i++){
        Bucket bkt1,bkt2;
        p1.first.get()[i]->getcolor(bkt1);
        p2.first.get()[i]->getcolor(bkt2);
        if(bkt1 == bkt2)
            ;
        else
            return false;
    }
    return true;
}

bool MultiSet::generateFromToken(const token &tk) {
    if(tid != tk->getmytype() || (tid == productsort && sid != tk->getsid())){
        cerr << "generateFromToken error! type doesn't match!"<<endl;
        return false;
    }
    auto iter = mapTokens.find(tk);
    if(iter != mapTokens.end())
        iter->second++;
    else {
        mapTokens.emplace(tk, 1);
        color_count++;
    }
    return true;
}

bool MultiSet::operator>=(const MultiSet &ms1) {
    if(tid != ms1.tid || (tid == productsort && sid != ms1.sid)){
        cerr << "error! MultiSet >= type doesn't match!"<<endl;
        exit(-1);
    }
    auto iter2 = ms1.mapTokens.begin();
    while(iter2 != ms1.mapTokens.end()){
        auto iter1 = mapTokens.find(iter2->first);
        if(iter1 == mapTokens.end())
            return false;
        if(iter1->second<iter2->second)
            return false;
        iter2++;
    }
    return true;
}

bool MultiSet::operator==(const MultiSet &ms1) {
    if(tid != ms1.tid || (tid == productsort && sid != ms1.sid)){
        cerr << "error! MultiSet >= type doesn't match!"<<endl;
        exit(-1);
    }
    if(ms1.color_count!=color_count)
        return false;
    auto iter2 = ms1.mapTokens.begin();
    while(iter2 != ms1.mapTokens.end()){
        auto iter1 = mapTokens.find(iter2->first);
        if(iter1 == mapTokens.end())
            return false;
        if(iter1->second != iter2->second)
            return false;
        iter2++;
    }
    return true;
}

void MultiSet::operator=(const MultiSet &ms) {
    tid = ms.tid;
    sid = ms.sid;
    mapTokens = ms.mapTokens;
    color_count = ms.color_count;
}
