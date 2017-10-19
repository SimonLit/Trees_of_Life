#include "Pulse.h"

//Used for normal pulse
Pulse::Pulse(CRGB * StripLeds, int Num_leds, int Hue) {
  this->leds = StripLeds;
  this->num_leds = Num_leds;
  this->hue = Hue;
  pulseIndex = 0 - (firstWave + secondWave); // in case of tickSawWave
  pulseDec = pulseIndex;

  //Temp so we can prototype with the old strips.
  //If id equals a new strip then use new strip settings
  if (stripTypeNew) {
    pulseSpeed = NEW_PULSESPEED;
    fallSpeed = NEW_FALLSPEED;
    firstWaveAmplitudeFactor = NEW_FIRSTWAVEAMPLITUDEFACTOR;
    secondWaveAmplitudeFactor = NEW_SECONDWAVEAMPLITUDEFACTOR;
  }
  else {
    pulseSpeed = OLD_PULSESPEED;
    fallSpeed = OLD_FALLSPEED;
    firstWaveAmplitudeFactor = OLD_FIRSTWAVEAMPLITUDEFACTOR;
    secondWaveAmplitudeFactor = OLD_SECONDWAVEAMPLITUDEFACTOR;
  }
}

//used for rest pulse
Pulse::Pulse(CRGB* StripLeds, int Num_leds, int Hue, int customIndex) {
  this->leds = StripLeds;
  this->num_leds = Num_leds;
  this->hue = Hue;
  pulseIndex = customIndex;
  pulseDec = pulseIndex;
  if (stripTypeNew) {
    restPulseSpeed = NEW_RESTPULSESPEED;
  }
  else {
    restPulseSpeed = OLD_RESTPULSESPEED;
  }
  startTime = millis();
}

bool Pulse::tick() {
  if ((pulseIndex + hPulseSize ) >= num_leds) {
  }
  else {
    for ( int i = 0; i < hPulseSize; i ++) {
      leds[pulseIndex + i] = CHSV(hue, 255, brightness);
      //leds[pulseIndex + i] = currentColour[0];
    }
    pulseIndex += pulseSpeed;
    pulseDec = pulseIndex; //Inefficient to do this, but performance is good enough.
  }
  return true;
}

void Pulse::SawToothWave(int x, int waveSize, int tailWave, int amplitudeFactor) {
  int currentPixel;
  int firstWaveSize = waveSize - tailWave;
  for (currentPixel = 0; currentPixel < firstWaveSize; currentPixel++) {
    if ((currentPixel + x) >= 0 && (currentPixel + x ) < num_leds) {
      int waveParticle = (triwave8(currentPixel) * amplitudeFactor + 20);

      if (waveParticle > 0) {
        leds[x + currentPixel] = CHSV(hue, 255, waveParticle);
      }
    }
  }
  //Serial.println("Secondwave");
  for (int i = tailWave, e = firstWaveSize; i >= 0; i--, currentPixel++, e -= 2) {
    if ((currentPixel + x) >= 0 && (currentPixel + x ) < num_leds) {
      int waveParticle = (triwave8(e) * amplitudeFactor + 20);
      if (waveParticle > 0) {
        leds[x + currentPixel] = CHSV(hue, 255, waveParticle);
      }
    }
  }
}

bool Pulse::tickCustomWave() {
  if (pulseIndex + (waveSize * 2) >= num_leds) {
    //Do nothing
  }
  else {
    int e = 0;
    Serial.println("Bigwave");
    for (int i = 0; i < waveSize; i++, e++) {
      Serial.print("LED:");
      Serial.print(pulseIndex + i);
      Serial.print("\t");
      Serial.println(bigWaveBrightness[i]);
      leds[pulseIndex + i] = CHSV(hue, 255, bigWaveBrightness[i]);
    }
    Serial.println("Smallwave");
    for (int i = 0; i < waveSize; i++, e++) {
      Serial.print("LED:");
      Serial.print(pulseIndex + e);
      Serial.print("\t");
      Serial.println(bigWaveBrightness[i]);
      leds[pulseIndex + e] = CHSV(hue, 255, smallWaveBrightness[i]);
    }
    pulseDec += restPulseSpeed;
    pulseIndex = (int)pulseDec; //THIS ENHANCES CUSTOMIZABILITY BUT MIGHT AFFECT SPEED. VERIFY LATER!
  }
  return true;
}
bool Pulse::tickSawWave() {
  if (pulseIndex >= num_leds) {
    return false;
  }
  else {
    SawToothWave(pulseIndex + secondWave, firstWave, waveTail, firstWaveAmplitudeFactor);
    //SawToothWave(pulseIndex, secondWave, waveTail, secondWaveAmplitudeFactor);

    pulseDec += restPulseSpeed;
    pulseIndex = (int)pulseDec; //THIS ENHANCES CUSTOMIZABILITY BUT MIGHT AFFECT SPEED. VERIFY LATER!
  }
  return true;
}


bool Pulse::tickRestWave() {
  Serial.print("Ticking rest wave: \t");
  if (pulseIndex >= num_leds) {
    Serial.print("index too high");
    Serial.println(pulseIndex);
    return false;
  }
  else {
    Serial.println("Ticking rest pulse");
    int currentTime = millis();
    if (startTime + restpulseLifeTime > currentTime) {
      for (int i = 0; i < restpulseSize; i++) {
        if (pulseIndex + i < num_leds ) {
          leds[pulseIndex + i] = CHSV(restHue, 255, brightness);
        }
      }
    }
    else {
      //Start fading
      //But for now end
      return false;
    }
    pulseDec += restPulseSpeed;
    pulseIndex = (int)pulseDec; //THIS ENHANCES CUSTOMIZABILITY BUT MIGHT AFFECT SPEED. VERIFY LATER!
  }
  return true;
}
bool Pulse::fall() {
  //Serial.println(pulseIndex <= (pulseIndex - (firstWave + secondWave + 1));
  if (pulseIndex <= (0 - (firstWave + secondWave))) { //Customized for sawtoothwave size!!
    return false; //finished collapsing
  }
  else {
    SawToothWave(pulseIndex + secondWave, firstWave, waveTail, firstWaveAmplitudeFactor);
    SawToothWave(pulseIndex, secondWave, waveTail, secondWaveAmplitudeFactor);
  }
  pulseDec -= fallSpeed;
  pulseIndex = (int)pulseDec; //THIS ENHANCES CUSTOMIZABILITY BUT MIGHT AFFECT SPEED. VERIFY LATER!
  return true;
}
