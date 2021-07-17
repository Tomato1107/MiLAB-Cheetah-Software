#include "Lowlevel_Controller.hpp"

/*
 * This is a low level (joint level) controller interface of Milab robot, which is designed to communicate with python by LCM.
 * As a high level controller, you can send desired joint position, joint velocity, joint feedforward torque to robot and get states
 * including current joint position, joint velocity, robot's rpy, angular velocity, linear velocity.
 * and leg_enable param in jpos-user-parameters.yaml. KP, KD, feedforward-torque can also be tuned in that file.
 * By modifying this file, the robot motion logic can be changed as you need.
 *
 * Wang Yinuo, 04/24/2021, dbdxwyn@163.com
 */
void Lowlevel_Controller::initializeController(){

    _legController->_maxTorque = userParameters.max_tau;
    _legController->_legsEnabled = true;

    if(userParameters.calibrate > 0.4) {  // This param is only used in TI board in Cheetah3
        _legController->_calibrateEncoders = userParameters.calibrate;
    } else {
        if (userParameters.zero > 0.5) {  //　This param is only used in TI board in Cheetah3
            _legController->_zeroEncoders = true;
        } else {
            _legController->_zeroEncoders = false;
        }
    }
    for (int leg = 0; leg < 4; ++leg) {
        _jpos_fold[leg] << 0.f, -1.65f, 2.6f;
        _jpos_stand[leg] << 0.f, -0.92f, 1.6f;
        _jpos_pre_off[leg] << 0.f, -1.45f, 2.78f;
    }
}

void Lowlevel_Controller::runController(){

    kpMat << userParameters.kp[0], 0, 0, 0,  userParameters.kp[1], 0, 0, 0,  userParameters.kp[2];
    kdMat << userParameters.kd[0], 0, 0, 0, userParameters.kd[1], 0, 0, 0, userParameters.kd[2];

    ++j_iter;
    // Get robot initial joint position
    if(j_iter < 100){
        for(int leg(0); leg<4; ++leg){
            for(int jidx(0); jidx<3; ++jidx){
                _jpos_ini[leg][jidx] = _legController->datas[leg].q[jidx];
            }
        }
    }
//    std::cout<<_jpos_ini[0]<<std::endl;
    if (j_iter%5000 == 0 && j_iter < userParameters.max_iter)
        printf("[Jpos_Ctrl] INFO: Control iteration is %ld now ...\n\n", j_iter);

    if(userParameters.calibrate <= 0.4 && !_legController->_zeroEncoders) {
        /*
         * This is a test code which used to control any legs or any joints dependently to move a sine trajectory
         * */
        #ifdef JPOS_TEST
        if (j_iter>= 100 && j_iter <= userParameters.max_iter){
            for(int leg(0); leg<4; ++leg){
                if (userParameters.leg_enable[leg]){ // check leg enable state
                    for(int jidx(0); jidx<3; ++jidx){
                        float pos = std::sin(.001f * (j_iter-100));

                        if (userParameters.joint_enable[jidx]){ //check joint enable state
                            if (jidx == 0)
                                _legController->commands[leg].qDes[jidx] = _jpos_ini[leg][jidx];
//                                        userParameters.move_range * pos + _jpos_ini[leg][jidx] - userParameters.move_offset;
                            else
                                _legController->commands[leg].qDes[jidx] = userParameters.move_range * pos + _jpos_ini[leg][jidx];
                            _legController->commands[leg].qdDes[jidx] = 0.;
                            _legController->commands[leg].tauFeedForward[jidx] = userParameters.tau_ff;
                        }else{
                            _legController->commands[leg].tauFeedForward[jidx] = 0.;
                            kpMat(jidx,jidx) = 0.;
                            kdMat(jidx,jidx) = 0.;
                        }
                    }
                    _legController->commands[leg].kpJoint = kpMat;
                    _legController->commands[leg].kdJoint = kdMat;
                }else{
                    leg_Passive(leg);
                }
            }
        }
        else{
            for (int leg = 0; leg < 4; ++leg) {
                leg_Passive(leg);
            }

        }
#endif
#ifdef JPOS_UP_DOWN

        motion_iter++;
//          fold legs
        if (j_iter>= 100 && flag == FOLDLEG){
            for (int leg = 0; leg < 4; ++leg) {
                if (motion_iter >= fold_iter+wait_iter){
                    motion_iter =0;
                    flag = STANDUP;
                }
                else if (motion_iter > fold_iter && motion_iter < fold_iter+wait_iter)
                    leg_JointPD(leg, _jpos_fold[leg], zero_vec3);
                else
                    leg_Interpolation(motion_iter,fold_iter,leg,_jpos_ini[leg],_jpos_fold[leg]);
            }

//            stand up and keep standing
        } else if (flag == STANDUP){
            for (int leg = 0; leg < 4; ++leg) {
                if (motion_iter >= stand_iter+keep_iter){
                    motion_iter =0;
                    flag = SQUAD;
                }
                else if (motion_iter > stand_iter && motion_iter < stand_iter+keep_iter)
                    leg_JointPD(leg, _jpos_stand[leg], zero_vec3);
                else
                    leg_Interpolation(motion_iter,stand_iter,leg,_jpos_fold[leg],_jpos_stand[leg]);
            }
//            squad down and motor switch down
        } else if (flag == SQUAD) {
            for (int leg = 0; leg < 4; ++leg) {
                if (motion_iter >= squad_iter + wait_iter)
                    leg_Passive(leg);
                else if (motion_iter > squad_iter && motion_iter < squad_iter + wait_iter)
                    leg_JointPD(leg, _jpos_pre_off[leg], zero_vec3);
                else
                    leg_Interpolation(motion_iter, squad_iter, leg, _jpos_stand[leg], _jpos_pre_off[leg]);
            }
        }
#endif
    }
}

void Lowlevel_Controller::leg_Interpolation(const size_t & curr_iter, size_t max_iter, int leg,
                                        const Vec3<float> & ini, const Vec3<float> & fin){

    float a(0.f);
    float b(1.f);

    // interpolating
    if(curr_iter <= max_iter) {
      b = (float)curr_iter/(float)max_iter;
      a = 1.f - b;
    }

    // compute setpoints
    Vec3<float> inter_pos = a * ini + b * fin;

    // send control commands
    leg_JointPD(leg, inter_pos, zero_vec3);
}

void Lowlevel_Controller::leg_JointPD(int leg, Vec3<float> qDes, Vec3<float> qdDes) {

    _legController->commands[leg].kpJoint = kpMat;
    _legController->commands[leg].kdJoint = kdMat;

    _legController->commands[leg].qDes = qDes;
    _legController->commands[leg].qdDes = qdDes;
}

void Lowlevel_Controller::leg_Passive(int leg){

    _legController->commands[leg].kpJoint.setZero();
    _legController->commands[leg].kdJoint.setZero();
    for (int jidx = 0; jidx < 3; ++jidx) {
        _legController->commands[leg].tauFeedForward[jidx] = 0.;
    }

}