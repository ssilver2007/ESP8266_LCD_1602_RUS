//Пример для подключения двух дисплеев по шине I2C
//
#include <ESP8266_LCD_1602_RUS_ALL.h>

LCD_1602_RUS lcd1(0x3B, 16, 2);
LCD_1602_RUS lcd2(0x3F, 16, 2);

void setup()
{
  String str;
  str = "система";

  lcd1.init(); // Инициализация LCD №1
  lcd2.init(); // Инициализация LCD №2
  // Печать сообщения на LCD №1
  lcd1.backlight();
  lcd1.setCursor(3, 0);
  lcd1.print("Десятичная");
  lcd1.setCursor(4, 1);
  lcd1.print(str);

  // Печать сообщения на LCD №2
  lcd2.backlight();
  lcd2.setCursor(2, 0);
  lcd2.print("Русский язык");
  lcd2.setCursor(5, 1);
  lcd2.print("супер");
}

void loop()
{
}
