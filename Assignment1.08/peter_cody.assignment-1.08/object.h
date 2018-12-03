//
//  object.h
//  
//
//  Created by Cody Peter on 11/4/18.
//

#ifndef object_h
#define object_h

#include <stdio.h>
# include <stdint.h>
# include <vector>
# include <string>
# include "dice.h"
# include "dims.h"
# include "descriptions.h"

using namespace std;

class object {
public:
  string name, description;
  object_type_t type;
  uint32_t color;
  int32_t hit, dodge, defence, weight, speed, attribute, value;
  dice damage;
  bool artifact;
  uint32_t rarity;
  bool used;
  object(string name, string description, object_type_t type,
         uint32_t color, int32_t hit, int32_t dodge,
         int32_t defence, int32_t weight, int32_t speed,
         int32_t attribute, int32_t value, dice damage,
         bool artifact, uint32_t rarity) {
    this->name = name;
    this->description = description;
    this->type = type;
    this->color = color;
    this->hit = hit;
    this->dodge = dodge;
    this->defence = defence;
    this->weight = weight;
    this->speed = speed;
    this->attribute = attribute;
    this->value = value;
    this->damage = damage;
    this->artifact = artifact;
    this->rarity = rarity;
  }
  void set(const std::string &name,
           const std::string &description,
           const object_type_t type,
           const uint32_t color,
           const uint32_t &hit,
           const dice &damage,
           const int32_t &dodge,
           const int32_t &defence,
           const int32_t &weight,
           const int32_t &speed,
           const int32_t &attrubute,
           const int32_t &value,
           const bool artifact,
           const uint32_t rarity);
  std::ostream &print(std::ostream &o);
  pair_t position;
  /* Need all these accessors because otherwise there is a *
   * circular dependancy that is difficult to get around.  */
  inline const std::string &get_name() const { return name; }
  inline const std::string &get_description() const { return description; }
  inline const object_type_t get_type() const { return type; }
  inline const uint32_t get_color() const { return color; }
  inline const int32_t &get_hit() const { return hit; }
  inline const dice &get_damage() const { return damage; }
  inline const int32_t &get_dodge() const { return dodge; }
  inline const int32_t &get_defence() const { return defence; }
  inline const int32_t &get_weight() const { return weight; }
  inline const int32_t &get_speed() const { return speed; }
  inline const int32_t &get_attribute() const { return attribute; }
  inline const int32_t &get_value() const { return value; }
};

#endif /* object.h */
