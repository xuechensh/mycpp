#include <iostream>

using namespace std;

class  Product{
public:
    virtual void ShowSelf() = 0;
    virtual ~Product() {};
};

class ProductA: public Product{
public:
    void ShowSelf(){
        cout << "I am product A" << endl;
    }

    ~ProductA() {};
};

class ProductB: public Product{
public:
    void ShowSelf(){
        cout << "I am product B" << endl;
    }

    ~ProductB() {};
};

class ProductC: public Product{
public:
    void ShowSelf(){
        cout << "I am product C" << endl;
    }

    ~ProductC() {};
};

enum ProductType{
    TYPE_A = 0,
    TYPE_B,
    TYPE_C,
    TYPE_ERROR
};

class Factory{
public:
    Product* CreateProduct(ProductType type){
        Product* result = NULL;

        switch( type ){
        case TYPE_A:
            result = new ProductA();
            break;
        case TYPE_B:
            result = new ProductB();
            break;
        case TYPE_C:
            result = new ProductC();
            break;
        default:
            result = NULL;
        }

        return result;
    }
};

int main(int argc, char* argv[]){
    Factory *f = new Factory();

    Product *a = f->CreateProduct(TYPE_A);
    Product *b = f->CreateProduct(TYPE_B);
    Product *c = f->CreateProduct(TYPE_C);
    Product *d = f->CreateProduct(TYPE_ERROR);

    if(a) { a->ShowSelf(); delete a;}
    if(b) { b->ShowSelf(); delete b;}
    if(c) { c->ShowSelf(); delete c;}
    if(d) { d->ShowSelf(); delete d;}

    return 1;
}

    
