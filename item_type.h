#ifndef _ITEM_TYPE_H_
#define _ITEM_TYPE_H_

#include "glyph.h"
#include "enum.h"
#include "dice.h"
#include "tool.h"
#include "status_effect.h"
#include "skill.h"  // For launcher skill (handgun, rifle, etc) and books
#include <string>
#include <istream>
#include <vector>

/* After adding an Item_class, be sure to edit Item_type.cpp and add the class
 * to the function item_class_name().
 */
enum Item_class
{
  ITEM_CLASS_MISC = 0,
  ITEM_CLASS_CLOTHING,
  ITEM_CLASS_AMMO,
  ITEM_CLASS_LAUNCHER,
  ITEM_CLASS_FOOD,
  ITEM_CLASS_TOOL,
  ITEM_CLASS_BOOK,
  ITEM_CLASS_CONTAINER,
  ITEM_CLASS_CORPSE,
  ITEM_CLASS_MAX
};

Item_class lookup_item_class(std::string name);
std::string item_class_name_plural(Item_class iclass);
std::string item_class_name(Item_class iclass, bool plural = false);

Item_flag lookup_item_flag(std::string name);
std::string item_flag_name(Item_flag flag);

enum Book_genre
{
  GENRE_NULL = 0,
  GENRE_AUTOBIOGRAPHY,
  GENRE_HISTORY,
  GENRE_NOVEL,    // Typeless novel.
  GENRE_SCIFI,
  GENRE_FANTASY,
  GENRE_MYSTERY,
  GENRE_ROMANCE,
  GENRE_MAX
};

Book_genre lookup_book_genre(std::string name);
std::string book_genre_name(Book_genre genre);

class Item_type
{
public:
  Item_type();
  virtual ~Item_type();

  int uid;
  std::string name;
  std::string display_name;
  std::string plural_name;
  std::string description;
  int weight;       // In 1/10ths of a pound / 0.045 kg (sorry)
  int volume;       // 1 volume = a ping pong ball
  glyph sym;

  int damage[DAMAGE_MAX];
  int to_hit;
  int attack_speed;
  Dice  thrown_variance;// Angle our thrown attack is off by, in 1/10ths of
                        // a degree; defaults to 5d20!
  int thrown_dmg_percent; // Percent of normal damage done when thrown
  int thrown_speed; // AP cost of throwing; if 0, it's calculated

  int mission_experience; // How much XP does a mission using this item reward
  std::string container;  // Name of another item type!

  void assign_uid(int id);
  std::string get_data_name();
  std::string get_name();
  virtual std::string get_name_singular();  // Slightly more verbose
  virtual std::string get_name_plural();
  virtual bool load_data(std::istream &data);
  virtual bool handle_data(std::string ident, std::istream &data);

  virtual Item_class get_class() { return ITEM_CLASS_MISC; }
  virtual Item_action default_action() { return IACT_NULL; }
  virtual std::string get_property_description() { return ""; }
  virtual int  time_to_reload()  { return 0; }
  virtual int  time_to_fire()    { return 0; }
  virtual int  default_charges() { return 0; }
  virtual bool uses_charges()       { return false; }
  virtual bool always_combines()    { return true;  }
  virtual bool combine_by_charges() { return false; }
  virtual int  time_to_read()       { return 0; }
  bool has_flag(Item_flag flag);

private:
  std::vector<bool> flags;
};

class Item_type_clothing : public Item_type
{
public:
  Item_type_clothing();
  ~Item_type_clothing(){}

  virtual Item_class get_class() { return ITEM_CLASS_CLOTHING; }
  virtual Item_action default_action() { return IACT_WEAR; }
  virtual std::string get_property_description();

  virtual bool handle_data(std::string ident, std::istream &data);

  int carry_capacity;
  int armor_bash;
  int armor_cut;
  int armor_pierce;
  int protection; // Protection from Status_effects that use this vector
  int encumbrance;
  bool covers[BODY_PART_MAX];

};

class Item_type_ammo : public Item_type
{
public:
  Item_type_ammo();
  ~Item_type_ammo(){}

  virtual Item_class get_class() { return ITEM_CLASS_AMMO; }

  virtual std::string get_name_singular();  // Slightly more verbose
  virtual std::string get_name_plural();
  virtual std::string get_property_description();

  virtual bool handle_data(std::string ident, std::istream &data);

  virtual int default_charges() { return count; }
  virtual bool combine_by_charges() { return true;  }
  virtual bool uses_charges()       { return true;  }

  std::string ammo_type;  // Ammo type - links this to a launcher
  int damage;       // Base damage
  int armor_pierce; // Armor ignored
  int range;
  Dice accuracy;    // Low is good!  In 1/10ths of a degree
  int count;        // How many to a box
  int pellets;      // How many projectiles in a single shot?  Only for 00 shot.
};

