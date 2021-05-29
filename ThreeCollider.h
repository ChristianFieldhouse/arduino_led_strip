
class ThreeCollider {
  public:
    int chunk_size;
    int x0;
    int x1;
    int x2;
    int d0;
    int d1;
    int d2;
    CRGB col0;
    CRGB col1;
    CRGB col2;
    ThreeCollider(int chunksize, int x0, int x1, int x2);
    void step_leds();
};

ThreeCollider::ThreeCollider(int chunksize, int x0, int x1, int x2)
{
  this->chunk_size = chunksize;
  this->x0 = x0;
  this->x1 = x1;
  this->x2 = x2;
  this->d0 = 1;
  this->d1 = 1;
  this->d2 = 1;
  this->col0 = CHSV( random8(), 255, 255);
  this->col1 = CHSV( random8(), 255, 255);
  this->col2 = CHSV( random8(), 255, 255);
}

void ThreeCollider::step_leds()
{
  this->x0 += this->d0;
  this->x1 += this->d1;
  this->x2 += this->d2;

  if (random(0, 4) == 3){
    this->x1 -= this->d1;
  }
  int bounce = 2;
  Serial.println(this->x2);
  if (this->x1 - this->x0 < this->chunk_size){
    if (this->d0 * this->d1 < 0){
       this->d1 *= -1; 
    }
    this->d0 *= -1;
    this->x0 += bounce * this->d0;
    this->x1 += bounce * this->d1;
  }
  if (this->x2 - this->x1 < this->chunk_size){
    if (this->d2 * this->d1 < 0){
       this->d1 *= -1;
    }
    this->d2 *= -1;
    this->x2 += bounce * this->d2;
    this->x1 += bounce * this->d1;
  }
  if (this->x0 < this->chunk_size){
    this->d0 = 1;
  }
  if (this->x2 > NUM_LEDS){
    this->d2 = -1;
  }
  //Serial.println("stepping");

  for (int i=0; i < NUM_LEDS; ++i){
    if ((i < this->x0)&&(i > this->x0 - this->chunk_size)){
      leds[i] = CRGB::Purple;
    }
    else if ((i < this->x1)&&(i > this->x1 - this->chunk_size)){
      leds[i] = CRGB::Green;
    }
    else if ((i < this->x2)&&(i > this->x2 - this->chunk_size)){
      leds[i] = CRGB::Red;
    }
    else{
      leds[i] = CRGB::Black;
    }
  }
  //Serial.println("done stepping");
}
