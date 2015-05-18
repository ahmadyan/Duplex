#include "graphics.h"

class UnitTester{
public:
    UnitTester();
    int testGraphic();
};

int UnitTester::testGraphic(){
    Graphics* g = new Graphics("");
    return 0;
}

int main(int argc, char** argv){
    UnitTester* tester = new UnitTester();
    tester->testGraphic();
}