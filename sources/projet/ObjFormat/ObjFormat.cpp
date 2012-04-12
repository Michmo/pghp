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

#include "ObjFormat.h"
#include <iostream>
#include <fstream>
#include <assert.h>
#ifdef HAS_GZSTREAM
#include "gzstream.h"
#include <dirent.h>
#endif

//--------------------------------------------------------------------------------
// ObjMaterial implementation
//--------------------------------------------------------------------------------

ObjMaterial* ObjMaterial::clone(const ObjString& name) const
{
    ObjMaterial* pClone = new ObjMaterial(name);

    pClone->mKa = this->mKa;
    pClone->mKd = this->mKd;
    pClone->mKs = this->mKs;
    pClone->mNs = this->mNs;
    pClone->mTr = this->mTr;
    pClone->mNi = this->mNi;

    pClone->mType = this->mType;

    pClone->mMapKa = this->mMapKa;
    pClone->mMapKd = this->mMapKd;
    pClone->mMapKs = this->mMapKs;
    pClone->mMapNs = this->mMapNs;
    pClone->mMapNs = this->mMapTr;
    pClone->mMapBump = this->mMapBump;
    pClone->mMapRefl = this->mMapRefl;

    pClone->mProperties = this->mProperties;

    return pClone;
}

ObjString ObjMaterial::getProperty(const ObjString& key) const
{
    std::map<ObjString,ObjString>::const_iterator it = mProperties.find(key);
    if (it!=mProperties.end())
    {
        return it->second;
    }
    return "";
}

//--------------------------------------------------------------------------------
// ObjSubMesh implementation
//--------------------------------------------------------------------------------

void ObjSubMesh::setMaterialId(int matId)
{
    mDefaultMaterialId = matId;
    mFMaterialIds.clear();
}

ObjFaceHandle ObjSubMesh::createFace(unsigned int nofVertices, Obj::Options att, int matId/*=-1*/)
{
    assert(matId<0 || mpOwner->getMaterial(matId)!=0 && "The specified material's id is not valid");

    if (mNofFaces==0)
    {
        // first face, let us assume a constant nof vertices per face
        mConstNofVertices = nofVertices;
    }
    else if (!mFSizes.empty())
    {
        mFSizes.push_back(nofVertices);
        mFOffsets.push_back(mVPositionIds.size());
    }
    else if (mConstNofVertices!=int(nofVertices))
    {
        mFSizes.resize(mNofFaces);
        mFOffsets.resize(mNofFaces);
        for (unsigned int i=0 ; i<mNofFaces ; ++i)
        {
            mFSizes[i] = mConstNofVertices;
            mFOffsets[i] = mConstNofVertices * i;
        }
        mConstNofVertices = -1;

        mFSizes.push_back(nofVertices);
        mFOffsets.push_back(mVPositionIds.size());
    }

    if (!mFMaterialIds.empty())
    {
        if (matId<0)
            mFMaterialIds.push_back(mDefaultMaterialId);
        else
            mFMaterialIds.push_back(matId);
    }
    else if (matId>=0)
    {
        if (mDefaultMaterialId<0)
        {
            // first valid material id
            mDefaultMaterialId = matId;
        }
        else if (matId!=mDefaultMaterialId)
        {
            // first different material id
            mFMaterialIds.resize(mNofFaces);
            for (unsigned int i=0 ; i<mNofFaces ; ++i)
            {
                mFMaterialIds[i] = mDefaultMaterialId;
            }
            mFMaterialIds.push_back(matId);
        }
    }

    mNofFaces++;

    mVPositionIds.resize(mVPositionIds.size()+nofVertices);

    if (att&Obj::Texcoord)
    {
        int oldSize = mVTexcoordIds.size();
        mVTexcoordIds.resize(mVTexcoordIds.size()+nofVertices);
        for (unsigned int i=oldSize ; i<mVTexcoordIds.size() ; ++i)
            mVTexcoordIds[i] = -1;
    }

    if (att&Obj::Normal)
    {
        int oldSize = mVNormalIds.size();
        mVNormalIds.resize(mVNormalIds.size()+nofVertices);
        for (unsigned int i=oldSize ; i<mVNormalIds.size() ; ++i)
            mVNormalIds[i] = -1;
    }

    return editFace(mNofFaces-1);
}

//--------------------------------------------------------------------------------
// ObjMesh implementation
//--------------------------------------------------------------------------------

ObjMesh::~ObjMesh()
{
    for (unsigned int i=0 ; i<mMaterials.size() ; ++i)
        delete mMaterials[i];

    for (unsigned int i=0 ; i<mSubMeshes.size() ; ++i)
        delete mSubMeshes[i];
}

