
// Copyright (C) 2008-2011 Gael Guennebaud <gael.guennebaud@inria.fr>

#include "Mesh.h"
#include "Shader.h"

#include <iostream>
#include <fstream>
#include <limits>

#include <QCoreApplication>
#include <Eigen/Geometry>
#include "../ObjFormat/ObjFormat.h"

using namespace Eigen;
using namespace std;

Mesh::Mesh(const std::string& filename)
    : mIsInitialized(false)
{
    std::string ext = filename.substr(filename.size()-3,3);
    if(ext=="off" || ext=="OFF")
        loadOFF(filename);
    else if(ext=="obj" || ext=="OBJ")
        loadOBJ(filename);
    else
        std::cerr << "Mesh: extension \'" << ext << "\' not supported." << std::endl;
}

void Mesh::loadOFF(const std::string& filename)
{
    std::ifstream in(filename.c_str(),std::ios::in);
    if(!in)
    {
        std::cerr << "File not found " << filename << std::endl;
        return;
    }

    std::string header;
    in >> header;

    // check the header file
    if(header != "OFF")
    {
        std::cerr << "Wrong header = " << header << std::endl;
        return;
    }

    int nofVertices, nofFaces, inull;
    int nb, id0, id1, id2;
    Vector3 v;

    in >> nofVertices >> nofFaces >> inull;

    for(int i=0 ; i<nofVertices ; ++i)
    {
        in >> v.x() >> v.y() >> v.z();
        mVertices.push_back(v);
    }

    for(int i=0 ; i<nofFaces ; ++i)
    {
        in >> nb >> id0 >> id1 >> id2;
        assert(nb==3);
        mFaces.push_back(FaceIndex(id0, id1, id2));
    }

    in.close();
}

void Mesh::loadOBJ(const std::string& filename)
{
    ObjMesh* pRawObjMesh = ObjMesh::LoadFromFile(filename);

    if (!pRawObjMesh)
    {
        std::cerr << "Mesh::loadObj: error loading file " << filename << "." << std::endl;
        return;
    }

    // Makes sure we have an indexed face set
    ObjMesh* pObjMesh = pRawObjMesh->createIndexedFaceSet(Obj::Options(Obj::AllAttribs|Obj::Triangulate));
    delete pRawObjMesh;
    pRawObjMesh = 0;

    // copy vertices
    mVertices.resize(pObjMesh->positions.size());

    for (int i=0 ; i<pObjMesh->positions.size() ; ++i)
    {
        mVertices[i] = Vertex(Vector3f(pObjMesh->positions[i]));

        if(!pObjMesh->texcoords.empty()) {
            mVertices[i].texcoord = Vector2f(pObjMesh->texcoords[i]);
            //      std::cerr << i << ": " << Vector2f(pObjMesh->texcoords[i]).transpose() << "\n";
        }

        if(!pObjMesh->normals.empty()) {
            mVertices[i].normal = Vector3f(pObjMesh->normals[i]);
            //std::cerr << i << ": " << Vector3f(pObjMesh->normals[i]).transpose() << "\n";
        }
    }

    // copy faces
    for (int smi=0 ; smi<pObjMesh->getNofSubMeshes() ; ++smi)
    {
        const ObjSubMesh* pSrcSubMesh = pObjMesh->getSubMesh(smi);

        mFaces.reserve(pSrcSubMesh->getNofFaces());

        for (uint fid = 0 ; fid<pSrcSubMesh->getNofFaces() ; ++fid)
        {
            ObjConstFaceHandle srcFace = pSrcSubMesh->getFace(fid);
            mFaces.push_back(Vector3i(srcFace.vPositionId(0), srcFace.vPositionId(1), srcFace.vPositionId(2)));
        }
    }

}

Mesh::~Mesh()
{
    if(mIsInitialized)
    {
        glDeleteBuffers(1,&mVertexBufferId);
        glDeleteBuffers(1,&mIndexBufferId);
    }
}

