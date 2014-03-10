#ifndef _MONSTER_TYPE_H_
#define _MONSTER_TYPE_H_

#include "glyph.h"
#include "enum.h"
#include "attack.h"
#include "entity_ai.h"
#include "dice.h"
#include <string>
#include <istream>
#include <vector>

struct Monster_genus;

struct Monster_type
{
  Monster_type();
  ~Monster_type();

  Monster_genus *genus; // See below
  std::string name;     // UNIQUE Data name
  std::string display_name; // Name as the player sees it; if blank, use (name)
  std::string display_name_plural;  // Plural name
  int uid;  // Unique UID for this type
  glyph sym;  // See glyph.h

  Dice hp_dice; // Dice to roll to determine HP; may be static!
  bool hp_set;  // Temp variable used to copy from genus
  int speed;    // 100 = player's base speed
  int chance;   // How frequently this appears
  std::vector<Attack> attacks;  // Melee attacks - see attack.h
  std::vector<Ranged_attack> ranged_attacks;  // See attack.h
  int total_attack_weight;  // Variable for choosing an attack
  int total_ranged_attack_weight; // Variable for choosing a ranged attack
  Entity_AI AI; // AI set

  void set_genus(Monster_genus *mg);
  void assign_uid(int id);
  std::string get_data_name();
  std::string get_name();
  std::string get_name_plural();
  bool load_data(std::istream &data);

  bool has_sense(Sense_type type);

private:
// These bools determine whether we've copied data from our genus
  bool attacks_copied_from_genus;
  bool ranged_attacks_copied_from_genus;
  bool senses_copied_from_genus;
  std::vector<bool> senses;

};

struct Monster_genus
{
  Monster_genus();
  ~Monster_genus();

  std::string name;
  std::string display_name;
  int uid;

  Monster_type default_values; // Default values for monsters in this genus
// Spawning stuff
  std::vector<Monster_type*> members;
  int total_chance;

  void add_member(Monster_type* member);
  Monster_type* random_member();

  void assign_uid(int id);
  std::string get_data_name();
  std::string get_name();
  bool load_data(std::istream &data);

};

#endif
