#include "Common.h"
#include "QuadControl.h"

#include "Utility/SimpleConfig.h"

#include "Utility/StringUtils.h"
#include "Trajectory.h"
#include "BaseController.h"
#include "Math/Mat3x3F.h"

#ifdef __PX4_NUTTX
#include <systemlib/param/param.h>
#endif

void QuadControl::Init()
{
  BaseController::Init();
    
#ifndef __PX4_NUTTX
  // Load params from simulator parameter system
  ParamsHandle config = SimpleConfig::GetInstance();
   
  // Load parameters (default to 0)
  kpPosXY = config->Get(_config+".Kp_pos_xy", 0);
  kpPosZ = config->Get(_config + ".Kp_pos_z", 0);
     
  kpVelXY = config->Get(_config + ".Kp_vel_xy", 0);
  kpVelZ = config->Get(_config + ".Kp_vel_z", 0);

  kpBank = config->Get(_config + ".Kp_bank", 0);
  kpYaw = config->Get(_config + ".Kp_yaw", 0);

  kpPQR = config->Get(_config + ".Kp_pqr", V3F());

  maxDescentRate = config->Get(_config + ".maxDescentRate", 100);
  maxAscentRate = config->Get(_config + ".maxAscentRate", 100);
  maxSpeedXY = config->Get(_config + ".maxSpeedXY", 100);
  maxAccelXY = config->Get(_config + ".maxHorizAccel", 100);

  maxTiltAngle = config->Get(_config + ".maxTiltAngle", 100);

  minMotorThrust = config->Get(_config + ".minMotorThrust", 0);
  maxMotorThrust = config->Get(_config + ".maxMotorThrust", 100);
#else
  // load params from PX4 parameter system
  //TODO
  param_get(param_find("MC_PITCH_P"), &Kp_bank);
  param_get(param_find("MC_YAW_P"), &Kp_yaw);
#endif
}

VehicleCommand QuadControl::GenerateMotorCommands(float collThrustCmd, V3F momentCmd)
{
  // Convert a desired 3-axis moment and collective thrust command to 
  //   individual motor thrust commands
  // INPUTS: 
  //   desCollectiveThrust: desired collective thrust [N]
  //   desMoment: desired rotation moment about each axis [N m]
  // OUTPUT:
  //   set class member variable cmd (class variable for graphing) where
  //   cmd.desiredThrustsN[0..3]: motor commands, in [N]

  // HINTS: 
  // - you can access parts of desMoment via e.g. desMoment.x
  // You'll need the arm length parameter L, and the drag/thrust ratio kappa

  ////////////////////////////// BEGIN STUDENT CODE ///////////////////////////

  cmd.desiredThrustsN[0] = mass * 9.81f / 4.f; // front left
  cmd.desiredThrustsN[1] = mass * 9.81f / 4.f; // front right
  cmd.desiredThrustsN[2] = mass * 9.81f / 4.f; // rear left
  cmd.desiredThrustsN[3] = mass * 9.81f / 4.f; // rear right

  /////////////////////////////// END STUDENT CODE ////////////////////////////
  
  /////////////////////////////// BEGIN SOLUTION //////////////////////////////
  // Convert desired moment into differential thrusts
  V3F diffThrust;

  // for X shaped quad
  diffThrust.x = momentCmd.x / L / 2.f / sqrtf(2);
  diffThrust.y = momentCmd.y / L / 2.f / sqrtf(2);
  diffThrust.z = momentCmd.z / 4.f / kappa;

  // MIXING
  // combine the collective thrust with the differential thrust commands to find desired motor thrusts
  // X Shaped Quad (NED Frame)
  cmd.desiredThrustsN[0] = collThrustCmd / 4.f - diffThrust.z + diffThrust.y + diffThrust.x; // front left
  cmd.desiredThrustsN[1] = collThrustCmd / 4.f + diffThrust.z + diffThrust.y - diffThrust.x; // front right
  cmd.desiredThrustsN[2] = collThrustCmd / 4.f + diffThrust.z - diffThrust.y + diffThrust.x; // rear left
  cmd.desiredThrustsN[3] = collThrustCmd / 4.f - diffThrust.z - diffThrust.y - diffThrust.x; // rear right
  
  //////////////////////////////// END SOLUTION ///////////////////////////////

  return cmd;
}

