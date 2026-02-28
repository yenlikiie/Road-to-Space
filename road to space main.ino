const int L_LPWM = 5, L_RPWM = 7;
const int R_LPWM = 6, R_RPWM = 8;
const int DRILL_LPWM = 9;
const int DRILL_RPWM = 10;
const int BUCKET_LPWM = 11;
const int BUCKET_RPWM = 12;

char command;

void setup() {
  Serial.begin(9600);
  pinMode(L_LPWM, OUTPUT); pinMode(L_RPWM, OUTPUT);
  pinMode(R_LPWM, OUTPUT); pinMode(R_RPWM, OUTPUT);
  pinMode(DRILL_LPWM, OUTPUT); pinMode(DRILL_RPWM, OUTPUT);
  pinMode(BUCKET_LPWM, OUTPUT); pinMode(BUCKET_RPWM, OUTPUT);
  stopAll();
}

void loop() {
  if (Serial.available() > 0) {
    command = Serial.read();
    switch (command) {
      case 'F': moveForward(); break;
      case 'L': turnLeft(); break;
      case 'R': turnRight(); break;
      case 'S': stopTracks(); break;
      case 'B': startDrill(); break;
      case 'b': stopDrill(); break;
      case 'D': bucketDown(); break;
      case 'U': bucketUp(); break;
      case 'E': buryModule(); break;
    }
  }
}

void moveForward() {
  analogWrite(L_LPWM, 200); analogWrite(L_RPWM, 0);
  analogWrite(R_LPWM, 200); analogWrite(R_RPWM, 0);
}

void turnLeft() {
  analogWrite(L_LPWM, 0);   analogWrite(L_RPWM, 200);
  analogWrite(R_LPWM, 200); analogWrite(R_RPWM, 0);
  delay(500);
  stopTracks();
}

void turnRight() {
  analogWrite(L_LPWM, 200); analogWrite(L_RPWM, 0);
  analogWrite(R_LPWM, 0);   analogWrite(R_RPWM, 200);
  delay(500);
  stopTracks();
}

void stopTracks() {
  analogWrite(L_LPWM, 0); analogWrite(L_RPWM, 0);
  analogWrite(R_LPWM, 0); analogWrite(R_RPWM, 0);
}

void startDrill() {
  analogWrite(DRILL_LPWM, 200);
  analogWrite(DRILL_RPWM, 0);
}

void stopDrill() {
  analogWrite(DRILL_LPWM, 0);
  analogWrite(DRILL_RPWM, 0);
}

void bucketDown() {
  analogWrite(BUCKET_LPWM, 200);
  analogWrite(BUCKET_RPWM, 0);
  delay(1500);
  stopBucket();
}

void bucketUp() {
  analogWrite(BUCKET_LPWM, 0);
  analogWrite(BUCKET_RPWM, 200);
  delay(1500);
  stopBucket();
}

void stopBucket() {
  analogWrite(BUCKET_LPWM, 0);
  analogWrite(BUCKET_RPWM, 0);
}

void buryModule() {
  startDrill();
  delay(3000);
  stopDrill();
  bucketDown();
  bucketUp();
  stopTracks();
}

void stopAll() {
  stopTracks();
  stopDrill();
  stopBucket();
}