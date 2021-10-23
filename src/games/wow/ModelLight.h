/*
 * ModelLight.h
 *
 *  Created on: 21 oct. 2013
 *
 */

#ifndef _MODELLIGHT_H_
#define _MODELLIGHT_H_

#include "animated.h"

class GameFile;

#include "GL/glew.h" // GLuint
#include "glm/glm.hpp"

struct ModelLight 
{
  ssize_t type;    // Light Type. MODELLIGHT_DIRECTIONAL = 0 or MODELLIGHT_POINT = 1
  ssize_t parent;    // Bone Parent. -1 if there isn't one.
  glm::vec3 pos, tpos, dir, tdir;
  Animated<glm::vec3> diffColor, ambColor;
  Animated<float> diffIntensity, ambIntensity, AttenStart, AttenEnd;
  Animated<int> UseAttenuation;

  void init(GameFile * f, M2Light &mld, std::vector<uint32> & global);
  void setup(size_t time, GLuint l);
};


#endif /* _MODELLIGHT_H_ */
