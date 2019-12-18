#include <sstream>

#include "SymMeshExport.h"

#include "3dsmaxsdk_preinclude.h"
#include "IGame/IGame.h"

#include "SYM_Mesh.h"


#define SymMeshExport_CLASS_ID	Class_ID(0x6fe7cddc, 0x529c08b0)

//Strips the path from a filename, returns file + extension
std::string GetFilename(std::string Path)
{
	int Start = Path.find_last_of("\\") + 1;
	return Path.substr(Start, Path.length() - Start);
}

SYM::SYM_MESHSUBSET MaxToSymSubset(IGameNode *Node)
{
	//New subset
	SYM::SYM_MESHSUBSET Subset;

	IGameObject *Object = Node->GetIGameObject();
	IGameMesh *Mesh = (IGameMesh*) Object;

	//Initialise face data
	Mesh->InitializeData();

	//Loop through vertices
	for (int i = 0; i < Mesh->GetNumberOfVerts(); i++)
	{
		Point3 Vertex;

		//Get current vertex
		Mesh->GetVertex(i, Vertex);

		//Push back vertex
		Subset.Vertices.push_back(SYM::SYM_VECTOR3(Vertex.x, Vertex.y, Vertex.z));
	}

	//Loop through faces
	for (int i = 0; i < Mesh->GetNumberOfFaces(); i++)
	{
		//Get face
		FaceEx *Face = Mesh->GetFace(i);

		//Get UV coords
		Point2 TexCoord[3];
		Mesh->GetTexVertex(Face->texCoord[0], TexCoord[0]);
		Mesh->GetTexVertex(Face->texCoord[1], TexCoord[1]);
		Mesh->GetTexVertex(Face->texCoord[2], TexCoord[2]);

		//New SYM Face
		SYM::SYM_FACE SymFace;

		//Add indices
		SymFace.Indices[0] = Face->vert[0];
		SymFace.Indices[1] = Face->vert[1];
		SymFace.Indices[2] = Face->vert[2];

		//Add UV coords
		SymFace.UVCoords[0].u = TexCoord[0].x;
		SymFace.UVCoords[0].v = TexCoord[0].y;

		SymFace.UVCoords[1].u = TexCoord[1].x;
		SymFace.UVCoords[1].v = TexCoord[1].y;

		SymFace.UVc.u = TexCoord[2].x;
		SymFace.UVc.v = TexCoord[2].y;

		//Push back face
		Subset.Faces.push_back(SymFace);
	}

	//Loop through modifiers
	for (int i = 0; i < Object->GetNumModifiers(); i++)
	{
		IGameModifier *Modifier = Object->GetIGameModifier(i);

		//Find the skin modifier
		if (Modifier->IsSkin())
		{
			IGameSkin *Skin = (IGameSkin*) Modifier;
			
			//Export skinned verts
			for (int v = 0; v < Skin->GetNumOfSkinnedVerts(); v++)
			{
				int BoneCount = 0;

				for (int b = 0; b < Skin->GetNumberOfBones(v); b++)
				{
					if(b < 4)
					{
						Subset.Vertices[v].BoneIDs[BoneCount] = Skin->GetBoneID(v, b);
						Subset.Vertices[v].Weights[BoneCount] = Skin->GetWeight(v, b);
						BoneCount++;
					}
					else
					{
						static bool UserWarned = false;

						//Bone-to-vertex count exceeded
						if(!UserWarned) MessageBox(NULL, "Warning: Max bones per vertex exceeded, skinning data will not be exported correctly", "Symmetry Mesh Exporter", MB_OK);

						UserWarned = true;
					}
				}
			}
		}
	}

	//Set name
	Subset.Name = std::string(Node->GetName());

	//Get material of first face
	IGameMaterial *Material;
	if(Mesh->GetNumberOfFaces() > 0) Material = Mesh->GetMaterialFromFace(Mesh->GetFace(0));

	//Get material name
	if(Material)
	{
		CStr MatFile(Material->GetMaterialName());
		MatFile.toLower();
		Subset.MaterialName = MatFile.data();
		Subset.MaterialName +=  ".sym.material";
	}

	return Subset;
}

