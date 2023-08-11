// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright (c) 2008-2023 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#ifndef OMNI_PVD_PX_SAMPLER_H
#define OMNI_PVD_PX_SAMPLER_H

/*
The below helper macros are to be used in SDK code to send information to PVD as unintrusively as possible.

Principles:
* Data created or changed by the user should be sent as soon as it has been written to an SDK class, ideally right at the end of the API create() or set() call.
* Objects should ideally be destroyed from their destructors (since release() calls might just decrement).
* Data written by the SDK should be sent at the end of the simulate frame.
* Ideally use Px-pointers as object handles.  Beware that multiple inheritance can result in different pointer values so its best to cast to the Px-type pointer / reference explicitly.
	Even if the code works by passing in different equivalent pointer types, this will generate unnecessary duplicate template code.
*/
#if PX_SUPPORT_OMNI_PVD

// You can use this in conditional statements to check for a connection
	#define OMNI_PVD_ACTIVE				(::OmniPvdPxSampler::getInstance() != NULL)

// Create object reference o of PVD type classT.  Example: 	OMNI_PVD_CREATE(scene, static_cast<PxScene &>(*npScene));
#if PX_GCC_FAMILY
	#define OMNI_PVD_CREATE(classT, o) \
	_Pragma("GCC diagnostic push") \
	_Pragma("GCC diagnostic ignored \"-Wundefined-func-template\"") \
	if (::OmniPvdPxSampler::getInstance() != NULL) {::OmniPvdPxSampler::getInstance()->createObject(OmniPvdPxSampler::classHandle_##classT, o); } \
	_Pragma("GCC diagnostic pop")
#else
	#define OMNI_PVD_CREATE(classT, o) \
	if (::OmniPvdPxSampler::getInstance() != NULL) {::OmniPvdPxSampler::getInstance()->createObject(OmniPvdPxSampler::classHandle_##classT, o); }
#endif

// Destroy object reference o of PVD type classT.  Example: OMNI_PVD_DESTROY(scene, static_cast<PxScene &>(*npScene));
#if PX_GCC_FAMILY		
	#define OMNI_PVD_DESTROY(classT, o)	\
	_Pragma("GCC diagnostic push") \
	_Pragma("GCC diagnostic ignored \"-Wundefined-func-template\"") \
	if (::OmniPvdPxSampler::getInstance() != NULL) {::OmniPvdPxSampler::getInstance()->destroyObject(o); } \
	_Pragma("GCC diagnostic pop")
#else
	#define OMNI_PVD_DESTROY(classT, o)	\
	if (::OmniPvdPxSampler::getInstance() != NULL) {::OmniPvdPxSampler::getInstance()->destroyObject(o); }
#endif

// Set PVD attribute a of object reference o of PVD type classT to value v.  v is passed as reference to value; PVD object handles are passed as reference to POINTER here!
// Example: OMNI_PVD_SET(PxActor, isdynamic, a, false)  
#if PX_GCC_FAMILY
	#define OMNI_PVD_SET(classT, a, o, v) \
	_Pragma("GCC diagnostic push") \
	_Pragma("GCC diagnostic ignored \"-Wundefined-func-template\"") \
	if (::OmniPvdPxSampler::getInstance() != NULL) {::OmniPvdPxSampler::getInstance()->setAttribute(OmniPvdPxSampler::attributeHandle_##classT##_##a, o, v); } \
	_Pragma("GCC diagnostic pop")
#else
	#define OMNI_PVD_SET(classT, a, o, v) \
	if (::OmniPvdPxSampler::getInstance() != NULL) {::OmniPvdPxSampler::getInstance()->setAttribute(OmniPvdPxSampler::attributeHandle_##classT##_##a, o, v); }
#endif

// Same as set, but for variable length attributes like vertex buffers.  pv is the address of the data, and n is the size in bytes.
#if PX_GCC_FAMILY
	#define OMNI_PVD_SETB(classT, a, o, pv, n) \
	_Pragma("GCC diagnostic push") \
	_Pragma("GCC diagnostic ignored \"-Wundefined-func-template\"") \
	if (::OmniPvdPxSampler::getInstance() != NULL) {::OmniPvdPxSampler::getInstance()->setAttributeBytes(OmniPvdPxSampler::attributeHandle_##classT##_##a, o, pv, n); } \
	_Pragma("GCC diagnostic pop")
#else
	#define OMNI_PVD_SETB(classT, a, o, pv, n) \
	if (::OmniPvdPxSampler::getInstance() != NULL) {::OmniPvdPxSampler::getInstance()->setAttributeBytes(OmniPvdPxSampler::attributeHandle_##classT##_##a, o, pv, n); }
#endif

// Same as set, but for attribute sets of unique things like an array of references.  v is passed as a REFERENCE.
#if PX_GCC_FAMILY
	#define OMNI_PVD_ADD(classT, a, o, v) \
	_Pragma("GCC diagnostic push") \
	_Pragma("GCC diagnostic ignored \"-Wundefined-func-template\"") \
	if (::OmniPvdPxSampler::getInstance() != NULL) {::OmniPvdPxSampler::getInstance()->addToUniqueList(OmniPvdPxSampler::attributeHandle_##classT##_##a, o, v); } \
	_Pragma("GCC diagnostic pop")
#else
	#define OMNI_PVD_ADD(classT, a, o, v) \
	if (::OmniPvdPxSampler::getInstance() != NULL) {::OmniPvdPxSampler::getInstance()->addToUniqueList(OmniPvdPxSampler::attributeHandle_##classT##_##a, o, v); }
#endif

// TO remove a member handle from the set.
#if PX_GCC_FAMILY
	#define OMNI_PVD_REMOVE(classT, a, o, v) \
	_Pragma("GCC diagnostic push") \
	_Pragma("GCC diagnostic ignored \"-Wundefined-func-template\"") \
	if (::OmniPvdPxSampler::getInstance() != NULL) {::OmniPvdPxSampler::getInstance()->removeFromUniqueList(OmniPvdPxSampler::attributeHandle_##classT##_##a, o, v); } \
	_Pragma("GCC diagnostic pop")
#else
	#define OMNI_PVD_REMOVE(classT, a, o, v) \
	if (::OmniPvdPxSampler::getInstance() != NULL) {::OmniPvdPxSampler::getInstance()->removeFromUniqueList(OmniPvdPxSampler::attributeHandle_##classT##_##a, o, v); }
#endif


#else
	#define OMNI_PVD_ACTIVE				(false)
	#define OMNI_PVD_CREATE(classT, p)
	#define OMNI_PVD_DESTROY(classT, p)
	#define OMNI_PVD_SET(classT, a, p, v)
	#define OMNI_PVD_SETB(classT, a, p, v, n)
	#define OMNI_PVD_ADD(classT, a, p, v)
	#define OMNI_PVD_REMOVE(classT, a, p, v)
#endif


#if PX_SUPPORT_OMNI_PVD
#include "omnipvd/PxOmniPvd.h"

#include "foundation/PxSimpleTypes.h"
#include "foundation/PxHashMap.h"
#include "foundation/PxMutex.h"
#include "OmniPvdChunkAlloc.h"
#include "foundation/PxUserAllocated.h"

#include "../../../pvdruntime/include/OmniPvdDefines.h"

namespace physx
{
	class PxScene;
	class PxBase;
	class NpActor;
	class NpScene;
	class NpShape;
	class NpShapeManager;
	class PxActor;
	class PxGeometryHolder;
	class NpOmniPvd;
	class PxGeometry;
	class PxJoint;
	class PxShape;
	class PxMaterial;

	class PxFEMClothMaterial;
	class PxFEMMaterial;
	class PxFEMSoftBodyMaterial;
	class PxFLIPMaterial;
	class PxMPMMaterial;
	class PxParticleMaterial;
	class PxPBDMaterial;
}

extern void streamActorName(physx::PxActor & a, const char* name);
extern void streamSceneName(physx::PxScene & s, const char* name);

void streamShapeMaterials(physx::PxShape* shapePtr, physx::PxMaterial* const * mats, physx::PxU32 nbrMaterials);

void streamShapeMaterials(physx::PxShape* shapePtr, physx::PxFEMClothMaterial* const * mats, physx::PxU32 nbrMaterials);
void streamShapeMaterials(physx::PxShape* shapePtr, physx::PxFEMMaterial* const * mats, physx::PxU32 nbrMaterials);
void streamShapeMaterials(physx::PxShape* shapePtr, physx::PxFEMSoftBodyMaterial* const * mats, physx::PxU32 nbrMaterials);
void streamShapeMaterials(physx::PxShape* shapePtr, physx::PxFLIPMaterial* const * mats, physx::PxU32 nbrMaterials);
void streamShapeMaterials(physx::PxShape* shapePtr, physx::PxMPMMaterial* const * mats, physx::PxU32 nbrMaterials);
void streamShapeMaterials(physx::PxShape* shapePtr, physx::PxParticleMaterial* const * mats, physx::PxU32 nbrMaterials);
void streamShapeMaterials(physx::PxShape* shapePtr, physx::PxPBDMaterial* const * mats, physx::PxU32 nbrMaterials);


enum OmniPvdSharedMeshEnum {
	eOmniPvdTriMesh     = 0,
	eOmniPvdConvexMesh  = 1,
	eOmniPvdHeightField = 2,
};

class OmniPvdLoader;
class OmniPvdWriter;
class OmniPvdPxSampler;
class OmniPvdWriteStream;
class OmniPvdActorMod;
class OmniPvdShapeMod;
class OmniPvdPxScene;

class OmniPvdPxSampler : public physx::PxUserAllocated
{
public:
	OmniPvdPxSampler();
	~OmniPvdPxSampler();
	//enables sampling: 
	void startSampling();
	bool isSampling();
	//sets destination: 
	void setOmniPvdWriter(OmniPvdWriter* omniPvdWriter);	

	// writes all contacts to the stream
	void streamSceneContacts(physx::NpScene& scene);

	// call at the end of a simulation step: 
	void sampleScene(physx::NpScene* scene);
	//// convenience function to handle different kinds of geometry correctly, including capturing meshes:
	//void createGeometry(const physx::PxGeometry & g);
	//void destroyGeometry(const physx::PxGeometry & g);

	//simplified generic API to be used via simple macros above: 
	template <typename ClassType> void createObject(OmniPvdClassHandle, ClassType const & objectId);
	template <typename ClassType> void destroyObject(ClassType const & objectId);
	template <typename ClassType, typename AttributeType> void setAttribute(OmniPvdAttributeHandle, ClassType const & objectId,  AttributeType const & value);
	template <typename ClassType, typename AttributeType> void setAttributeBytes(OmniPvdAttributeHandle, ClassType const & objectId, AttributeType const * value, unsigned nBytes);
	template <typename ClassType, typename AttributeType> void addToUniqueList(OmniPvdAttributeHandle, ClassType const & objectId, AttributeType const & value);
	template <typename ClassType, typename AttributeType> void removeFromUniqueList(OmniPvdAttributeHandle, ClassType const & objectId, AttributeType const & value);

	//handles for all SDK classes and attributes

#define OMNI_PVD_CLASS(classT) static OmniPvdClassHandle classHandle_##classT;
#define OMNI_PVD_CLASS_DERIVED(classT, baseClass) OMNI_PVD_CLASS(classT)
#define OMNI_PVD_ENUM(classT) OMNI_PVD_CLASS(classT)
#define OMNI_PVD_ENUM_VALUE(classT, a)	
#define OMNI_PVD_ATTRIBUTE(classT, a, attrT, t, n) static OmniPvdAttributeHandle attributeHandle_##classT##_##a;
#define OMNI_PVD_ATTRIBUTE_UNIQUE_LIST(classT, a, attrT) static OmniPvdAttributeHandle attributeHandle_##classT##_##a;
#define OMNI_PVD_ATTRIBUTE_FLAG(classT, a, attrT, enumClassT) static OmniPvdAttributeHandle attributeHandle_##classT##_##a;


#include "OmniPvdTypes.h"	//SDK classes and attributes declared here
#undef OMNI_PVD_ENUM
#undef OMNI_PVD_ENUM_VALUE
#undef OMNI_PVD_CLASS
#undef OMNI_PVD_CLASS_DERIVED
#undef OMNI_PVD_ATTRIBUTE
#undef OMNI_PVD_ATTRIBUTE_UNIQUE_LIST
#undef OMNI_PVD_ATTRIBUTE_FLAG

	static OmniPvdPxSampler* getInstance();

	void onObjectAdd(const physx::PxBase* object);
	void onObjectRemove(const physx::PxBase* object);


private:
	OmniPvdPxScene* getSampledScene(physx::NpScene* scene);
};

#endif

#endif
