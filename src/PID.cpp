#include "PID.h"
#include <math.h>
using namespace std;

/*
* TODO: Complete the PID class.
*/

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp, double Ki, double Kd) {
  this -> Kp = Kp;
  this -> Ki = Ki;
  this -> Kd = Kd;
  p_error = 0;
  d_error = 0;
  i_error = 0;
  error_sum = 0;
}

void PID::UpdateError(double cte) {
    d_error = cte - p_error;
    p_error = cte;
    i_error += cte;
    error_sum +=cte*cte;
}
double PID::GetAvgError(int i) {
    return (error_sum/i)*(2+1*exp(-i/2500));
}
void PID::SetParams(double Kp, double Ki, double Kd) {
    this -> Kp = Kp;
    this -> Ki = Ki;
    this -> Kd = Kd;
}
void PID::Reset() {
    p_error = 0;
    d_error = 0;
    i_error = 0;
    error_sum = 0;
}
double PID::TotalError() {
  return Kp*p_error + Kd*d_error + Ki* i_error;
}
