#ifndef BALL
#define BALL

#include <json.hpp>

struct vect {
  int x;
  int y;
};

class ball {
  public:
    ball () {
      screen_dim = { 640, 480 };
      dim = { 50, 50 };
      pos = { screen_dim.x/2, screen_dim.y/2 };
      vel = { 10, 25 };
    }

    void update() {
      // Check horisontal boundaries
      if ( pos.x < 0 || pos.x + dim.x > screen_dim.x )
        vel.x *= -1;
    
      // Check vertical boundaries
      if ( pos.y < 0 || pos.y + dim.y > screen_dim.y )
        vel.y *= -1;

      // Update position
      pos.x += vel.x;
      pos.y += vel.y;
    }

    nlohmann::json getPosJSON () {
      using json = nlohmann::json;

      json posJSON = {
        {"pos", {
          {"x", pos.x},
          {"y", pos.y}
        } }
      };

      return posJSON;
    }

  private:
    vect dim;
    vect pos;
    vect vel;

    vect screen_dim;

};

#endif