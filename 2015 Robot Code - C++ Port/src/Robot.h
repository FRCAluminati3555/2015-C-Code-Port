#ifndef SRC_ROBOT_H_
#define SRC_ROBOT_H_

    enum ControlMode { OperatorControl, Autonomous, Test, Practice };

	void compressorThread();
    ControlMode getControlMode();

    void togglePiston();
    void setPistonOn();
    void setPistonOff();

    void mecanumSpin(double);
    void mecanumDrive(double, double);

#endif /* SRC_ROBOT_H_ */
