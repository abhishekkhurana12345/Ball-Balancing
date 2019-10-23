#include <stdlib.h>
#include <math.h>
#include "pid.h"

pid_ctrl_t::pid_ctrl_t() {
	pid_set_gains(1., 0., 0.);
	integrator = 0.;
	previous_error = 0.;
	integrator_limit = INFINITY;
	frequency = 1.;
}


void pid_ctrl_t::pid_set_gains(float _kp, float _ki, float _kd)
{
	kp = _kp;
	ki = _ki;
	kd = _kd;
}

void pid_ctrl_t::pid_get_gains(float *_kp, float *_ki, float *_kd)
{
	*_kp = kp;
	*_ki = ki;
	*_kd = kd;
}

float pid_ctrl_t::pid_get_integral_limit()
{
	return integrator_limit;
}

float pid_ctrl_t::pid_get_integral()
{
	return integrator;
}

float pid_ctrl_t::pid_process(float error)
{
	float output;
	integrator += error;

	if (integrator > integrator_limit) {
		integrator = integrator_limit;
	}
	else if (integrator < -integrator_limit) {
		integrator = -integrator_limit;
	}

	output = -kp * error;
	output += -ki * integrator / frequency;
	output += -kd * (error - previous_error) * frequency;

	previous_error = error;
	return output;
}

void pid_ctrl_t::pid_set_integral_limit(float max)
{
	integrator_limit = max;
}

void pid_ctrl_t::pid_reset_integral()
{
	integrator = 0.;
}

void pid_ctrl_t::pid_set_frequency(float _frequency)
{
	frequency = _frequency;
}

float pid_ctrl_t::pid_get_frequency()
{
	return frequency;
}
