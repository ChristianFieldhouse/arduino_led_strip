class Particles {
  public:
    int count;
    float * tail_lengths;
    float * positions;
    float * speeds;
    CRGB * colours;
    Particles(int count);
    void step_leds();
};

Particles::Particles(int count)
{
  this->count = count;
  this->tail_lengths = (float *) malloc(sizeof(float) * count);
  this->positions = (float *) malloc(sizeof(float) * count);
  this->speeds = (float *) malloc(sizeof(float) * count);
  this->colours = (CRGB *) malloc(sizeof(CRGB) * count);
  for (int i=0; i < this->count; ++i){
    positions[i] = float(random8() % NUM_LEDS);
    speeds[i] = (float(random8()) - 127.f) / 127.f;
    tail_lengths[i] = 5.f;
    colours[i] = CHSV( random8(), random8(), 255);
  }
}

void Particles::step_leds()
{
  for (int i=0; i < this->count; ++i){
    this->positions[i] += this->speeds[i];
    if (this->positions[i] > NUM_LEDS){
      this->positions[i] = 1.f;
    }
    else if (this->positions[i] < 0){
      this->positions[i] += NUM_LEDS;
    }
    const int dir = 2 * int(speeds[i] > 0) - 1;
    /*for (int k = 1; k < 10; k += 1){
      leds[k] = CRGB::White;
    }*/
    //Serial.print("position ");
    //Serial.print(i);
    //Serial.print(" : ");
    //Serial.println(this->positions[i]);
    for (int k = 0; k < this->tail_lengths[i]; k += 1){
      float scale = (this->tail_lengths[i] - k) / this->tail_lengths[i];
      scale = scale * scale;
      const int led = (int(positions[i]) - dir * k + NUM_LEDS) % NUM_LEDS;
      leds[led] = CRGB(int(colours[i].r * scale), int(colours[i].g * scale), int(colours[i].b * scale));
    }
    const int led = int(positions[i] - dir * this->tail_lengths[i] + NUM_LEDS) % NUM_LEDS;
    //Serial.println(led);
    leds[led] = CRGB::Black;
  }
}
