#include <avr/interrupt.h>
#include <Wire.h> 
#include "LiquidCrystal_I2C.h"
#include "Adafruit_BMP085.h"
#include <math.h> 
#include "Protocol.h"
LiquidCrystal_I2C lcd(0x27,20,4);

// Button Pin
#define   ON            0  
#define   OF            1
#define   L_ON          LOW 
#define   L_OF          HIGH 
#define   EN_ON         HIGH  
#define   EN_OF         LOW
#define   DIR_ON        LOW  
#define   DIR_OF        HIGH
#define   B_ON          LOW
#define   B_OF          HIGH
#define   PWM       2
#define   DIR       3
#define   EN        4
#define   BUZ       7
#define   B_RED     8
#define   L_RED     9
#define   B_GREEN   10
#define   L_GREEN   11
#define   IN        12
unsigned char       g_but_green_1 = OF, g_but_green_2 = OF;
unsigned char       g_but_red_1 = OF,   g_but_red_2 = OF;
unsigned char       g_Led = OF, g_Time_led = 0;
unsigned char       g_start = OF;

#define   MODE      4
#define   A_VC      0
#define   SIMV      1
#define   PRVC      2
#define   SET       3
#define   TRIGGER   3
long                g_active_trigger = 0, g_active_trigger_bk;
struct my_data_set
{
  long g_Vt;
  long g_Ti;
  long g_Te;
  long g_F;
  long g_P;
  long g_IE;
  long g_Peep;
  long g_Pip;
  long g_Sup;
  long g_Mode;
};
struct my_data_set   st_data_set;

#define   NUM       5
long                g_arr[NUM], g_threshold_P_H2O;

#define   HMI_UART  Serial
unsigned char       g_HMI_config = OF;

