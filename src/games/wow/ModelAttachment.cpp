/*
 * ModelAttachment.cpp
 *
 *  Created on: 20 oct. 2013
 *
 */

#include "ModelAttachment.h"

#include "Bone.h"
#include "WoWModel.h"
#include "GL/glew.h"

#include "glm/gtc/type_ptr.hpp"



void ModelAttachment::init(M2Attachment &mad)
{
  pos = mad.position;
  bone = mad.bone;
  id = mad.id;
}

void ModelAttachment::setup() const
{
  glMultMatrixf(glm::value_ptr(model->bones[bone].mat));
  glTranslatef(pos.x, pos.y, pos.z);
}

void ModelAttachment::setupParticle() const
{
  glMultMatrixf(glm::value_ptr(model->bones[bone].mat));
  glTranslatef(pos.x, pos.y, pos.z);
}


