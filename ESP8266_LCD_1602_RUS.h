#ifndef _ESP8266_LCD_1602_RUS_h
#define _ESP8266_LCD_1602_RUS_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

//#include <avr/pgmspace.h>
#include <LiquidCrystal_I2C.h>
#include <Print.h>
#include <Wire.h>
#include "font_LCD_1602_RUS.h"

#define BYTE 0
typedef uint8_t* _uint_farptr_t;
#ifndef memcpy_PF
#define memcpy_PF(dest, src, len) memcpy((dest), (src), (len))
#endif

//Описание класса набора изменяемых символов
class Symbol {
  public:
    Symbol (uint16_t _code)//Конструктор с начальной инициализацией класса по коду символа в unicode
    {
      index = 255;//Начальная инициализация (неинициализированы = 255)
      code = _code;
    }

    _uint_farptr_t getBitmap()//указатель на растровое изображение символа в дальней памяти. Для работы с таким указателем необходимо использовать функцию memcpy_PF(...)
    {
      uint16_t _tmp_unicode;

      for (uint8_t i = 0; i < ((sizeof(symbol_image)) / (sizeof(symbol_image[0]))); i++)
      {
        memcpy_PF(&_tmp_unicode, (_uint_farptr_t)&symbol_image[i]._unicode, sizeof(_tmp_unicode));//Загрузка значения из дальней памяти
        if (_tmp_unicode == code)
          return (_uint_farptr_t)symbol_image[i]._rastr;
      }
      return (_uint_farptr_t)0xFF;//Возвращает 255 при отсутствии совпадения (ошибка)
    }

    uint8_t index;
    uint16_t code;//Код символа в unicode
};

//Класс LCD_1602_RUS
//user_custom_symbols - количество символов, доступное пользователю для использования в своих целях (не более 8)
//Всего переопределяемых символов в LCD - 8.
//Символы с индексом от 0 до (7 - user_custom_symbols) используются библиотекой
//Символы с индексом от (8 - user_custom_symbols) до 7 - можно переопределять пользователю
//По умолчанию количество переопределяемых символов равно 0
class LCD_1602_RUS : public LiquidCrystal_I2C {
  public:
    LCD_1602_RUS (uint8_t lcd_Addr, uint8_t lcd_cols, uint8_t lcd_rows, uint8_t user_custom_symbols = 0) : LiquidCrystal_I2C (lcd_Addr, lcd_cols, lcd_rows)//Конструктор для подключения I2C
    {
      Init(user_custom_symbols);
    }

    //Для ESP8266
#ifdef FDB_LIQUID_CRYSTAL_I2C_H
#if defined (ESP8266)
    void init(uint8_t _sda = SDA, uint8_t _scl = SCL)
    {
      Wire.pins(_sda, _scl);
      begin();
    }
#else
    void init()
    {
      begin();
    }
#endif
#endif

    void print(const wchar_t* _str)
    {
      int current_char  = 0;
      int size = 0;

      //Определяем длину строки (количество символов)
      while (_str[size] != 0)
      {
        size++;
      }

      while (current_char < size)
      {
        printwc(_str[current_char]);
        current_char++;
        cursor_col++;
      }
    }
    void print(const char *c_str) {
      wchar_t _str;
      int current_char = 0;
      int size = strlen(c_str);

      while (current_char < size)
      {
        current_char += mbtowc(&_str, (char *)c_str + current_char, 2);
        printwc(_str);
        cursor_col++;
      }
    }
    void print(const String & str) {
      wchar_t _str;
      const char *c_str = str.c_str();
      int current_char = 0;
      int size = str.length();

      while (current_char < size)
      {
        current_char += mbtowc(&_str, (char*)c_str + current_char, 2);
        printwc(_str);
        cursor_col++;
      }
    }

    template <typename T> void print(T val, int LiquidCrystal_I2C)
    {
      cursor_col += LiquidCrystal_I2C::print(val, LiquidCrystal_I2C);
    }

    void clear()
    {
      LiquidCrystal_I2C::clear();
      ResetAllIndex();
    }

    void setCursor(uint8_t col, uint8_t row)
    {
      cursor_col = col;
      cursor_row = row;
      LiquidCrystal_I2C::setCursor(cursor_col, cursor_row);
    }
    uint8_t getCursorCol()
    {
      return cursor_col;
    }
    uint8_t getCursorRow()
    {
      return cursor_row;
    }

