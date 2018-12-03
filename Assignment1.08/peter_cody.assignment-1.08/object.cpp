//
//  item.cpp
//  
//
//  Created by Cody Peter on 11/4/18.
//

#include "object.h"

void object::set(const std::string &name,
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
                 const bool art,
                 const uint32_t rrty)
{
    this->name = name;
    this->description = description;
    this->type = type;
    this->color = color;
    this->hit = hit;
    this->damage = damage;
    this->dodge = dodge;
    this->attribute = attrubute;
    this->value = value;
    this->artifact = art;
    this->rarity = rrty;
}
