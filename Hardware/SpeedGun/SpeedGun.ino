#include <arduinoFFT.h>
//#include <stdint.h>

#define INPUT_PIN 36 /* ADC1_CHANNEL_0 */

#define SAMPLE_NUMBER 1024 /* Samples */
#define SAMPLING_FREQUENCY 20480 /* Hz */
#define SAMPLE_DURATION 50 /* ms (SAMPLE_NUMBER / SAMPLING_FREQUENCY) */
#define FREQUENCY_RESOLUTION 20 /* Hz (SAMPLING_FREQUENCY / SAMPLE_NUMBER) */
#define DOPPLER_TO_VELOCITY 44.4444 /* km/h */

double vReal[SAMPLE_NUMBER]={0};
double vImag[SAMPLE_NUMBER]={0};
uint16_t binMax=0;
uint16_t usSamplingPeriod=round(1.0/SAMPLING_FREQUENCY*1000000);
uint32_t timestamp=0;
double dominantFrequency=0;
arduinoFFT FFT;

void setup() 
{
  FFT = arduinoFFT(vReal, vImag, SAMPLE_NUMBER, SAMPLING_FREQUENCY);
  Serial.begin(115200);
}

void loop() 
{
  for (register uint16_t i=0; i<SAMPLE_NUMBER; i++)
  {
    timestamp=micros();
    /* analogRead takes 9.7uS on ESP32 TODO */
    vReal[i]=analogRead(INPUT_PIN);
    vImag[i]=0;
    while ((micros() - timestamp) < usSamplingPeriod)
    {
      /* wait */
    }
  }

  FFT.DCRemoval();
  FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(FFT_FORWARD);
  FFT.ComplexToMagnitude();

  binMax=0;
  for (register uint16_t i=1; i<SAMPLE_NUMBER/2; i++)
  {
    if (vReal[i]>vReal[binMax])
    {
      binMax=i;
    }
  }
  dominantFrequency=binMax*FREQUENCY_RESOLUTION;

  Serial.print("Dominant frequency: ");
  Serial.print(dominantFrequency);
  Serial.println(" Hz");

  Serial.print("Detected speed: ");
  Serial.print(dominantFrequency/DOPPLER_TO_VELOCITY);
  Serial.println(" km/h");
}
