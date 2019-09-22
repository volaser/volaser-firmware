#include "SerialCommands.h"
#include "TFMini.h"

//Mode to be used ("0" = 0-2 meters, "3" = 0.5-5 meters, "7" = 1-12 meters)
#define fixMode 3
#define Laser_RX 16
#define Laser_TX 17

TFMini laser;

void blink()
{
  for (int i = 0; i < 1; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
  }
}

char serial_commands_buffer[32];
SerialCommands serial_commands(&Serial, serial_commands_buffer, sizeof(serial_commands_buffer));

void get_distance(SerialCommands *sender)
{
  uint16_t distance = laser.getDistance();
  sender->GetSerial()->println(distance);
}

void get_strength(SerialCommands *sender)
{
  uint16_t strength = laser.getRecentSignalStrength();
  sender->GetSerial()->println(strength);
}

void get_mode(SerialCommands *sender)
{
  uint8_t mode = laser.getMode();
  sender->GetSerial()->println(mode);
}

SerialCommand cmd_D_("D", get_distance);
SerialCommand cmd_S_("S", get_strength);
SerialCommand cmd_M_("M", get_mode);

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting Volaser");
  serial_commands.AddCommand(&cmd_D_);
  serial_commands.AddCommand(&cmd_S_);
  serial_commands.AddCommand(&cmd_M_);

  Serial1.begin(TFMINI_BAUDRATE, SERIAL_8N1, Laser_RX, Laser_TX);
  laser.begin(&Serial1);
  delay(100);
  laser.setMeasurementMode(MIDDLE);
  laser.setSignalThreshold(20, 956);

  pinMode(LED_BUILTIN, OUTPUT);
  blink();
}

void loop()
{
  while (Serial1.available())
  {
    Serial1.read();
  }
  serial_commands.ReadSerial();
}