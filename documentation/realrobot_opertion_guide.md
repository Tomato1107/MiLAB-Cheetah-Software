# Real Robot Operation Guide
Author: Yinuo Wang\
Date: 2021.04.13\
Email: dbdxwyn@163.com
## Remote Commander
**1.Introduction**


**2.Key Function**
```
* Left Stick: X--Adjust Pitch Angle
              Y--Turn Left / Right

* Right Stick: X--Go Ahead / Back
               Y--Go Left / Right

* Switch A: UP-- Emergency Stop (Torque=0)
            DWN--Work with SwE

* Switch E: UP-- Work with SwG
            MID--StandUp Mode (0.35m)
            DWN--SquartDown Mode (0.18m)

* Switch G: UP-- RecoveryStand Mode
            MID--BalanceStand Mode
            DWN--Locomotion Mode (0.50m)

* Switch B: UP-- Work with SwC to select 6 Locomotion gaits
            DWN--Standing gait

* Switch C: UP-- Trotting gait    /  Walking gait
            MID--Trotrunning gait /  Pronking gait
            DWN--Pacing gait      /  Galloping gait

* Switch D: UP-- Work with SwC to select first gait group
            DWN--Work with SwC to select second gait group

* Variable B: Adjust Step Height (0-0.2m)
```
**3.Operation Procedure**
* Initial Configuration
```
SwA = UP
SwB = UP
SwC = UP
SwD = UP
SwE = UP
SwG = UP
```
* Start Up To RecoveryStand Mode
```
SwA = DWN
SwB = UP
SwC = UP
SwD = UP
SwE = UP
SwG = UP
```

* Trans To BalanceStand Mode
```
SwA = DWN
SwB = UP
SwC = UP
SwD = UP
SwE = UP
SwG = MID
```

* Trans To Locomotion Mode
```
SwA = DWN
SwB = UP
SwC = UP
SwD = UP
SwE = UP
SwG = DWN
```

* Switch Gaits
```
SwA = DWN
SwE = UP
SwG = DWN

--------------------------------------------
SwB = UP    |   SwD = UP   |    SwD = DWN   |
---------------------------------------------
SwD = UP    |   Trotting   |     Walking    |
SwD = MID   | Trotrunning  |    Pronking    |
SwD = DWN   |    Pacing    |    Galloping   |
---------------------------------------------
---------------------------------------------
SwB = DWN   |            Standing           |
---------------------------------------------
```

* Emergency Stop\
**IN ANY SITUATIONS (IGNORE OTHER SWITCHES' STATES):**
```
SwA = UP
```

* Trans To StandUp Mode
```
SwA = DWN
SwB = UP
SwC = UP
SwD = UP
SwE = MID
SwG = UP
```

* Trans To SquartDown Mode 
```
SwA = DWN
SwB = UP
SwC = UP
SwD = UP
SwE = DWN
SwG = UP
```

* Shutdown Procedures
```
1. Trans to BalanceStand Mode / Trans To RecoveryStand Mode
2. Trans To StandUp Mode
3. Trans To SquartDown Mode 
4. Shut Down (Trans To Initial Configuration)
5. Robot Power Off 
```