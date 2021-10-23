/*----------------------------------------------------------------------*\
| This file is part of WoW Model Viewer                                  |
|                                                                        |
| WoW Model Viewer is free software: you can redistribute it and/or      |
| modify it under the terms of the GNU General Public License as         |
| published by the Free Software Foundation, either version 3 of the     |
| License, or (at your option) any later version.                        |
|                                                                        |
| WoW Model Viewer is distributed in the hope that it will be useful,    |
| but WITHOUT ANY WARRANTY; without even the implied warranty of         |
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          |
| GNU General Public License for more details.                           |
|                                                                        |
| You should have received a copy of the GNU General Public License      |
| along with WoW Model Viewer.                                           |
| If not, see <http://www.gnu.org/licenses/>.                            |
\*----------------------------------------------------------------------*/

/*
 * FBXHeaders.cpp
 *
 *  Created on: 14 may 2019
 *   Copyright: 2019 , WoW Model Viewer (http://wowmodelviewer.net)
 */

#define _FBXHEADERS_CPP_
#include "FBXHeaders.h"
#undef _FBXHEADERS_CPP_

// Includes / class Declarations
//--------------------------------------------------------------------
// STL

// Qt
#include <qthreadpool.h>

// Externals
#include "fbxsdk.h"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/transform.hpp"


// Other libraries
#include "FBXAnimExporter.h"
#include "ModelRenderPass.h"
#include "WoWModel.h"

#include "util.h" // SLASH

// Current library


// Namespaces used
//--------------------------------------------------------------------

// Beginning of implementation
//--------------------------------------------------------------------

// Functions
//--------------------------------------------------------------------

bool FBXHeaders::createFBXHeaders(FbxString fileVersion, QString l_FileName, FbxManager* &l_Manager, FbxExporter* &l_Exporter, FbxScene* &l_Scene)
{
  //LOG_INFO << "Building FBX Header...";

  // Create an exporter.
  l_Exporter = 0;
  l_Exporter = FbxExporter::Create(l_Manager, "");

  if (!l_Exporter->Initialize(l_FileName.toStdString().c_str(), -1, l_Manager->GetIOSettings()))
  {
    LOG_ERROR << "Unable to create the FBX SDK exporter";
    return false;
  }
  //LOG_INFO << "FBX SDK exporter successfully created";

  // make file compatible with older fbx versions
  l_Exporter->SetFileExportVersion(fileVersion);

  l_Scene = FbxScene::Create(l_Manager, "My Scene");
  if (!l_Scene)
  {
    LOG_ERROR << "Unable to create FBX scene";
    return false;
  }

  // convert scene from Yup to Zup
  FbxAxisSystem conv(FbxAxisSystem::eMayaZUp); // we desire to convert the scene from Y-Up to Z-Up
  conv.ConvertScene(l_Scene);

  //LOG_INFO << "FBX SDK scene successfully created";
  return true;
}