SYM::SYM_BONE MaxToSymBone(IGameNode *Node)
{
	//Retrieve matrix
	Matrix3 Matrix = Node->GetWorldTM().ExtractMatrix3();

	//Get position
	Point3 Position = Matrix.GetTrans();

	//Get quaternion
	Quat Rotation(Matrix);

	//New bone
	SYM::SYM_BONE Bone;

	//Set position/rotation
	Bone.Position = SYM::SYM_VECTOR3(Position.x, Position.y, Position.z);
	Bone.Rotation = SYM::SYM_QUATERNION(Rotation.x, Rotation.y, Rotation.z, -Rotation.w);

	//Get ID
	Bone.ID = Node->GetNodeID();

	//Get name
	Bone.Name = Node->GetName();

	//Get parent bone
	if(Node->GetNodeParent())
	{
		IGameObject *ParentObj = Node->GetNodeParent()->GetIGameObject();
		if(ParentObj->GetIGameType() == IGameObject::IGAME_BONE) Bone.ParentID = Node->GetNodeParent()->GetNodeID();
	}

	//Return bone
	return Bone;
}

class SymMeshExport : public SceneExport 
{
	public:
		
		virtual int				ExtCount();					// Number of extensions supported
		virtual const TCHAR *	Ext(int n);					// Extension #n (i.e. "3DS")
		virtual const TCHAR *	LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
		virtual const TCHAR *	ShortDesc();				// Short ASCII description (i.e. "3D Studio")
		virtual const TCHAR *	AuthorName();				// ASCII Author name
		virtual const TCHAR *	CopyrightMessage();			// ASCII Copyright message
		virtual const TCHAR *	OtherMessage1();			// Other message #1
		virtual const TCHAR *	OtherMessage2();			// Other message #2
		virtual unsigned int	Version();					// Version number * 100 (i.e. v3.01 = 301)
		virtual void			ShowAbout(HWND hWnd);		// Show DLL's "About..." box

		virtual BOOL SupportsOptions(int ext, DWORD options);
		virtual int	DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

		//Constructor/Destructor
		SymMeshExport();
		virtual ~SymMeshExport();

	protected:
		int GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode =NULL);

	private:
		static HWND hParams;
};



class SymMeshExportClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 		{ return new SymMeshExport(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return SCENE_EXPORT_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return SymMeshExport_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("SymMeshExport"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
	

};

static SymMeshExportClassDesc SymMeshExportDesc;
ClassDesc2* GetSymMeshExportDesc() { return &SymMeshExportDesc; }





INT_PTR CALLBACK SymMeshExportOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static SymMeshExport *imp = NULL;

	switch(message) {
		case WM_INITDIALOG:
			imp = (SymMeshExport *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return 1;
	}
	return 0;
}


//--- SymMeshExport -------------------------------------------------------
SymMeshExport::SymMeshExport()
{

}

SymMeshExport::~SymMeshExport() 
{

}

int SymMeshExport::ExtCount()
{
	// Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *SymMeshExport::Ext(int n)
{		
	//#pragma message	(TODO("Return the 'i-th' file name extension (i.e. \"3DS\")."))
	return _T("sym.mesh");
}

const TCHAR *SymMeshExport::LongDesc()
{
	//#pragma message	(TODO("Return long ASCII description (i.e. \"Targa 2.0 Image File\""))
	return _T("Symmetry Mesh File");
}
	
const TCHAR *SymMeshExport::ShortDesc() 
{			
	//#pragma message	(TODO("Return short ASCII description (i.e. \"Targa\")"))
	return _T("Symmetry Mesh");
}

const TCHAR *SymMeshExport::AuthorName()
{			
	//#pragma message	(TODO("Return ASCII Author name"))
	return _T("Matt Phillips");
}

const TCHAR *SymMeshExport::CopyrightMessage() 
{	
	//#pragma message	(TODO("Return ASCII Copyright message"))
	return _T("(c) Matt Phillips 2008");
}

const TCHAR *SymMeshExport::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("http://www.hysteriagaming.com");
}

const TCHAR *SymMeshExport::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int SymMeshExport::Version()
{				
	//#pragma message	(TODO("Return Version number * 100 (i.e. v3.01 = 301)"))
	return 100;
}

void SymMeshExport::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL SymMeshExport::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}

int SymMeshExport::GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode /*=NULL*/)
{
	int i;
	if (i_currentNode == NULL)
	{
		i_currentNode = GetCOREInterface()->GetRootNode();
	}
	else // IGame will crash 3ds Max if it is initialised with the root node.
	{
	    i_nodeTab.AppendNode(i_currentNode);
	}
	for (i = 0; i < i_currentNode->NumberOfChildren(); i++)
	{
		GetSceneNodes(i_nodeTab, i_currentNode->GetChildNode(i));
	}
	return i_nodeTab.Count();
}


