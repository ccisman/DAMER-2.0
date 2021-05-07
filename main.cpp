
#include<map>
#include "cpn.h"
#include "v_table.h"


int main(int argc,char **argv) {
    string check_file = "test.c";
    //1.preprocess and build program's AST
    gtree *tree = create_tree(check_file, true);
    cut_tree(tree);
//    if (showtree) {
        intofile_tree(tree);
        makeGraph("tree.dot", "tree.png");
//    }
    CPN *cpnet = new CPN;

    v_tables.clear();
    init_v_table();

    //2.construct program's CPN
    cpnet->init();
    cpnet->initDecl();
    cpnet->getDecl(tree);

    token tk,tk2,tk3;
    tk = fint.generateSortValue();
    tk2 = fint.generateSortValue();
    tk3 = fpro.generateSortValue(0);
    Bucket bkt;
    bkt.tid = Integer;
    bkt.integer = 1;
    tk->setcolor(bkt);
    bkt.integer = 2;
    tk2->setcolor(bkt);
    return 0;
}
