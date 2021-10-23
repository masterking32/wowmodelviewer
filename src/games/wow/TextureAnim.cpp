/*
 * TextureAnim.cpp
 *
 *  Created on: 21 oct. 2013
 *
 */

#include "TextureAnim.h"

#include "GL/glew.h"

void TextureAnim::calc(ssize_t anim, size_t time)
{
  if (trans.uses(anim)) {
    tval = trans.getValue(anim, time);
  }
  if (rot.uses(anim)) {
      //  rval = rot.getValue(anim, time); // @TODO to be fixed
  }
  if (scale.uses(anim)) {
         sval = scale.getValue(anim, time);
  }
}

void TextureAnim::setup(ssize_t anim)
{
  glLoadIdentity();
  if (trans.uses(anim)) {
    glTranslatef(tval.x, tval.y, tval.z);
  }
  if (rot.uses(anim)) {
    glRotatef(rval.x, 0, 0, 1); // this is wrong, I have no idea what I'm doing here ;)
  }
  if (scale.uses(anim)) {
    glScalef(sval.x, sval.y, sval.z);
  }
}

void TextureAnim::init(GameFile * f, M2TextureTransform &mta, std::vector<uint32> & global)
{
  trans.init(mta.translation, f, global);
  rot.init(mta.rotation, f, global);
  scale.init(mta.scaling, f, global);
}


