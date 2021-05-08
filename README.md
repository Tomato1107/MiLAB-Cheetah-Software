# MiLAB-Cheetah-Software 
*Creation Date: 2020.03.01* \
*Contributor: Wang Yinuo, Yang Shuyu* \
*Current Maintainer: Wang Yinuo* \
*Email: dbdxwyn@163.com*

## Introduction
Based on [MIT-Cheetah-Software](https://github.com/mit-biomimetics/Cheetah-Software) open-source project, we developed this repository which contains the Robot and Simulation software for our MiLAB quadruped robot. For a getting started guide, see [Getting Started](https://github.com/AWang-Cabin/MiLAB-Cheetah-Software/blob/master/documentation/getting_started.md).

* The **common** folder contains the common library with dynamics and utilities
* The **resources** folder will contain data files, like CAD of the robot used for the visualization
* The **robot** folder will contain the robot program
* The **sim** folder will contain the simulation program. It is the only program which depends on QT.
* The **third-party** will contain *small* third party libraries that we have modified. This should just be libsoem for Cheetah 3, which Pat modified at one point.
* The **config** folder contains simulator and robot's configuration or parameter files.
* The **scripts** folder will be used when run in a real robot.
* The **lcm-types** folder contains all lcm message definition files and corresponding compiled include files in ./cpp folder.
* The **debug-tools** folder contains several debug tool written in Python when develop this project.

## Robot Basic Definition
Although part of the following definitions and settings are also applicable to MIT or UNITREE robots, they are specifically written for MiLAB Robot.
* Default Units in project
```
Length:           m
Angle:            rad
Angular velocity: rad/s
Torque:           N.m
Mass:             kg
Inertia tensor:   kg·m^2
```
* Serial numbers of the legs, joints and links:
```
leg 0: FR -- Front-Right           joint 0: Abduction/Adduction(Ab/Ad)       link 0: Hip link
leg 1: FL -- Front-Left            joint 1: Hip                              link 1: Upper link   
leg 2: RR -- Rear-Right            joint 2: Knee                             link 2: Lower link
leg 3: RL -- Rear-Right
```
* Joint limitation

Joint  | LowerBound | UpperBound
-------|------------|-----------
Ab/Ad  |-90°/45°    | 45°/90°
Hip    |-240°       |  60°
Knee   |36°         | 166°

* Size and mass parameters

Part       | Length | Mass
---------  | ------ | ------
Hip link   | 0.1    | 0.766
Upper link | 0.3    | 1.598
Lower link | 0.34   | 0.363
Motor rotor|    \   | 0.084
Body       |0.5779 x 0.152 x 0.153| 13.777
Total robot|    \   | **26.35**

* The Milab Robot model in simulator needs to use at least 5 kinds and totally 13 pieces of mesh parts, because our robot's upper link is mirror symmetrical. Noted that the MIT cheetah robots only use 4 kinds of mesh parts.

## Coordinate Definition
* The coordinate definition and the zero degrees position of each joint are shown as below.\
The rotation axis of the ab/ad joints is the x axis, and the rotation axis of the hip joint and the knee joint is the y axis. 
Due to joint limitation, although we indicate the nominal zero position of knee joints, it's not actually possible to reach there.\
<img src="https://user-images.githubusercontent.com/69251304/112633818-cbb63a80-8e74-11eb-9679-37c465ab8043.png" width="500" height="500" alt="The Coordinate Definition"/><br/>

* The joint rotation axis definition in simulation is shown as below.\
Note that our joint rotation axis definition is different from UNITREE, but it is consistent with MIT, and the positive direction of rotation conforms to the right-hand rule.\
<img src="https://user-images.githubusercontent.com/69251304/112635651-0c16b800-8e77-11eb-9bb2-08ddf950ed4c.png" width="500" height="500" alt="The motorframe"/><br/>

* The actual motor rotation axis definition is shown as below.\
For each motor in our robot, the rotation axis points along the motor shaft from the motor output to the motor driver.\
<img src="https://user-images.githubusercontent.com/69251304/115496834-12912780-a29d-11eb-8142-d62174f64656.png" width="300" height="300" alt="real motorframe"/><br/>

## Build 
* [Install all Dependencies](https://github.com/AWang-Cabin/MiLAB-Cheetah-Software/blob/dev2/README.md#dependencies) on computer.
* To avoid error about Qt5, following settings should be add to sim/CMakeLists.txt:
    ```
    set(CMAKE_PREFIX_PATH ~/Your_Qt_PATH/Your_Qt_VERSION/gcc_64)

    set(Qt5Core_DIR ~/Your_Qt_PATH/Your_Qt_VERSION/gcc_64/lib/cmake/Qt5Core)
    set(Qt5Widgets_DIR ~/Your_Qt_PATH/Your_Qt_VERSION/gcc_64/lib/cmake/Qt5Widgets)
    set(Qt5Gamepad_DIR ~/Your_Qt_PATH/Your_Qt_VERSION/gcc_64/lib/cmake/Qt5Gamepad)
    ```
* To avoid Stack Overflow, append following commands to the end of ~/.bashrc (Not a mandatory step):
    ```
    ulimit -s 102400
    echo "[Bash Info] Stack size has been changed to $(ulimit -s) KB for Milab Quadrupedal"
    ```
* To build all code:
    ```
    mkdir build
    cd build
    cmake ..
    ./../scripts/make_types.sh
    make -j8
    ```

If you are building code on your computer that you would like to copy over to the real robot, go to [Run Real Robot] for details.

If you are building code on the robot's UP-board, you do not need to change above commands.

This build process builds the common library, robot code, and simulator. If you just change robot code, you can simply run `make -j4` again. 

If you change LCM types, you'll need to run `cmake ..; make -j8`. This automatically runs `make_types.sh`.

To test the common library, run `common/test-common`. To run the robot code, run `robot/robot`. To run the simulator, run `sim/sim`.

Part of this build process will automatically download the googletest software testing framework and sets it up. 

After it is done building, it will produce a `libbiomimetics.a` static library and an executable `test-common`.  Run the tests with `common/test-common`. 

This output should hopefully end with
```
[----------] Global test environment tear-down
[==========] 97 tests from 20 test suites ran. (1212 ms total)
[  PASSED  ] 97 tests.
```
## Run simulator
* To run the simulator, open a command window:
    ```
    cd MiLAB-Cheetah-Software/build
    ./sim/sim
    ```
* In the another command window in the same path, run the robot controller:
    ```
    ./user/${controller_folder}/${controller_name} ${robot_name} ${target_system}
    ``` 
    Example:
    ```
    ./user/MiLAB_Controller/milab_ctrl i s 
    ```
    i: Milab robot,  3: Cheetah 3,  m: Mini Cheetah \
    s: simulation,  r: robot

## Run Real Robot
* Install Linux System (Recommend Ubuntu 16.04) and RT kernel for UP-board. 
* [Install all Dependencies except Qt](https://github.com/AWang-Cabin/MiLAB-Cheetah-Software/blob/dev2/README.md#dependencies) on robot's UP-board.
* Open terminal and create mc-build folder:
    ```
    cd MiLAB-Cheetah-Software
    mkdir mc-build && cd mc-build
    ```
* Build for milab robot: 
    ```
    cmake -DMILAB_BUILD=TRUE ..
    make -j8
    ``` 
* In a new terminal, connect to robot's UP-board over ethernet or WIFI:
    *  By ethernet: 
        * Set server PC's ethernet ip as`10.0.0.2` 
        * robot's ethernet ip as`10.0.0.21` and then
        ```
        ssh robot@10.0.0.21
        ``` 
    * By WIFI without other pre-settings:
        ```
        ssh robot@10.61.6.124
        ``` 
    The password of robot's UP-board is `1`

* Go back to the terminal that under server PC account and mc-build path.
* Copy robot-software to robot's UP-board with: 
    ```
    sh ../scripts/milab_scripts/send_to_milab_cheetah.sh
    ``` 
   *More details in **send_to_milab_cheetah.sh**, default option is sending MiLAB_Controller by WIFI if not specified*

* Go to ssh terminal and enter the robot program folder: 
    ```
    cd robot-software-XXX
    ```
* Run robot controller: 
    ```
    ./run_mc.sh ./milab_ctrl
    ``` 
    or
    ```
    ./run_mc.sh ./jpos_ctrl
    ```
 * For more guides, go to [Running Real Robot](https://github.com/AWang-Cabin/MiLAB-Cheetah-Software/blob/master/documentation/running_real_robot.md).

## Change Controller or Robot
Go to the [Instruction of changing Controller Parameters or Robots](https://github.com/AWang-Cabin/MiLAB-Cheetah-Software/blob/dev2/documentation/ChangeController.md) for details.

## Operation Guide
* [Real robot operation guide](https://github.com/AWang-Cabin/MiLAB-Cheetah-Software/blob/dev2/documentation/realrobot_opertion_guide.md) 
* [Simulation operation guide]()

## Dependencies
To use Ipopt, use CMake Ipopt option. Example: cmake -DIPOPT_OPTION=ON ..
* Update linux software repositories:
    ```
    sudo add-apt-repository ppa:ubuntu-toolchain-r/test
    sudo apt update
    sudo apt upgrade
    ```
* Intall dependent packages:
    ```
    sudo apt install mesa-common-dev freeglut3-dev coinor-libipopt-dev libblas-dev liblapack-dev gfortran cmake gcc build-essential libglib2.0-dev
    ```
* Check cmake version (Must higher than 3.5)
    ```
    cmake --version
    ```
* Install gcc (If lower than 5.0): 
    ```
    g++ --version
    gcc -v
    ```
    ```
    sudo apt-get install gcc-5 g++-5
    sudo updatedb && sudo ldconfig
    ```
* Install openjdk (Must installed before LCM):
    ```
    sudo apt-get install openjdk-8-jdk default-jdk
    ```
* Install LCM (Recommend lcm-1.4)
    * Download package [lcm-1.4.0](https://github.com/AWang-Cabin/MiLAB-Cheetah-Software/releases/download/v0.9.6/lcm-1.4.0.zip) 
    * Unzip to /home and install (Must unzip to /home)
        ```
        cd lcm-1.4.0
        mkdir build && cd build
        cmake ..
        make -j4
        sudo make install
        sudo ldconfig
        ```
    * [LCM main page](https://lcm-proj.github.io/)

* Install [Eigen](http://eigen.tuxfamily.org/)
    ```
    sudo apt-get install libeigen3-dev
    ```
* Install Qt5 on Ubuntu 16.04 (Recommend Qt5.10)
    * Download package [qt5.10.0](https://github.com/AWang-Cabin/MiLAB-Cheetah-Software/releases/download/v0.9.6/qt-opensource-linux-x64-5.10.0.run) 
    * Run installer 
      ```
      sudo chmod a+x qt-opensource-linux-x64-5.10.0.run
      ./qt-opensource-linux-x64-5.10.0.run
      ```
    * Follow [config instructions](https://github.com/AWang-Cabin/MiLAB-Cheetah-Software/blob/dev2/documentation/qt_install.md)
    * On Ubuntu 18.10 or 19.04, you may instead install Qt directly with command:
        ```
        sudo apt install libqt5 libqt5gamepad5
        ```
* Install IPOPT (Recommend ipopt-3.12.7 or newer)
    * Install dependency
        ```  
        sudo apt-get install cppad subversion patch wget checkinstall
        ```
    * Download package [ipopt.3.12.7](https://github.com/AWang-Cabin/MiLAB-Cheetah-Software/releases/download/v0.9.6/qt-opensource-linux-x64-5.10.0.run) 
    * Unzip to /home and install third-party lib
        ```
        unzip Ipopt-3.12.7.zip
        cd Ipopt-3.12.7
        ```
        ```
        cd ThirdParty/Blas
        ./get.Blas
    
        cd ../ASL
        ./get.ASL
    
        cd ../Lapack
        ./get.Lapack
    
        cd ../Mumps
        ./get.Mumps
    
        cd ../Metis
        ./get.Metis
        ```
    * Make and install on /usr/local
        ```
        cd ../..
        mkdir build && cd build
        ../configure --prefix=/usr/local/
        sudo make
        sudo make test
        sudo make install
        sudo ldconfig
        ```

## Change Log
This list records nearly all files we modified or created for our own MiLAB quadrupedal. \
Remember to check corresponding include files of following source files. \
Whenever you change or add other project files, please update this list. \
**done**: have finished and submitted \
**doing**: working on it now \
**todo**: plan to modify recently or still have *TODO* tips in the file \
**new**: new file specifically for Milab robot
```
**********/MiLAB-Cheetah-Software********************************************
CMakeLists.txt                     *

********/resources*********************************************
                                  done   doing    todo    new
milab_body.obj                     *                       *
milab_hip.obj                      *                       *
milab_upper_link.obj               *                       *
milab_upper_link_mirror.obj        *                       *
milab_lower_link.obj               *                       *


********/config************************************************
                                  done   doing    todo    new
milab-robot-defaults.yaml          *                       *
milab-user-defaults.yaml           *                       *
default-terrain.yaml               *
jpos-user-parameters.yaml          *                       *

********/common************************************************
******/src**                     done   doing    todo     new
****/Dynamics**
Quadruped.cpp                      * todo
FloatingBaseModel.cpp              * 
****/SimUtilities**
SpineBoard.cpp                     *
****/ControllerParameters**
ControlParameters.cpp              * 

******/include**
cppTypes.h                         *
****/Dynamics**
Milab.h                            * todo                  *
****/ControlParameters**
SimulatorParameters.h              *
ControlParameters.h                *
RobotParameters.h                  *
****/SimUtilities**
SpineBoard.h                       *
****/Utilities**
utilities.h                        *

********/robot*************************************************
******/src**                     done   doing    todo     new
RobotRunner.cpp                    * todo
main_helper.cpp                    *
HardwareBridge.cpp                 *
SimulationBridge.cpp                 *
****/rt**
rt_rc_interface.cpp                *
rt_subs.cpp                        *
rt_spi.cpp                         *todo

******/include**
HardwareBridge.h                   *


********/scripts***********************************************
******/milab_scripts**           done   doing    todo     new
send_to_milab_cheetah.sh           *                       *
run_milab.sh                       *                       *


********/sim***************************************************
******/src**                     done   doing    todo     new
Simulation.cpp                     * todo
SimControlPanel.cpp                * todo
SimControlPanel.ui                 *
RobotInterface.cpp                 *
Graphics3D.cpp                     *
DrawList.cpp                       * todo


********/user**************************************************
******/MiLAB_Controller**        done   doing    todo     new
****/Controllers**
**/convexMPC**
RobotState.cpp                     * todo
SolverMPC.cpp                      *
convexMPC_interface.cpp            *
ConvexMPCLocomotion.cpp            * todo
RobotState.h                       *
SolverMPC.h                        *
convexMPC_interface.h              *
ConvexMPCLocomotion.h              *

**/WBC_Ctrl** 
WBC_Ctrl.cpp                       * todo
*/LocomotionCtrl
LocomotionCtrl.cpp                 * todo

****/FSM_States**
FSM_State.cpp                      * todo
FSM_State_Locomotion.cpp           * todo
FSM_State_RecoveryStand.cpp        * todo
FSM_State_StandUp.cpp              * todo
FSM_BalanceStand.cpp               * todo
FSM_SquatDown.cpp                  *                        *
FSM_SquatDown.h                    *                        *
SafetyChecker.cpp                  *

******/MiLAB_JPos_Controller**
jpos_user_parameters.cpp           *
JPosUserParameters.h               *


********/debug-data**********************************************
leg_controller_plot.py             *                        *
positive_matrix_check.py           *                        *

```