void Mesh::makeUnitary()
{
    // computes the lowest and highest coordinates of the axis aligned bounding box,
    // which are equal to the lowest and highest coordinates of the vertex positions.
    Eigen::Vector3f lowest, highest;
    lowest.fill(std::numeric_limits<float>::max());   // "fill" sets all the coefficients of the vector to the same value
    highest.fill(-std::numeric_limits<float>::max());

    for(VertexArray::iterator v_iter = mVertices.begin() ; v_iter!=mVertices.end() ; ++v_iter)
    {
        // - v_iter is an iterator over the elements of mVertices,
        //   an iterator behaves likes a pointer, it has to be dereferenced (*v_iter, or v_iter->) to access the referenced element.
        // - Here the .aray().min(_) and .array().max(_) operators work per component.
        //
        lowest  = lowest.array().min(v_iter->position.array());
        highest = highest.array().max(v_iter->position.array());
    }

    // TODO: appliquer une transformation à tous les sommets de mVertices de telle sorte
    // que la boite englobante de l'objet soit centrée en (0,0,0)  et que sa plus grande dimension soit de 1
    Eigen::Vector3f center = (lowest+highest)/2.0;
    float m = (highest-lowest).maxCoeff();
    for(VertexArray::iterator v_iter = mVertices.begin() ; v_iter!=mVertices.end() ; ++v_iter)
        v_iter->position = (v_iter->position - center) / m + Vector3f::UnitZ() / 2;
}


void Mesh::drawGeometry(int prg_id)
{
    if(!mIsInitialized)
    {
        mIsInitialized = true;
        // this is the first call to drawGeometry
        // => create the BufferObjects and copy the related data into them.
        glGenBuffers(1,&mVertexBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*mVertices.size(), mVertices[0].position.data(), GL_DYNAMIC_DRAW);

        glGenBuffers(1,&mIndexBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(FaceIndex)*mFaces.size(), mFaces[0].data(), GL_DYNAMIC_DRAW);
    }

    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferId);

    int vertex_loc   = glGetAttribLocation(prg_id, "vtx_position");
    int normal_loc   = glGetAttribLocation(prg_id, "vtx_normal");
    int texcoord_loc = glGetAttribLocation(prg_id, "vtx_texcoord");

    // specify the vertex data
    if(vertex_loc>=0)
    {
        glVertexAttribPointer(vertex_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(vertex_loc);
    }
    if(normal_loc>=0)
    {
        glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(Vector3f));
        glEnableVertexAttribArray(normal_loc);
    }
    if(texcoord_loc>=0)
    {
        glVertexAttribPointer(texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2*sizeof(Vector3f)));
        glEnableVertexAttribArray(texcoord_loc);
    }

    // send the geometry
    glDrawElements(GL_TRIANGLES, 3*mFaces.size(), GL_UNSIGNED_INT, (void*)0);

    // at this point the mesh has been drawn and raserized into the framebuffer!

    if(vertex_loc>=0)     glDisableVertexAttribArray(vertex_loc);
    if(normal_loc>=0)     glDisableVertexAttribArray(normal_loc);
    if(texcoord_loc>=0)   glDisableVertexAttribArray(texcoord_loc);
}

void Mesh::Initialize() {
    if(!mIsInitialized)
    {
        mIsInitialized = true;
        // this is the first call to drawGeometry
        // => create the BufferObjects and copy the related data into them.
        glGenBuffers(1,&mVertexBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*mVertices.size(), mVertices[0].position.data(), GL_DYNAMIC_DRAW);

        glGenBuffers(1,&mIndexBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(FaceIndex)*mFaces.size(), mFaces[0].data(), GL_DYNAMIC_DRAW);
    }
}