ObjString ObjMesh::createUsemapMaterial(const ObjString& usemap, const ObjString& usemtl /*= ""*/)
{
    ObjString matName;
    if (usemtl!="")
        matName = usemtl + "/" + usemap;
    else
        matName = usemap;

    ObjMaterial* pMat;

    pMat = editMaterial(matName);
    if (pMat)
        return matName;

    pMat = editMaterial(usemtl);
    if (pMat)
        pMat = pMat->clone(matName);
    else
        pMat = new ObjMaterial(matName);

    pMat->setMapKd(usemap);
    mMaterials.push_back(pMat);
    mMaterialsByName[matName] = mMaterials.size() - 1;

    return matName;
}

ObjSubMesh* ObjMesh::createSubMesh(const ObjString& aName /*= ""*/)
{
    ObjString name = aName;
    ObjSubMesh* pSubMesh = 0;
    if (name=="")
    {
        name = ObjString("submesh_") << mSubMeshes.size();
    }
    else
    {
        pSubMesh = editSubMesh(name);
        if (pSubMesh)
            return pSubMesh;
    }

    pSubMesh = new ObjSubMesh(name,this);
    mSubMeshes.push_back(pSubMesh);
    mSubMeshesByName[name] = pSubMesh;

    return pSubMesh;
}

ObjMaterial* ObjMesh::getOrCreateMaterial(const ObjString& aName /*= ""*/)
{
    ObjString name = aName;
    ObjMaterial* pMat = 0;
    if (name=="")
    {
        name = ObjString("Material_") << mMaterials.size();
    }
    else
    {
        pMat = editMaterial(name);
        if (pMat)
            return pMat;
    }

    pMat = new ObjMaterial(name);
    mMaterialsByName[name] = mMaterials.size();
    mMaterials.push_back(pMat);

    return pMat;
}

ObjString ObjMesh::findTexture(const ObjString& _textureFile,bool isMBC) const
{
    ObjString textureFile = _textureFile;
    if (isMBC)
    {
        // clean the filename
        textureFile = _textureFile.split("/\\").back();
    }

    ObjString completefilename = mTexturePath + textureFile;

    if (!findFile(completefilename))
    {
        std::cout << "ObjLoader::Warning - texture file " << textureFile << " not found.\n";
        return textureFile;
    }

    ObjString localpath;
    ObjString filename;
    textureFile.decomposePathAndFile(localpath,filename);

    completefilename = localpath + completefilename.split("/\\").back();
    std::cout << "findTexture: " << textureFile << " -> " << completefilename << "\n";
    return completefilename;
}

bool ObjMesh::loadMaterialFile(const ObjString& filename)
{
    std::ifstream in(filename.c_str());
    if (!in)
    {
        std::cerr << "ObjLoader::Error - Could not open input mtl file \"" << filename << "\"\n";
        return false;
    }

    // does the material file comes from the ModelBankCollection
    bool isMBC = false;

    std::vector<ObjString> words;
    ObjString command, args;

    const int maxBuffersize = 2048;
    char buffer[maxBuffersize];

    ObjMaterial* pMat = 0;

    // process the file line per line
    while (in.getline(buffer, maxBuffersize))
    {
        ObjString line(buffer);
        if (line.beginsWith("#ModelBankCollection"))
        {
            std::cout << "IS MBC !!!!!!\n";
            isMBC = true;
            continue;
        }
        // remove comments
        words = line.split("#");
        if (words.empty())
            continue;

        words = words[0].split();
        command = words[0];
        args = "";
        for(unsigned int i=1 ; i<words.size() ; ++i)
            args += words[i] + " ";
        args.trim();

        if (command=="newmtl")
        {
            std::cout << "add mat " << args << "\n";
            pMat = this->getOrCreateMaterial(args);
            continue;
        }
        else if (!pMat)
        {
            continue;
        }

        pMat->setProperty(command,args);

        if (command=="Ka")
        {
            pMat->setKa(args.toVector3());
        }
        else if (command=="Kd")
        {
            pMat->setKd(args.toVector3());
        }
        else if (command=="Ks")
        {
            pMat->setKs(args.toVector3());
        }
        else if (command=="Ns")
        {
            pMat->setNs(args.toFloat());
        }
        else if (command=="Ni")
        {
            pMat->setNi(args.toFloat());
        }
        else if (command=="Tr")
        {
            if (words.size()==4)
                pMat->setTr(args.toVector3());
            else
                pMat->setTr(ObjVector3(args.toFloat()));
        }
        else if ((command=="d") && !pMat->hasProperty("Tr"))
        {
            pMat->setTr(ObjVector3(args.toFloat()));
        }
        else if (command=="map_Ka")
        {
            pMat->setMapKa(findTexture(args,isMBC));
        }
        else if (command=="map_Kd")
        {
            pMat->setMapKd(findTexture(args,isMBC));
        }
        else if (command=="map_Ks")
        {
            pMat->setMapKs(findTexture(args,isMBC));
        }
        else if (command=="map_Bump")
        {
            pMat->setMapBump(findTexture(args,isMBC));
        }
        else if (command=="map_Refl")
        {
            pMat->setMapRefl(findTexture(args,isMBC));
        }
        else if (command=="type")
        {
            pMat->setType(words[1]);
        }
    }

    return true;
}

