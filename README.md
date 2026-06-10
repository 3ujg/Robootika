# [Arduino 2wd robot]

## 1. Projekti idee ja eesmärk
[Projekti eesmärk oli leida juhend internetist. Kokku panna 2wd kit või 3d printida välja oma versioon sellest. Lisada arduino ja juhtmed, programeerida ja testimine. ]

---

## 2. Roboti kirjeldus ja tööpõhimõte
[Puldi vasak juhtkang määrab kiiruse ja paremaga saab keerata vasakule ja paremale. Kood määrab ära mis pinni peale on vastuvõtja ühendatud ja lisab liigutustele juhised.]

---

## 3. Skeemid, joonised ja fotod
!<img width="4032" height="3024" alt="IMG_3701" src="https://github.com/user-attachments/assets/b0b29936-f48e-4050-9035-48f94327bdaa" />

```text
[ AKU / TOIDE ] ──(+/-)──> [ MOTOR SHIELD (Vin / GND klemmid) ]

                            ┌────────────────────────┐
                            │  ARDUINO MOTOR SHIELD  │
[ PAREM MOOTOR ] ◄──(+/-)───┤  Kruviklemm A          │
                            │                        │
[ VASAK MOOTOR ] ◄──(+/-)───┤  Kruviklemm B          │
                            └───────┬────────┬───────┘
                                    │        │ (Shield istub Arduino otsas)
                            ┌───────┴────────┴───────┐
                            │      ARDUINO UNO       │
                            └─┬─┬─┬─┬────────────────┘
                              │ │ │ │  
      ┌───────────────────────┘ │ │ └──────────┐
      │     ┌───────────────────┘ │            │
      │     │     ┌───────────────┘            │
      │     │     │                            │
    (5V)  (GND) (Pin 5)                      (Pin 6)
      │     │     │                            │
      ▼     ▼     ▼                            ▼
    (VCC) (GND) (CH3)                        (CH1)
┌──────────────────────────────────────────────────┐
│                FLYSKY VASTUVÕTJA                 │
│           (Signaal on ülemine rida)              │
└──────────────────────────────────────────────────┘

!video: 

Robot auto
https://youtube.com/shorts/3E_-Qhqxjcw?feature=share



---

## 4. Kasutatud komponendid
[Loetle siin kõik detailid, mis roboti küljes on:]
* [Arduino Uno R3]
* [Arduino Motor Shield Rev3]
* [FlySky FS-i6AB pult ja vastuvõtja]
* [2x9v patareid]
* [Juhtmed]
* [Kinnitus kruvid]
* [Arduino 2wd platvorm ja dc mootorite kit]

---

## 5. Programmikood koos selgitustega
See kood määrab mis pinnidest tuleb mis signaal.
```cpp
// // ====================================================================
// RC CAR MASTER CONTROLLER CODE
// Setup: Left Stick Throttle (CH3 -> Pin 5), Right Stick Steering (CH1 -> Pin 6)
// Features: Signal Smoothing, Inverted Motor Directions, Smart Failsafe
// ====================================================================

// --- Input Pins ---
const int THROTTLE_PIN = 5; // CH3 on FlySky Receiver
const int STEERING_PIN = 6; // CH1 on FlySky Receiver

// --- Arduino Motor Shield Rev3 Pins (Hardwired) ---
const int DIR_A = 12; // Left Motor Direction
const int PWM_A = 3;  // Left Motor Speed
const int DIR_B = 13; // Right Motor Direction
const int PWM_B = 11; // Right Motor Speed

// --- Tuning Configurations ---
const int THROTTLE_MIN = 1050; 
const int STEERING_DEADZONE = 35; // Prevents stick twitching at rest

// Variables for Signal Smoothing
float smoothedSteering = 1500; 

void setup() {
  Serial.begin(9600); // For live diagnostics if plugged into USB
  
  pinMode(STEERING_PIN, INPUT);
  pinMode(THROTTLE_PIN, INPUT);

  pinMode(DIR_A, OUTPUT);
  pinMode(PWM_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  
  // Explicitly disengage shield brake pins (Pins 8 and 9)
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  digitalWrite(9, LOW);
  digitalWrite(8, LOW);
}

void loop() {
  // Read raw PWM signals from the receiver (with a 18ms timeout to prevent lag)
  long throttleRaw = pulseIn(THROTTLE_PIN, HIGH, 18000);
  long steeringRaw = pulseIn(STEERING_PIN, HIGH, 18000);

  // SMART FAILSAFE: Default to neutral values if a wire drops out
  if (throttleRaw == 0) throttleRaw = 1000; 
  if (steeringRaw == 0) steeringRaw = 1500; 

  // --- ANTI-JITTER FILTER ---
  // Blends 70% of the previous stick position with 30% of the new pulse
  smoothedSteering = (smoothedSteering * 0.7) + (steeringRaw * 0.3);

  int motorSpeed = 0;
  int steeringOffset = 0;

  // 1. THROTTLE MATH (Down = 0 speed, Up = Max 255 speed)
  if (throttleRaw > THROTTLE_MIN) {
    motorSpeed = map(throttleRaw, THROTTLE_MIN, 2000, 0, 255);
    motorSpeed = constrain(motorSpeed, 0, 255);
  }

  // 2. STEERING MATH (Uses the smoothed value to eliminate stutters)
  int steeringMapped = map(smoothedSteering, 1000, 2000, -150, 150);
  if (abs(steeringMapped) > STEERING_DEADZONE) {
    steeringOffset = steeringMapped;
  }

  // 3. MIXING FOR 2WD DIFFERENTIAL STEERING
  int leftSpeed = motorSpeed + steeringOffset;
  int rightSpeed = motorSpeed - steeringOffset;

  // Keep final speeds within standard PWM limits (-255 to 255)
  leftSpeed = constrain(leftSpeed, -255, 255);
  rightSpeed = constrain(rightSpeed, -255, 255);

  // 4. EXECUTE MOTOR COMMANDS
  controlLeftMotor(leftSpeed);
  controlRightMotor(rightSpeed);

  delay(10); // Tight loop for responsive handling
}

// --- MOTOR CONTROL FUNCTIONS (With Inverted Directions Fixed) ---

void controlLeftMotor(int speed) {
  if (speed >= 0) {
    digitalWrite(DIR_A, LOW);  // Set to LOW to fix backward throttling
    analogWrite(PWM_A, speed);
  } else {
    digitalWrite(DIR_A, HIGH); 
    analogWrite(PWM_A, abs(speed));
  }
}

void controlRightMotor(int speed) {
  if (speed >= 0) {
    digitalWrite(DIR_B, LOW);  // Set to LOW to fix backward throttling
    analogWrite(PWM_B, speed);
  } else {
    digitalWrite(DIR_B, HIGH); 
    analogWrite(PWM_B, abs(speed));
  }
}