void Mesh::drawGeometry(int prg_id) const
{
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferId);

    int vertex_loc   = glGetAttribLocation(prg_id, "vtx_position");
    int normal_loc   = glGetAttribLocation(prg_id, "vtx_normal");
    int texcoord_loc = glGetAttribLocation(prg_id, "vtx_texcoord");

    // specify the vertex data
    if(vertex_loc>=0)
    {
        glVertexAttribPointer(vertex_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(vertex_loc);
    }
    if(normal_loc>=0)
    {
        glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(Vector3f));
        glEnableVertexAttribArray(normal_loc);
    }
    if(texcoord_loc>=0)
    {
        glVertexAttribPointer(texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2*sizeof(Vector3f)));
        glEnableVertexAttribArray(texcoord_loc);
    }

    // send the geometry
    glDrawElements(GL_TRIANGLES, 3*mFaces.size(), GL_UNSIGNED_INT, (void*)0);

    // at this point the mesh has been drawn and raserized into the framebuffer!

    if(vertex_loc>=0)     glDisableVertexAttribArray(vertex_loc);
    if(normal_loc>=0)     glDisableVertexAttribArray(normal_loc);
    if(texcoord_loc>=0)   glDisableVertexAttribArray(texcoord_loc);
}

void Mesh::computeNormals()
{
  // pass 1: set the normal to 0
  for(VertexArray::iterator v_iter = mVertices.begin() ; v_iter!=mVertices.end() ; ++v_iter)
    v_iter->normal.setZero();

  // pass 2: compute face normals and accumulate
  for(FaceIndexArray::iterator f_iter = mFaces.begin() ; f_iter!=mFaces.end() ; ++f_iter)
  {
    Vector3f v0 = mVertices[(*f_iter)(0)].position;
    Vector3f v1 = mVertices[(*f_iter)(1)].position;
    Vector3f v2 = mVertices[(*f_iter)(2)].position;

    Vector3f n = (v1-v0).cross(v2-v0).normalized();

    mVertices[(*f_iter)(0)].normal += n;
    mVertices[(*f_iter)(1)].normal += n;
    mVertices[(*f_iter)(2)].normal += n;
  }

  // pass 3: normalize
  for(VertexArray::iterator v_iter = mVertices.begin() ; v_iter!=mVertices.end() ; ++v_iter)
    v_iter->normal.normalize();
}

float Mesh::findZ(float x, float y) {
  float z = 2, zTmp = 0;
  int nb_vertices = 0;
  float zValues[3];
  for(VertexArray::iterator vx=mVertices.begin(); vx!=mVertices.end(); ++vx){
    if(vx->position.x() < x+1 && vx->position.x() > x-1
       && vx->position.y() <= y+1 && vx->position.y() > y-1 ){
      zValues[nb_vertices] = vx->position.z();
      nb_vertices ++;
      if(nb_vertices  == 3){
        break;
      }
    }
  }

  if(nb_vertices == 0){
    return z;
  }

  for(int i = 0; i < nb_vertices; i++)
    zTmp += zValues[i];
  z = zTmp/nb_vertices;
  return z;
}

Eigen::Vector3f Mesh::findNormal(float x, float y){
  Eigen::Vector3f normal = Eigen::Vector3f::UnitX();
  for(VertexArray::iterator vx=mVertices.begin(); vx!=mVertices.end(); ++vx){
    if(vx->position.x() <= x+0.5 && vx->position.x() > x-0.5
       && vx->position.y() < y+0.5 && vx->position.y() > y-0.5 ){
      if(vx->normal.x() >= 0 && vx->normal.y() >= 0 && vx->normal.z() >= 0){
        normal.x() = vx->normal.x();
        normal.y() = vx->normal.y();
        normal.z() = vx->normal.z();
      }
      else{
        normal.x() = 0;
        normal.y() = 0;
        normal.z() = 0;
      }
    }
  }
  return normal;
}

Eigen::Matrix4f Mesh::orientMesh(const Eigen::Vector3f& position, const Eigen::Vector3f& target, const Eigen::Vector3f& up){
    Eigen::Matrix4f tmp;
    Matrix3f R;
    Eigen::Vector3f vz = (position - target).normalized();
    Eigen::Vector3f vx = (up.cross(vz)).normalized();
    Eigen::Vector3f vy = (vz.cross(vx)).normalized();
    R.col(2)=vz;
    R.col(0)=vx;
    R.col(1)=vy;
    tmp.block<3,3>(0,0)=R.transpose();
    tmp.block<3,1>(0,3)=-(R.transpose()*position);
    return tmp;
}
