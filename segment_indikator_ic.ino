#include "TM1637.h" // подключение библиотеки
#include <Bounce2.h>

#define CLK 9 
#define DIO 8
#define BTN_PLUS_PIN 4
#define BTN_MINUS_PIN 5
#define ZUMMER_PIN 7
#define TIMEOUT_LONG_PRESS 200
#define LONG_PRESS_ACTIVATE 2000

#define DEFAULT_TIME 120 // seconds
// 10 minutes by start
int timer_seconds=DEFAULT_TIME; // Отображение секунд 
unsigned long prevmicros = 0;

TM1637 tm1637(CLK, DIO); // Настройка индикатора

Bounce btnPlus = Bounce();
Bounce btnMinus = Bounce();
void setup()
{ 
    Serial.begin(9600);
    
    pinMode(ZUMMER_PIN, OUTPUT);
    digitalWrite(ZUMMER_PIN, LOW);
    tm1637.init(); // Сброс дисплея
    tm1637.set(BRIGHT_TYPICAL); // Уровень яркости 
    // BRIGHT_DARKEST = 0,BRIGHTEST = 7 BRIGHT_TYPICAL = 2&#894;
    tm1637.point(POINT_ON); // Активация точки посредине

    // Теперь настраиваем экземпляр класса для той кнопки:
    btnPlus.attach(BTN_PLUS_PIN);
    btnPlus.interval(50); // interval in ms

    btnMinus.attach(BTN_MINUS_PIN);
    btnMinus.interval(50);
    delay(1000); // Задержка 1 секунда
}


bool is_work = false;
bool is_long_plus = false;
bool is_long_minus = false;
unsigned long prevmicrosTwoBtn = 0;
unsigned long prevmicrosPlusBtn = 0;
unsigned long prevmicrosPlusBtnMin = 0;

unsigned long prevmicrosMiunsBtn = 0;
unsigned long prevmicrosMinusBtnMin = 0;
void loop()
{
    btnPlus.update();
    btnMinus.update();
    
    if (btnPlus.read() && btnMinus.read()) {
        if((millis()-prevmicrosTwoBtn) > 800) {
          prevmicrosTwoBtn = millis();
          is_work = !is_work;
          tone(ZUMMER_PIN, 500, 200);
          return true;
        }
    }

    doLongPlus(btnPlus, btnMinus);

    doLongMinus(btnPlus, btnMinus);

    doClickPlus(btnPlus);
    doClickMinus(btnMinus);

    doDisplayEndTimer();
    if((millis()-prevmicros) > 1000) {
      prevmicros = millis();
      onDisplayTimer();
    }
}

void doClickPlus(Bounce btnPlus)
{
  int value = btnPlus.fell();
  if (value) {
      if (!is_long_plus) {
        Serial.println("+1min");
        timer_seconds = timer_seconds+60;
        onDisplayTimer();
      }
      is_long_plus = false;
   }

   return true;
}

void doClickMinus(Bounce btnMinus)
{
   int value = btnMinus.fell();
  
   if (value) {
      if (!is_long_minus) {
         Serial.println("-1min");
         timer_seconds = timer_seconds-60;
         onDisplayTimer();  
      }
      is_long_minus = false;
   }

   return true;
}

void doLongPlus(Bounce btnPlus, Bounce btnMinus)
{
   if (btnPlus.read() && !btnMinus.read() && (millis()-prevmicrosPlusBtn) > LONG_PRESS_ACTIVATE) {
          prevmicrosPlusBtn = millis();
          Serial.println("2 seconds");
          is_long_plus = true;
    }

    if(is_long_plus && (millis()-prevmicrosPlusBtnMin) > TIMEOUT_LONG_PRESS) {
        prevmicrosPlusBtnMin = millis();
        timer_seconds = timer_seconds+60;
        onDisplayTimer();
    }

    return true;
}

void doLongMinus(Bounce btnPlus, Bounce btnMinus)
{
   // long minus
    if (!btnPlus.read() && btnMinus.read() && (millis()-prevmicrosMiunsBtn) > LONG_PRESS_ACTIVATE) {
          prevmicrosMiunsBtn = millis();
          is_long_minus = true;
    }

    if(is_long_minus && (millis()-prevmicrosMinusBtnMin) > TIMEOUT_LONG_PRESS) {
        prevmicrosMinusBtnMin = millis();
        timer_seconds = timer_seconds-60;
        onDisplayTimer();
    }

    return true;
}

int minutes, seconds;
int firstnum, secondnum, thirdnum, fournum;
/**
 * @used timer_seconds
 */
void onDisplayTimer()
{ 
    int minutes, seconds;
    int firstnum, secondnum, thirdnum, fournum;
    
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
        thirdnum = seconds/10%10;
        fournum = seconds%10;
    } else { 
        thirdnum = 0;
        fournum = seconds;
    }
    //if((millis()-prevmicros) > 1000) {
   //     prevmicros = millis();
        if (is_work) {
          timer_seconds = timer_seconds - 1;
        }
        
        tm1637.clearDisplay(); // Очистка дисплея
        if (minutes > 9) { 
             tm1637.display(0,firstnum); 
            // Serial.print(firstnum); 
        } 
        if (minutes > 0) { 
           tm1637.display(1,secondnum); 
         // Serial.print(secondnum); 
        } 
        if (seconds > 9 || minutes > 0) { 
          tm1637.display(2,thirdnum);
        //  Serial.print(thirdnum); 
        } 
        tm1637.display(3,fournum);
      //  Serial.print(fournum); 
      
      //  Serial.print("\n");
  //  } 
}

int zummerCount = 5;
int count = 0;
void doDisplayEndTimer()
{
   if((millis()-prevmicros) > 1000) {
   // Проверка, запустился ли таймер 
     while (timer_seconds <= 0) { 
        tm1637.clearDisplay(); // Очистка дисплея 
        tm1637.display(0,0); 
        tm1637.display(1,0); 
        tm1637.display(2,0); 
        tm1637.display(3,0); 
  
        if(count < zummerCount) {
          tone(ZUMMER_PIN, 1500, 300);
          delay(500);
          tone(ZUMMER_PIN, 1500, 300);
          delay(500);
          tone(ZUMMER_PIN, 1500, 300);
          delay(500); //ждем 100 Мсs
          noTone(ZUMMER_PIN);
        }
        count++;
           
       if(digitalRead(BTN_PLUS_PIN) && digitalRead(BTN_MINUS_PIN)) {
          noTone(ZUMMER_PIN);
          timer_seconds = DEFAULT_TIME;
          onDisplayTimer();
          count = 0;
          break;
       }  
     } 
   }
}
