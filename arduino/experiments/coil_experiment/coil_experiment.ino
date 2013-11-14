// Boiler Experiment

// The question we're looking to explore is:
//
//   What impact does turning on the boiler for a variable period
//   of time have on the temperature of the water in the kettle?
//
//   Once the boiler has been turned on for some duration of time 
//   and then shut off, the water temperature continues to rise 
//   from the residual heat dissipating from the heating element.
//
//   Not trying to be super scientific here, just get a general sense
//   of the ranges we are working with.

// Need to filter temperature sensor data
#include <SignalFilter.h>

// Pins
const int THERMOMETER    = A0,
          HEAT_RELAY_LED = 3,
          HEAT_RELAY     = 4;
          
// States
const int PRIME_UP_PULSE = 0,
          PRIME_UP_WAIT  = 1,
          PRIME_DOWN     = 2,
          APPLY_HEAT     = 3,
          COAST          = 4,
          DONE           = 5;
          
// Test Parameters
const int PRIME_TARGET       = 516,
          APPLY_HEAT_TARGET  = 512,
          COAST_TARGET       = 508,
          EXPERIMENTS        = 10;

// Misc
const int WAIT_MS_TO_START         = 5000,
          SERIAL_BAUD              = 9600,
          SECONDS_TO_MILLISECONDS  = 1000,
          LONG_TIME                = 10000;

// Variables
int experiment     = 1,         // Seconds of Relay
    state          = PRIME_UP_PULSE,
    therm_voltage  = 0,
    heat_relay     = LOW;
    
unsigned long current_time          = 0,
              pulse_ends_at         = 0,
              experiment_started_at = 0,
              experiment_ends_at    = 0,
              next_print            = 0,
              delay_by              = 100;
              
SignalFilter filter;

// Let's walk through the stages of the experiment:

// We want to start every experiment at the same starting temp.
// So we'll "prime" up the water by heating it just past our 
// experimental starting point. Then we'll let it cool ("prime down"
// making this up as I go) to our starting temp.
void primeUpPulse() {
  if(pulse_ends_at == 0) {
    Serial.print("PRIMING ");
    Serial.println(therm_voltage);
    // delta = PRIME_TARGET - therm_voltage;
    pulse_ends_at = current_time + (5 * SECONDS_TO_MILLISECONDS);
  }
  if(pulse_ends_at >= current_time && therm_voltage <= PRIME_TARGET) {
    heat_relay     = HIGH;
  } else {
    heat_relay     = LOW;
    delay_by       = 15 * SECONDS_TO_MILLISECONDS;
    state          = PRIME_UP_WAIT;
    pulse_ends_at  = 0;
  }
}

void primeUpWait() {
  Serial.println("PRIME WAITING");
  heat_relay       = LOW;
  if(therm_voltage <= PRIME_TARGET) {
    state = PRIME_UP_PULSE;
    delay_by       = 5 * SECONDS_TO_MILLISECONDS;
  } else {
    Serial.print("PRIME DOWN ");
    Serial.println(therm_voltage);
    state = PRIME_DOWN;
  }
}

void primeDown() {
  if(therm_voltage >= APPLY_HEAT_TARGET) {
    heat_relay = LOW;
  } else {
    state = APPLY_HEAT;
  }
}

// We are now ready to start our experiment by turning on the heating
// coil. We need to remember when we started applying heat at the 
// start of every experiment so that we can go into coast mode the
// correct number of seconds after the experiment has started.
void applyHeat() {
  if(experiment_started_at == 0) {
    // Initialize times on the first call of applyHeat() in an experiment
    experiment_started_at  = current_time;
    experiment_ends_at     = current_time + (experiment * 2 * SECONDS_TO_MILLISECONDS);
    Serial.print("=== Starting Experiment ");
    Serial.print(experiment);
    Serial.print(" of ");
    Serial.print(EXPERIMENTS);
    Serial.println(" ===");
  }
  
  if(current_time < experiment_ends_at) {
    heat_relay = HIGH;
  } else {
    // give the pulse a little time to take effect before coasting
    if(current_time < (experiment_ends_at + (30 * SECONDS_TO_MILLISECONDS))) {
      heat_relay = LOW;
    } else {
      state = COAST;
      experiment_ends_at = 0;
    }
  }
}

void coast() {
  if(therm_voltage > COAST_TARGET) {
    heat_relay = LOW;
  } else {
    state = DONE;
  }
}

void done() {
  heat_relay = LOW;
  if(experiment < EXPERIMENTS) {
    Serial.print("=== Experiment ");
    Serial.print(experiment);
    Serial.print(" of ");
    Serial.print(EXPERIMENTS);
    Serial.println(" Completed ===");
    
    // Start priming for the next experiment!
    experiment_started_at = 0;
    experiment += 1;
    state = PRIME_UP_PULSE;
  } else {
    Serial.println("=== All Experiments Completed ===");
    delay(LONG_TIME);
  }
}

// Initialize Pins & Serial
void setup() {
  pinMode(HEAT_RELAY_LED, OUTPUT);
  pinMode(HEAT_RELAY,     OUTPUT);
  
  // Setup the Filter
  filter.begin();
  filter.setFilter('b');
  filter.setOrder(1);
  therm_voltage = analogRead(THERMOMETER);
  for(int i = 0; i < 100; i++) {
    filter.run(therm_voltage);
  }
  
  Serial.begin(SERIAL_BAUD);
  
  delay(WAIT_MS_TO_START);
  Serial.println("=== Coil Experiment Beginning ===");
}

// Run the Experiment Dispatch Loop
void loop() {
  
  // Read Input Data
  current_time  = millis();
  therm_voltage = filter.run(analogRead(THERMOMETER));
  delay_by      = 100;
  
  // Dispatch to the correct phase of the
  // experiment
  switch(state) {
    case PRIME_UP_PULSE:   primeUpPulse(); break;
    case PRIME_UP_WAIT:    primeUpWait();  break;
    case PRIME_DOWN:       primeDown();    break;
    case APPLY_HEAT:       applyHeat();    break;
    case COAST:            coast();        break;
    case DONE:             done();         break;
  }
  
  // Write Outputs
  digitalWrite(HEAT_RELAY,     heat_relay);
  digitalWrite(HEAT_RELAY_LED, heat_relay);
  
  // Print Results Every Second
  if(current_time > next_print) {
    next_print = current_time + (1 * SECONDS_TO_MILLISECONDS);
    if(state == APPLY_HEAT || state == COAST) {
      Serial.print(experiment);
      Serial.print(",");
      Serial.print(heat_relay);
      Serial.print(",");
      Serial.print(current_time - experiment_started_at);
      Serial.print(",");
      Serial.print(therm_voltage);
      Serial.print(",");
      Serial.println(analogRead(THERMOMETER));
    }
  }  
  
  delay(delay_by);
}
