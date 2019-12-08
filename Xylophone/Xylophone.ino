#define LED 13

const int PIEZO_PINS[] = {A1, A2, A3, A4, A5};

unsigned long debouncers[5] = {0U, 0U, 0U, 0U, 0U};

char piezos = 0;

void setup() 
{
  // Serial monitor
  Serial.begin(9600);

  // Wait for the serial port to start (max 3 seconds)
  while(!Serial && (millis() < 3000)) {}
  
  // Initialize the LED pin
  pinMode(LED, OUTPUT);

  // Setup the SPI device
  SPCR |= _BV(SPE);
  pinMode(SS, INPUT);

  Serial.println("Silent Orchestra - Xylophone");
  Serial.print("SS Pin "); Serial.println((int)SS);
  Serial.print("MISO Pin "); Serial.println((int)MOSI);
  pinMode(MISO, OUTPUT);
}

void loop() 
{
  bool bUpdate = false;
  
  digitalWrite(LED, (millis() % 1000) < 500);

  for (char i = 0; i < 5; i++)
  {
    int piezoADC = analogRead(PIEZO_PINS[i]);
    float piezoVoltage = piezoADC / 1023.0 * 5.0;

    unsigned long time_since_last_hit = millis() - debouncers[i];

    // Check if the piezo has been hit
    if (piezoVoltage > 0.9)
    {
      // Only allow the button to trigger every 100ms at most
      if (time_since_last_hit > 100U)
      {
        debouncers[i] = millis();
        piezos |= (1 << i);
        bUpdate = true;
      }
    }

    // After 100ms, clear the button state
    if (piezos && (1 << i))
    {
      if (time_since_last_hit > 100U)
      {
        piezos &= ~(1 << i);
        bUpdate = true;
      }
    }
  }
  
  SPDR = piezos;

  if (bUpdate)
  {
    Serial.print("Piezos: "); Serial.println((int)piezos);
  }
}
