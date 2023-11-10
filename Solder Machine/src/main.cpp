#include <Arduino.h>
#include <manager.h>

Manager M1;

void setup()
{
  Serial.begin(115200);
  M1.begin(Serial);
}

void loop()
{
  M1.display();
  M1.extrudeAllMotors();
}


/*

b = analogRead(data.btn1_2);
  Serial.println(b);

  if (b >= 712 && b <= 835)
  {
    A.extrudeM3DE(100, false);
  }

  else if (b >= 835)
  {
    A.extrudeM3DE(100, true);
    delay(10);
  }

*/