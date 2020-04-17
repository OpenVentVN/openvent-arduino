#include <avr/interrupt.h>
#include <Wire.h> 
#include "LiquidCrystal_I2C.h"
#include "Adafruit_BMP085.h"
#include <math.h> 
LiquidCrystal_I2C lcd(0x27,16,2);

// Button Pin
#define   TRUE          1
#define   FAIL          0
#define   ON            LOW  
#define   OF            HIGH
#define   L_ON          LOW 
#define   L_OF          HIGH 
#define   EN_ON         HIGH  
#define   EN_OF         LOW
#define   DIR_ON        LOW  
#define   DIR_OF        HIGH
#define   B_ON          HIGH  
#define   B_OF          LOW

#define   B_RED     8
#define   L_RED     9
#define   B_GREEN   10
#define   L_GREEN   11
#define   IN        12
#define   BUZ       13
#define   PWM       2
#define   DIR       3
#define   EN        4

unsigned char   g_Led = OF, g_time_led = 0;
unsigned char   g_start = OF, g_run_motor = OF;
long            g_Vt,    g_Ti, g_ti,   g_Te, g_te,   g_F;
long            g_Vt_p;
#define   MODE      4
#define   CMV       0
#define   CPAP      1
#define   VAC       2
#define   TEST      3
unsigned char   mode = 0;
long            g_active_VAC = 0, g_active_VAC_bk;

unsigned char g_but_green_1 = OF, g_but_green_2 = OF;
unsigned char g_but_red_1 = OF,   g_but_red_2 = OF;