int ObjMesh::getOrCreateMaterialId(const ObjString& name)
{
    std::map<ObjString,int>::const_iterator it = mMaterialsByName.find(name);
    if (it!=mMaterialsByName.end())
        return it->second;

    // else create a new default material
    ObjMaterial* pMat = new ObjMaterial(name);
    mMaterialsByName[name] = mMaterials.size();
    mMaterials.push_back(pMat);

    return mMaterials.size()-1;
}

const ObjMaterial* ObjMesh::getMaterial(const ObjString& name) const
{
    std::map<ObjString,int>::const_iterator it = mMaterialsByName.find(name);
    if (it==mMaterialsByName.end())
        return 0;
    return mMaterials[it->second];
}
ObjMaterial* ObjMesh::editMaterial(const ObjString& name)
{
    return const_cast<ObjMaterial*>(getMaterial(name));
}

const ObjSubMesh* ObjMesh::getSubMesh(const ObjString& name) const
{
    std::map<ObjString,ObjSubMesh*>::const_iterator it = mSubMeshesByName.find(name);
    if (it==mSubMeshesByName.end())
        return 0;
    return it->second;
}
ObjSubMesh* ObjMesh::editSubMesh(const ObjString& name)
{
    return const_cast<ObjSubMesh*>(getSubMesh(name));
}

