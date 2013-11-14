// Let's boil some water!

class Task {
public:
  virtual void run() = 0;
};

class Boil :  
public Task {
public:
  virtual void run();
};

void Boil::run() {
  Serial.println("Boiling");
}

class Scheduler {
public:
  Scheduler();
  void in(unsigned long time, Task* task);
  inline void tick();
private:
  unsigned long _last;
  Task* _tasks[];
};

Scheduler::Scheduler() {
  tick();
}

void Scheduler::tick() {
  _last = millis();
}

void Scheduler::in(unsigned long time, Task* task) {
  Serial.println("Sheduled");
}

const int THERMOMETER   = A0,
POWER_BUTTON  = 2,
POWER_LED     = 3,
POWER_POT     = 4;

int       power_button_down    = LOW,
          power                = LOW,
          temp                 = 0;

float     t_v, t_c, t_f;

Scheduler schedule;

void setup() {
  //schedule = Scheduler();
  //Task* boil = new Boil();
  //schedule.in(500, boil);


  pinMode(POWER_BUTTON, INPUT);
  pinMode(POWER_LED,    OUTPUT);
  pinMode(POWER_POT,    OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // schedule.tick();
  
  int power_button = digitalRead(POWER_BUTTON);
  if(power_button == HIGH && power_button_down == LOW) {
    power_button_down = HIGH;
  }
  if(power_button == LOW && power_button_down == HIGH) {
    power_button_down = LOW;
    if(power == LOW) {
      power = HIGH;
    } 
    else {
      power = LOW;
    }
  }
  digitalWrite(POWER_LED, power);
  digitalWrite(POWER_POT, power);

  temp = analogRead(THERMOMETER);
  Serial.print("temp: ");
  Serial.print(temp);
  
  t_v  = (temp / 1024.0) * 5.0;
  Serial.print(" t_v: ");
  Serial.print(t_v);
  
  t_c = 0.3611 + (49.3436 * t_v) + (-5.5215 * pow(t_v, 2));
  Serial.print(" t_c: ");
  Serial.print(t_c);
  
  t_f = (t_c * 9.0 / 5.0) + 32.0;
  Serial.print(" t_f: ");
  Serial.println(t_f);

  delay(1000);
}

