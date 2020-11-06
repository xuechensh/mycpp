#include <iostream>

using namespace std;

class  CPU{
public:
    virtual void ShowSelf() = 0;
    virtual ~CPU() {};
};

class Board{
public:
    virtual void ShowSelf() = 0;
    virtual ~Board() {};
};

class CPU_Intel: public CPU{
public:
    virtual void ShowSelf(){
        cout << "I am Intel CPU" << endl;
    }
};

class Board_Intel:public Board{
public:
    virtual void ShowSelf(){
        cout << "I am Intel board" << endl;
    }
};

class CPU_Amd:public CPU{
public:
    virtual void ShowSelf(){
        cout << "I am Amd CPU" << endl;
    }
};

class Board_Amd: public Board{
public:
    virtual void ShowSelf(){
        cout << "I am Amd Board" << endl;
    }
};

class Factory{
public:
    virtual CPU* GetCpu() = 0;
    virtual Board* GetBoard() = 0;
    virtual ~Factory() {};
};

class Intel: public Factory{
public:
    virtual CPU* GetCpu(){
        CPU* a = new CPU_Intel();
        return a;
    }
    virtual Board* GetBoard(){
        Board* b = new Board_Intel();
        return b;
    }
};

class Amd: public Factory{
public:
    virtual CPU* GetCpu(){
        CPU* a = new CPU_Amd();
        return a;
    }
    virtual Board* GetBoard(){
        Board* b = new Board_Amd();
        return b;
    }
};

int main(int argc, char* argv[]){
    Factory *f = NULL;
    CPU *c = NULL;
    Board *b = NULL;

    f = new Intel();
    if( f ){
        c = f->GetCpu();
        if( c ){ c->ShowSelf(); delete c; c = NULL; }
        b = f->GetBoard();
        if( b ){ b->ShowSelf(); delete b; b = NULL; }
    }
    delete f;
    f = NULL;


    f = new Amd();
    if( f ){
        c = f->GetCpu();
        if( c ){ c->ShowSelf(); delete c; c = NULL; }
        b = f->GetBoard();
        if( b ){ b->ShowSelf(); delete b; b = NULL; }
    }
    delete f;
    f = NULL;

    return 1;
}

    