V3F QuadControl::BodyRateControl(V3F pqrCmd, V3F pqr)
{
  // Calculate a desired 3-axis moment given a desired and current body rate
  // INPUTS: 
  //   body_rate_cmd: desired body rates [rad/s]
  //   body_rate: current or estimated body rates [rad/s]
  // OUTPUT:
  //   return a V3F containing the desired moments for each of the 3 axes

  // HINTS: 
  //  - you can use V3Fs just like scalars: V3F a(1,1,1), b(2,3,4), c; c=a-b;
  //  - you'll need parameters for moments of inertia Ixx, Iyy, Izz
  //  - you'll also need the gain parameter kpPQR (it's a V3F)

  V3F moment_cmd;

  ////////////////////////////// BEGIN STUDENT CODE ///////////////////////////

  

  /////////////////////////////// END STUDENT CODE ////////////////////////////

  /////////////////////////////// BEGIN SOLUTION //////////////////////////////
  V3F rate_error = pqrCmd - pqr;
  V3F omega_dot_des = rate_error * kpPQR;
  moment_cmd = omega_dot_des * V3F(Ixx, Iyy, Izz);
  //////////////////////////////// END SOLUTION ///////////////////////////////

  return moment_cmd;
}

// returns a desired roll and pitch rate 
V3F QuadControl::ReducedAttitudeControl(V3F accelCmd, Quaternion<float> attitude, float collThrustCmd)
{
  // Calculate a desired pitch and roll angle rates based on a desired global
  //   lateral acceleration, the current attitude of the quad, and desired
  //   collective thrust command
  // INPUTS: 
  //   desAccel: desired acceleration in global XY coordinates [m/s2]
  //   attitude: current or estimated attitude of the vehicle
  //   desCollectiveThrust: desired collective thrust of the quad [N]
  // OUTPUT:
  //   return a V3F containing the desired pitch and roll rates. The Z
  //     element of the V3F can be left at its default value (0)

  // HINTS: 
  //  - we already provide rotation matrix R: to get element R[1,2] (python) use R(1,2) (C++)
  //  - you'll need the roll/pitch gain kpBank
  //  - desCollectiveThrust is a force! You'll likely want to convert it to acceleration first

  V3F pqrCmd;
  Mat3x3F R = attitude.RotationMatrix_IwrtB();

  ////////////////////////////// BEGIN STUDENT CODE ///////////////////////////



  /////////////////////////////// END STUDENT CODE ////////////////////////////

  /////////////////////////////// BEGIN SOLUTION //////////////////////////////

  
  float target_R13 = -CONSTRAIN(accelCmd[0] / (collThrustCmd / mass), -maxTiltAngle, maxTiltAngle);
  float target_R23 = -CONSTRAIN(accelCmd[1] / (collThrustCmd / mass), -maxTiltAngle, maxTiltAngle);
    
  if (collThrustCmd < 0)
  {
    target_R13 = 0;
    target_R23 = 0;
  }
  pqrCmd.x = (1 / R(2, 2))*(-R(1, 0) * kpBank*(R(0, 2) - target_R13) + R(0, 0) * kpBank*(R(1, 2) - target_R23));
  pqrCmd.y = (1 / R(2, 2))*(-R(1, 1) * kpBank*(R(0, 2) - target_R13) + R(0, 1) * kpBank*(R(1, 2) - target_R23));

  //////////////////////////////// END SOLUTION ///////////////////////////////
  return pqrCmd;
}

float QuadControl::AltitudeControl(float posZCmd, float velZCmd, float posZ, float velZ, Quaternion<float> attitude, float accelZCmd)
{
  // Calculate desired quad thrust based on altitude setpoint, actual altitude,
  //   vertical velocity setpoint, actual vertical velocity, and a vertical 
  //   acceleration feed-forward command
  // INPUTS: 
  //   posZCmd, velZCmd: desired vertical position and velocity in NED [m]
  //   posZ, velZ: current vertical position and velocity in NED [m]
  //   accelZCmd: feed-forward vertical acceleration in NED [m/s2]
  // OUTPUT:
  //   return a collective thrust command in [N]

  // HINTS: 
  //  - we already provide rotation matrix R: to get element R[1,2] (python) use R(1,2) (C++)
  //  - you'll need the gain parameters kpPosZ and kpVelZ
  //  - max_ascent_rate and max_descent_rate are maximum vertical speeds. Note they're both >=0!
  //  - make sure to return a force, not an acceleration
  //  - remember that for an upright quad in NED, thrust should be HIGHER if the desired Z acceleration is LOWER

  Mat3x3F R = attitude.RotationMatrix_IwrtB();
  float thrust = 0;

  ////////////////////////////// BEGIN STUDENT CODE ///////////////////////////



  /////////////////////////////// END STUDENT CODE ////////////////////////////

  /////////////////////////////// BEGIN SOLUTION //////////////////////////////

  velZCmd += kpPosZ * (posZCmd - posZ);

  velZCmd = CONSTRAIN(velZCmd, -maxAscentRate, maxDescentRate);

  float desAccel = kpVelZ * (velZCmd - velZ) + accelZCmd - 9.81f;

  thrust = -(desAccel / R(2, 2) * mass);

  //////////////////////////////// END SOLUTION ///////////////////////////////
  
  return thrust;
}

