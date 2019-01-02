#pragma once


class pid{
private:
	double lastErr = 0;
	double dErr = 0;
public:
	double val = 0;
	double kp = 0.3;
	double kd = 0.2;
	double minO = 60;
	double maxO = 120;
	
	double pt = 0;
	double getPIDVal() {
		dErr = val - pt - lastErr;
		lastErr = val - pt;
		double rVal = kp * (val - pt) + kd * dErr;
		rVal += (maxO + minO) / 2;
		if (rVal > maxO)rVal = maxO;
		if (rVal < minO)rVal = minO;
		return rVal;
	}
};