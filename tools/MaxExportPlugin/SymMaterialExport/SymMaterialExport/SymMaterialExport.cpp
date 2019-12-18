#include <sstream>
#include <vector>
#include <string>

#include "SymMaterialExport.h"

#include "3dsmaxsdk_preinclude.h"
#include "IGame/IGame.h"


#define SymMaterialExport_CLASS_ID	Class_ID(0x7e19ce58, 0x7e1bb9fc)

//Strips the path from a filename, returns file + extension
std::string GetFilename(std::string Path)
{
	int Start = Path.find_last_of("\\") + 1;
	return Path.substr(Start, Path.length() - Start);
}

SYM::SYM_MATERIAL MaxToSymMaterial(IGameMaterial *Material)
{
	//New material
	SYM::SYM_MATERIAL SymMaterial;

	//Get name
	SymMaterial.Name = std::string(Material->GetMaterialName());

	//Get colours
	Point3 Ambient;
	Point3 Diffuse;
	Point3 Specular;
	Material->GetAmbientData()->GetPropertyValue(Ambient);
	Material->GetDiffuseData()->GetPropertyValue(Diffuse);
	Material->GetSpecularData()->GetPropertyValue(Specular);

	SymMaterial.Ambient = SYM::SYM_COLOUR(Ambient.x, Ambient.y, Ambient.z);
	SymMaterial.Diffuse = SYM::SYM_COLOUR(Diffuse.x, Diffuse.y, Diffuse.z);
	SymMaterial.Specular = SYM::SYM_COLOUR(Specular.x, Specular.y, Specular.z);

	//Get transparency and shininess
	int Opacity;
	int Shininess;
	Material->GetOpacityData()->GetPropertyValue(Opacity);
	Material->GetGlossinessData()->GetPropertyValue(Shininess);

	SymMaterial.Transparency = 100 - Opacity;
	SymMaterial.Shininess = Shininess;

	//Get texture map
	if(Material->GetNumberOfTextureMaps() > 0) SymMaterial.TextureFile = GetFilename(Material->GetIGameTextureMap(0)->GetBitmapFileName());

	return SymMaterial;
}

class SymMaterialExport : public SceneExport 
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
		SymMaterialExport();
		virtual ~SymMaterialExport();

	protected:
		int GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode =NULL);

	private:
		static HWND hParams;
};



class SymMaterialExportClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 		{ return new SymMaterialExport(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return SCENE_EXPORT_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return SymMaterialExport_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("SymMaterialExport"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
	

};

static SymMaterialExportClassDesc SymMaterialExportDesc;
ClassDesc2* GetSymMaterialExportDesc() { return &SymMaterialExportDesc; }





INT_PTR CALLBACK SymMaterialExportOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static SymMaterialExport *imp = NULL;

	switch(message) {
		case WM_INITDIALOG:
			imp = (SymMaterialExport *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return 1;
	}
	return 0;
}


//--- SymMaterialExport -------------------------------------------------------
SymMaterialExport::SymMaterialExport()
{

}

SymMaterialExport::~SymMaterialExport() 
{

}

int SymMaterialExport::ExtCount()
{
	// Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *SymMaterialExport::Ext(int n)
{		
	//#pragma message	(TODO("Return the 'i-th' file name extension (i.e. \"3DS\")."))
	return _T("sym.material");
}

const TCHAR *SymMaterialExport::LongDesc()
{
	//#pragma message	(TODO("Return long ASCII description (i.e. \"Targa 2.0 Image File\""))
	return _T("Symmetry Material File");
}
	
const TCHAR *SymMaterialExport::ShortDesc() 
{			
	//#pragma message	(TODO("Return short ASCII description (i.e. \"Targa\")"))
	return _T("Symmetry Material");
}

const TCHAR *SymMaterialExport::AuthorName()
{			
	//#pragma message	(TODO("Return ASCII Author name"))
	return _T("Matt Phillips");
}

const TCHAR *SymMaterialExport::CopyrightMessage() 
{	
	//#pragma message	(TODO("Return ASCII Copyright message"))
	return _T("(c) Matt Phillips 2008");
}

const TCHAR *SymMaterialExport::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("http://www.hysteriagaming.com");
}

const TCHAR *SymMaterialExport::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int SymMaterialExport::Version()
{				
	//#pragma message	(TODO("Return Version number * 100 (i.e. v3.01 = 301)"))
	return 100;
}

void SymMaterialExport::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL SymMaterialExport::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}

int SymMaterialExport::GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode /*=NULL*/)
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


int	SymMaterialExport::DoExport(const TCHAR* name,ExpInterface* ei,Interface* i, BOOL suppressPrompts, DWORD options)
{
	//Get IGame scene interface
	IGameScene *SceneInterface = GetIGameInterface();

	//Setup OpenGL coodinate system
	IGameConversionManager *ConvManager = GetConversionManager();
	ConvManager->SetCoordSystem(IGameConversionManager::IGAME_OGL);

	//Initialise IGame scene interface
	SceneInterface->InitialiseIGame(true);
	SceneInterface->SetStaticFrame(0);

	std::vector<SYM::SYM_MATERIAL> Materials;

	//Retrieve materials
	for(int i = 0; i < SceneInterface->GetRootMaterialCount(); i++)
	{
		Materials.push_back(MaxToSymMaterial(SceneInterface->GetRootMaterial(i)));
	}

	std::stringstream Stream;
	Stream << "Exported materials:\n\n";

	//Loop through materials
	for(int i = 0; i < Materials.size(); i++)
	{
		//Write the file, get filesize
		CStr Filename(Materials[i].Name.c_str());
		Filename.toLower();
		Filename += ".sym.material";
		int Filesize = Materials[i].Write(Filename.data());

		//Error check
		if (Filesize > 0)
		{
			Stream << "Filename: " << Materials[i].Name.c_str() << ".sym.material\n";
		}
		else
		{
			MessageBox(NULL, "Could not write material file", "Error", MB_OK);
			Stream << "***ERROR EXPORTING MATERIAL: " << Materials[i].Name.c_str() << "\n\n";
		}
	}

	Stream << "\n\n";
	
	MessageBox(NULL, Stream.str().c_str(), "Symmetry Material Exporter", MB_OK);
	
	return TRUE;
}


 
