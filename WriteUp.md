# Building a Controller Project Write Up

In this document I am going to present how I solved all the tasks of
the project and explain how my submission address each rubric point.

## Provide a Writeup / README that includes all the rubric points and how you addressed each one. You can submit your writeup as markdown or pdf.

It’s this file!

## Implemented body rate control in C++.

The body rate controller is implemented in the `BodyRateControl` member
function of the `QuadControl` class. It is a P controller taking the
desired (`pqrCmd`) and estimated (`pqr`) angular velocities in the body 
frame to compute the required angular acceleration (`pqrBar`) proportional 
to the error and the angular moment (`momentCmd`) multiplying the 
acceleration with the moments of inertia (`Ixx`, `Iyy` and `Izz`).

After some experiments I set the proportional constant `kpPQR` to 
`85, 85, 6` in the `QuadControlParams.txt` file.

## Implement roll pitch control in C++.

The roll-pitch controller is implement in the `RollPitchControl` member 
function of the `QuadControl` class. It takes the desired lateral 
accelerations (`accelCmd.x` and `accelCmd.y`), the currently estimated
attitude (`attitude`) and the commanded total thrust (`collThrustCmd`)
to compute the roll and pitch rates required to achieve that goal.

It achieves this by first computing the required rotation matrix components
and then using a P controller to get their rates. It then transforms these
rates in the corresponding roll-pitch ones. The proportional constant
`kpBank` is set to `8` in the `QuadControlParams.txt` file.

## Implement altitude controller in C++.

The altitude controller is implemented in the `AltitudeControl` member
function of the `QuadControl` class. It is a feed forward PID controller
taking the desired and estimated altitutes and velocities to compute the 
total thrust required by the four propellers. The three constants `kpPosZ`,
`KiPosZ` and `kpVelZ` are set to `25`, `30` and `10` in the 
`QuadControlParams.txt` file.

## Implement lateral position control in C++.

The lateral position controller is implemented in the 
`LateralPositionControl` member function of the `QuadControl` class. It is 
a PD controller taking the expected and estimated positions and velocities
and returning the commanded accelerations. The constants `kpPosXY` and 
`kpVelXY` are set to `35`, `15` in the `QuadControlParams.txt` file.

## Implement yaw control in C++.

The yaw controller is implemented in the `YawControl` member function of 
the `QuadControl` class. It is a simple P controller taking the desidered
and estimated angles and returning the desired rate. Since yaw is an angle
there is some logic to get the minimum difference between the two angles.

The `kpYaw` constant is set to `2` in the `QuadControlParams.txt` file.

## Implement calculating the motor commands given commanded thrust and moments in C++.

The motor commanded thrusts are computed in the `GenerateMotorCommands`
member function of the `QuadControl` class. It computes the thrusts by
inverting a linear system.

## Your C++ controller is successfully able to fly the provided test trajectory and visually passes inspection of the scenarios leading up to the test trajectory.

The controller is able to fly and pass all scenarios.
