/***********************************************************************
 //  Temperatures
 ***********************************************************************/
 
class Temperatures
{
public:
  Temperatures();
  ~Temperatures();
  
  float getTemp(int chip);
  void calcTemps(float B, float T0, float R0, float R_Balance);
  float getHighestTemp();
private:
  float temperatures[4];
};
