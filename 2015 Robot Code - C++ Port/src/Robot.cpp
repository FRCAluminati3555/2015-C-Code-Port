#include <math.h>
#include <thread>
#include "WPILib.h"
#include "Robot.h"
	using namespace std;

class Robot: public SampleRobot
{

public:
	const string VERSION = "Robot Code - 1.5.8";

    Joystick joyD;
    Joystick joyOp;

    /**Rear Left*/
    Talon frontL;
	/**Front Left*/
    Talon frontR;
    /**Rear Right*/
    Talon rearL;
    /**Front Right*/
    Talon rearR;

    SendableChooser autonomousChoose;


	/********************************************************************************\
								Pneumatic's Stuff
	\********************************************************************************/

    ControlMode getControlMode() {
		if(IsAutonomous()) return ControlMode::Autonomous;
		if(IsOperatorControl()) return ControlMode::OperatorControl;
		if(IsTest()) return ControlMode::Test;

		return ControlMode::Practice;
	}

    Relay compressorSpike;
    Compressor compressor;
    Solenoid pistonOn;
    Solenoid pistonOff;

    void compressorThread() {
    	ControlMode startMode = getControlMode();

    	bool voltTrip = false;
    	while(IsEnabled() && startMode == getControlMode()) {
    		voltTrip = DriverStation::GetInstance()->GetBatteryVoltage() < (!voltTrip ? 8.0 : 10.0);
    		if(compressor.Enabled() && !voltTrip)
    			compressorSpike.Set(compressorSpike.Value::kForward);
    		else
    			compressorSpike.Set(compressorSpike.Value::kOff);

//    		SmartDashboard.putbool("voltTrip: ", voltTrip);
//    		SmartDashboard.putString("volt: ", ""+ds.getBatteryVoltage());
    	}

    	compressorSpike.Set(compressorSpike.Value::kOff);
    }

	Robot() :
		joyD(0), joyOp(1),

		frontL(2), //1
		frontR(1), //0
		rearL(3),  //3
		rearR (0),  //2

		autonomousChoose(),

		compressorSpike(0),
		compressor(),
		pistonOn(0),
		pistonOff(1)

		{}

	void Autonomous() {
	}

/******************************************************************************************************************************\
											Operator Methods
\******************************************************************************************************************************/
	    void OperatorControl() {
	    	thread compressorOCThread(&Robot::compressorThread);
	    	bool lastPistonState = false;

	    	while(IsEnabled() && IsOperatorControl()) {
	         /********************************************************************************\
										Drive Controls
			 \********************************************************************************/
	//    		int povAngle = joyD.getPOV();
	//    		if(povAngle == 270) mecanumSpin(povAngle, 0.75);
	//    		if(povAngle == 90)  mecanumSpin(povAngle, 0.75);
	//    		if(povAngle == 0)   mecanumDrive(0, -1);
	//    		if(povAngle == 180) mecanumDrive(0,  1);

	    		mecanumDrive(joyD.GetRawAxis(1) / (joyD.GetRawAxis(3)<0.1? 2 : 1), -joyD.GetRawAxis(0));
		      	mecanumSpin(joyD.GetRawAxis(4) / 2);

	      	/********************************************************************************\
										Operator Controls
			\********************************************************************************/
		      	if(joyOp.GetRawButton(1) && !lastPistonState) {
		      		togglePiston();
		      	}

		      	lastPistonState = joyOp.GetRawButton(1);

	      	/********************************************************************************\
										Print Statements
			\********************************************************************************/

	    		SmartDashboard::PutBoolean(" Compressor Enable", compressor.Enabled());
	    		SmartDashboard::PutString("Version: ", VERSION);
	    	}

	    	if(compressorOCThread.joinable()) {
	    		compressorOCThread.join();
	    	}
	    }

	    void disabled() {
	    	compressorSpike.Set(compressorSpike.Value::kOff);
	    }

/******************************************************************************************************************************\
										Piston & Sensor Methods
\******************************************************************************************************************************/

	    void togglePiston() {
	    	pistonOn.Set(pistonOff.Get());
	  		pistonOff.Set(!pistonOn.Get());
	    }

	    void setPistonOn() {
	    	pistonOn.Set(true);
	  		pistonOff.Set(false);
	    }

	    void setPistonOff() {
	    	pistonOn.Set(false);
	  		pistonOff.Set(true);
	    }

/******************************************************************************************************************************\
	  										Drive Methods
\******************************************************************************************************************************/
	    const double DRV_CONFIG_FORWARD[4] = {1, -1, 1, -1};
	    const double DRV_CONFIG_LEFT[4] = {-1, -1, 1, 1};

	//    private void mecanumStrife(double rawAxis) {
	//		frontL.Set(-rawAxis);
	//		frontR.Set(-rawAxis);
	//    	rearL.Set(rawAxis);
	//    	rearR.Set(rawAxis);
	//    }
	//
	//    private void mecanumDriveStraight(double rawAxis) {
	//		frontL.Set(rawAxis);
	//		frontR.Set(rawAxis);
	//    	rearL.Set(rawAxis);
	//    	rearR.Set(rawAxis);
	//    }

	    const float JOYSTICK_SENSITVITY = 0.1f;
	    void mecanumDrive(double x, double y) {
	    	double A = 0, B = 0, C = 0, D = 0;

	    	if(abs(x) < JOYSTICK_SENSITVITY) x = 0;
	    	if(abs(y) < JOYSTICK_SENSITVITY) y = 0;

	    	A = DRV_CONFIG_FORWARD[0]*y + DRV_CONFIG_LEFT[0]*x;
	    	B = DRV_CONFIG_FORWARD[1]*y + DRV_CONFIG_LEFT[1]*x;
	    	C = DRV_CONFIG_FORWARD[2]*y + DRV_CONFIG_LEFT[2]*x;
	    	D = DRV_CONFIG_FORWARD[3]*y + DRV_CONFIG_LEFT[3]*x;

	    	// This code is used to cap the values of the Motors
	//    	A = ((double)((int)(A*10)))/10.0; A = abs(A) > 0.1 ? 1.0 * A < 0 ? -1 : 1 : A;
	//    	B = ((double)((int)(B*10)))/10.0; B = abs(B) > 0.1 ? 1.0 * B < 0 ? -1 : 1 : B;
	//    	C = ((double)((int)(C*10)))/10.0; C = abs(C) > 0.1 ? 1.0 * C < 0 ? -1 : 1 : C;
	//    	D = ((double)((int)(D*10)))/10.0; D = abs(D) > 0.1 ? 1.0 * D < 0 ? -1 : 1 : D;

	    	frontL.Set(A); 	frontR.Set(B);
	    	rearL.Set(C);	rearR.Set(D);
	    }

	    void mecanumSpin(double angle) {
	    	if(abs(angle) > JOYSTICK_SENSITVITY) {
	    		frontL.Set(angle);	frontR.Set(angle);
	    		rearL.Set(angle);	rearR.Set(angle);
	    	}
	    }
};

START_ROBOT_CLASS(Robot);
