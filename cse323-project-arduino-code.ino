#include <Servo.h>

Servo sl;   // 180° positional servo
Servo cr;   // 360° continuous servo

int trigPin = 5;
int echoPin = 6;
long duration;
int cm;

// Start assuming no hand is detected within 15 cm
bool objectClose = false;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  sl.attach(9);   // 180 degree servo signal pin
  cr.attach(10);  // 360 degree continuous servo signal pin

  sl.write(0);    // Ensure starting position of 180 degree servo
  cr.write(90);   // Stop the 360° continuous servo's spinning at the start to prevent it from spinning automatically
}

void loop() {
  // Trigger the ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echo (with timeout)
  duration = pulseIn(echoPin, HIGH, 20000); // timeout after 20 ms (~3.4 m range)

  // If no echo received
  if (duration == 0) {
    Serial.println("No echo");

    if (objectClose) {
      // Return servos to initial state if they were active, because we have found a problem of "no echo" by ultrasonic sensor
      sl.write(0);      // Reset 180 degree servo to return to inisial state
      cr.write(180);   // Make the 360 degree servo spin counter-clockwise (CCW)
      delay(140);      // then, back rotation to return initial state (approximate)
      cr.write(90);    // then, stop 360 degree servo's spinning
      objectClose = false;
    }
    return; 
  }

  // Calculate distance in cm
  cm = duration / 2 / 29.1;

  // Limit detection range
  if (cm <= 0 || cm > 300) {
    Serial.println("Out of range");

    if (objectClose) {
      sl.write(0);
      cr.write(180);
      delay(140);
      cr.write(90);
      objectClose = false;
    }
    return;
  }

  Serial.print("Distance: ");
  Serial.print(cm);
  Serial.println(" cm");

    // Control the servos based on distance
  if (cm <= 15 && !objectClose) {
    // Object just came close
    
    for (int i = 0; i < 2; i++) {   // Servos pump sanitizer 2 times for each time detection
      sl.write(120);        // Move 180 degree servo ~120 degree
      cr.write(0);          // Make the 360 degree servo spin clockwise (CW)
      delay(500);           // then, ~120 degree rotation approximate 
      cr.write(90);         // then, stop 360 degree servo's spinning
      
      sl.write(0);          // Reset 180 degree servo to return to inisial state
      cr.write(180);        // Make the 360 degree servo spin counter-clockwise (CCW)
      delay(140);           // then, back rotation to return initial state (approximate)
      cr.write(90);         // then, stop 360 degree servo's spinning
      
      delay(300);           // Small pause between pumps
    }

    objectClose = true;   // Mark that hand is detected within 15 cm
  }  
  else if (cm > 15 && objectClose) {
    // Object moved away, return servos to initial position
    sl.write(0);          // Reset 180 degree servo to return to inisial state
    cr.write(180);        // Make the 360 degree servo spin counter-clockwise (CCW)
    delay(140);           // then, back rotation to return initial state (approximate)
    cr.write(90);         // then, stop 360 degree servo's spinning
    objectClose = false;  // Mark that hand is detected within 15 cm
  }

  delay(100); // Small delay to avoid too frequent readings
}
