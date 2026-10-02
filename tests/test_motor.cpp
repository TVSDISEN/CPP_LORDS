#include "Motor.h"
#include <bits/stdc++.h>
using namespace std;
int main(){
    Motor m(0.02,0.05,0.5,0.0,2.0);
    m.setLoadDisturbance(5.0);
    for (int i=0;i<100000;i++){
        double t=i*0.001;
        m.update(10.0,t,0.001);
    }
    cout<<m.getSpeed()<<'\n';
}