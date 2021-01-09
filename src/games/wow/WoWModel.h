#ifndef _WOWMODEL_H
#define _WOWMODEL_H

// C++ files
#include <map>
#include <set>
#include <vector>
//#include <stdlib.h>
//#include <crtdbg.h>

// Our files

#include "animated.h"
#include "AnimManager.h"
#include "Bone.h"
#include "CASCChunks.h"
#include "CharDetails.h"
#include "CharTexture.h"
#include "displayable.h"
#include "Model.h"
#include "ModelAttachment.h"
#include "ModelCamera.h"
#include "ModelColor.h"
#include "ModelEvent.h"
#include "modelheaders.h"
#include "ModelLight.h"
#include "ModelTransparency.h"
#include "particle.h"
#include "TabardDetails.h"
#include "TextureAnim.h"
#include "TextureManager.h"
#include "wow_enums.h"
#include "WoWItem.h"

#include "metaclasses/Container.h"

#include "glm/glm.hpp"

class CASCFile;
class GameFile;
class ModelRenderPass;

class QXmlStreamWriter;
class QXmlStreamReader;

#ifdef _WIN32
#    ifdef BUILDING_WOW_DLL
#        define _WOWMODEL_API_ __declspec(dllexport)
#    else
#        define _WOWMODEL_API_ __declspec(dllimport)
#    endif
#else
#    define _WOWMODEL_API_
#endif

#define TEXTURE_MAX 32

class _WOWMODEL_API_ WoWModel : public ManagedItem, public Displayable, public Model, public Container<WoWItem>
{
  // VBO Data
  GLuint vbuf, nbuf, tbuf;
  size_t vbufsize;

  // Non VBO Data
  GLuint dlist;
  bool forceAnim;

  // ===============================
  // Texture data
  // ===============================
  std::vector<GLuint> textures;
  std::vector<int> specialTextures;
  std::vector<GLuint> replaceTextures;

  inline void drawModel();
  void initCommon();
  bool isAnimated();
  void initAnimated();
  void initStatic();
  void initRaceInfos();


  void animate(ssize_t anim);
  void calcBones(ssize_t anim, size_t time);

  void lightsOn(GLuint lbase);
  void lightsOff(GLuint lbase);

  std::vector<uint16> boundTris;
  std::vector<glm::vec3> bounds;

  void refreshMerging();
  std::set<WoWModel *> mergedModels;

  // raw values read from file (useful for merging)
  std::vector<ModelVertex> rawVertices;
  std::vector<uint32> rawIndices;
  std::vector<ModelRenderPass *> rawPasses;
  std::vector<ModelGeosetHD *> rawGeosets;
  std::vector<uint32> skinFileIDs;
    
  void restoreRawGeosets();
  static bool sortPasses(ModelRenderPass* mrp1, ModelRenderPass* mrp2);
  
  std::vector<uint32> globalSequences;
  std::vector<ParticleSystem> particleSystems;
  std::vector<RibbonEmitter> ribbons;
  std::vector<TextureAnim> texAnims;
  std::vector<ModelColor> colors;
  std::vector<ModelTransparency> transparency;
  std::vector<ModelLight> lights;
  std::vector<ModelEvent> events;

  bool animGeometry, animBones;

  std::vector<AFID> readAFIDSFromFile(GameFile * f);
  void readAnimsFromFile(GameFile * f, std::vector<AFID> & afids, modelAnimData & data, uint32 nAnimations, uint32 ofsAnimation, uint32 nAnimationLookup, uint32 ofsAnimationLookup);
  std::vector<TXID> readTXIDSFromFile(GameFile * f);

public:
  bool model24500; // flag for build 24500 model changes to anim chunking and other things

  GameFile * gamefile;

  std::vector<uint> replacableParticleColorIDs;
  bool replaceParticleColors;
  
  int mergedModelType; // 1 for customization models (e.g. body parts) / -1 for gear (armour sections)
  
  uint nbLights() const
  {
    return lights.size();
  }

  // Start, Mid and End colours, for cases where the model's particle colours are
  // overridden by values from ParticleColor.dbc, indexed from CreatureDisplayInfo:
  typedef std::vector<glm::vec4> particleColorSet;