// returns a desired acceleration in global frame
V3F QuadControl::HorizontalControl(V3F posCmd, V3F velCmd, V3F pos, V3F vel, V3F accelCmd)
{
  // Calculate a desired horizontal acceleration based on 
  //  desired lateral position/velocity/acceleration and current pose
  // INPUTS: 
  //   position_cmd: desired position, in NED [m]
  //   velocity_ff: desired velocity, in NED [m/s]
  //   position: current position, NED [m]
  //   velocity: current velocity, NED [m/s]
  //   acceleration_ff: desired acceleration, NED [m/s2]
  // OUTPUT:
  //   return a V3F with desired horizontal accelerations. 
  //     the Z component should be 0
  // HINTS: 
  //  - use fmodf(foo,b) to constrain float foo to range [0,b]
  //  - use the gain parameters Kp_pos_xy and Kp_vel_xy
  //  - make sure you cap the horizontal velocity and acceleration
  //    to max_speed_xy and max_horiz_accel

  // make sure we don't have any incoming z-component
  accelCmd.z = 0;
  velCmd.z = 0;
  posCmd.z = pos.z;

  ////////////////////////////// BEGIN STUDENT CODE ///////////////////////////

  

  /////////////////////////////// END STUDENT CODE ////////////////////////////

  /////////////////////////////// BEGIN SOLUTION //////////////////////////////

  velCmd += kpPosXY * (posCmd - pos);

  if (velCmd.mag() > maxSpeedXY)
  {
    velCmd = velCmd * maxSpeedXY / velCmd.mag();
  }

  accelCmd += kpVelXY * (velCmd - vel);
  if (accelCmd.mag() > maxAccelXY)
  {
    accelCmd = accelCmd * maxAccelXY / accelCmd.mag();
  }

  //////////////////////////////// END SOLUTION ///////////////////////////////

  return accelCmd;
}

// returns desired yaw rate
float QuadControl::YawControl(float yawCmd, float yaw)
{
  // Calculate a desired yaw rate to control yaw to yaw_cmd
  // INPUTS: 
  //   yaw_cmd: commanded yaw [rad]
  //   yaw: current yaw [rad]
  // OUTPUT:
  //   return a desired yaw rate [rad/s]
  // HINTS: 
  //  - use fmodf(foo,b) to constrain float foo to range [0,b]
  //  - use the yaw control gain parameter Kp_yaw

  float yawRateCmd=0;
  ////////////////////////////// BEGIN STUDENT CODE ///////////////////////////


  /////////////////////////////// END STUDENT CODE ////////////////////////////

  /////////////////////////////// BEGIN SOLUTION //////////////////////////////

  float yawError = yawCmd - yaw;
  yawError = fmodf(yawError, F_PI*2.f);
  if (yawError > F_PI)
  {
    yawError -= 2.f * F_PI;
  }
  else if (yawError < -F_PI)
  {
    yawError += 2.f * F_PI;
  }
  yawRateCmd = yawError * kpYaw;

  //////////////////////////////// END SOLUTION ///////////////////////////////
  
  return yawRateCmd;
}

VehicleCommand QuadControl::RunControl(float dt, float sim_time)
{
  curTrajPoint = GetNextTrajectoryPoint(sim_time);

  float desCollThrust = AltitudeControl(curTrajPoint.position.z, curTrajPoint.velocity.z, estPos.z, estVel.z, estAtt, curTrajPoint.accel.z);

  // reserve some thrust margin for angle control
  float thrustMargin = .1f*(maxMotorThrust - minMotorThrust);
  desCollThrust = CONSTRAIN(desCollThrust, (minMotorThrust+ thrustMargin)*4.f, (maxMotorThrust-thrustMargin)*4.f);
  
  V3F desAcc = HorizontalControl(curTrajPoint.position, curTrajPoint.velocity, estPos, estVel, curTrajPoint.accel);
  
  V3F desOmega = ReducedAttitudeControl(desAcc, estAtt, desCollThrust);
  desOmega.z = YawControl(curTrajPoint.attitude.Yaw(), estAtt.Yaw());

  V3F desMoment = BodyRateControl(desOmega, estOmega);

  return GenerateMotorCommands(desCollThrust, desMoment);
}
