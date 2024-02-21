#include <iarduino_HC_SR04.h>

#define rSpeed 5
#define rIr 4 // DIR

#define lSpeed 6
#define lIr 7 // DIR

#define Kp 1 // коэф пропорциональности
#define lMaxSpeed 105
#define rMaxSpeed 105
#define rBaseSpeed 90
#define lBaseSpeed 90

int distFront = 10;
int distRight = 10;
bool stopFlag = false;

int front;
int right;

// TRIG and ECHO
iarduino_HC_SR04 hcsrRight(3,9); // Правый
iarduino_HC_SR04 hcsrFront(12,13);  // Передний

void setup() {
  Serial.begin(9600);
  for (int i = 4; i < 8; ++i) {
    pinMode(i, OUTPUT);
  }
}

void sensors() {
  front = hcsrFront.distance();
  right = hcsrRight.distance();
}

void forward() {
  sensors();
  int error = distRight - right;
  int motorSpeed = Kp * error;
  int rMotorSpeed = rBaseSpeed + motorSpeed;
  int lMotorSpeed = lBaseSpeed - motorSpeed;

  if (rMotorSpeed > rMaxSpeed) rMotorSpeed = rMaxSpeed;
  if (lMotorSpeed > lMaxSpeed) lMotorSpeed = lMaxSpeed;
  if (rMotorSpeed < 0) rMotorSpeed = 0;
  if (lMotorSpeed < 0) lMotorSpeed = 0;

  digitalWrite(rIr, HIGH); // вперёд
  analogWrite(rSpeed, rMaxSpeed); // макс скорость
  // полный вперёд
  digitalWrite(lIr, HIGH); // вперёд
  analogWrite(lSpeed, lMaxSpeed); // макс скорость
}

void turnRight() {
  digitalWrite(rIr, HIGH); // вперёд
  analogWrite(rSpeed, rMaxSpeed); // макс скорость
  // ротация направо
  digitalWrite(lIr, LOW); // назад
  analogWrite(lSpeed, lMaxSpeed); // макс скорость
}

void turnLeft() {
  digitalWrite(rIr, LOW); // назад
  analogWrite(rSpeed, rMaxSpeed); // макс скорость
  // ротация налево
  digitalWrite(lIr, HIGH); // вперёд
  analogWrite(lSpeed, lMaxSpeed); // макс скорость
}

void stopRobot() {
  digitalWrite(rIr, LOW); // назад
  analogWrite(rSpeed, rMaxSpeed); // ноль
  // эффект торможения, полная остановка
  digitalWrite(lIr, LOW); // назад
  analogWrite(lSpeed, lMaxSpeed); // ноль
}

/*
В арсенале только передний и правый датчик, двигаемся по правой руке.
TODO: Функции которые нужно реализовать:
(r - готово, p - в процессе)
r 1) свободная езда вперёд с растоянием и фиксированием от стенки
делается это через прямопропорциональный регулятор.
p 2) поворот вправо при виде препятствия впереди
при значениях front < 10 нужно !остановить его stopRobot(),
!повернуть на 90 градусов против часовой, !заставить дальше ехать.
значение фронта как-то нужно зафискировать и заставить выполнить
дальнейший комплекс функций, а потом уже считывать датчики дальше.
p 3) поворот влево при отсутствии считывания впереди
при значениях right > 19 нужно !front < 16 (ближайшая передняя стенка),
!остановить его с stopRobot(); !повернуть на 90 по часовой, !форвард
или опять поворот...
*/

void debugSensors() {
  Serial.println("Front: ");
  Serial.println(hcsrFront.distance());
  Serial.println("Right: ");
  Serial.println(hcsrRight.distance());
  Serial.println(" ");  //  space
}

void mainMaze() {
  sensors();
  if (front < 10 && right < 10) {
    front = 9; right = 9;
    if (!stopFlag) { stopRobot(); stopFlag = true; }
    turnRight();
  }
  if (front < 16 && right > 19) {
    right = 20; front = 15;
    if (!stopFlag) { stopRobot(); stopFlag = true; }
    turnLeft();
  }
  forward();
  /*
  Если не работает:
  1) Значения неточны и требуют каллибровки
  2) Заменить if на while
  3) Я напутал с логикой флажка
  4) ...
  
  */
}

void loop() {
  debugSensors();
  mainMaze();
  stopFlag = false;
}
