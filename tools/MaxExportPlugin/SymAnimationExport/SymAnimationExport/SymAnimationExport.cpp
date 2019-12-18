#include <sstream>

#include "SymAnimationExport.h"

#include "3dsmaxsdk_preinclude.h"
#include "IGame/IGame.h"

#define SymAnimationExport_CLASS_ID	Class_ID(0xd9b02582, 0x8ca4fd33)

SYM::SYM_SKELETAL_ANIMATION MaxToSymSkeletalAnimation(Tab<IGameNode*> Bones)
{
	SYM::SYM_SKELETAL_ANIMATION Animation;

	//Loop through bones
	for (int i = 0; i < Bones.Count(); i++)
	{
		int FPS = 5;

		//Get IGame object
		IGameObject *Object = Bones[i]->GetIGameObject();

		//Get controller
		IGameKeyTab Keys;
		IGameControl *Controller = Bones[i]->GetIGameControl();
		
		std::vector<SYM::SYM_MATRIX4> BlankMatrix;

		//Get keys by point3
		if(Controller->GetFullSampledKeys(Keys, FPS, IGAME_TM, true))
		{
			//Keys found, initialise map
			Animation.Keys[Bones[i]->GetNodeID()] = BlankMatrix;

			//Loop through keys
			for (int k = 0; k < Keys.Count(); k++)
			{
				GMatrix Matrix = Keys[k].sampleKey.gval;

				float *Mat = &Matrix[0][0];
				
				SYM::SYM_MATRIX4 SymMatrix(&Matrix[0][0]);

				Animation.Keys[Bones[i]->GetNodeID()].push_back(SymMatrix);
			}
		}
	}

	return Animation;
}

class SymAnimationExport : public SceneExport 
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
		SymAnimationExport();
		virtual ~SymAnimationExport();

	protected:
		int GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode =NULL);

	private:
		static HWND hParams;
};



class SymAnimationExportClassDesc : public ClassDesc2 
{
public:
	virtual int IsPublic() 							{ return TRUE; }
	virtual void* Create(BOOL /*loading = FALSE*/) 		{ return new SymAnimationExport(); }
	virtual const TCHAR *	ClassName() 			{ return GetString(IDS_CLASS_NAME); }
	virtual SClass_ID SuperClassID() 				{ return SCENE_EXPORT_CLASS_ID; }
	virtual Class_ID ClassID() 						{ return SymAnimationExport_CLASS_ID; }
	virtual const TCHAR* Category() 				{ return GetString(IDS_CATEGORY); }

	virtual const TCHAR* InternalName() 			{ return _T("SymAnimationExport"); }	// returns fixed parsable name (scripter-visible name)
	virtual HINSTANCE HInstance() 					{ return hInstance; }					// returns owning module handle
	

};

static SymAnimationExportClassDesc SymAnimationExportDesc;
ClassDesc2* GetSymAnimationExportDesc() { return &SymAnimationExportDesc; }





INT_PTR CALLBACK SymAnimationExportOptionsDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	static SymAnimationExport *imp = NULL;

	switch(message) {
		case WM_INITDIALOG:
			imp = (SymAnimationExport *)lParam;
			CenterWindow(hWnd,GetParent(hWnd));
			return TRUE;

		case WM_CLOSE:
			EndDialog(hWnd, 0);
			return 1;
	}
	return 0;
}


//--- SymAnimationExport -------------------------------------------------------
SymAnimationExport::SymAnimationExport()
{

}

SymAnimationExport::~SymAnimationExport() 
{

}

int SymAnimationExport::ExtCount()
{
	// Returns the number of file name extensions supported by the plug-in.
	return 1;
}

const TCHAR *SymAnimationExport::Ext(int n)
{		
	//#pragma message	(TODO("Return the 'i-th' file name extension (i.e. \"3DS\")."))
	return _T("sym.animation");
}

const TCHAR *SymAnimationExport::LongDesc()
{
	//#pragma message	(TODO("Return long ASCII description (i.e. \"Targa 2.0 Image File\""))
	return _T("Symmetry Skeletal Animation");
}
	
const TCHAR *SymAnimationExport::ShortDesc() 
{			
	//#pragma message	(TODO("Return short ASCII description (i.e. \"Targa\")"))
	return _T("Symmetry Animation");
}

const TCHAR *SymAnimationExport::AuthorName()
{			
	//#pragma message	(TODO("Return ASCII Author name"))
	return _T("Matt Phillips");
}

const TCHAR *SymAnimationExport::CopyrightMessage() 
{	
	//#pragma message	(TODO("Return ASCII Copyright message"))
	return _T("(c) Matt Phillips 2008");
}

const TCHAR *SymAnimationExport::OtherMessage1() 
{		
	//TODO: Return Other message #1 if any
	return _T("http://www.hysteriagaming.com");
}

const TCHAR *SymAnimationExport::OtherMessage2() 
{		
	//TODO: Return other message #2 in any
	return _T("");
}

unsigned int SymAnimationExport::Version()
{				
	//#pragma message	(TODO("Return Version number * 100 (i.e. v3.01 = 301)"))
	return 100;
}

void SymAnimationExport::ShowAbout(HWND hWnd)
{			
	// Optional
}

BOOL SymAnimationExport::SupportsOptions(int ext, DWORD options)
{
	// TODO Decide which options to support.  Simply return
	// true for each option supported by each Extension 
	// the exporter supports.

	return TRUE;
}

int SymAnimationExport::GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode /*=NULL*/)
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


int	SymAnimationExport::DoExport(const TCHAR* name,ExpInterface* ei,Interface* i, BOOL suppressPrompts, DWORD options)
{
	//Get IGame scene interface
	IGameScene *SceneInterface = GetIGameInterface();

	//Setup OpenGL coodinate system
	IGameConversionManager *ConvManager = GetConversionManager();
	ConvManager->SetCoordSystem(IGameConversionManager::IGAME_OGL);

	//Initialise IGame scene interface
	SceneInterface->InitialiseIGame(true);
	SceneInterface->SetStaticFrame(0);

	//Get bones
	Tab<IGameNode*> Bones = SceneInterface->GetIGameNodeByType(IGameObject::IGAME_BONE);

	//Retrieve animation
	SYM::SYM_SKELETAL_ANIMATION SymAnimation = MaxToSymSkeletalAnimation(Bones);

	std::stringstream Stream;

	Stream << "Exported animation:\n\n";

	//Write the file, get filesize
	CStr Filename(name);
	Filename.toLower();
	int Filesize = SymAnimation.Write(Filename.data());

	if(Filesize > 0)
	{
		int NumKeys = 0;

		for(std::map<int, std::vector<SYM::SYM_MATRIX4>>::iterator i = SymAnimation.Keys.begin(); i != SymAnimation.Keys.end(); i++)
		{
			Stream << "BoneID: " << i->first << "\tNumKeys: " << i->second.size() << "\n";
		}

		Stream << "Filesize: " << Filesize << "\n\n";
	}

	MessageBox(NULL, Stream.str().c_str(), "Symmetry Animation Exporter", MB_OK);
	
	return TRUE;
}


 