#define NUM   5
long          g_arr[NUM], g_threshold_P_H2O;
Adafruit_BMP085 bmp;
/////////////////////////////////////////////////////////////////////////////////////////
// Function /////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
void delay_us(long us)
{
    us = (us*4)/5;
    while(us--) {
      delayMicroseconds(2);
    }
}
////////////////////////////////////////////////////
long read_update_P_sensor(unsigned char j)
{
  long P, P_H2O;
    while(j--){
        P = 0;
        for(int i=0; i<(NUM-1); i++){
            g_arr[i] = g_arr[i+1];
            P = P + g_arr[i];
        }
        g_arr[NUM-1] = bmp.readPressure();
        P = P + g_arr[NUM-1];
        P = P/5;
        P_H2O = P/98;       //98 H2O
        Serial.print("Pressure = ");
        //Serial.print(P);      Serial.print(" Pa     ");
        Serial.print(P_H2O - g_threshold_P_H2O);  Serial.println(" cm H2O");
    }    
    return P_H2O;
}
////////////////////////////////////////////////////
void display_lcd(unsigned short a, unsigned short b, unsigned short c)
{
  unsigned short ng, tr, ch, dv, temp;
    lcd.setCursor(0,0);   lcd.print("Vt");
    lcd.setCursor(4,0);   lcd.print("Ti");
    lcd.setCursor(9,0);   lcd.print("F");
    lcd.setCursor(12,0);  lcd.print("Mode");
    if      (mode == CMV) {lcd.setCursor(12,1); lcd.print("CMV ");}
    else if (mode == CPAP){lcd.setCursor(12,1); lcd.print("CPAP");}
    else if (mode == VAC) {lcd.setCursor(12,1); lcd.print("VAC ");}
    else if (mode == TEST) {lcd.setCursor(12,1); lcd.print("TEST");}

    ng = a/1000; 
    temp = a%1000;
    tr = temp/100;                       
    temp = a%100; ch = temp/10;       
    dv = a%10;
    if (mode == CPAP) { lcd.setCursor(0,1);   lcd.print("   ");}
    else {              lcd.setCursor(0,1);   /*lcd.write(ng+0x30);*/ lcd.write(tr+0x30); lcd.write(ch+0x30); lcd.write(dv+0x30);}   
    ng = b/1000; 
    temp = b%1000;
    tr = temp/100;                       
    temp = b%100; ch = temp/10;       
    dv = b%10;
    if (mode == CPAP) { lcd.setCursor(4,1);   lcd.print("    ");}
    else {              lcd.setCursor(4,1);   lcd.write(ng+0x30); lcd.write(tr+0x30); lcd.write(ch+0x30); lcd.write(dv+0x30);}  
    tr = c/100;                       
    temp = c%100; ch = temp/10;       
    dv = c%10;
    if (mode == CPAP) { lcd.setCursor(9,1);   lcd.print("   ");}
    else {              lcd.setCursor(9,1);  /*lcd.write(tr+0x30);*/ lcd.write(ch+0x30); lcd.write(dv+0x30);}  
}
////////////////////////////////////////////////////
long calculate_pulse(long Vt)
{
    float Vt_p = Vt/800.0;
    Vt_p = sqrt(Vt_p)*450;
    
    return (long)(Vt_p) + 140;  //offset 150*2 pulse
}
////////////////////////////////////////////////////
void go_home(void)
{
  unsigned char sum_sensor, i_h = 10;

    //check Switch
    while(i_h--){
        if(digitalRead(IN) == ON) {sum_sensor++;}
        else                      {sum_sensor = 0;}
        delay_us(10);
    }    
    if(sum_sensor >= 5){
        //digitalWrite(EN, EN_OF);
        return;
    }
    //go home & check Switch
    long t_st = 1000;
    digitalWrite(DIR, DIR_OF); 
    digitalWrite(EN, EN_ON);
    while(1){
        if(digitalRead(IN) == ON) {sum_sensor++;}
        else                      {sum_sensor = 0;}
        if(sum_sensor >= 5){
            //digitalWrite(EN, EN_OF);
            break;
        }
        delay_us(380 + t_st);   //400
        digitalWrite(PWM, ON);
        delay_us(380 + t_st);   //400
        digitalWrite(PWM, OF);

        if(t_st > 0) t_st -= 10;
    }    
}
////////////////////////////////////////////////////
void go_compress (long Vt_p, long ti)
{
      long pulse = Vt_p, t_st = 500;
      long t_pulse = ti/2, t_p = ti/pulse;
      
      digitalWrite(DIR, DIR_ON); 
      digitalWrite(L_GREEN, L_ON);
      while(pulse--){
          delay_us((t_pulse/2) + t_st); 
          digitalWrite(PWM, ON);
          delay_us((t_pulse/2) + t_st); 
          digitalWrite(PWM, OF);

          if(t_st > 0) t_st -= 50;
          t_pulse += t_p;
      }
      delay(10);
      digitalWrite(L_GREEN, L_OF);  
}
////////////////////////////////////////////////////
////////////////////////////////////////////////////
void run_CMV(long Vt_p, long ti, long te)
{  
      g_run_motor = ON;
      
    //ON
      digitalWrite(EN, EN_ON);
      go_compress(Vt_p, ti);
      
    //OFF
      go_home();
      delay(te);
      Serial.print("a\n");
      if(g_start == OF) {digitalWrite(EN, EN_OF);  Serial.print("b\n");} //???  
      
      g_run_motor = OF;
        
}
////////////////////////////////////////////////////
void run_CPAP(void)
{
        g_run_motor = ON;
        
    //ON
        digitalWrite(EN, EN_ON);
        long pulse = 600, t_pulse = 700;
        digitalWrite(DIR, DIR_ON);
        digitalWrite(L_GREEN, L_ON);
        while(pulse--){
            t_pulse = t_pulse + 12;
            delay_us(t_pulse); 
            digitalWrite(PWM, ON);
            delay_us(t_pulse); 
            digitalWrite(PWM, OF);
        }
        digitalWrite(L_GREEN, L_OF);
        
     //OFF
        go_home();
        Serial.print("a\n");
        if(g_start == OF) {digitalWrite(EN, EN_OF);  Serial.print("b\n");} //???
        
        g_run_motor = OF;
}
////////////////////////////////////////////////////
void run_VAC(long Vt_p, long ti)
{
    long Pressure_H2O = read_update_P_sensor(1) - g_threshold_P_H2O;    
    if( (Pressure_H2O > 2) && (g_active_VAC > 0) ) return;
    g_active_VAC = g_active_VAC_bk;

        g_run_motor = ON;
        
      //ON
        digitalWrite(EN, EN_ON);
        go_compress(Vt_p, ti);

        read_update_P_sensor(NUM);

      //OFF
        go_home();     
        Serial.print("a\n");
        if(g_start == OF) {digitalWrite(EN, EN_OF);  Serial.print("b\n");} //???   
        
        g_run_motor = OF;
}
////////////////////////////////////////////////////
void run_TEST(long Vt_p, long ti)
{
    long Pressure_H2O = read_update_P_sensor(1) - g_threshold_P_H2O;    
    if(Pressure_H2O > 2) return;

        g_run_motor = ON;
        
      //ON
        digitalWrite(EN, EN_ON);
        go_compress(Vt_p, ti);

        read_update_P_sensor(NUM);

      //OFF
        go_home();     
        //Serial.print("a\n");
        if(g_start == OF) {digitalWrite(EN, EN_OF);  Serial.print("b\n");} //???   
        
        g_run_motor = OF;     
}
/////////////////////////////////////////////////////////////////////////////////////////
// Timer int ////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
ISR (TIMER1_OVF_vect) 
{
    TCNT1 = 45535;            //10ms

    g_time_led++;
    if(g_time_led >= 25) {
        g_time_led = 0;
        g_Led = !g_Led;
        if(g_start == ON) digitalWrite(L_RED, g_Led);
    }

    g_but_red_1 = g_but_red_2;
    g_but_red_2 = digitalRead(B_RED);
    if((g_but_red_1 == ON) && (g_but_red_2 == OF) ) {
        mode++;
        if(mode == MODE) mode = 0;
    }
    
    g_but_green_1 = g_but_green_2;
    g_but_green_2 = digitalRead(B_GREEN);
    if((g_but_green_1 == ON) && (g_but_green_2 == OF) ) {
        if(g_start == OF) {g_start = ON;}
        else {
            g_start = OF; 
            digitalWrite(L_RED, L_OF);
            if(g_run_motor == OF){digitalWrite(EN, EN_OF);}
        }
    }

    if(g_active_VAC > 0) g_active_VAC--;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Setup & Main /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
    lcd.init();                      // initialize the lcd 
    pinMode(DIR, OUTPUT);
    pinMode(PWM, OUTPUT);
    pinMode(EN, OUTPUT);
    pinMode(L_GREEN, OUTPUT);
    pinMode(B_GREEN, INPUT_PULLUP);
    pinMode(L_RED, OUTPUT);
    pinMode(B_RED, INPUT_PULLUP);
    pinMode(IN, INPUT_PULLUP);
    pinMode(BUZ, OUTPUT);

    digitalWrite(BUZ, B_OF);
    digitalWrite(L_GREEN, L_OF);
    digitalWrite(L_RED, L_OF);
    digitalWrite(EN, EN_OF);

    Serial.begin(115200);
    Serial.println("Go Home");
    go_home(); digitalWrite(EN, EN_OF);
    
    // Print a message to the LCD.
    lcd.backlight();
    display_lcd(0, 0, 0);

    //init BMP180
    if (!bmp.begin()) {
        Serial.println("Could not find a valid BMP085 sensor, check wiring!");
        while (1) {}
    }
    memset(g_arr, 0, NUM);
    g_threshold_P_H2O = read_update_P_sensor(NUM);
    Serial.print("Threshold Pressure H2O = ");
    Serial.print(g_threshold_P_H2O);      Serial.println(" cm H2O");

    /* Reset Timer/Counter1 */
    TCCR1A = 0; TCCR1B = 0; TIMSK1 = 0;
    /* Setup Timer/Counter1 */
    TCCR1B |= (1 << CS11);    // prescale = 8
    TCNT1 = 45535;            //10ms
    TIMSK1 = (1 << TOIE1);    // Overflow interrupt enable 
    sei();                    // cho phép ngắt toàn cục
}
/////////////////////////////////////////////////////////////////////////
void loop()
{
    //////////Read volume Vt, ti, F
    g_Vt = analogRead(0);     //200 -> 800
        g_Vt = (g_Vt*6)/10    + 200;         
        if(g_Vt > 800)  g_Vt  = 800; 
    g_Ti = analogRead(1);     //500 -> 2000
        g_Ti = (g_Ti*3)/2     + 500;     
        if(g_Ti > 2000) g_Ti = 2000;
        if(g_Ti < g_Vt) g_Ti = g_Vt;
    g_F = analogRead(2);      //10 -> 40
        g_F = g_F/34          + 10; 
        if(g_F > 40)     g_F  = 40;
    
    //////////Calculate pulse & timer for Vt_p, ti, g_F, te, g_active_VAC
        g_Vt_p = calculate_pulse(g_Vt);
        
        g_ti = (g_Ti*1000)/g_Vt_p;          //us

        while(1) {
            g_Te = (60000000/g_F) - (g_Ti*1000);
            g_te = g_Te - 800*g_Vt_p;       //us
            g_te = g_te/1000;               //ms
            if(g_te >= 0) {break;}
            else {g_F--;}
        }

        g_active_VAC_bk = 6000/g_F;

        display_lcd(g_Vt, g_Ti, g_F);
        
    //////////Control Step motor
    if(g_start == ON) {
        if      (mode == 0) run_CMV(g_Vt_p, g_ti, g_te);
        else if (mode == 1) run_CPAP();
        else if (mode == 2) run_VAC(g_Vt_p, g_ti);
        else if (mode == 3) run_TEST(g_Vt_p, g_ti);
    }
   
}