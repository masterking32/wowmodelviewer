/*
 * ModelAttachment.cpp
 *
 *  Created on: 20 oct. 2013
 *
 */

#include "ModelAttachment.h"

#include "Bone.h"
#include "matrix.h"
#include "WoWModel.h"
#include "GL/glew.h"



void ModelAttachment::init(ModelAttachmentDef &mad)
{
	pos = fixCoordSystem(mad.pos);
	bone = mad.bone;
	id = mad.id;
}

void ModelAttachment::setup()
{
	Matrix m = model->bones[bone].mat;
	m.transpose();
	glMultMatrixf(m);
	glTranslatef(pos.x, pos.y, pos.z);
}

void ModelAttachment::setupParticle()
{
	Matrix m = model->bones[bone].mat;
	m.transpose();
	glMultMatrixf(m);
	glTranslatef(pos.x, pos.y, pos.z);
}