  // The particle will get its replacement colour set from 0, 1 or 2,
  // depending on whether its ParticleColorIndex is set to 11, 12 or 13:
  std::vector<particleColorSet> particleColorReplacements;
  // Raw Data
  std::vector<ModelVertex> origVertices;

  typedef int GeosetNum;

  glm::vec3 *normals;
  glm::vec2 *texCoords;
  glm::vec3 *vertices;
  std::vector<uint32> indices;
  // --

  WoWModel(GameFile * file, bool forceAnim = false);
  ~WoWModel();

  std::vector<ModelCamera> cam;
  std::string modelname;
  std::string lodname;

  std::vector<ModelRenderPass *> passes;
  std::vector<ModelGeosetHD *> geosets;

  // ===============================
  // Toggles
  bool showBones;
  bool showBounds;
  bool showWireframe;
  bool showParticles;
  bool showModel;
  bool showTexture;
  float alpha_;
  float scale_;

  // Position and rotation vector
  glm::vec3 pos_;
  glm::vec3 rot_;

  //
  bool ok;
  bool ind;
  bool hasCamera;
  bool hasParticles;
  bool isWMO;
  bool isMount;
  bool animated;

  // Misc values
  float rad;
  float trans;

  // -------------------------------

  // ===============================
  // Bone & Animation data
  // ===============================
  std::vector<ModelAnimation> anims;
  std::vector<int16> animLookups;
  AnimManager *animManager;
  std::vector<Bone> bones;

  size_t currentAnim;
  bool animcalc;
  size_t anim, animtime;

  void reset()
  {
    animcalc = false;
  }

  void update(int dt);

  // -------------------------------

  CharTexture tex;
  // -------------------------------

  // ===============================
  // 

  // ===============================
  // Rendering Routines
  // ===============================
  void drawBones();
  void drawBoundingVolume();
  void drawParticles();
  void draw();
  // -------------------------------

  void updateEmitters(float dt);
  void setLOD(int index);

  void setupAtt(int id);
  void setupAtt2(int id);

  std::vector<ModelAttachment> atts;
  static const size_t ATT_MAX = 60;
  int16 attLookup[ATT_MAX];
  int16 keyBoneLookup[BONE_MAX];

  ModelType modelType;
  CharModelDetails charModelDetails;
  CharDetails cd;
  RaceInfos infos;
  TabardDetails td;
  ModelHeader header;
  std::set<GeosetNum> creatureGeosetData;
  uint creatureGeosetDataID;
  bool bSheathe;
  bool mirrored_;

  friend class ModelRenderPass;

  WoWItem * getItem(CharSlots slot);
  int getItemId(CharSlots slot);
  bool isWearingARobe();

  void updateTextureList(GameFile * tex, int special);
  void displayHeader(ModelHeader & a_header);
  bool canSetTextureFromFile(int texnum);

  std::map<int, std::wstring> getAnimsMap();

  void save(QXmlStreamWriter &);
  void load(QString &);

  static QString getCGGroupName(CharGeosets cg);

  // @TODO use geoset id instead of geoset index in vector
  void showGeoset(uint geosetindex, bool value);  
  void hideAllGeosets();
  bool isGeosetDisplayed(uint geosetindex);
  void setGeosetGroupDisplay(CharGeosets group, int val);
  void setCreatureGeosetData(std::set<GeosetNum> cgd);

  WoWModel* mergeModel(QString & name, int type = 1, bool noRefresh = false);
  WoWModel* mergeModel(uint fileID, int type = 1, bool noRefresh = false);
  WoWModel* mergeModel(WoWModel * model, int type = 1, bool noRefresh = false);
  void unmergeModel(QString & name);
  void unmergeModel(uint fileID);
  void unmergeModel(WoWModel * model);
  WoWModel* getMergedModel(uint fileID);

  void refresh();
  
  QString getNameForTex(uint16 tex);
  GLuint getGLTexture(uint16 tex) const;
  void dumpTextureStatus();

  friend _WOWMODEL_API_ std::ostream& operator<<(std::ostream& out, const WoWModel& m);

  friend class ModelRenderPass; // to allow access to rendering elements (texAnims, etc.)
};


#endif
