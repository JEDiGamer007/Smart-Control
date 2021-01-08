#ifndef SHUTTERSWITCH_HH
#define SHUTTERSWITCH_HH

#include <Arduino.h>

using pin=const uint8_t;

class ShutterSwitch {
  private:
    pin _pins[2];
    int8_t _state;
    bool _lastSwitchState[2];
    bool _risingEdge[2];

  public:
   unsigned long timer;

   ShutterSwitch(pin (&pinsIN)[2]) :
      _pins{pinsIN[0], pinsIN[1]},
      _state{0},
      _lastSwitchState{debouncedRead(0), debouncedRead(1)},
      _risingEdge{_lastSwitchState[0], _lastSwitchState[1]},
      timer(0)
      {}
    ShutterSwitch(pin (&pinsIN)[2], const uint8_t &stateIN) :
      _pins{pinsIN[0], pinsIN[1]},
      _state(stateIN),
      timer(0)
      {}
    
    int read(uint8_t i_pin) {return digitalRead(_pins[i_pin]);}

    int debouncedRead(uint8_t i_pin) {
      uint8_t counter = 0;
      for (int i = 0; i < 10; i++) {
        if (digitalRead(_pins[i_pin]) == LOW) {counter++;}
        delayMicroseconds(200);
      }
      if (counter > 5) {return LOW;}
      return HIGH;
    }

    bool hasChanged(uint8_t index) {
      //Get pin state
      bool buttonState = debouncedRead(_pins[index]);
      
      //Here starts the code for detecting an edge
      if (buttonState != _lastSwitchState[index]) {
          if (buttonState == LOW) {
              _risingEdge[index] = 0;
          }
          else {
            _risingEdge[index] = 1;
          }
          _lastSwitchState[index] = buttonState;
          return true;
      }
      
      return false;
    }

    bool hasChanged() {return hasChanged(0) || hasChanged(1);}

    int8_t getState() {return _state;}
    //TODO: maybe read GPIO and set internal vars here
    void update() {
      //BAD, could be put into hasChanged()
      if (hasChanged(0)) { if (!_risingEdge[0]) {_state = 1;} }
      else if (hasChanged(1)) { if (!_risingEdge[1]) {_state = -1;} }
      else {_state = 0;}
    }
};

#endif