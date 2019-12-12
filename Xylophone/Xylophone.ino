#define LED 13

static uint16_t debounce[5] = {0U, 0U, 0U, 0U, 0U};
static bool key_hit[5] = {false, false, false, false, false};

static const uint16_t DEBOUNCE_RELOAD = 250UL;

static char keys = 0;
static bool update = false;

void setup() 
{
  // Serial monitor
  Serial.begin(9600);

  // Wait for the serial port to start (max 3 seconds)
  while(!Serial && (millis() < 3000)) {}
  
  // Initialize pins and interrupts  
  pinMode(0,INPUT_PULLUP);
  pinMode(1,INPUT_PULLUP);
  pinMode(2,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
  pinMode(7,INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(0), key0ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(1), key1ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(2), key2ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), key3ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(7), key4ISR, FALLING);

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
  static unsigned long last_millis = 0UL;

  // Flash LED every second
  digitalWrite(LED, (millis() % 1000) < 500);

  if (last_millis != millis())
  {
    last_millis = millis();

    for (uint8_t i = 0; i<5; i++)
    {
      if (key_hit[i])
      {
        key_hit[i] = false;
        Serial.println((int)i);
        keys |= (1 << i);
        update = true;
      }

      if (debounce[i])
      {
        debounce[i]--;
        if (debounce[i] == 0)
        {
          keys &= ~(1 << i);
          update = true;
        }
      }
    }
  }
  
  SPDR = keys;

  if (update)
  {
    Serial.print("Piezos: "); Serial.println((int)keys);
    update = false;
  }
}

void key0ISR()
{
  if (debounce[0] == 0U)
  {
    key_hit[0] = true;
    debounce[0] = DEBOUNCE_RELOAD;
  }
}

void key1ISR()
{
  if (debounce[1] == 0U)
  {
    key_hit[1] = true;
    debounce[1] = DEBOUNCE_RELOAD;
  }
}

void key2ISR()
{
  if (debounce[2] == 0U)
  {
    key_hit[2] = true;
    debounce[2] = DEBOUNCE_RELOAD;
  }
}

void key3ISR()
{
  if (debounce[3] == 0U)
  {
    key_hit[3] = true;
    debounce[3] = DEBOUNCE_RELOAD;
  }
}

void key4ISR()
{
  if (debounce[4] == 0U)
  {
    key_hit[4] = true;
    debounce[4] = DEBOUNCE_RELOAD;
  }
}