HMI_protocol HMI_Protocol;
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
        //Serial.print("Pressure = ");
        //Serial.print(P);      Serial.print(" Pa     ");
        //Serial.print(P_H2O - g_threshold_P_H2O);  Serial.println(" cm H2O");
    }    
    return P_H2O;
}
////////////////////////////////////////////////////
void display_lcd(unsigned short a, unsigned short b, unsigned short c, unsigned short d, unsigned short e, unsigned short f)
{
  unsigned short ng, tr, ch, dv, temp;
    
    lcd.setCursor(0,0);   lcd.print("Vt   Ti    F    Mode");
    lcd.setCursor(0,2);   lcd.print("Peep Pip   Sup");
    
    if      (st_data_set.g_Mode == A_VC) {lcd.setCursor(16,1); lcd.print("AC/ "); lcd.setCursor(16,2); lcd.print("  VC");}
    else if (st_data_set.g_Mode == SIMV) {lcd.setCursor(16,1); lcd.print("SIMV"); lcd.setCursor(16,2); lcd.print(" +PS");}
    else if (st_data_set.g_Mode == PRVC) {lcd.setCursor(16,1); lcd.print("PRVC"); lcd.setCursor(16,2); lcd.print("    ");}
    else if (st_data_set.g_Mode == SET)  {lcd.setCursor(16,1); lcd.print("SET "); lcd.setCursor(16,2); lcd.print("    ");}

    ng = a/1000; 
    temp = a%1000;
    tr = temp/100;                       
    temp = a%100; ch = temp/10;       
    dv = a%10;
    lcd.setCursor(0,1);   lcd.write(tr+0x30); lcd.write(ch+0x30); lcd.write(dv+0x30);
    ng = b/1000; 
    temp = b%1000;
    tr = temp/100;                       
    temp = b%100; ch = temp/10;       
    //dv = b%10;
    lcd.setCursor(5,1);   lcd.write(ng+0x30); lcd.write('.');     lcd.write(tr+0x30);   lcd.write(ch+0x30);
    tr = c/100;                       
    temp = c%100; ch = temp/10;       
    dv = c%10;
    lcd.setCursor(11,1);  lcd.write(ch+0x30); lcd.write(dv+0x30);

    ch = d/10;       
    dv = d%10;
    lcd.setCursor(0,3);   lcd.write(ch+0x30); lcd.write(dv+0x30);
    ch = e/10;       
    dv = e%10;
    lcd.setCursor(5,3);   lcd.write(ch+0x30); lcd.write(dv+0x30);
    f *= 10;
    tr = f/100;                       
    ch = f%100; ch = ch/10;       
    dv = f%10;
    lcd.setCursor(11,3);  lcd.write(tr+0x30); lcd.write(ch+0x30); lcd.write(dv+0x30); lcd.print("%");

}
////////////////////////////////////////////////////
long calculate_pulse(long Vt)
{
    float Vt_p = Vt/800.0;
    Vt_p = sqrt(Vt_p)*450;
                                //Max 600 pulse
    return (long)(Vt_p) + 70;  //offset 150*2 pulse
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
        delay_us(380 + t_st);   //380 min
        digitalWrite(PWM, ON);
        delay_us(380 + t_st);   //380 min
        digitalWrite(PWM, OF);

        if(t_st > 0) t_st -= 10;
    }    
}
////////////////////////////////////////////////////
void go_compress (long Vt, long Ti)
{
  if(Vt == 0) return;
      long pulse  = calculate_pulse(Vt);
      long t_pulse = 380; //380 min
      //long t_pulse = (Vt*1000)/pulse, t_p = t_pulse/pulse;
      //t_pulse = t_pulse/4;
      long t_st = 500;
      
      digitalWrite(DIR, DIR_ON); 
      digitalWrite(EN, EN_ON);
      digitalWrite(L_GREEN, L_ON);
      while(pulse--){
          delay_us(t_pulse + t_st); 
          digitalWrite(PWM, ON);
          delay_us(t_pulse + t_st); 
          digitalWrite(PWM, OF);

          if(t_st > 0) t_st -= 50;
          t_pulse += 2;
          //if(pulse%2 == 0)  t_pulse += 1;
          //else              t_pulse += 2;
      }
      
      Ti = Ti - Vt; //vt
      long t = 10 + st_data_set.g_Peep;
      while(1){
          delay(t);   Ti -= t; if(Ti < 0) break;
          digitalWrite(PWM, ON); 
          delay(t);   Ti -= t; if(Ti < 0) break;
          digitalWrite(PWM, OF); 
      }
      
      digitalWrite(L_GREEN, L_OF);
}
////////////////////////////////////////////////////
////////////////////////////////////////////////////
void run_A_VC(long Vt, long Ti)
{
    long Pressure_H2O = read_update_P_sensor(1) - g_threshold_P_H2O;    
    if( (Pressure_H2O > (st_data_set.g_Peep-TRIGGER)) && (g_active_trigger > 0) ) return;
    if(g_active_trigger == 0) {g_active_trigger = g_active_trigger_bk;}
        
      //ON
        go_compress(Vt, Ti);
        read_update_P_sensor(NUM);

      //OFF
        go_home();  
}
////////////////////////////////////////////////////
void run_SIMV(long Vt, long Ti)
{  
    long Pressure_H2O = read_update_P_sensor(1) - g_threshold_P_H2O;    
    if( (Pressure_H2O > (st_data_set.g_Peep-TRIGGER)) && (g_active_trigger > 0) ) return;
    if(g_active_trigger == 0) {g_active_trigger = g_active_trigger_bk;}
    if(Pressure_H2O <= (st_data_set.g_Peep-TRIGGER)) {
        Vt = (Vt*st_data_set.g_Sup)/10; Ti = (Ti*st_data_set.g_Sup)/10; 
        if(Ti < 500) {Ti = 500;}
    }
        
      //ON
        go_compress(Vt, Ti);
        read_update_P_sensor(NUM);

      //OFF
        go_home();
}
////////////////////////////////////////////////////
void run_PRVC(long Vt, long Ti)
{

}
////////////////////////////////////////////////////
void setting(void)
{
  unsigned short temp;
  
    temp = analogRead(0);   //5 -> 10
    temp = temp/200         + 5; 
    if(temp > 10)     temp  = 10;
    st_data_set.g_Peep = temp;

    temp = analogRead(1);   //20 -> 40
    temp = temp/51          + 20; 
    if(temp > 40)     temp  = 40;
    st_data_set.g_Pip = temp;

    temp = analogRead(2);   //0 -> 10
    temp = temp/100; 
    if(temp > 10)     temp   = 10;
    st_data_set.g_Sup = temp;
}
/////////////////////////////////////////////////////////////////////////////////////////
// Timer int ////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
ISR (TIMER1_OVF_vect)
{
    TCNT1 = 45535;            //10ms

    g_Time_led++;
    if(g_Time_led >= 25) {
        g_Time_led = 0;
        g_Led = !g_Led;
        if(g_start == ON) digitalWrite(L_RED, g_Led);
    }

    if(g_HMI_config == OF) {
        g_but_red_1 = g_but_red_2;
        g_but_red_2 = digitalRead(B_RED);
        if((g_but_red_1 == ON) && (g_but_red_2 == OF) ) {
            st_data_set.g_Mode++;
            if(st_data_set.g_Mode == MODE) st_data_set.g_Mode = 0;
        }
    }
    
    g_but_green_1 = g_but_green_2;
    g_but_green_2 = digitalRead(B_GREEN);
    if((g_but_green_1 == ON) && (g_but_green_2 == OF) ) {
        if(g_start == OF) {g_start = ON;}
        else {
            g_start = OF; 
            digitalWrite(L_RED, L_OF);
        }
    }

    if(g_active_trigger > 0) g_active_trigger--;
    if(HMI_Protocol.timeout_uart_rx > 0) HMI_Protocol.timeout_uart_rx--;
}

