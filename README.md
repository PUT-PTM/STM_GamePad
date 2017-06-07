# STM_GamePad

Overview

The project is a simple version of the gamepad. The STM32 Discovery gets data from buttons and joysticks and sends to a application (via Bluetooth), which displays the results.

Description

The board used for computing is STM32f407vg. The external power supply for the board is the powerbank (The system is wireless). The casing is made of wood. Gamepad uses 14 buttons (8 located on the top, 4 located on the front of the casing and 2 located in the joystics) and 2 joysticks ( located on the top of the casing). The data gathered from the buttons and joysticks is packed into a package and sent to the application in this form: „OA1=0A2=0A3=1...AN1Y=4AN2X=4095AN3Y=2048!”
"O" is a starting byte. "A 1=0" is the current state of the button. "AN3Y=2048" is the current state of the joystick’s posistion (horizontally/perpendicularly). "!" ends the packet.
Data are sending to the Application via Bluetooth HC-06. Module is sending char after char (baud-rate 9600).

Tools

STM32 programme wrote in C (CooCox CoIDE Version: 1.7.8). The PC application was made in C++ Microsoft Visual Studio 2017.

How to run

Connect the external power (Powerbank). Pair your PC with the HC SR-06 module. Pass 1234 as the PIN number. Open your app, pick the port COM.

How to compile

Compile using the CoIDE

Future improvements

Sometimes buttons have troubles with interferences and the bad state is sent (a better cabling could solve this problem). The casing could be done more ergonomically and comfortable for user.

License

MIT

Credits

Jakub Prałat, Kacper Olek

The project was conducted during the Microprocessor Lab course held by the Institute of Control and Information Engineering, Poznan University of Technology. Supervisor: Marek Kraft/Michał Fularz/Tomasz Mańkowski/Adam Bondyra
