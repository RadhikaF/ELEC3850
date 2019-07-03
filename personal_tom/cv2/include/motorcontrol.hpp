#ifndef _MOTORCONTROL_HPP_
#define _MOTORCONTROL_HPP_

#define PIN_BASE 300
#define MAX_PWM 4096
#define HERTZ 50

#define NO_POINTS 10.0

void* MotorControl(void*);
void SetMotorCtrl(std::vector <double>);
std::vector<double> GetMotorCtrl();
void StopMotors();
int xbox_init();
int xbox_main();
void CalcRL(double x, double y);
void XboxControlEnable(bool set);
bool GetXboxControl();

#endif