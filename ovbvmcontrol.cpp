#include "ovbvmcontrol.h"
#include "ovconfig.h"


OVBVMControl::OVBVMControl()
{

}

void OVBVMControl::Compress(long Vt, long Ti)
{
#if defined(ARDUINO)
    if(Vt == 0) return;
        long pulse  = calculatePulse(Vt);
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
#endif
}

void OVBVMControl::Home()
{
#if defined(ARDUINO)
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
#endif
}