class Item_type_launcher : public Item_type
{
public:
  Item_type_launcher();
  ~Item_type_launcher(){}

  virtual Item_class get_class() { return ITEM_CLASS_LAUNCHER; }

  virtual bool handle_data(std::string ident, std::istream &data);

  virtual std::string get_property_description();
  virtual int time_to_reload() { return reload_ap; }
  virtual int time_to_fire()   { return fire_ap; }
  virtual bool uses_charges()       { return true;  }

// TODO: Add in gun class / skill used / however we do that
  std::string ammo_type;  // Ammo type - links this to a launcher
  int damage;     // Damage bonus
  Dice accuracy;   // Low is good!  In 1/10ths of a degree
  int recoil;     // Recoil added
  int durability; // HP basically
  int capacity;   // Shots per reload
  int reload_ap;  // action_points per reload
  int fire_ap;    // action_points per shot fired
  Skill_type skill_used;  // Handguns, shotguns, SMGs, or rifles
  std::vector<int> modes; // Each element is a number of shots

};

class Item_type_food : public Item_type
{
public:
  Item_type_food();
  ~Item_type_food(){};

  virtual Item_class get_class() { return ITEM_CLASS_FOOD; }
  virtual Item_action default_action() { return IACT_EAT; }
  virtual std::string get_property_description();
  virtual int  default_charges() { return charges; }
  virtual bool uses_charges()       { return (charges > 1); }

  virtual bool handle_data(std::string ident, std::istream &data);

// TODO: Add morale boost?

  int food;
  int water;
  int effect_chance;    // Chance to inflict the effect below
  Status_effect effect; // Special effect inflicted; see status_effect.h
  int charges;  // So that you can partially consume a food item
  std::string verb; // Verb for consuming this.  "You ____ your <name>"

};

class Item_type_tool : public Item_type
{
public:
  Item_type_tool();
  ~Item_type_tool(){};

  virtual Item_class get_class() { return ITEM_CLASS_TOOL; }
  virtual Item_action default_action() { return IACT_APPLY; }
  virtual int default_charges() { return def_charges; }
  virtual std::string get_property_description();

  virtual bool handle_data(std::string ident, std::istream &data);

  virtual bool uses_charges(); // true if max_charges > 0 && charges_per_use > 0
  virtual bool always_combines() { return false; }

  Tool_action applied_action;   // Action when applied
  Tool_action powered_action;   // Action every turn, while powered
  Tool_action countdown_action; // Action when countdown finishes

  int def_charges;  // Charges it starts with
  int max_charges;      // Max charges.  If 0, doesn't use charges.
  int subcharges;       // How many turns, while powered, until we use 1 charge
  int countdown_timer;  // How many turns after applying before countdown_action
  std::string fuel;     // Ammo name - matches this with an ammo type for fuel
  std::string powered_text; // Text for when it's powered; e.g. "on", "active"
};


class Item_type_book : public Item_type
{
  public:
  Item_type_book();
  ~Item_type_book(){}

  virtual Item_class get_class() { return ITEM_CLASS_BOOK; }
  virtual Item_action default_action() { return IACT_READ; }

  virtual std::string get_name_singular();  // Slightly more verbose
  virtual std::string get_name_plural();
  virtual std::string get_property_description();

  virtual int time_to_read();

  virtual bool handle_data(std::string ident, std::istream& data);

  Skill_type skill_learned;
  Book_genre genre;
  int cap_limit;  // What's the highest cap this will take us to?
  int int_required;  // What intelligence do we need to read this?
  int skill_required; // What skill level do we need to read this?
  int high_int_bonus; // How much extra do we get for high intelligence?
  int bonus_int_required; // What intelligence do we need to get the bonus?

  int fun;          // Morale gained (or lost) from reading this
  int chapters;     // How many times can we read this before it's "used up?"
                    // (Chapters only applies to non-skill books.)
};

class Item_type_container : public Item_type
{
public:
  Item_type_container();
  ~Item_type_container(){}

  virtual Item_class get_class() { return ITEM_CLASS_CONTAINER; }
  virtual Item_action default_action() { return IACT_UNLOAD; }
  virtual std::string get_property_description();

  virtual bool always_combines() { return false; }

  virtual bool handle_data(std::string ident, std::istream &data);

  int capacity; // Volume capacity, that is
  std::string preposition; // A plastic bottle <preposition> water, e.g. "of"
  bool use_article; // If true: "A plastic bottle <preposition> SOME water
};

class Item_type_corpse : public Item_type
{
public:
  Item_type_corpse();
  ~Item_type_corpse(){}

  virtual Item_class get_class() { return ITEM_CLASS_CORPSE; }

  virtual bool handle_data(std::string ident, std::istream &data);
};

#endif
