#ifndef DATABASE_H
#define DATABASE_H

// Combined the previous 5 various "db" files into one.
// trying to cut down on excess files.
// Also instead of declaring the db objects inside various classes
// may aswell declare them as globals since pretty much most the
// different objects need to access them at one point or another.

// STL
#include <vector>
#include <map>


#include <QString>

// wmv database
class ItemDatabase;
struct NPCRecord;

class ItemDatabase;

extern ItemDatabase items;
extern std::vector<NPCRecord> npcs;

// ==============================================

// -----------------------------------
// Item Stuff
// -----------------------------------

struct ItemRecord {
  QString name;
  int id, itemclass, subclass, type, model, sheath, quality;

  ItemRecord(const std::vector<QString> &);
  ItemRecord():id(0), itemclass(-1), subclass(-1), type(0), model(0), sheath(0), quality(0)
  {}

  int slot();
};

class ItemDatabase {
public:
  ItemDatabase();

  std::vector<ItemRecord> items;
  std::map<int, int> itemLookup;

  const ItemRecord& getById(int id);
};

// ============/////////////////=================/////////////////


// ------------------------------
// NPC Stuff
// -------------------------------
struct NPCRecord
{
  QString name;
  int id, model, type;

  NPCRecord(QString line);
  NPCRecord(const std::vector<QString> &);
  NPCRecord(): id(0), model(0), type(0) {}
  NPCRecord(const NPCRecord &r): name(r.name), id(r.id), model(r.model), type(r.type) {}

};
#endif

