#ifndef PID_H
#define PID_H

class PID {
public:
  /*
  * Errors
  */
  double p_error;
  double i_error;
  double d_error;

  /*
  * Coefficients
  */
  double Kp;
  double Ki;
  double Kd;
  int i = 0;
  double error_sum = 0;

  /*
  * Constructor
  */
  PID();

  /*
  * Destructor.
  */
  virtual ~PID();

  /*
  * Initialize PID.
  */
  void Init(double Kp, double Ki, double Kd);
  void SetParams(double Kp, double Ki, double Kd);
  double GetAvgError(int i);

  /*
  * Update the PID error variables given cross track error.
  */
  void UpdateError(double cte);
  void Reset();
  /*
  * Calculate the total PID error.
  */
  double TotalError();
};

#endif /* PID_H */
