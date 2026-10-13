# Intro2Robotics-Demo-3

FILES
- robot_demo3.html: browser interface, standard-DH forward kinematics, numerical multistart IK, solution selection and pick/place sequence.
- roboticsdemo_3.ino: Arduino Mega + PCA9685 serial controller code.

SETUP
1. Arduino IDE: install Adafruit PWM Servo Driver Library.
2. Open/upload robotdemo_3.ino to the Arduino Mega at 115200 baud.
3. On Windows, open a terminal in this folder and run: python -m http.server 8000
4. In Chrome or Edge, open http://localhost:8000/robot_demo3.html
5. Click Connect to Arduino and choose the correct serial port.

IMPORTANT
- Web Serial is supported in desktop Chrome/Edge on localhost/HTTPS; not all browsers support it.
- The DH rows in the HTML are placeholders. The first five rows approximate the symbolic table; row 6 is provisional. Numeric link values are not yet verified distances between joint axes. 
- IK count is numerical candidates found, not guaranteed exhaustive.
- Servo channels 0–5 are assumed for six arm joints, channel 6 for gripper. Verify wiring.
- The pick/place motion is open-loop and timed. No collision detection or block/joint feedback.
