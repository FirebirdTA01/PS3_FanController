#include "Temperatures.h"
#include <arduino.h>
#include <math.h>

#define CPU_THERM A1
#define GPU_THERM A2
#define NB_THERM A3
#define EXHAUST_THERM A4

//10k resistor actual measured values
#define R_BalanceCPU 9830
#define R_BalanceGPU 9840
#define R_BalanceNB 9860
#define R_BalanceEXHAUST 9820

Temperatures::Temperatures(){
  temperatures[0] = 0.00; //cpu
  temperatures[1] = 0.00; //gpu
  temperatures[2] = 0.00; //northbridge
  temperatures[3] = 0.00; //exhaust
}

Temperatures::~Temperatures(){
	//dtor
}

float Temperatures::getTemp(int chip)
{
  return temperatures[chip];
}

void Temperatures::calcTemps(float B, float T0, float R0, float R_Balance)
{
  //loop for each thermistor
  for(int i = 0; i <= 3; i++)
  {
    float resistance = 0.0f;
    //take an average of 20 readings from each thermistor
    for(int a = 1; a <= 20; a ++)
    {
      switch(i){
        case 0:
        {
          resistance += analogRead(CPU_THERM);
          break;
        }
        case 1:
        {
          resistance += analogRead(GPU_THERM);
          break;
        }
        case 2:
        {
          resistance += analogRead(NB_THERM);
          break;
        }
        case 3:
        {
          resistance += analogRead(EXHAUST_THERM);
          break;
        }
        default:
          break;
      }
    }
    resistance = resistance/20;
    
    //calculate temperature
    float R,T;
    
    //using absolute 10k resistor values
    //R=1024.0f*10000/resistance-R_Balance;
    //using actual measured values
    switch(i){
      case 0:
        {
          R=1024.0f*10000/resistance-R_BalanceCPU;
          break;
        }
        case 1:
        {
          R=1024.0f*10000/resistance-R_BalanceGPU;
          break;
        }
        case 2:
        {
          R=1024.0f*10000/resistance-R_BalanceNB;
          break;
        }
        case 3:
        {
          R=1024.0f*10000/resistance-R_BalanceEXHAUST;
          break;
        }
        default:
          break;
    }
    T=1.0f/(1.0f/T0+(1.0f/B)*log(R/R0));
    
    temperatures[i] = T-273.15f; //degrees celcius
  }
}

float Temperatures::getHighestTemp()
{
  float highestTemp = 0.00f;
  for(int i = 0; i <= 3; i++)
  {
    float currentRead = getTemp(i);
    //just temporary, skip cpu until circuit bug is fixed
    if(i == 0)
    currentRead = 0;
    if(currentRead > highestTemp)
      highestTemp = currentRead;
  }
  return highestTemp;
}
