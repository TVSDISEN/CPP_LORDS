#include "Simulator.h"
#include "CSVOutput.h"
#include <iostream>
#include <memory>
#include <string>
using namespace std;
// ===== TUNABLE CONSTANTS =====
const double DT           = 0.001;
const double DURATION     = 10.0;
double setpoint; // USER CHOICE 
double initialSpeed;//USER CHOICE
double loadTorque;//USER CHOICE
const double INERTIA      = 0.02;
const double FRICTION     = 0.05;
const double TORQUE_CONST = 0.5;
const double LOAD_TIME    = 5.0;
const double DEFAULT_LOAD_TORQUE=2.0;

const double KP = 1.5;
const double KI = 6.0;
const double KD = 0.02;

const double FEEDBACK_H     = 1.0;   // sensor gain (1 = true speed)
const double INTEGRAL_LIMIT = 3.0;   // anti-windup clamp on the I sum
const double MAX_VOLTAGE    = 24.0;  // supply voltage limit
// ==============================

void runScenario(const std::string& name,
                 double kp, double ki, double kd) {

    Motor motor(
        INERTIA,
        FRICTION,
        TORQUE_CONST,
        initialSpeed,
        loadTorque
    );

    motor.setLoadDisturbance(LOAD_TIME);

    Controller controller(kp, ki, kd, FEEDBACK_H, INTEGRAL_LIMIT, MAX_VOLTAGE);

    std::string path = "data/" + name + ".csv";

    Simulator sim(
        motor,
        controller,
        std::make_unique<CSVOutput>(path),
        DT,
        setpoint
    );

    sim.run(DURATION);


    double finalSpeed=sim.getFinalSpeed();

    cout<<name<<":\n";
    cout<<"Target Speed :"<<setpoint<<'\n';
    cout<<"Final Speed :"<<finalSpeed<<'\n';

    if (abs(finalSpeed-setpoint)<0.01){
        cout<<"Status        :Target Reached"<<"\n\n";
    }
    else{
        cout<<"Status        :Target Not Reached"<<"\n\n";
    }
}

int main() {
    try {
        cout<<"ENTER THE TARGET SPEED (rad/sec) :";
        if (!(cin>>setpoint)){
            throw runtime_error("INVALID TARGET SPEED");
        }
        cout<<"ENTER THE INITIAL SPEED (rad/sec) :";
        if (!(cin>>initialSpeed)){
            throw runtime_error("INVALID INITIAL SPEED");
        }
        char choice;
        cout<<"DO YOU WANT TO ENTER A LOAD ?(Y/n) :";
        if (!(cin>>choice)){
            throw runtime_error("INVALID CHOICE CHOOSEN");
        }
        if (choice=='y' || choice=='Y'){
            cout<<"ENTER THE LOAD TORQUE (N*m): ";
            if (!(cin>>loadTorque)){
            throw runtime_error("INVALID LOAD TORQUE");
        }
        }
        else{
            loadTorque=DEFAULT_LOAD_TORQUE;
        }
        runScenario("pid", KP, KI, KD);
        runScenario("p_only", KP, 0.0, 0.0);
        runScenario("pi", KP, KI, 0.0);
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}