// Create mesh.
FbxNode * FBXHeaders::createMesh(FbxManager* &l_manager, FbxScene* &l_scene, WoWModel * model, const glm::mat4 & matrix, const glm::vec3 & offset)
{
  // Create a node for the mesh.
  FbxNode *meshNode = FbxNode::Create(l_manager, qPrintable(model->name()));

  // Create new Matrix Data
  const auto m = glm::scale(glm::vec3(matrix[0][0], matrix[1][1], matrix[2][2]));

  // Create mesh.
  const auto num_of_vertices = model->origVertices.size();
  FbxMesh* mesh = FbxMesh::Create(l_manager, model->name().toStdString().c_str());
  mesh->InitControlPoints((int)num_of_vertices);
  FbxVector4* vertices = mesh->GetControlPoints();

  // Set the normals on Layer 0.
  auto layer = mesh->GetLayer(0);
  if (layer == nullptr)
  {
    mesh->CreateLayer();
    layer = mesh->GetLayer(0);
  }

  // We want to have one normal for each vertex (or control point),
  // so we set the mapping mode to eBY_CONTROL_POINT.
  FbxLayerElementNormal* layer_normal = FbxLayerElementNormal::Create(mesh, "");
  layer_normal->SetMappingMode(FbxLayerElement::eByControlPoint);
  layer_normal->SetReferenceMode(FbxLayerElement::eDirect);

  // Create UV for Diffuse channel.
  FbxLayerElementUV* layer_texcoord = FbxLayerElementUV::Create(mesh, "DiffuseUV");
  layer_texcoord->SetMappingMode(FbxLayerElement::eByControlPoint);
  layer_texcoord->SetReferenceMode(FbxLayerElement::eDirect);
  layer->SetUVs(layer_texcoord, FbxLayerElement::eTextureDiffuse);

  // Fill data.
  LOG_INFO << "Adding main mesh Verts...";
  for (size_t i = 0; i < num_of_vertices; i++)
  {
    M2Vertex &v = model->origVertices[i];
    glm::vec3 Position = glm::vec3(m * glm::vec4((v.pos + offset), 1.0f));
    vertices[i].Set(Position.x * SCALE_FACTOR, Position.y * SCALE_FACTOR, Position.z * SCALE_FACTOR);
    glm::vec3 vn = glm::normalize(v.normal);
    layer_normal->GetDirectArray().Add(FbxVector4(vn.x, vn.y, vn.z));
    layer_texcoord->GetDirectArray().Add(FbxVector2(v.tex_coords[0].x, 1.0 - v.tex_coords[0].y));
  }

  // Create polygons.
  size_t num_of_passes = model->passes.size();
  FbxLayerElementMaterial* layer_material = FbxLayerElementMaterial::Create(mesh, "");
  layer_material->SetMappingMode(FbxLayerElement::eByPolygon);
  layer_material->SetReferenceMode(FbxLayerElement::eIndexToDirect);
  layer->SetMaterials(layer_material);

  int mtrl_index = 0;
  LOG_INFO << "Setting main mesh Polys...";
  for (size_t i = 0; i < num_of_passes; i++)
  {
    ModelRenderPass * p = model->passes[i];
    if (p->init())
    {
      // Build material name.
      FbxString mtrl_name = "testToChange";
      mtrl_name.Append("_", 1);
      char tmp[32];
      _itoa((int)i, tmp, 10);
      mtrl_name.Append(tmp, strlen(tmp));
      FbxSurfaceMaterial* material = l_scene->GetMaterial(mtrl_name.Buffer());
      meshNode->AddMaterial(material);

      M2SkinSectionHD * g = model->geosets[p->geoIndex];
      size_t num_of_faces = g->indexCount / 3;
      for (size_t j = 0; j < num_of_faces; j++)
      {
        mesh->BeginPolygon(mtrl_index);
        mesh->AddPolygon(model->indices[g->indexStart + j * 3]);
        mesh->AddPolygon(model->indices[g->indexStart + j * 3 + 1]);
        mesh->AddPolygon(model->indices[g->indexStart + j * 3 + 2]);
        mesh->EndPolygon();
      }

      mtrl_index++;
    }
  }

  layer->SetNormals(layer_normal);

  // Set mesh smoothness.
  mesh->SetMeshSmoothness(FbxMesh::eFine);

  // Set the mesh as the node attribute of the node.
  meshNode->SetNodeAttribute(mesh);

  // Set the shading mode to view texture.
  meshNode->SetShadingMode(FbxNode::eTextureShading);

  return meshNode;
}

