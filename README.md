# ESP8266_LCD_1602_RUS
ESP8266-совместимая библиотека поддержки кириллицы для дисплеев на базе контроллера HD44780 без встроенной кириллицы с подключением по интерфейсу I2C.

Максимально возможно отображение 8 уникальных по начертанию русских символа (например Ж, Д, И, Ю и т.п.) Символы, одинаковые по начертанию с английскими (A, B, C, O, P и т.п.) используются из английского набора символов. Дополнительно встроена возможность печати знака градуса Цельсия. Для этого в тексте программы необходимо набрать код UTF-8 (Alt+0176).

Добавлена поддержка букв украинского языка.

При использовании подключения по I2C должна быть установлена базовая библиотека LiquidCrystal_I2C. Например, может быть рекомендована эта: 
https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library

Файлы примеров:

HelloWorld - простой пример вывода кириллических символов на LCD экран.

HelloWorld_I2C_2Displays - пример вывода кириллических символов на два (или более) LCD экрана, подключенных по шине I2C.

HelloWorldCustom - пример вывода задаваемых пользователем и кириллических символов на LCD экран.

SerialToLCD - Ввод символов (в т.ч. кириллических) с монитора порта и их вывод на LCD экран.

(c) Сергей Сироткин

По вопросам обращаться: ssilver2007@rambler.ru
