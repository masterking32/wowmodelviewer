#ifndef WMO_H
#define WMO_H

// STL
#include <vector>
#include <string>

#include "GL/glew.h"
#include "glm/glm.hpp"


// Our headers
#include "displayable.h"
#include "manager.h"
#include "ModelManager.h"
#include "WMOFog.h"
#include "WMOLight.h"
#include "WMOModelInstance.h"
#include "WoWModel.h"

class WMO;
class WMOGroup;

class GameFile;

#define  WMO_MATERIAL_CULL  0x04  // Remove the back-facing polygons
#define WMO_MATERIAL_LUM  0x10  // Bright at Night
struct WMOMaterial {
  int flags;
  int SpecularMode;
  int transparent; // Blending: 0 for opaque, 1 for transparent
  int nameStart; // Start position for the first texture filename in the MOTX data block
  unsigned int color1;
  unsigned int flag1;
  int nameEnd; // Start position for the second texture filename in the MOTX data block
  unsigned int color2;
  unsigned int flag2;
  float f1,f2;
  int dx[5];
  // read up to here -_-
  GLuint tex;
};

struct WMOPV {
  glm::vec3 a,b,c,d;
};

struct WMOPR {
  short portal; // Portal index
  short group; // WMO group index
  short dir; // 1 or -1
  short reserved; // always 0
};

struct WMOVB {
  unsigned short firstVertex;
  unsigned short count;
};

struct WMODoodadSet {
  char name[0x14]; // set name
  int start; // index of first doodad instance in this set
  uint32 size; // number of doodad instances in this set
  int unused; // unused? (always 0)
};

struct WMOLiquidHeader {
  int X, Y, A, B;
  glm::vec3 pos;
  short type;
};

struct WMOHeader {
  int nTextures; // number of materials
  int nGroups; // number of WMO groups
  int nP; // number of portals
  int nLights; // number of lights
  int nModels; // number of M2 models imported
  int nDoodads; // number of dedicated files
  int nDoodadSets; // number of doodad sets
  unsigned int col; // ambient color? RGB
  int nX; // WMO ID (column 2 in WMOAreaTable.dbc)
  glm::vec3 v1; // Bounding box corner 1
  glm::vec3 v2; // Bounding box corner 2
  int LiquidType;
};


class WMO: public ManagedItem, public Displayable {
public:
  //WMOHeader header;
  uint32 nTextures; // number of materials
  uint32 nGroups; // number of WMO groups
  uint32 nP; // number of portals
  uint32 nLights; // number of lights
  uint32 nModels; // number of M2 models imported
  uint32 nDoodads; // number of dedicated files
  uint32 nDoodadSets; // number of doodad sets
  unsigned int col; // ambient color? RGB
  int nX; // WMO ID (column 2 in WMOAreaTable.dbc)
  glm::vec3 v1; // Bounding box corner 1
  glm::vec3 v2; // Bounding box corner 2
  int LiquidType;

  glm::vec3 maxCoord;
  glm::vec3 minCoord;

  WMOGroup *groups;
  WMOMaterial *mat;
  bool ok;
  char *groupnames;
  std::vector<QString> textures;
  std::vector<std::string> models;
  std::vector<WMOModelInstance> modelis;
  ModelManager loadedModels;

  glm::vec3 viewpos;
  glm::vec3 viewrot;

  std::vector<WMOLight> lights;
  std::vector<WMOPV> pvs;
  std::vector<WMOPR> prs;

  std::vector<WMOFog> fogs;

  std::vector<WMODoodadSet> doodadsets;

  WoWModel *skybox;
  int sbid;

  WMO(QString name);
  ~WMO();
  
  int doodadset;
  bool includeDefaultDoodads;
  
  void draw();
  void drawSkybox();
  void drawPortals();
  
  void update(int dt);

  void loadGroup(int id);
  void showDoodadSet(int id);
  void updateModels();

  static void flipcc(QString &);

};

#endif