template <typename IStream>
ObjMesh* ObjMesh::LoadFromStream(IStream& in, const ObjString& texturePath)
{
    ObjMesh* pMesh = new ObjMesh();
    pMesh->mTexturePath = texturePath;

    ObjString command, args;
    std::vector<ObjString> words, ids;

    const int maxBuffersize = 2048;
    char buffer[maxBuffersize];

    ObjSubMesh* pSubMesh = pMesh->createSubMesh();
    bool emptySubMesh = true;

    // material support
    // The states of usemap/usemtl at the first face of group determine the material of the group.
    ObjString usemap = "";
    ObjString usemtl = "";
    int currentMaterialId = -1;

    // process the file line per line
    while (in.getline(buffer, maxBuffersize))
    {
        // remove comments
        words = ObjString(buffer).split("#");
        if (words.empty())
            continue;

        words = words[0].split(" ");
        command = words[0];
        args = "";
        for (unsigned int i=1 ; i<words.size() ; ++i)
            args += words[i] + " ";
        args.trim();

        if (command=="mtllib") // material file
        {
            pMesh->loadMaterialFile(pMesh->getTexturePath() + args);
        }
        else if (command == "g") // group <=> submesh
        {
            args.trim();
            if (!emptySubMesh) // skip empty objects
            {
                pSubMesh = pMesh->createSubMesh(args);
                emptySubMesh = true;
                std::cout << "\tObjMeshReader: new group " << args << std::endl;
            }
            else
            {
                pSubMesh->rename(args);
                std::cout << "\tObjMeshReader: skip group, new group = " << args << std::endl;
            }
            // should we reset the current material ?
//             usemap = "";
//             usemtl = "";
        }
        else if (command == "v") // add a vertex position
        {
            if (words.size()!=4)
            {
                std::cerr << "ObjLoader: Error parsing line : " << buffer << "\n";
                return false;
            }
            // std::cerr << "add vertex\n";
            pMesh->positions.push_back(args.toVector3());
        }
        else if(command == "vt")
        {
            if (words.size()<3)
            {
                std::cerr << "ObjLoader: Error parsing line : " << buffer << "\n";
                return false;
            }
            pMesh->texcoords.push_back(args.toVector2());
        }
        else if(command == "vn")
        {
            if (words.size()!=4)
            {
                std::cerr << "ObjLoader: Error parsing line : " << buffer << "\n";
                return false;
            }
            pMesh->normals.push_back(args.toVector3());
//             std::cout << args.toVector3() << " ";
        }
        else if (command == "f") // add a face (= triangle)
        {
            if (words.size()<4)
            {
                std::cerr << "ObjLoader: Error parsing line : " << buffer << "\n";
                return false;
            }

            int nofVertices = words.size()-1;
            bool hasNormalIds = false;
            bool hasTexcoordIds = false;
            for (int k=0 ; k<nofVertices ; ++k)
            {
                ids = words[k+1].split("/");
                if ((ids.size()>=2) && (ids[1]!="")) hasTexcoordIds = true;
                if ((ids.size()>=3) && (ids[2]!="")) hasNormalIds = true;
            }

            ObjFaceHandle face = pSubMesh->createFace(
                nofVertices,
                Obj::Options((hasTexcoordIds?Obj::Texcoord:0) | (hasNormalIds?Obj::Normal:0)),
                currentMaterialId);

            for (int k=0 ; k<nofVertices ; ++k)
            {
                // ObjString::toInt() returns the first integer, so it's always fine.
                face.vPositionId(k) = words[k+1].toInt()-1;

                ids = words[k+1].split("/");

                if (hasTexcoordIds)
                {
                    if (ids.size()>=2 && ids[1]!="")
                        face.vTexcoordId(k) = ids[1].toInt()-1;
                    else
                        face.vTexcoordId(k) = -1;
                }

                if (hasNormalIds)
                {
                    if (ids.size()>=3 && ids[2]!="")
                        face.vNormalId(k) = ids[2].toInt()-1;
                    else
                        face.vNormalId(k) = -1;
                }
            }

            emptySubMesh = false;
        }
        else if ( (command=="usemtl" && usemtl!=args) || (command=="usemap" && usemap!=args))
        {
            // material change
            if (command=="usemtl")
                usemtl = args;
            else if (command=="usemap")
                usemap = args;

            // instanciate the material
            if (usemtl!="")
            {
                if (pMesh->getMaterial(usemtl)==0)
                {
                    std::cerr << "ObjLoader::Warning - Material \"" << usemtl << "\" not found.\n";
                }

                if (usemap!="")
                {
                    ObjString matName = pMesh->createUsemapMaterial(usemap,usemtl);
                    currentMaterialId = pMesh->getOrCreateMaterialId(matName);
                }
                else
                {
                    currentMaterialId = pMesh->getOrCreateMaterialId(usemtl);
                }
            }
            else if (usemap!="")
            {
                ObjString matName = pMesh->createUsemapMaterial(usemap);
                currentMaterialId = pMesh->getOrCreateMaterialId(matName);
            }
        }
    }

    return pMesh;
}

ObjMesh* ObjMesh::LoadFromFile(const ObjString& filename)
{
    // get the texture path
    ObjString texturePath, dummy;
    filename.decomposePathAndFile(texturePath,dummy);

    ObjMesh* pMesh = 0;

    if (filename.endsWith(".obj"))
    {
        std::ifstream ifs(filename);
        if (!ifs)
        {
            std::cerr << "ObjLoader: Could not open input obj file " << filename << "\n";
            return 0;
        }

        pMesh = LoadFromStream(ifs,texturePath);
    }
    else if (filename.endsWith(".obj.gz"))
    {
#ifdef HAS_GZSTREAM
        igzstream ifs(filename);
        if (!ifs)
        {
            std::cerr << "ObjLoader: Could not open input obj file " << filename << "\n";
            return 0;
        }

        pMesh = LoadFromStream(ifs,texturePath);
#else
		std::cerr << "support for compressed stream is disabled\n";
		return 0;
#endif
    }
    else
    {
        std::cerr << "ObjLoader: Invalid file extension: " << filename << "\n";
        return 0;
    }

    return pMesh;
}