/////////////////////////////////////////////////////////////////////////////////////////
// HMI UART /////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
void uart_sent_data(unsigned char *str, unsigned char len)
{ 
  unsigned char i_s, j_s = 0;
  unsigned short crc;
  
      while(len--) {
        HMI_Protocol.uart_sent[j_s+5] = str[j_s];
        j_s++;
      }

      HMI_Protocol.uart_sent[0] = ADRESS;  //Adress of button & printer
      HMI_Protocol.uart_sent[1] = HMI_Protocol.st_uart.function;
      HMI_Protocol.uart_sent[2] = HMI_Protocol.st_uart.tranid;
      HMI_Protocol.uart_sent[3] = (j_s >>8) & 0xff;
      HMI_Protocol.uart_sent[4] = j_s & 0xff;

      crc = HMI_Protocol.crc16(HMI_Protocol.uart_sent, j_s + 5);
      HMI_Protocol.uart_sent[j_s + 5] = (crc >>8) & 0xff;
      HMI_Protocol.uart_sent[j_s + 6] = crc & 0xff;

      j_s = j_s + 6;
      for(i_s=0; i_s<=j_s; i_s++) {
        HMI_UART.write(HMI_Protocol.uart_sent[i_s]);
      } 
}

/////////////////////////////////////////////////////
void process_cmd(unsigned char ck_sum)
{
    //if(ck_sum != PAS) return;

    g_HMI_config = ON; 
    if(HMI_Protocol.st_uart.function == S_START) {
        g_start = ON;
        
        HMI_Protocol.st_uart.uart_data[0] = 1;
        uart_sent_data(HMI_Protocol.st_uart.uart_data, 1);
    }
    else if(HMI_Protocol.st_uart.function == S_STOP) {
        g_start = OF; 
        digitalWrite(L_RED, L_OF);
        
        HMI_Protocol.st_uart.uart_data[0] = 1;
        uart_sent_data(HMI_Protocol.st_uart.uart_data, 1);
    }
    else if(HMI_Protocol.st_uart.function == S_READ) {
        HMI_Protocol.st_uart.uart_data[0] = st_data_set.g_Mode;
        HMI_Protocol.st_uart.uart_data[1] = st_data_set.g_Vt>>8;
        HMI_Protocol.st_uart.uart_data[2] = st_data_set.g_Vt;
        HMI_Protocol.st_uart.uart_data[3] = st_data_set.g_Ti>>8;
        HMI_Protocol.st_uart.uart_data[4] = st_data_set.g_Ti;
        HMI_Protocol.st_uart.uart_data[5] = st_data_set.g_F;
        HMI_Protocol.st_uart.uart_data[6] = st_data_set.g_Peep;
        HMI_Protocol.st_uart.uart_data[7] = st_data_set.g_Pip;
        HMI_Protocol.st_uart.uart_data[8] = st_data_set.g_Sup;
        uart_sent_data(HMI_Protocol.st_uart.uart_data, 9);
    }
    else if(HMI_Protocol.st_uart.function == S_WRITE) {
        st_data_set.g_Mode  = HMI_Protocol.st_uart.uart_data[0];
        st_data_set.g_Vt    = HMI_Protocol.st_uart.uart_data[1]*256 + HMI_Protocol.st_uart.uart_data[2];
        st_data_set.g_Ti    = HMI_Protocol.st_uart.uart_data[3]*256 + HMI_Protocol.st_uart.uart_data[4];
        st_data_set.g_F     = HMI_Protocol.st_uart.uart_data[5];
        st_data_set.g_Peep  = HMI_Protocol.st_uart.uart_data[6];
        st_data_set.g_Pip   = HMI_Protocol.st_uart.uart_data[7];
        st_data_set.g_Sup   = HMI_Protocol.st_uart.uart_data[8];

        while(1) {
            st_data_set.g_Te = (60000000/st_data_set.g_F) - (st_data_set.g_Ti*1000);
            st_data_set.g_Te = st_data_set.g_Te - 800*calculate_pulse(st_data_set.g_Vt);  //us
            if(st_data_set.g_Te >= 0) {break;}
            else {st_data_set.g_F--;}
        }
        HMI_Protocol.st_uart.uart_data[5] = st_data_set.g_F;
        uart_sent_data(HMI_Protocol.st_uart.uart_data, 9);
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Setup & Main /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
    lcd.init();                      // initialize the lcd 
    pinMode(DIR,      OUTPUT);
    pinMode(PWM,      OUTPUT);
    pinMode(EN,       OUTPUT);
    pinMode(L_GREEN,  OUTPUT);
    pinMode(B_GREEN,  INPUT_PULLUP);
    pinMode(L_RED,    OUTPUT);
    pinMode(B_RED,    INPUT_PULLUP);
    pinMode(IN,       INPUT_PULLUP);
    pinMode(BUZ,      OUTPUT);
    digitalWrite(BUZ,     B_OF);
    digitalWrite(L_GREEN, L_OF);
    digitalWrite(L_RED,   L_OF);
    digitalWrite(EN,      EN_OF);
    st_data_set.g_Peep = 5; 
    st_data_set.g_Pip  = 20; 
    st_data_set.g_Sup  = 5;
    st_data_set.g_Mode = 0;

    Serial.begin(115200);
    Serial.println("Go Home");
    go_home(); digitalWrite(EN, EN_OF);
    
    // Print a message to the LCD.
    lcd.backlight();

    //init BMP180
    if (!bmp.begin()) {
        Serial.println("Could not find a valid BMP085 sensor, check wiring!");
        while (1) {}
    }
    memset(g_arr, 0, NUM);
    g_threshold_P_H2O = read_update_P_sensor(NUM);
    Serial.print("Threshold Pressure H2O = "); Serial.print(g_threshold_P_H2O); Serial.println(" cm H2O");

    /* Reset Timer/Counter1 */
    TCCR1A = 0; TCCR1B = 0; TIMSK1 = 0;
    /* Setup Timer/Counter1 */
    TCCR1B |= (1 << CS11);    // prescale = 8
    TCNT1 = 45535;            // 10ms
    TIMSK1 = (1 << TOIE1);    // Overflow interrupt enable 
    sei();                    // cho phép ngắt toàn cục
}
/////////////////////////////////////////////////////////////////////////
void loop()
{
    ///////////process USB_UART
    if(HMI_UART.available()) {
        HMI_Protocol.timeout_uart_rx = 2;  //20 ms
        unsigned char i = 0;
        HMI_Protocol.uart_read[i] = HMI_UART.read();
        i++;
        while(HMI_Protocol.timeout_uart_rx) {
            if(HMI_UART.available()) {
                HMI_Protocol.uart_read[i] = HMI_UART.read();
                i++;
            }
        }
        if(i >= 7) {
            unsigned char d_temp = HMI_Protocol.checksum_data(HMI_Protocol.uart_read, i);
            process_cmd(d_temp);
        }
    }
    
    //////////Read volume Vt, ti, F
    if( (st_data_set.g_Mode != SET) && (g_HMI_config == OF) ) {
        st_data_set.g_Vt = analogRead(0);                 //200 -> 800
            st_data_set.g_Vt = (st_data_set.g_Vt*6)/100   + 20;         
            if(st_data_set.g_Vt > 80)  st_data_set.g_Vt   = 80; 
            st_data_set.g_Vt *=10;
        st_data_set.g_Ti = analogRead(1);                 //500 -> 2000
            st_data_set.g_Ti = (st_data_set.g_Ti*3)/20    + 50;     
            if(st_data_set.g_Ti > 200) st_data_set.g_Ti   = 200;
            st_data_set.g_Ti *=10;
            if(st_data_set.g_Ti < st_data_set.g_Vt) st_data_set.g_Ti = st_data_set.g_Vt;
        st_data_set.g_F = analogRead(2);                  //5 -> 40
            st_data_set.g_F = st_data_set.g_F/29          + 5; 
            if(st_data_set.g_F > 40)     st_data_set.g_F  = 40;
    //////////Calculate pulse & timer for st_data_set.g_F, st_data_set.g_IE, g_active_trigger
        while(1) {
            st_data_set.g_Te = (60000000/st_data_set.g_F) - (st_data_set.g_Ti*1000);
            st_data_set.g_Te = st_data_set.g_Te - 800*calculate_pulse(st_data_set.g_Vt);  //us
            if(st_data_set.g_Te >= 0) {break;}
            else {st_data_set.g_F--;}
        }
        st_data_set.g_IE = st_data_set.g_Te/(st_data_set.g_Ti*100);
        //Serial.print(st_data_set.g_Te); Serial.print(" "); Serial.println(st_data_set.g_Ti);
        //Serial.print("I:E = 1:"); Serial.print(st_data_set.g_IE/10); Serial.print("."); Serial.println(st_data_set.g_IE%10);  
    }
    
    g_active_trigger_bk = 6000/st_data_set.g_F;

    display_lcd(st_data_set.g_Vt, st_data_set.g_Ti, st_data_set.g_F, st_data_set.g_Peep, st_data_set.g_Pip, st_data_set.g_Sup);
        
    //////////Control Step motor
    if(g_start == ON) {
        if      (st_data_set.g_Mode == A_VC) run_A_VC(st_data_set.g_Vt, st_data_set.g_Ti);
        else if (st_data_set.g_Mode == SIMV) run_SIMV(st_data_set.g_Vt, st_data_set.g_Ti);
        else if (st_data_set.g_Mode == PRVC) run_PRVC(st_data_set.g_Vt, st_data_set.g_Ti);
    }
    else { digitalWrite(EN, EN_OF); }
    if(st_data_set.g_Mode == SET)  setting();
   
}