    //Перевод символа из кодировки ASCII в Win1251 (для печати расширенных русских символов на LCD)
    wchar_t *ascii_win1251(unsigned char ascii)
    {
      if (ascii == 168) *char_utf8 = 0x401; //код ASCII буквы Ё (0xA8)
      else if (ascii == 184) *char_utf8 = 0x451; //код ASCII буквы ё (0xB8)
      else if (ascii == 165) *char_utf8 = 0x490; //код Ukr. ASCII буквы Г (0xA5)
      else if (ascii == 180) *char_utf8 = 0x491; //код Ukr. ASCII буквы г (0xB4)
      else if (ascii == 170) *char_utf8 = 0x404; //код Ukr. ASCII буквы Е (0xAA)
      else if (ascii == 186) *char_utf8 = 0x454; //код Ukr. ASCII буквы е (0xBA)
      else if (ascii == 175) *char_utf8 = 0x407; //код Ukr. ASCII буквы I (0xAF)
      else if (ascii == 191) *char_utf8 = 0x457; //код Ukr. ASCII буквы i (0xBF)
      else if (ascii >= 192) //остальные буквы русского алфавита
      {
        *char_utf8 = ascii + 848;
      }
      else *char_utf8 = ascii;

      return char_utf8;
    }
    //Перевод символа из кодировки ASCII в UTF-8 (для печати расширенных русских символов на LCD)
    //*ascii - указатель на массив из двух байт
    wchar_t *ascii_utf8(unsigned char *ascii)
    {
      if (ascii[0] > 0x7F)
      { //Кириллица
        if ((ascii[0] == 0xD0) && (ascii[1] == 0x81)) *char_utf8 = 0x401; //код ASCII буквы Ё (0xD081)
        else if ((ascii[0] == 0xD1) && (ascii[1] == 0x91)) *char_utf8 = 0x451; //код ASCII буквы ё (0xD191)
        else if ((ascii[0] == 0xD2) && (ascii[1] == 0x90)) *char_utf8 = 0x490; //код Ukr. ASCII буквы Г (0xD290)
        else if ((ascii[0] == 0xD2) && (ascii[1] == 0x91)) *char_utf8 = 0x491; //код Ukr. ASCII буквы г (0xD291)
        else if ((ascii[0] == 0xD0) && (ascii[1] == 0x84)) *char_utf8 = 0x404; //код Ukr. ASCII буквы Е (0xD084)
        else if ((ascii[0] == 0xD1) && (ascii[1] == 0x94)) *char_utf8 = 0x454; //код Ukr. ASCII буквы е (0xD194)
        else if ((ascii[0] == 0xD0) && (ascii[1] == 0x87)) *char_utf8 = 0x407; //код Ukr. ASCII буквы I (0xD087)
        else if ((ascii[0] == 0xD1) && (ascii[1] == 0x97)) *char_utf8 = 0x457; //код Ukr. ASCII буквы i (0xD087)
        else if ((ascii[0] == 0xD0) && (ascii[1] >= 0x90) && (ascii[1] <= 0xBF)) //остальные буквы русского алфавита (А...Я а...п)
        {
          *char_utf8 = ascii[1] + 896;
        }
        else if ((ascii[0] == 0xD1) && (ascii[1] >= 0x80) && (ascii[1] <= 0x8F)) //остальные буквы русского алфавита (р...я)
        {
          *char_utf8 = ascii[1] + 960;
        }
      }
      else
      { //Латиница
        *char_utf8 = ascii[0];
      }
      return char_utf8;
    }

    void ResetAllIndex()
    {
      symbol_index = 0;
      for (uint8_t i = 0; i < ((sizeof(font)) / (sizeof(font[0]))); i++)
        font[i].index = 255;
    }

  private:
    void Init(uint8_t _user_custom_symbols)//Инициализация конструктора
    {
      max_symbol_count = 8 - _user_custom_symbols;
      cursor_col = 0;
      cursor_row = 0;
      ResetAllIndex();//Сброс значений индексов (неинициализированы = 255)
    }

    uint8_t getIndex(uint16_t unicode)//Функция возвращает индекс символа в массиве font по передаваемому в функцию коду unicode
    //unicode - код символа в unicode
    {
      for (uint8_t i = 0; i < ((sizeof(font)) / (sizeof(font[0]))); i++)
      {
        if (unicode == font[i].code)
          return i;
      }
      return 0xFF;//Возвращает 255 при отсутствии совпадения (ошибка)
    }

    void CharSetToLCD(uint8_t *array, uint8_t *index)//Загрузка растрового изображения символа в LCD
    {
      uint8_t x, y;

      if (*index == 255) // Если символ еще не создан, то создаем
      {
        x = getCursorCol();
        y = getCursorRow();
        LiquidCrystal_I2C::createChar(symbol_index, (uint8_t *)array);// Создаем символ на текущем (по очереди) месте в знакогенераторе (от 0 до MAX_SYMBOL_COUNT)
        setCursor(x, y);
        LiquidCrystal_I2C::write(symbol_index);// Выводим символ на экран
        //Запомианем, что букве соответствует определенный индекс
        *index = symbol_index;
        symbol_index++;
        if (symbol_index > max_symbol_count)
        {
          ResetAllIndex();
        }
      }
      else   //Иначе печатаем уже существующий
        LiquidCrystal_I2C::write(*index);
    }