ObjMesh* ObjMesh::createIndexedFaceSet(Obj::Options options) const
{
    ObjMesh* pClone = new ObjMesh();

    pClone->mTexturePath = this->mTexturePath;

    // clone Materials
    for (unsigned int i=0 ; i<mMaterials.size() ; ++i)
    {
        ObjMaterial* pMat = mMaterials[i]->clone(mMaterials[i]->getName());
        pClone->mMaterials.push_back(pMat);
        pClone->mMaterialsByName[pMat->getName()] = i;
    }

    // store for each vertex position the indices of the new vertices.
    // a vertex having multiple different normals or texcoords will appear multiple times.
    // the key value is a mix of the normal and texcoord indices.
    std::vector< std::map<long long int,unsigned int> > vertices;
    vertices.resize(positions.size());

    for (unsigned int smi=0 ; smi<mSubMeshes.size() ; ++smi)
    {
        ObjSubMesh* pSrcSubMesh = mSubMeshes[smi];
        int matId = pSrcSubMesh->getMaterialId();
        ObjSubMesh* pDstSubMesh;
        if (matId<0)
        {
            matId = pSrcSubMesh->getFace(0).materialId();
            ObjString name = pSrcSubMesh->getName() + "/";
            name << matId;
            pDstSubMesh = pClone->createSubMesh(name);
            pDstSubMesh->setMaterialId(matId);
        }
        else
        {
            pDstSubMesh = pClone->createSubMesh(pSrcSubMesh->getName());
            pDstSubMesh->setMaterialId(matId);
        }

        // this map is used to classify the local sub meshes per material id
        std::map<int,ObjSubMesh*> localSubMesh;
        localSubMesh[matId] = pDstSubMesh;

        for (unsigned int fid = 0 ; fid<pSrcSubMesh->getNofFaces() ; ++fid)
        {
            ObjConstFaceHandle sface = pSrcSubMesh->getFace(fid);

            if (sface.materialId()!=matId)
            {
                // material change
                matId = sface.materialId();
                std::map<int,ObjSubMesh*>::iterator smeshit = localSubMesh.find(matId);
                if (smeshit==localSubMesh.end())
                {
                    // a local sub mesh with this material does not exist yet.
                    // let's create it:
                    ObjString name = pSrcSubMesh->getName() + "/";
                    name << matId;
                    pDstSubMesh = pClone->createSubMesh(name);
                    localSubMesh[matId] = pDstSubMesh;
                }
                else
                {
                    pDstSubMesh = smeshit->second;
                }
            }

            int dstNofVertices = 3;
            if ( (!(options&int(Obj::Triangulate))) && (pSrcSubMesh->getConstNofVerticesPerFace()>0) )
                dstNofVertices = pSrcSubMesh->getConstNofVerticesPerFace();

            int ids[3];
            ids[0] = 0;
            ids[1] = 1;
            ids[2] = 2;

            unsigned int nofSubPolygons = 1 + (sface.nofVertices()-dstNofVertices);
            for (unsigned int polyId=0 ; polyId<nofSubPolygons ; ++polyId)
            {
                ObjFaceHandle dface = pDstSubMesh->createFace(dstNofVertices,Obj::None,matId);
                for (unsigned int k=0 ; k<3 ; ++k)
                {
                    // construction of the unique key value of the vertex:
                    long long int key = 0;
                    if (options&Obj::Texcoord)
                        key = (long long int)(sface.vTexcoordId(ids[k])+1);
                    if (options&Obj::Normal)
                        key = key | ((long long int)(sface.vNormalId(ids[k])+1) << 32 );

                    assert(sface.vPositionId(k)<int(positions.size()));
                    assert(sface.vTexcoordId(k)<int(texcoords.size()));
                    assert(sface.vNormalId(k)<int(normals.size()));

                    std::map<long long int,unsigned int>::const_iterator vit = vertices[sface.vPositionId(ids[k])].find(key);
                    if (vit!=vertices[sface.vPositionId(ids[k])].end())
                    {
                        // the unique vertex already exist, let's reuse it
                        dface.vertexId(k) = vit->second;
                    }
                    else
                    {
                        // a new vertex has to be created
                        vertices[sface.vPositionId(ids[k])][key] = pClone->positions.size();
                        dface.vertexId(k) = pClone->positions.size();
                        pClone->positions.push_back(this->positions[sface.vPositionId(ids[k])]);

                        if ( (options&Obj::Texcoord) && (!this->texcoords.empty()) )
                        {
                            if (sface.vTexcoordId(ids[k])>=0)
                                pClone->texcoords.push_back(this->texcoords[sface.vTexcoordId(ids[k])]);
                            else
                                pClone->texcoords.push_back(ObjVector2(0,0));
                        }

                        if ( (options&Obj::Normal) && (!this->normals.empty()) )
                        {
                            if (sface.vNormalId(ids[k])>=0)
                            {
                                pClone->normals.push_back(this->normals[sface.vNormalId(ids[k])]);
                            }
                            else
                            {
                                pClone->normals.push_back(ObjVector3(0,0,0));
                            }
                        }
                    }
                }
                ids[1] = ids[2];
                ids[2]++;
            }
        }
    }

    return pClone;
}