int	SymMeshExport::DoExport(const TCHAR* name,ExpInterface* ei,Interface* i, BOOL suppressPrompts, DWORD options)
{
	//Get IGame scene interface
	IGameScene *SceneInterface = GetIGameInterface();

	//Setup OpenGL coodinate system
	IGameConversionManager *ConvManager = GetConversionManager();
	ConvManager->SetCoordSystem(IGameConversionManager::IGAME_OGL);

	//Initialise IGame scene interface
	SceneInterface->InitialiseIGame(true);
	SceneInterface->SetStaticFrame(0);

	//Meshes and bones
	SYM::SYM_MESH SymMesh;
	Tab<IGameNode*> Meshes = SceneInterface->GetIGameNodeByType(IGameObject::IGAME_MESH);
	Tab<IGameNode*> Bones = SceneInterface->GetIGameNodeByType(IGameObject::IGAME_BONE);

	//Retrieve meshes
	for (int i = 0; i < Meshes.Count(); i++)
	{
		IGameObject *CurrentObject = Meshes[i]->GetIGameObject();
		if(CurrentObject->IsRenderable() && CurrentObject->GetIGameType() != IGameObject::IGAME_BONE)
			SymMesh.Subsets.push_back(MaxToSymSubset(Meshes[i]));
	}

	//Retrieve bones
	for (int i = 0; i < Bones.Count(); i++)
	{
		SYM::SYM_BONE Bone = MaxToSymBone(Bones[i]);
		if(Bone.ID >= 0) SymMesh.Bones[Bone.ID] = Bone;
	}

	std::stringstream Stream;

	//Write the file, get filesize
	CStr Filename(name);
	Filename.toLower();
	int Filesize = SymMesh.Write(Filename.data());

	//Generate message
	Stream << "Exported mesh:\n\n";

	//Error check
	if (Filesize > 0)
	{
		for (int i = 0; i < SymMesh.Subsets.size(); i++)
		{
			Stream	<< "Subset name: "	<< SymMesh.Subsets[i].Name.c_str()
					<< "\tMaterial name: "<< SymMesh.Subsets[i].MaterialName.c_str()
					<< "\tVertices: "	<< SymMesh.Subsets[i].Vertices.size() 
					<< "\tFaces: "		<< SymMesh.Subsets[i].Faces.size() << "\n";
		}

		for (std::map<int, SYM::SYM_BONE>::iterator i = SymMesh.Bones.begin(); i != SymMesh.Bones.end(); i++)
		{
			Stream	<< "Bone name: " << (*i).second.Name.c_str()
					<< "\tBone ID: " << (*i).first
					<< "\tParent ID: " << (*i).second.ParentID << "\n";
		}

		Stream << "\nMesh filesize: " << Filesize << "\n\n\n";
	}
	else
	{
		MessageBox(NULL, "Could not write mesh", "Error", MB_OK);
		Stream << "***ERROR EXPORTING MESH: " << GetFilename(name) << "\n\n";
	}

	MessageBox(NULL, Stream.str().c_str(), "Symmetry Mesh Exporter", MB_OK);

	return TRUE;
}


 
