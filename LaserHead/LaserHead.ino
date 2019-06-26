#include "SerialCommands.h"

char serial_commands_buffer[32];
SerialCommands serial_commands(&Serial, serial_commands_buffer, sizeof(serial_commands_buffer));

void get_laser(SerialCommands *sender)
{
  sender->GetSerial()->println("1.23");
}

SerialCommand cmd_L_("L", get_laser);

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting Volaser");
  serial_commands.AddCommand(&cmd_L_);
}

void loop()
{
  serial_commands.ReadSerial();
}