void FBXHeaders::createSkeleton(WoWModel * l_model, FbxScene *& l_scene, FbxNode *& l_skeletonNode, std::map<int, FbxNode*>& l_boneNodes)
{
  l_skeletonNode = FbxNode::Create(l_scene, qPrintable(QString::fromWCharArray(wxT("%1_rig")).arg(l_model->name())));
  FbxSkeleton* bone_group_skeleton_attribute = FbxSkeleton::Create(l_scene, "");
  bone_group_skeleton_attribute->SetSkeletonType(FbxSkeleton::eRoot);
  bone_group_skeleton_attribute->Size.Set(10.0 * SCALE_FACTOR);
  l_skeletonNode->SetNodeAttribute(bone_group_skeleton_attribute);

  std::vector<FbxSkeleton::EType> bone_types;
  size_t num_of_bones = l_model->bones.size();

  // Set bone type.
  std::vector<bool> has_children;
  has_children.resize(num_of_bones);
  for (size_t i = 0; i < num_of_bones; ++i)
  {
    Bone bone = l_model->bones[i];
    if (bone.parent != -1)
      has_children[bone.parent] = true;
  }

  bone_types.resize(num_of_bones);
  for (size_t i = 0; i < num_of_bones; ++i)
  {
    Bone bone = l_model->bones[i];

    if (bone.parent == -1)
    {
      bone_types[i] = FbxSkeleton::eRoot;
    }
    else if (has_children[i])
    {
      bone_types[i] = FbxSkeleton::eLimb;
    }
    else
    {
      bone_types[i] = FbxSkeleton::eLimbNode;
    }
  }

  // Create bone.
  for (size_t i = 0; i < num_of_bones; ++i)
  {
    Bone &bone = l_model->bones[i];
    glm::vec3 trans = bone.pivot;

    int pid = bone.parent;
    if (pid > -1)
      trans -= l_model->bones[pid].pivot;

    FbxString bone_name(qPrintable(l_model->name()));
    bone_name += "_bone_";
    bone_name += static_cast<int>(i);

    FbxNode* skeleton_node = FbxNode::Create(l_scene, bone_name);
    l_boneNodes[i] = skeleton_node;
    skeleton_node->LclTranslation.Set(FbxVector4(trans.x * SCALE_FACTOR, trans.y * SCALE_FACTOR, trans.z * SCALE_FACTOR));

    FbxSkeleton* skeleton_attribute = FbxSkeleton::Create(l_scene, bone_name);
    skeleton_attribute->SetSkeletonType(bone_types[i]);

    if (bone_types[i] == FbxSkeleton::eRoot)
    {
      skeleton_attribute->Size.Set(10.0 * SCALE_FACTOR);
      l_skeletonNode->AddChild(skeleton_node);
    }
    else if (bone_types[i] == FbxSkeleton::eLimb)
    {
      skeleton_attribute->LimbLength.Set(5.0 * SCALE_FACTOR * (sqrtf(trans.x * trans.x + trans.y * trans.y + trans.z * trans.z)));
      l_boneNodes[pid]->AddChild(skeleton_node);
    }
    else
    {
      skeleton_attribute->Size.Set(1.0 * SCALE_FACTOR);
      l_boneNodes[pid]->AddChild(skeleton_node);
    }

    skeleton_node->SetNodeAttribute(skeleton_attribute);
  }
}

// Protected methods
//--------------------------------------------------------------------

// Private methods
//--------------------------------------------------------------------

void FBXHeaders::storeBindPose(FbxScene* &l_scene, std::vector<FbxCluster*> l_boneClusters, FbxNode* l_meshNode)
{
  FbxPose* pose = FbxPose::Create(l_scene, "Bind Pose");
  pose->SetIsBindPose(true);

  for (auto it : l_boneClusters)
  {
    FbxNode*  node = it->GetLink();
    FbxMatrix matrix = node->EvaluateGlobalTransform();
    pose->Add(node, matrix);
  }

  pose->Add(l_meshNode, l_meshNode->EvaluateGlobalTransform());

  l_scene->AddPose(pose);
}

void FBXHeaders::storeRestPose(FbxScene* &l_scene, FbxNode* &l_SkeletonRoot)
{
  // Not ready yet...
  return;

  if (l_SkeletonRoot == NULL || l_SkeletonRoot == nullptr)
    return;
  FbxString lNodeName;
  FbxNode* lKFbxNode;
  FbxMatrix lTransformMatrix;
  FbxVector4 lT, lR, lS(1.0, 1.0, 1.0);

  // Create the rest pose
  FbxPose* pose = FbxPose::Create(l_scene, "Rest Pose");

  // Set the skeleton root node to the global position (10, 10, 10)
  // and global rotation of 45deg along the Z axis.
  lT.Set(10.0, 10.0, 10.0);
  lR.Set(0.0, 0.0, 45.0);

  lTransformMatrix.SetTRS(lT, lR, lS);

  // Add the skeleton root node to the pose
  lKFbxNode = l_SkeletonRoot;
  pose->Add(lKFbxNode, lTransformMatrix, false /*it's a global matrix*/);

  // Set the lLimbNode1 node to the local position of (0, 40, 0)
  // and local rotation of -90deg along the Z axis. This show that
  // you can mix local and global coordinates in a rest pose.
  lT.Set(0.0, 40.0, 0.0);
  lR.Set(0.0, 0.0, -90.0);
  lTransformMatrix.SetTRS(lT, lR, lS);

  // Add the skeleton second node to the pose
  lKFbxNode = lKFbxNode->GetChild(0);
  pose->Add(lKFbxNode, lTransformMatrix, true /*it's a local matrix*/);

  // Set the lLimbNode2 node to the local position of (0, 40, 0)
  // and local rotation of 45deg along the Z axis.
  lT.Set(0.0, 40.0, 0.0);
  lR.Set(0.0, 0.0, 45.0);
  lTransformMatrix.SetTRS(lT, lR, lS);

  // Add the skeleton second node to the pose
  lKFbxNode = lKFbxNode->GetChild(0);
  lNodeName = lKFbxNode->GetName();
  pose->Add(lKFbxNode, lTransformMatrix, true /*it's a local matrix*/);

  // Now add the pose to the scene
  l_scene->AddPose(pose);
}

