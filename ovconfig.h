#ifndef OVCONFIG_H
#define OVCONFIG_H

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

#define   MODE      4
#define   A_VC      0
#define   SIMV      1
#define   PRVC      2
#define   SET       3
#define   TRIGGER   3

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

extern struct my_data_set st_data_set;

#endif // OVCONFIG_H
