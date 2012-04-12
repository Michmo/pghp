////NOEXPEHEADER////
//-------------------------------------------------------------------------------
// The ObjFormat library
// Copyright (C) 2007  Gael Guennebaud
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// http://www.gnu.org/licenses/lgpl.html
//-------------------------------------------------------------------------------

#ifndef _ObjFormat_h_
#define _ObjFormat_h_

#include "ObjUtil.h"
#include <vector>
#include <map>
#include <assert.h>
#include <iostream>

namespace Obj
{
    enum Options {None=0,Texcoord=1,Normal=2,AllAttribs=3,Triangulate=4};
}

/** Represent a material.
    Legend:
 \b type type of the material (lambert,phong,blinn)
 \b Ka ambient color
 \b Kd diffuse color
 \b Ks specular color
 \b Ns shininess (cosine exponent for phong, eccentricity [0,1] for blinn)
 \b Tr transparency color (1 means opaque)
 \b Ni refractive index
 \b Bump the bump map
 \b Refl the environement texture
 
 \note The most important texture map if the diffuse one (MapKd).
*/
class ObjMaterial
{
public:
    
    ObjMaterial(const ObjString& name)
        : mName(name), mKa(0.2), mKd(0.8), mKs(0.), mTr(1.), mNs(0.), mNi(1.), mType("phong")
    {}
    
    const ObjString& getName(void) const {return mName;}
    
    const ObjString& getType(void) const {return mType;}
    
    const ObjVector3& getKa(void) const {return mKa;}
    const ObjVector3& getKd(void) const {return mKd;}
    const ObjVector3& getKs(void) const {return mKs;}
    const ObjVector3& getTr(void) const {return mTr;}
    float getNs(void) const {return mNs;}
    float getNi(void) const {return mNi;}
    
    const ObjString& getMapKa(void) const {return mMapKa;}
    const ObjString& getMapKd(void) const {return mMapKd;}
    const ObjString& getMapKs(void) const {return mMapKs;}
    const ObjString& getMapNs(void) const {return mMapNs;}
    const ObjString& getMapTr(void) const {return mMapTr;}
    const ObjString& getMapBump(void) const {return mMapBump;}
    const ObjString& getMapRefl(void) const {return mMapRefl;}
    
    void setType(const ObjString& type) {mType = type;}
    
    void setKa(const ObjVector3& ka) {mKa = ka;}
    void setKd(const ObjVector3& kd) {mKa = kd;}
    void setKs(const ObjVector3& ks) {mKa = ks;}
    void setTr(const ObjVector3& tr) {mTr = tr;}
    void setNs(float ns) {mNs = ns;}
    void setNi(float ni) {mNi = ni;}
    
    void setMapKa(const ObjString& mapname) {mMapKa = mapname;}
    void setMapKd(const ObjString& mapname) {mMapKd = mapname;}
    void setMapKs(const ObjString& mapname) {mMapKs = mapname;}
    void setMapNs(const ObjString& mapname) {mMapNs = mapname;}
    void setMapTr(const ObjString& mapname) {mMapTr = mapname;}
    void setMapBump(const ObjString& mapname) {mMapBump = mapname;}
    void setMapRefl(const ObjString& mapname) {mMapRefl = mapname;}
    
    ObjMaterial* clone(const ObjString& name) const;
    
    /** \name Generic property
        Allows to access to all properties including the standard ones (Ka,Kd,etc.)
        as well the ones which are not handled by the above functions (e.g., custum properties).
    */
    //@{
    bool hasProperty(const ObjString& key) const {return mProperties.find(key)!=mProperties.end();}
    ObjString getProperty(const ObjString& key) const;
    void setProperty(const ObjString& key,const ObjString& val) {mProperties[key] = val;}
    //@}
    
protected:

    ObjString mName;

    ObjVector3 mKa, mKd, mKs, mTr;
    float mNs, mNi;
    
    ObjString mMapKa, mMapKd, mMapKs, mMapNs, mMapTr, mMapBump, mMapRefl;
    ObjString mType;
    
    std::map<ObjString,ObjString> mProperties;
};


class ObjSubMesh;
class ObjMesh;

/** Allows to access and modify a face (polygon)
*/
class ObjFaceHandle
{
friend class ObjSubMesh;
public:
    
    inline unsigned int nofVertices(void) const;
    
    inline int materialId(void) const;
    
    inline int& vertexId(unsigned int i);
    inline int& vPositionId(unsigned int i);
    inline int& vTexcoordId(unsigned int i);
    inline int& vNormalId(unsigned int i);
    
protected:

    ObjFaceHandle(ObjSubMesh* pOwner, unsigned int faceId, unsigned int offset, unsigned int nofVertices);
    
    ObjSubMesh* mpOwner;
    unsigned int mFaceId;
    unsigned int mOffset;
    unsigned int mNofVertices;
};

/** Allows to access a face (polygon)
*/
class ObjConstFaceHandle
{
friend class ObjSubMesh;
public:

    inline unsigned int nofVertices(void) const;
    
    inline int materialId(void) const;
    
