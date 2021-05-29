
class Collider {
  public:
    int chunk_size;
    int stage;
    CRGB colour;
    Collider(int chunksize);
    void step_leds();
};

Collider::Collider(int chunksize)
{
  this->chunk_size = chunksize;
  this->stage = 0;
  this->colour = CHSV( random8(), 255, 255);
}

void Collider::step_leds()
{
  Serial.println("stepping");
  if (this->stage < NUM_LEDS / 2){
    for (int i=max(0, this->stage - chunk_size); i < max(0, this->stage); ++i){
      leds[i] = CRGB::White;
      leds[NUM_LEDS - i - 1] = CRGB::White;
      //Serial.println("white chunks");
      //Serial.println(i);
      //Serial.println(stage);
    }
    if (this->stage - chunk_size > 0){
      leds[this->stage - chunk_size - 1] = CRGB::Black;
      leds[NUM_LEDS - (this->stage - chunk_size)] = CRGB::Black;
    }
  }
  if (this->stage >= NUM_LEDS / 2) {
    int w = this->stage - NUM_LEDS/2;
    for (int i=NUM_LEDS/2 - w; i < NUM_LEDS/2 + w; ++i){
      leds[i] = this->colour;
      //Serial.println("colour");
      //Serial.println(i);
    }
  }
  this->stage += 1;
  if (this->stage >= NUM_LEDS){
    this->stage = 0;
    this->colour = CHSV( random8(), 255, 255);
  }
  Serial.println("done stepping");
}
