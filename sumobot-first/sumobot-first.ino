// there are an astonishing number of libraries for the ultrasonic sensor 
// this one is from https://github.com/gamegine/HCSR04-ultrasonic-sensor-lib
#include <HCSR04.h>; 

// setting up the pin numbers in these following lines

// Ultrasonic
HCSR04 hc(7, 6); // (TRIG pin, ECHO pin)

// motor one 
int enA = 8;
int in1 = 12;
int in2 = 11;
// motor two
int enB = 13;
int in3 = 10;
int in4 = 9;
// infrared sensor output pin
int IROut = 5;

// used later on
float objDistance;
int floorColor;
int objLastSeen;

void setup()
{
  // set all the motor control pins to outputs
  // each motor has three pins
  // enA/B is analog, 0~255 (inclusive); controls the absolute voltage applied to the motor (how fast the motor goes)
  // in1/2 and in3/4 are digital, HIGH or LOW; they control the polarity of the voltage (what direction the motor spins)
  // if in1 is HIGH, in2 must be LOW and vice versa (so that current can flow from one pin to the other)
  // consider it this way: enA/B is like the absolute value of a number, and in1/2/3/4 is like the sign
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(IROut, INPUT);

  Serial.begin(9600); // data transmission rate in baud
}

void goForward()
{
  // turn on motor A
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  // set speed to 255 out of possible range 0~255
  analogWrite(enA, 255);
  // turn on motor B
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  // set speed to 255 out of possible range 0~255
  analogWrite(enB, 255);
}
void goBack()
{
  // turn on motor A
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  // set speed to 200 out of possible range 0~255
  analogWrite(enA, 200);
  // turn on motor B
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  // set speed to 200 out of possible range 0~255
  analogWrite(enB, 200);
}
void turnLeft()
{
  // turn on motor A
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  // set speed to 180 out of possible range 0~255
  analogWrite(enA, 180);
  // turn on motor B
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  // set speed to 180 out of possible range 0~255
  analogWrite(enB, 180);
}
void turnRight()
{
  // turn on motor A
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  // set speed to 180 out of possible range 0~255
  analogWrite(enA, 180);
  // turn on motor B
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  // set speed to 180 out of possible range 0~255
  analogWrite(enB, 180);
}
void stop() 
{
  analogWrite(enA, 0);
  analogWrite(enB, 0);
}
void loop() // this code section repeats until something stops it
{
  // hc.dist() returns the distance (cm) seen by the ultrasonic sensor
  // if distance is above the maximum distance it can detect, it returns 0.0
  objDistance = hc.dist();
  // IROut has a digital (two options) output: 0 means it detects a reflected signal, 1 means it does not
  // in practical terms, this means that 0 = white surface, 1 = black surface
  floorColor = digitalRead(IROut);
  if (floorColor == 0) { // if surface = white (in bounds), continue with operation
    // if no object is detected within range of the sensor, OR if an object is detected at a distance >= 75 cm,
    // AND no object has been seen recently, do this
    if (objDistance == 0.0 || objDistance >= 75 && objLastSeen >= 15) { 
      // This tells the robot, "If you don't see anything close in front of you, and you haven't seen anything recently, turn left..."
      turnLeft(); 
    }
    // if no object is detected within range of the sensor, OR if an object is detected at a distance >= 75 cm,
    // AND an object has been seen recently, do this
    else if (objDistance == 0.0 || objDistance >= 75 && objLastSeen < 15) {
      // This tells the robot, "...otherwise, if you don't see anything close in front of you, but you have seen something recently, turn right..."
      turnRight();
    }
    // if an object is detected within the range 0-75 cm (exclusive), do this
    else { 
      // This tells the robot, "...otherwise, go forward."
      goForward();
      // reset timer for object last being seen
      objLastSeen = 0;
    }
    delay(60); // Wait 60 milliseconds (this command may be unnecessary)
  }
  else { // if surface = black (out of bounds), stop the robot
    stop();
    // Note that, though the robot's motors have stopped, the loop is still running
    // This is useful in that, if the robot is placed back on a white surface, it will start operating again without needing a reboot
  }
  // send the outputs of the IR sensor and ultrasonic sensor to console 
  Serial.println(objLastSeen);
  Serial.println(floorColor); 
  Serial.println(objDistance);
  objLastSeen += 1;
}
