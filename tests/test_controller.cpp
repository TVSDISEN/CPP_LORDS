#include "Controller.h"
#include <bits/stdc++.h>
using namespace std;
int main(){
    Controller c(1.5,6.0,0.02);
    for (int i=0;i<100;i++){
        cout<<c.calculate(100.0,0.0,0.001)<<'\n';
    }
    return 0;
}