    void printwc(const wchar_t _chr) {
      uint8_t rus_[8];

      if (_chr < 128) //Английский алфавит без изменения
        LiquidCrystal_I2C::print((char)_chr);
      else
        //Кириллица
        //Единовременно может быть заменено только 8 символов
        switch (_chr)
        {
          //Русский алфавит, использующий одинаковые с английским алфавитом символы
          case 1040: //А
            LiquidCrystal_I2C::print("A");
            break;
          case 1042: //В
            LiquidCrystal_I2C::print("B");
            break;
          case 1045: //Е
            LiquidCrystal_I2C::print("E");
            break;
          case 1025: //Ё
            LiquidCrystal_I2C::print("E");
            break;
          case 1050: //К
            LiquidCrystal_I2C::print("K");
            break;
          case 1052: //M
            LiquidCrystal_I2C::print("M");
            break;
          case 1053: //H
            LiquidCrystal_I2C::print("H");
            break;
          case 1054: //O
            LiquidCrystal_I2C::print("O");
            break;
          case 1056: //P
            LiquidCrystal_I2C::print("P");
            break;
          case 1057: //C
            LiquidCrystal_I2C::print("C");
            break;
          case 1058: //T
            LiquidCrystal_I2C::print("T");
            break;
          case 1061: //X
            LiquidCrystal_I2C::print("X");
            break;
          case 1072: //а
            LiquidCrystal_I2C::print("a");
            break;
          case 1077: //е
            LiquidCrystal_I2C::print("e");
            break;
          case 1086: //o
            LiquidCrystal_I2C::print("o");
            break;
          case 1088: //p
            LiquidCrystal_I2C::print("p");
            break;
          case 1089: //c
            LiquidCrystal_I2C::print("c");
            break;
          case 1091: //y
            LiquidCrystal_I2C::print("y");
            break;
          case 1093: //x
            LiquidCrystal_I2C::print("x");
            break;
          case 0x00B0: //Знак градуса
            LiquidCrystal_I2C::write(223);
            break;
          default:
            //Русский алфавит, требующий новых символов
            memcpy_PF(rus_, (_uint_farptr_t)font[getIndex(_chr)].getBitmap(), 8);
            CharSetToLCD((uint8_t *)rus_, &font[getIndex(_chr)].index);
            break;
        }
    }

    uint8_t mbtowc(wchar_t *_chr, char *_str, uint8_t mb_num)
    {
      if (mb_num != 2) return 0;
      if ((_str[0] & 0xC0) == 0xC0 && (_str[1] & 0x80) == 0x80) {
        *_chr = ((_str[0] & 0x1F) << 6) + (_str[1] & 0x3F);
        return 2;
      }
      else {
        *_chr = _str[0];
        return 1;
      }
    }

    uint8_t max_symbol_count;  //Максимальное количество переназначаемых символов (по умолчанию 8: от 0 до 7)
    int symbol_index;//Индекс символа (от 0 до MAX_SYMBOL_COUNT)
    uint8_t cursor_col;
    uint8_t cursor_row;
    wchar_t char_utf8[2] = {0};
    Symbol font[53] = { //Переменная font - набор изменияемых символов
      1041, //Б
      1043, //Г
      1044, //Д
      1046, //Ж
      1047, //З
      1048, //И

      1049, //Й
      1051, //Л
      1055, //П
      1059, //У
      1060, //Ф

      1062, //Ц
      1063, //Ч
      1064, //Ш
      1065, //Щ

      1066, //Ъ
      1067, //Ы
      1068, //Ь
      1069, //Э
      1070, //Ю
      1071, //Я

      1073, //б
      1074, //в
      1075, //г
      1076, //д
      1105, //ё
      1078, //ж

      1079, //з
      1080, //и
      1081, //й
      1082, //к
      1083, //л

      1084, //м
      1085, //н
      1087, //п
      1090, //т

      1092, //ф
      1094, //ц
      1095, //ч
      1096, //ш
      1097, //щ
      1098, //ъ

      1099, //ы
      1100, //ь
      1101, //э
      1102, //ю
      1103, //я

      0x490, //Укр. Г
      0x491, //укр. г
      0x404, //укр Е
      0x454, //укр е
      0x407, //укр I
      0x457 //укр i
    };
};

#endif
