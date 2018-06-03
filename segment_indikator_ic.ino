#include "TM1637.h" // подключение библиотеки

#define CLK 3 
#define DIO 2 
#define Pulse 2 // Выход для сигнализации 
int btnPlus = 4;
int btnMin = 5;
int zummerPin = 7;

int timer_val=10; // Обратный отсчет в минутах 
int timer_seconds=00; // Отображение секунд 
// Переменные для хранения индивидуальных номеров 
int firstnum=0; 
int secondnum=0; 
int thirdnum=0; 
int fournum=0; 
TM1637 tm1637(CLK,DIO); // Настройка индикатора

void setup(){ 
  Serial.begin(9600);
   pinMode(Pulse, OUTPUT);
   pinMode(zummerPin, OUTPUT); 
   digitalWrite(Pulse, LOW); 
  
   tm1637.init(); // Сброс дисплея 
   tm1637.set(BRIGHT_TYPICAL); // Уровень яркости 
   // BRIGHT_DARKEST = 0,BRIGHTEST = 7 BRIGHT_TYPICAL = 2&#894;
   tm1637.point(POINT_ON); // Активация точки посредине
   
   delay(1000); // Задержка 1 секунда
    
} 
int i = 0;
int eventTime = 1;
void loop() {

      
      
   if(digitalRead(btnPlus))         // Проверка нажатия зеленой кнопки и не нажатия красной кнопки.
    {                                                                 //
      while(digitalRead(btnPlus))    // Цикл слежения нажатой зеленой кнопки.
      {                                                               //
        delay(10);                                                   // Ждём пока мы её не отпустим.
        
        //if(i<50 && digitalRead(pinKeyRed)){act = 2;eventTime=0;}      // Проверка нажатия красной кнопки. Если нажаты обе, то переходим к дейстивию обратного отсчета.
        if(i<150) {
          i++;
        } else {
          delay(100);
          timer_val++;
          doPrepareTime();
          doDisplayTimer();
          //ShowTime();
        }           // Фиксируем, как долго удерживается кнопка pinKeyGreen, если дольше 2 секунд, то увеличиваем минуты, до отжатия кнопки.
      }                                                               //
      if(eventTime)                                                   // Проверяем переменную события.
      {                                                               //
        if(i<150){
          timer_val++;
          doPrepareTime();
          doDisplayTimer();
        }                                 // Если кнопка pinKeyGreen удерживалась меньше 2 секунд, то увеличиваем минуты.
      }                                                               //
      i = 0;                                                            // Обнуляем переменную счета.
    }

    if(digitalRead(btnMin))         // Проверка нажатия зеленой кнопки и не нажатия красной кнопки.
    {                                                                 //
      i=0;
      while(digitalRead(btnMin))    // Цикл слежения нажатой зеленой кнопки.
      {                                                               //
        delay(10);                                                   // Ждём пока мы её не отпустим.
       
        if(i<150) {
          i++;
        } else {
          delay(100);
          timer_val--;
          doPrepareTime();
          doDisplayTimer();
        }           // Фиксируем, как долго удерживается кнопка pinKeyGreen, если дольше 2 секунд, то увеличиваем минуты, до отжатия кнопки.
      }                                                               //
      if(eventTime)                                                   // Проверяем переменную события.
      {                                                               //
        if(i<150){
          timer_val--;
          doPrepareTime();
          doDisplayTimer();
        }                                 // Если кнопка pinKeyGreen удерживалась меньше 2 секунд, то увеличиваем минуты.
      }                                                               //
      i = 0;                                                            // Обнуляем переменную счета.
    }
    
   doDisplayEndTimer();

   doPrepareTime();

   doDisplayTimer();
}

int zummerCount = 3;
int count = 0;
void doDisplayEndTimer()
{

  
   // Проверка, запустился ли таймер 
   while (timer_val == 0 && timer_seconds == 0) { 
      tm1637.clearDisplay(); // Очистка дисплея 
      tm1637.display(0,0); 
      tm1637.display(1,0); 
      tm1637.display(2,0); 
      tm1637.display(3,0); 

      if(count < zummerCount) {
        tone (zummerPin, 500); //включаем на 500 Гц
        delay(100); //ждем 100 Мс
        tone(zummerPin, 1000); //включаем на 1000 Гц
        delay(100); //ждем 100 Мсs
        noTone(zummerPin);
      }
      count++;
      
      digitalWrite(Pulse, HIGH); // Активация сигнализации 
      delay(1000);  // Wait
      tm1637.clearDisplay(); 
      digitalWrite(Pulse, LOW); // Отключение сигнализации
      delay(500); // Ждем полсекунды
      
     if(digitalRead(btnPlus) && digitalRead(btnMin)) {
        noTone(zummerPin);
        timer_val = 10;
        doPrepareTime();
        doDisplayTimer();
        count = 0;
        break;
     }  
   } 
}
long prevmicros = 0;
boolean counter=false;   
void doDisplayTimer()
{
   // Отображаем обратный отсчет
    if((millis()-prevmicros) > 1000) {
       prevmicros = millis();
       Serial.print(prevmicros);
       Serial.print("\n");
       timer_seconds--;
       //delay(1000);

        if (timer_seconds == -1) { 
          timer_val=timer_val-1; 
          timer_seconds=59; 
       } 

       tm1637.clearDisplay(); // Очистка дисплея
       
       if (timer_val > 9) { 
          tm1637.display(0,firstnum); 
       } 
       if (timer_val > 0) { 
          tm1637.display(1,secondnum); 
       } 
       if (timer_seconds > 9 || timer_val > 0) { 
          tm1637.display(2,thirdnum); 
       } 
       tm1637.display(3,fournum); 
     }
}

void doPrepareTime()
{
   // Распределяем минуты и сикунды по отдельным сегментам 
   if (timer_val > 9) { 
      firstnum = timer_val/10%10; 
      secondnum = timer_val%10; 
   } 
   else { 
      secondnum = timer_val; 
   } 
   if (timer_seconds > 9) { 
      thirdnum = timer_seconds/10%10; 
      fournum = timer_seconds%10; 
   } 
   else { 
      thirdnum = 0; 
      fournum = timer_seconds; 
   } 
}
