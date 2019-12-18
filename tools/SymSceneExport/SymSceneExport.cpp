//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Appwizard generated plugin
//				3DXI file exporter project template
//				For a more in-depth exemple of a 3DXI exporter,
//				please refer to maxsdk\samples\igame\export.
// AUTHOR:		Jean-Francois Yelle - created Mar.20.2007
//***************************************************************************/

#include "SymSceneExport.h"

#include "3dsmaxsdk_preinclude.h"
#include "IGame/IGame.h"

#include <sstream>

#define SymSceneExport_CLASS_ID	Class_ID(0x84b1a3aa, 0x324763b3)

//Strips the path from a filename, returns file + extension
std::string GetFilename(std::string Path)
{
	int Start = Path.find_last_of("\\") + 1;
	return Path.substr(Start, Path.length() - Start);
}

sym::CCustomBrush MaxToSymCustBrush(IGameNode *Node)
{
	//New subset
	sym::CCustomBrush Subset;

	IGameObject *Object = Node->GetIGameObject();
	IGameMesh *Mesh = (IGameMesh*) Object;

	//Initialise face data
	Mesh->InitializeData();

	//Get scale
	Point3 Scale = Node->GetWorldTM().Scaling();

	//Loop through vertices
	for (int i = 0; i < Mesh->GetNumberOfVerts(); i++)
	{
		Point3 Vertex;

		//Get current vertex
		Mesh->GetVertex(i, Vertex, true);

		//Push back vertex
		Subset.Vertices.push_back(sym::CVector3(Vertex.x * Scale.x, Vertex.y * Scale.y, Vertex.z * Scale.z));
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
		sym::CFace SymFace;

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

	//Set name
	Subset.Name = std::string(Node->GetName());

	//Set position
	Subset.Position.x = Node->GetWorldTM().Translation().x;
	Subset.Position.y = Node->GetWorldTM().Translation().y;
	Subset.Position.z = Node->GetWorldTM().Translation().z;

	Quat Quaternion = Node->GetWorldTM().Rotation();

	Subset.Rotation.x = Quaternion.x;
	Subset.Rotation.y = Quaternion.y;
	Subset.Rotation.z = Quaternion.z;
	Subset.Rotation.w = -Quaternion.w;

	//Get material of first face
	IGameMaterial *Material = 0;
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

class SymSceneExport : public SceneExport 
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
		SymSceneExport();
		virtual ~SymSceneExport();

	protected:
		int GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode =NULL);

	private:
		static HWND hParams;
};



class SymSceneExportClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 		{ return new SymSceneExport(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return SCENE_EXPORT_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return SymSceneExport_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("SymSceneExport"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
	

};

static SymSceneExportClassDesc SymSceneExportDesc;
ClassDesc2* GetSymSceneExportDesc() { return &SymSceneExportDesc; }





INT_PTR CALLBACK SymSceneExportOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static SymSceneExport *imp = NULL;

	switch(message) {
		case WM_INITDIALOG:
			imp = (SymSceneExport *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return 1;
	}
	return 0;
}


//--- SymSceneExport -------------------------------------------------------
SymSceneExport::SymSceneExport()
{

}

SymSceneExport::~SymSceneExport() 
{

}

int SymSceneExport::ExtCount()
{
	// Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *SymSceneExport::Ext(int n)
{		
	//#pragma message	(TODO("Return the 'i-th' file name extension (i.e. \"3DS\")."))
	return _T("sym.scene");
}

const TCHAR *SymSceneExport::LongDesc()
{
	//#pragma message	(TODO("Return long ASCII description (i.e. \"Targa 2.0 Image File\""))
	return _T("Symmetry Scene File");
}
	
const TCHAR *SymSceneExport::ShortDesc() 
{			
	//#pragma message	(TODO("Return short ASCII description (i.e. \"Targa\")"))
	return _T("Symmetry Scene");
}

const TCHAR *SymSceneExport::AuthorName()
{			
	//#pragma message	(TODO("Return ASCII Author name"))
	return _T("Matt Phillips");
}

const TCHAR *SymSceneExport::CopyrightMessage() 
{	
	//#pragma message	(TODO("Return ASCII Copyright message"))
	return _T("(c) Matt Phillips 2009");
}

const TCHAR *SymSceneExport::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("http://www.hysteriagaming.com");
}

const TCHAR *SymSceneExport::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int SymSceneExport::Version()
{				
	//#pragma message	(TODO("Return Version number * 100 (i.e. v3.01 = 301)"))
	return 100;
}

void SymSceneExport::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL SymSceneExport::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}

int SymSceneExport::GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode /*=NULL*/)
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


int	SymSceneExport::DoExport(const TCHAR* name,ExpInterface* ei,Interface* i, BOOL suppressPrompts, DWORD options)
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
	sym::CScene SymScene;
	Tab<IGameNode*> Meshes = SceneInterface->GetIGameNodeByType(IGameObject::IGAME_MESH);
	Tab<IGameNode*> Bones = SceneInterface->GetIGameNodeByType(IGameObject::IGAME_BONE);

	//Retrieve meshes
	for (int i = 0; i < Meshes.Count(); i++)
	{
		IGameObject *CurrentObject = Meshes[i]->GetIGameObject();
		if(CurrentObject->IsRenderable() && CurrentObject->GetIGameType() != IGameObject::IGAME_BONE)
			SymScene.Geometry.push_back(MaxToSymCustBrush(Meshes[i]));
	}

	std::stringstream Stream;

	//Write the file, get filesize
	CStr Filename(name);
	Filename.toLower();
	int Filesize = SymScene.Write(Filename.data());

	//Generate message
	Stream << "Exported scene:\n\n";

	//Error check
	if (Filesize > 0)
	{
		for (int i = 0; i < SymScene.Geometry.size(); i++)
		{
			Stream	<< "Mesh name: "	<< SymScene.Geometry[i].Name.c_str()
					<< "\tMaterial name: "<< SymScene.Geometry[i].MaterialName.c_str()
					<< "\tVertices: "	<< SymScene.Geometry[i].Vertices.size() 
					<< "\tFaces: "		<< SymScene.Geometry[i].Faces.size() << "\n";
		}

		Stream << "\nScene filesize: " << Filesize << "\n\n\n";
	}
	else
	{
		MessageBox(NULL, "Could not write scene", "Error", MB_OK);
		Stream << "***ERROR EXPORTING SCENE: " << GetFilename(name) << "\n\n";
	}

	MessageBox(NULL, Stream.str().c_str(), "Symmetry Scene Exporter", MB_OK);

	return TRUE;

}


 