void FBXHeaders::createAnimation(WoWModel * l_model, FbxScene *& l_scene, QString animName, M2Sequence cur_anim, std::map<int, FbxNode*>& skeleton)
{
  if (skeleton.empty())
  {
    LOG_ERROR << "No bones in skeleton, so animation will not be exported";
    return;
  }

  // Animation stack and layer.
  FbxAnimStack* anim_stack = FbxAnimStack::Create(l_scene, qPrintable(animName));
  FbxAnimLayer* anim_layer = FbxAnimLayer::Create(l_scene, qPrintable(animName));
  anim_stack->AddMember(anim_layer);

  //LOG_INFO << "Animation length:" << cur_anim.length;
  float timeInc = cur_anim.duration / 60;
  if (timeInc < 1.0f)
  {
    timeInc = cur_anim.duration;
  }
  FbxTime::SetGlobalTimeMode(FbxTime::eFrames60);
  //LOG_INFO << "Skeleton Bone count:" << skeleton.size();

  //LOG_INFO << "Starting frame loop...";
  for (uint32 t = 0; t < cur_anim.duration; t += timeInc)
  {
    //LOG_INFO << "Starting frame" << t;
    FbxTime time;
    time.SetSecondDouble((float)t / 1000.0);

    for (auto it : skeleton)
    {
      int b = it.first;
      Bone& bone = l_model->bones[b];

      bool rot = bone.rot.uses(cur_anim.aliasNext);
      bool scale = bone.scale.uses(cur_anim.aliasNext);
      bool trans = bone.trans.uses(cur_anim.aliasNext);

      if (!rot && !scale && !trans) // bone is not animated, skip it
        continue;

      if (trans)
      {
        FbxAnimCurve* t_curve_x = skeleton[b]->LclTranslation.GetCurve(anim_layer, FBXSDK_CURVENODE_COMPONENT_X, true);
        FbxAnimCurve* t_curve_y = skeleton[b]->LclTranslation.GetCurve(anim_layer, FBXSDK_CURVENODE_COMPONENT_Y, true);
        FbxAnimCurve* t_curve_z = skeleton[b]->LclTranslation.GetCurve(anim_layer, FBXSDK_CURVENODE_COMPONENT_Z, true);

        glm::vec3 v = bone.trans.getValue(cur_anim.aliasNext, t);

        if (bone.parent != -1)
        {
          Bone& parent_bone = l_model->bones[bone.parent];
          v += (bone.pivot - parent_bone.pivot);
        }

        t_curve_x->KeyModifyBegin();
        int key_index = t_curve_x->KeyAdd(time);
        t_curve_x->KeySetValue(key_index, v.x * SCALE_FACTOR);
        t_curve_x->KeySetInterpolation(key_index, bone.trans.type == INTERPOLATION_LINEAR ? FbxAnimCurveDef::eInterpolationLinear : FbxAnimCurveDef::eInterpolationCubic);
        t_curve_x->KeyModifyEnd();

        t_curve_y->KeyModifyBegin();
        key_index = t_curve_y->KeyAdd(time);
        t_curve_y->KeySetValue(key_index, v.y * SCALE_FACTOR);
        t_curve_y->KeySetInterpolation(key_index, bone.trans.type == INTERPOLATION_LINEAR ? FbxAnimCurveDef::eInterpolationLinear : FbxAnimCurveDef::eInterpolationCubic);
        t_curve_y->KeyModifyEnd();

        t_curve_z->KeyModifyBegin();
        key_index = t_curve_z->KeyAdd(time);
        t_curve_z->KeySetValue(key_index, v.z * SCALE_FACTOR);
        t_curve_z->KeySetInterpolation(key_index, bone.trans.type == INTERPOLATION_LINEAR ? FbxAnimCurveDef::eInterpolationLinear : FbxAnimCurveDef::eInterpolationCubic);
        t_curve_z->KeyModifyEnd();
      }

      if (rot)
      {
        FbxAnimCurve* r_curve_x = skeleton[b]->LclRotation.GetCurve(anim_layer, FBXSDK_CURVENODE_COMPONENT_X, true);
        FbxAnimCurve* r_curve_y = skeleton[b]->LclRotation.GetCurve(anim_layer, FBXSDK_CURVENODE_COMPONENT_Y, true);
        FbxAnimCurve* r_curve_z = skeleton[b]->LclRotation.GetCurve(anim_layer, FBXSDK_CURVENODE_COMPONENT_Z, true);

        auto r = glm::eulerAngles(bone.rot.getValue(cur_anim.aliasNext, t));

        auto x = glm::degrees(r.x);
        auto y = glm::degrees(r.y);
        auto z = glm::degrees(r.z);
       
        r_curve_x->KeyModifyBegin();
        int key_index = r_curve_x->KeyAdd(time);
        r_curve_x->KeySetValue(key_index, x);
        r_curve_x->KeySetInterpolation(key_index, bone.rot.type == INTERPOLATION_LINEAR ? FbxAnimCurveDef::eInterpolationLinear : FbxAnimCurveDef::eInterpolationCubic);
        r_curve_x->KeyModifyEnd();

        r_curve_y->KeyModifyBegin();
        key_index = r_curve_y->KeyAdd(time);
        r_curve_y->KeySetValue(key_index, y);
        r_curve_y->KeySetInterpolation(key_index, bone.rot.type == INTERPOLATION_LINEAR ? FbxAnimCurveDef::eInterpolationLinear : FbxAnimCurveDef::eInterpolationCubic);
        r_curve_y->KeyModifyEnd();

        r_curve_z->KeyModifyBegin();
        key_index = r_curve_z->KeyAdd(time);
        r_curve_z->KeySetValue(key_index, z);
        r_curve_z->KeySetInterpolation(key_index, bone.rot.type == INTERPOLATION_LINEAR ? FbxAnimCurveDef::eInterpolationLinear : FbxAnimCurveDef::eInterpolationCubic);
        r_curve_z->KeyModifyEnd();
      }

      if (scale)
      {
        FbxAnimCurve* s_curve_x = skeleton[b]->LclScaling.GetCurve(anim_layer, FBXSDK_CURVENODE_COMPONENT_X, true);
        FbxAnimCurve* s_curve_y = skeleton[b]->LclScaling.GetCurve(anim_layer, FBXSDK_CURVENODE_COMPONENT_Y, true);
        FbxAnimCurve* s_curve_z = skeleton[b]->LclScaling.GetCurve(anim_layer, FBXSDK_CURVENODE_COMPONENT_Z, true);

        glm::vec3 v = bone.scale.getValue(cur_anim.aliasNext, t);

        s_curve_x->KeyModifyBegin();
        int key_index = s_curve_x->KeyAdd(time);
        s_curve_x->KeySetValue(key_index, v.x);
        s_curve_x->KeySetInterpolation(key_index, bone.scale.type == INTERPOLATION_LINEAR ? FbxAnimCurveDef::eInterpolationLinear : FbxAnimCurveDef::eInterpolationCubic);
        s_curve_x->KeyModifyEnd();

        s_curve_y->KeyModifyBegin();
        key_index = s_curve_y->KeyAdd(time);
        s_curve_y->KeySetValue(key_index, v.y);
        s_curve_y->KeySetInterpolation(key_index, bone.scale.type == INTERPOLATION_LINEAR ? FbxAnimCurveDef::eInterpolationLinear : FbxAnimCurveDef::eInterpolationCubic);
        s_curve_y->KeyModifyEnd();

        s_curve_z->KeyModifyBegin();
        key_index = s_curve_z->KeyAdd(time);
        s_curve_z->KeySetValue(key_index, v.z);
        s_curve_z->KeySetInterpolation(key_index, bone.scale.type == INTERPOLATION_LINEAR ? FbxAnimCurveDef::eInterpolationLinear : FbxAnimCurveDef::eInterpolationCubic);
        s_curve_z->KeyModifyEnd();
      }
    }

    //LOG_INFO << "Ended frame" << t;
  }
}