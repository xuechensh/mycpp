#include <iostream>

using namespace std;

class  Product{
public:
    virtual void ShowSelf() = 0;
    virtual ~Product() {};
};

class ProductA: public Product{
public:
    virtual void ShowSelf(){
        cout << "I am product A" << endl;
    }

    ~ProductA() {};
};

class ProductB: public Product{
public:
    virtual void ShowSelf(){
        cout << "I am product B" << endl;
    }

    ~ProductB() {};
};

class ProductC: public Product{
public:
    virtual void ShowSelf(){
        cout << "I am product C" << endl;
    }

    ~ProductC() {};
};

class Factory{
public:
    virtual Product* CreateProduct() = 0;
    virtual ~Factory() {};
};

class FactoryA: public Factory{
public:
    virtual Product* CreateProduct(){
        Product *a = new ProductA();
        return a;
    }
};

class FactoryB: public Factory{
public:
    virtual Product* CreateProduct(){
        Product *b = new ProductB();
        return b;
    }
};

class FactoryC: public Factory{
public:
    virtual Product* CreateProduct(){
        Product *c = new ProductC();
        return c;
    }
};

int main(int argc, char* argv[]){
    Factory *f = NULL;

    f = new FactoryA();
    Product *a = f->CreateProduct();
    if(a) { a->ShowSelf(); delete a;}
    delete f;

    f = new FactoryB();
    Product *b = f->CreateProduct();
    if(b) { b->ShowSelf(); delete b;}
    delete f;

    f = new FactoryC();
    Product *c = f->CreateProduct();
    if(c) { c->ShowSelf(); delete c;}
    delete f;

    return 1;
}

    