    inline int vertexId(unsigned int i) const;
    inline int vPositionId(unsigned int i) const;
    inline int vTexcoordId(unsigned int i) const;
    inline int vNormalId(unsigned int i) const;
    
protected:

    ObjConstFaceHandle(const ObjSubMesh* pOwner, unsigned int faceId, unsigned int offset, unsigned int nofVertices);
    
    const ObjSubMesh* mpOwner;
    unsigned int mFaceId;
    unsigned int mOffset;
    unsigned int mNofVertices;
};

/** A SubMesh is basically a list of faces (polygons).
*/
class ObjSubMesh
{
friend class ObjMesh;
friend class ObjFaceHandle;
friend class ObjConstFaceHandle;
public:

    /// Name of the sub mesh.
    const ObjString& getName(void) const {return mName;}
    void rename(const ObjString& name) {mName = name;}
    
    /** Returns true if all faces have the same material.
    */
    bool hasUniqueMaterial(void) const {return mFMaterialIds.empty();}
    
    /** Returns the default Material's id of the sub mesh.
    */
    int getMaterialId(void) const {return mDefaultMaterialId;}
    
    /** Set the default material's id and set it to all faces.
    */
    void setMaterialId(int matId);
    
    inline unsigned int getNofFaces(void) const {return mNofFaces;}
    
    inline ObjConstFaceHandle getFace(unsigned int fid) const
    {
        if (mFSizes.empty())
        {
            // means a constant number of vertices per face
            return ObjConstFaceHandle(this,fid,mConstNofVertices*fid,mConstNofVertices);
        }
        return ObjConstFaceHandle(this,fid,mFOffsets[fid],mFSizes[fid]);
    }
    inline ObjFaceHandle editFace(unsigned int fid)
    {
        if (mFSizes.empty())
        {
            // means a constant number of vertices per face
            return ObjFaceHandle(this,fid,mConstNofVertices*fid,mConstNofVertices);
        }
        return ObjFaceHandle(this,fid,mFOffsets[fid],mFSizes[fid]);
    }
    
    /** Return the number of vertices per face if it is constant, and -1 otherwise.
    */
    inline int getConstNofVerticesPerFace(void) const {return mConstNofVertices;}
    
    ObjFaceHandle createFace(unsigned int nofVertices, Obj::Options att, int matId=-1);
    
protected:

    ObjSubMesh(const ObjString& name, ObjMesh* pOwner)
        : mNofFaces(0), mConstNofVertices(-1), mName(name), mDefaultMaterialId(-1), mpOwner(pOwner)
    {}
    
    unsigned int mNofFaces;
    
    /// The list of face's sizes
    std::vector<unsigned int> mFSizes;
    int mConstNofVertices;
    
    /// The list of face's offsets
    std::vector<unsigned int> mFOffsets;
    
    /// The list of face's material indices.
    std::vector<int> mFMaterialIds;
    
    /// Pool of position indices
    std::vector<int> mVPositionIds;
    
    /// Pool of texcoord indices
    std::vector<int> mVTexcoordIds;
    
    /// Pool of normal indicies
    std::vector<int> mVNormalIds;
    
    ObjString mName;
    int mDefaultMaterialId;
    ObjMesh* mpOwner;
};



/** Represent a mesh.
    A mesh store several Materials and the list of vertex attributes (position, textcoord, normal).
    A Mesh is composed of several SubMesh.
*/
class ObjMesh
{
public:

    ObjMesh(void) {}
    
    ~ObjMesh();
    
    /** Create a mesh from a file.
        \note The filename extension must be either .obj of .obj.gz
        \return the mesh or 0 if the file failed to be load.
    */
    static ObjMesh* LoadFromFile(const ObjString& filename);
    
    /** Create a mesh where all the attribute indices of a vertex are the same
        and where all the faces of a sub mesh have the material and some number of vertices.
        \param options determines which attributes have to be preserved.
        For instance, if mode==None, then the normals and texcoords are lost.
        On the other hand, if mode contains Texcoord, then one vertex is created
        per pair of different position/texcoord and a vertex's position may appear several times.
        
        In particular, the returned mesh satisfies:
            - hasUniqueMaterial() is true for all sub meshes.
            - getConstNofVerticesPerFace()>=3 for all sub meshes.
            - only vertexId() (or vPositionId() that is an alias) is valid for the face
              handles and can be used to index the positions, texcoords and normals.
    */
    ObjMesh* createIndexedFaceSet(Obj::Options options = Obj::Options(Obj::AllAttribs|Obj::Triangulate)) const;
    
    /** Return the path to access texture file (i.e., the path containing the .obj file).
    */
    const ObjString& getTexturePath(void) const {return mTexturePath;}
    
    
    /// \name Material managment
    //@{
    ObjMaterial* getOrCreateMaterial(const ObjString& name = "");
    int getOrCreateMaterialId(const ObjString& name);
    unsigned int getNofMaterials(void) const {return mMaterials.size();}
    
    const ObjMaterial* getMaterial(int i) const {if (i<0||i>=int(mMaterials.size())) return 0; return mMaterials[i];}
    ObjMaterial* editMaterial(int i) {if (i<0||i>=int(mMaterials.size())) return 0; return mMaterials[i];}
    
