#include "TM1637.h" // подключение библиотеки

#define CLK 3 
#define DIO 2
#define BTN_PLUS_PIN 4
#define BTN_MINUS_PIN 5
#define ZUMMER_PIN 7

// 10 minutes by start
int timer_seconds=60*10; // Отображение секунд 
unsigned long prevmicros = 0;

TM1637 tm1637(CLK, DIO); // Настройка индикатора

void setup()
{ 
    Serial.begin(9600);
    
    pinMode(ZUMMER_PIN, OUTPUT);
    digitalWrite(ZUMMER_PIN, LOW);
    tm1637.init(); // Сброс дисплея
    tm1637.set(BRIGHT_TYPICAL); // Уровень яркости 
    // BRIGHT_DARKEST = 0,BRIGHTEST = 7 BRIGHT_TYPICAL = 2&#894;
    tm1637.point(POINT_ON); // Активация точки посредине
    delay(1000); // Задержка 1 секунда
    
}
void loop()
{
    if((millis()-prevmicros) > 1000) {
        prevmicros = millis();

        timer_seconds--;
        onDisplayTimer();        
    }
    
    
}



int minutes, seconds;
int firstnum, secondnum, thirdnum, fournum;
/**
 * @used timer_seconds
 */
void onDisplayTimer()
{ 
    minutes = timer_seconds / 60;
    
    if (minutes > 9) {
        firstnum = minutes/10%10;
        secondnum = minutes%10; 
    } else {
        firstnum = 0;
        secondnum = minutes; 
    }
    
    seconds = timer_seconds % 60;
    
    if (seconds > 9) {
        thirdnum = timer_seconds/10%10;
        fournum = timer_seconds%10;
    } else { 
        thirdnum = 0;
        fournum = timer_seconds;
    }
    
    tm1637.clearDisplay(); // Очистка дисплея
    
    tm1637.display(0,firstnum);
    
    tm1637.display(1,secondnum);
    
    tm1637.display(2,thirdnum);
    
    tm1637.display(3,fournum);
    
    if (minutes > 9) { 
        Serial.print(firstnum); 
    } 
    if (minutes > 0) { 
        Serial.print(secondnum); 
    } 
    if (seconds > 9 || minutes > 0) {
        Serial.print(thirdnum); 
    }
    
    Serial.print(fournum);
    
    Serial.print("\n");
    
}
