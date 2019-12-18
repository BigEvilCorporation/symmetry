#include "SYM_Model.h"
#include "SYM_Engine.h"

namespace sym
{
	CModel::CModel()
	{
		UseResourceManager = true;
	}

	CModel::~CModel()
	{
		if(!UseResourceManager) delete Mesh;
	}

	bool CModel::Load(std::string ModelName, std::string Filename, int PhysType, bool UseResManager)
	{
		UseResourceManager = UseResManager;

		//Load mesh from resource manager
		if(UseResManager) Mesh = Engine->ResourceManager.Meshes.Get(Filename);
		else
		{
			Mesh = new CMesh();
			Mesh->Load(Filename);
		}

		//Check for errors
		if(!Mesh) return false;

		//Set type
		Type = SYM_MDL;

		//Set name
		Name = ModelName;

		//Set filename
		ModelFilename = Filename;

		//Set physics type
		PhysicsType = PhysType;

		//Setup collision body
		switch(PhysicsType)
		{
			case PHYSICS_OBJ_BOX:
				CreateBox(Mesh->AABB.Size);
				break;

			case PHYSICS_OBJ_SPHERE:
				CreateSphere(Mesh->AABB.Size);
				break;

			case PHYSICS_OBJ_CAPSULE:
				CreateCapsule(Mesh->AABB.Size);
				break;

			case PHYSICS_OBJ_CUSTOM:
			{
				std::vector<CVector3> Vertices;

				for(std::vector<CMeshSubset>::iterator i = Mesh->Subsets.begin(); i != Mesh->Subsets.end(); i++)
				{
					Vertices.insert(Vertices.end(), i->Vertices.begin(), i->Vertices.end());
				}

				CreateCustom(Vertices);
				break;
			}

			case LEVEL_COLLISION:
			{
				CreateLevel(Mesh->Subsets[0].Vertices, Mesh->Subsets[0].Faces);
				break;
			}

			case NO_PHYSICS:
				break;

			default: 
				CreateBox(Mesh->AABB.Size);
				break;
		}

		//Set physics geometry
		SetGeometry(dynamic_cast<CGeometry*>(this));

		return true;
	}

} // Namespace
