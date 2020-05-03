Communication protocol format

Output:
[!    <3 letter data name>   :  <data value> ]
Example:
[!pos:100000]
Always used 3 letter data name!


Input command:
(!?<command name>)
Example:
(!?S)



Available output:

[!VER:0.1beta3] - current firmvare version
[!VED:03092018] - firmvare compile date
[!POS:1000] - stepper motor current position
[!SPD:1] - current stepper motor rotation speed
[!OPN:<1|0>] - relay "open" status: 1 (relayed closed) или 0 (relay opened)
[!CLS:<1|0>] - relay "closed" status: 1 (relayed closed) или 0 (relay opened)
So [!OPN:1] and [!CLS:0] means "Cover is open"
So [!OPN:0] and [!CLS:1] means "Cover is closed"
So [!OPN:0] and [!CLS:0] means "Cover is intermediate position"
So [!OPN:1] and [!CLS:1] means "Something went wrong"


[!TEX:24.3] - external temperature
[!THE:30.3] - heater temperature
[!TDH:39.3] - temperature (DHT22)
[!HUM:39.3] - relative humidity (DHT22)
[!RL:<1|0>] - heater relay status: 1 "opened", 0 "closed"

Available input:

(#MOVE) - init moving (auto mode)
(#STOP) - stop moving

(#SPD:xxx) - set stepper motor rotation speed (from 1 to ...)
(#MOV:xxx) - move motor to xxx steps. Negative to reverse
(!DR1) - move positive direction
(!DR2) - move negative direction

(!?RL1:1) - set relay 1|0
