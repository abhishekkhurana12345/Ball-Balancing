#ifndef PID_H_
#define PID_H_

/** Instance of a PID controller.
 *
 * @note This structure is only public to be able to do static allocation of it.
 * Do not access its fields directly.
 */
class pid_ctrl_t{
private:
	float kp;
	float ki;
	float kd;
	float integrator;
	float previous_error;
	float integrator_limit;
	float frequency;

public:
	
	float pid_get_frequency();//Gets the PID frequency for gain compensation.
	void pid_set_frequency(float _frequency);//Sets the PID frequency for gain compensation.
	void pid_reset_integral();//Resets the PID integrator to zero.
	void pid_set_integral_limit(float max);//Sets a maximum value for the PID integrator.
	float pid_process(float error);//Process one step if the PID algorithm.
	float pid_get_integral();//Returns the value of the PID integrator.
	float pid_get_integral_limit();//Returns the limit of the PID integrator.
	void pid_get_gains(float *_kp, float *_ki, float *_kd);//Returns the proportional gains of the controller.
	void pid_set_gains(float _kp, float _ki, float _kd);//Sets the gains of the given PID.
	pid_ctrl_t();//Initializes a PID controller.
};


#endif