    const ObjMaterial* getMaterial(const ObjString& name) const;
    ObjMaterial* editMaterial(const ObjString& name);
    //@}


    /// \name Sub-mesh managment
    //@{
    ObjSubMesh* createSubMesh(const ObjString& name = "");
    unsigned int getNofSubMeshes(void) const {return mSubMeshes.size();}
    
    const ObjSubMesh* getSubMesh(int i) const {return mSubMeshes[i];}
    ObjSubMesh* editSubMesh(int i) {return mSubMeshes[i];}
    
    const ObjSubMesh* getSubMesh(const ObjString& name) const;
    ObjSubMesh* editSubMesh(const ObjString& name);
    //@}
    

    /// List of vertex's positions
    std::vector<ObjVector3> positions;
    
    /// List of vertex's texture coordinates
    std::vector<ObjVector2> texcoords;
    
    /// List of vertex's normals
    std::vector<ObjVector3> normals;
    
protected:

    ObjString  createUsemapMaterial(const ObjString& usemap, const ObjString& usemtl = "");
    
    bool loadMaterialFile(const ObjString& filename);
    
    template <typename IStream> static ObjMesh* LoadFromStream(IStream& in, const ObjString& texturePath);
    
    ObjString findTexture(const ObjString& textureFile, bool isMBC = false) const;

    ObjString mTexturePath;

    std::vector<ObjSubMesh*> mSubMeshes;
    std::vector<ObjMaterial*> mMaterials;
    
    std::map<ObjString,ObjSubMesh*> mSubMeshesByName;
    std::map<ObjString,int> mMaterialsByName;
};

//-------------------------------------------------------------------------------
// inline implementation
//-------------------------------------------------------------------------------

inline ObjFaceHandle::ObjFaceHandle(ObjSubMesh* pOwner, unsigned int faceId, unsigned int offset, unsigned int nofVertices)
    : mpOwner(pOwner), mFaceId(faceId), mOffset(offset), mNofVertices(nofVertices)
{}

inline unsigned int ObjFaceHandle::nofVertices(void) const {return mNofVertices;}

inline int ObjFaceHandle::materialId(void) const
{
    if (mpOwner->mFMaterialIds.empty())
        return mpOwner->mDefaultMaterialId;
    return mpOwner->mFMaterialIds[mFaceId];
}

inline int& ObjFaceHandle::vertexId(unsigned int i)
{
    assert(i<nofVertices());
    unsigned int id = mOffset + i;
    assert(id<mpOwner->mVPositionIds.size());
    return mpOwner->mVPositionIds[id];
}

inline int& ObjFaceHandle::vPositionId(unsigned int i)
{
    assert(i<nofVertices());
    unsigned int id = mOffset + i;
    assert(id<mpOwner->mVPositionIds.size());
    return mpOwner->mVPositionIds[id];
}

inline int& ObjFaceHandle::vTexcoordId(unsigned int i)
{
    assert(i<nofVertices());
    unsigned int id = mOffset + i;
    assert(id<mpOwner->mVTexcoordIds.size());
    return mpOwner->mVTexcoordIds[id];
}

inline int& ObjFaceHandle::vNormalId(unsigned int i)
{
    assert(i<nofVertices());
    unsigned int id = mOffset + i;
    assert(id<mpOwner->mVNormalIds.size());
    return mpOwner->mVNormalIds[id];
}



inline ObjConstFaceHandle::ObjConstFaceHandle(const ObjSubMesh* pOwner, unsigned int faceId, unsigned int offset, unsigned int nofVertices)
    : mpOwner(pOwner), mFaceId(faceId), mOffset(offset), mNofVertices(nofVertices)
{}

inline unsigned int ObjConstFaceHandle::nofVertices(void) const {return mNofVertices;}

inline int ObjConstFaceHandle::materialId(void) const
{
    if (mpOwner->mFMaterialIds.empty())
        return mpOwner->mDefaultMaterialId;
    return mpOwner->mFMaterialIds[mFaceId];
}

inline int ObjConstFaceHandle::vertexId(unsigned int i) const
{
    assert(i<nofVertices());
    unsigned int id = mOffset + i;
    assert(id<mpOwner->mVPositionIds.size());
    return mpOwner->mVPositionIds[id];
}

inline int ObjConstFaceHandle::vPositionId(unsigned int i) const
{
    assert(i<nofVertices());
    unsigned int id = mOffset + i;
    assert(id<mpOwner->mVPositionIds.size());
    return mpOwner->mVPositionIds[id];
}

inline int ObjConstFaceHandle::vTexcoordId(unsigned int i) const
{
    assert(i<nofVertices());
    unsigned int id = mOffset + i;
    if (mpOwner->mVTexcoordIds.size()<=id)
        return -1;
    return mpOwner->mVTexcoordIds[id];
}

inline int ObjConstFaceHandle::vNormalId(unsigned int i) const
{
    assert(i<nofVertices());
    unsigned int id = mOffset + i;
    if (mpOwner->mVNormalIds.size()<=id)
        return -1;
    return mpOwner->mVNormalIds[id];
}


#endif
