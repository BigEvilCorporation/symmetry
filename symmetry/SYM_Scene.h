#ifndef __SYM_SCENE
#define __SYM_SCENE

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#include <map>

#include "SYM_AABB.h"
#include "SYM_Animation.h"
#include "SYM_Brush.h"
#include "SYM_Camera.h"
#include "SYM_Level.h"
#include "SYM_Model.h"
#include "SYM_Octree.h"
#include "SYM_Physics.h"
#include "SYM_Shader.h"
#include "SYM_Skybox.h"
#include "SYM_Terrain.h"
#include "SYM_Texture.h"

#define S_ROOT_SECTION		0x0001	//No data, defines whether or not this is a .sym.scene file

#define S_FILE_DESC		    0xA000  //No data, header to description of file contents
#define     S_SCENE_NAME	0xA001  //'\0' terminated CHAR string, name of scene
#define     S_SCENE_DESC	0xA002  //'\0' terminated CHAR string, detailed description of scene
#define     S_LOAD_SCREEN	0xA003  //'\0' terminated CHAR string, filename of a PNG file

#define S_GLOBALS		    0xB000  //No data, header to globals
#define     S_AMBIENT_LIGHT	0xB008  //3x UNSIGNED SHORT INT, colour R, G and B (0 - 255)
#define     S_TERRAIN_FILE	0xB009  //'\0' terminated CHAR string, filename of terrain file
#define     S_SKY_FILE	    0xB010  //'\0' terminated CHAR string, filename of sky file
#define     S_SCRIPT_FILE	0xB011  //'\0' terminated CHAR string, filename of script file
#define     S_GRAVITY	    0xB012  //UNSIGNED SHORT INT, percentage of gravity (0 - 100)

#define S_MODEL		        0xC000  //No data, header to a model
#define     S_MODEL_NAME    0xC001  //'\0' terminated CHAR string, name of model
#define     S_MDL_FILENAME	0xC002  //'\0' terminated CHAR string, filename of .sym.mesh model file
#define     S_MODEL_POS	    0xC003  //3x FLOAT, model position X, Y and Z
#define     S_MODEL_ROT	    0xC004  //3x FLOAT, model rotation, X, Y and Z
#define     S_MODEL_MASS    0xC005  //FLOAT, model mass
#define     S_MODEL_SCL	    0xC006  //3x FLOAT, model rotation, X, Y and Z
#define		S_MODEL_PHYSICS	0xC007	//int, physics type
#define		S_CUSTOM_DATA	0xC008	//1x int (Byte of custom data)

#define S_BRUSH             0xC100  //No data, header to a brush
#define     S_BRUSH_NAME    0xC101  //'\0' terminated CHAR string, name of model
#define     S_TYPE	        0xC102  //UNSIGNED SHORT INT, type of brush
#define     S_MATERIAL      0xC103  //'\0' terminated CHAR string, filename of material
#define     S_BRUSH_SIZE    0xC104  //3x FLOAT, dimentions, depending on type
#define     S_BRUSH_POS	    0xC105  //3x FLOAT, model position X, Y and Z
#define     S_BRUSH_ROT	    0xC106  //3x FLOAT, model rotation, X, Y and Z
#define     S_BRUSH_MASS    0xC107  //FLOAT, brush mass
#define		S_BRUSH_PHYSICS	0xC108	//int, physics type

#define		S_UV_TILE		0xC200	//2x FLOAT, U and V tiling
#define		S_UV_OFFSET		0xC201	//2x FLOAT, U and V tiling
#define		S_UV_ROTATE		0xC202	//FLOAT, UV rotation

#define S_CUSTOM_BRUSH      0xF300  //No data, header to a brush
#define     CB_BRUSH_NAME   0xF301  //'\0' terminated CHAR string, name of model
#define     CB_MATERIAL     0xF302  //'\0' terminated CHAR string, filename of material
#define     CB_BRUSH_POS	0xF303  //3x FLOAT, model position, X, Y and Z
#define     CB_BRUSH_ROT	0xF304  //3x FLOAT, model rotation, X, Y and Z

#define 	CB_NUM_VERTICES	0xF305 //UNSIGNED LONG INT, number of vertices in this object (also number of UV coords)
#define 	CB_NUM_FACES	0xF306 //UNSIGNED LONG INT, number of faces in this object
#define 	CB_VERTICES_LIST	0xF307 //4x FLOAT + INT (x NumVertices), vertex X, Y, Z
#define 	CB_FACE_LIST	0xF308 //3x UNSIGNED LONG INT (x NumFaces), a face consisting of vertex number A, B and C

#define S_TERRAIN			0xC300	//No data, header to a terrain
#define     S_TERR_NAME		0xC301  //'\0' terminated CHAR string, name of model
#define     S_HMAP_FILENAME	0xC302  //'\0' terminated CHAR string, filename of PNG heightmap file
#define     S_TERR_MATERIAL	0xC303  //'\0' terminated CHAR string, filename of material
#define     S_TERR_SCALE	0xC304  //3x FLOAT, scale
#define     S_TERR_POS	    0xC305  //3x FLOAT, position
#define     S_TERR_ROT	    0xC306  //3x FLOAT, rotation

#define S_LIGHT			    0xD000  //No data, header to a light
#define     S_LIGHT_NAME	0xD001  //'\0' terminated CHAR string, name of light
#define     S_LIGHT_TYPE    0xD002  //UNSIGNED SHORT INT, light type
#define     S_LIGHT_COLOUR	0xD003  //3x UNSIGNED SHORT INT, colour R, G and B (0 - 255)
#define     S_LIGHT_POS	    0xD004  //3x FLOAT, light position X, Y and Z
#define     S_LIGHT_ROT	    0xD005  //3x FLOAT, light rotation, X, Y and Z

#define S_SPAWN			    0xE000  //No data, header to a spawn point
#define     S_SPAWN_POS	    0xE001  //3x FLOAT, spawn point position X, Y and Z

#define S_PICKUP		    0xF000  //No data, header to a pickup
#define     S_PICKUP_NAME	0xF001  //'\0' terminated CHAR string, name of pickup
#define     S_PICKUP_TYPE	0xF002  //UNSIGNED SHORT INT, type of pickup
#define     S_PICKUP_POS	0xF003  //3x FLOAT, pickup position X, Y and Z

#define S_SOUND			    0xAA00  //No data, header to an ambient sound
#define     S_SND_NAME	    0xAA01  //'\0' terminated CHAR string, name of sound
#define     S_SND_FILE	    0xAA02  //'\0' terminated CHAR string, filename of sound
#define     S_SND_POS	    0xAA03  //3x FLOAT, sound position X, Y and Z
#define     S_SND_PLAY      0xAA04  //UNSIGNED SHORT INT, should sound be initially played: 0: Stop, 1: Play, 2: Loop
#define     S_SND_VOL       0xAA05  //UNSIGNED SHORT INT, volume of sound, 0 - 100

#define S_SKYBOX			0xAB00	//No data, header to skybox textures
#define		S_SKY_TEXTURE	0xAB01	//'\0' terminated CHAR string, filename of texture

#define		SKY_NORTH		0xAC01
#define		SKY_SOUTH		0xAC02
#define		SKY_EAST		0xAC03
#define		SKY_WEST		0xAC04
#define		SKY_TOP			0xAC05
#define		SKY_BOTTOM		0xAC06

//Pickup types - TODO: move to pickups header
#define SYM_HEALTHPACK 0
#define SYM_WEAPON1 1

//Light types - TODO: move to light header
#define SYM_SPOTLIGHT 0
#define SYM_DIRECLIGHT 1

namespace sym
{
	class CScene
	{
		public:
			//Constructor
			CScene();
			~CScene();

			bool Load(std::string Filename);
            bool Write(std::string Filename);

			//Initialise scene
			void Init();

			//Shutdown scene
			void Shutdown();

			//Clear the scene
			void ClearScene();

			//Render scene
			void Render();

			//Force a single render pass (called during loading screens, etc)
			void ForceRender();

			//Build the octree, set physics world size
			void ProcessScene();

			//Get world (octree) size
			CBoundingBox GetWorldSize();

			//Test if geometry exists
			bool GeometryExists(std::string Name);

			//Rename geometry
			bool RenameGeometry(CGeometry *Geom, std::string Name);

			//Add brushes
			CBrush* AddBox(std::string Name, int PhysType, CVector3 Dim = CVector3(5.0f, 5.0f, 5.0f));
			CBrush* AddPlane(std::string Name, int PhysType, CVector3 Dim = CVector3(5.0f, 5.0f, 5.0f));
			CBrush* AddCylinder(std::string Name, int PhysType, CVector3 Dim = CVector3(5.0f, 5.0f, 5.0f));
			CBrush* AddSphere(std::string Name, int PhysType, CVector3 Dim = CVector3(5.0f, 5.0f, 5.0f));
			CBrush* AddCone(std::string Name, int PhysType, CVector3 Dim = CVector3(5.0f, 5.0f, 5.0f));

			//Load a model
			CModel* AddModel(std::string Name, std::string Filename, int PhysType);

			//Add existing geometry
			void AddGeometry(CGeometry *Geom);

			//Load a heightmap
			bool AddTerrain(std::string Name, std::string Filename);

			//Delete object
			void DeleteGeometry(std::string Name);
			void DeleteGeometry(CGeometry *Geom);

			//Get pointer to geometry by name
			CGeometry *GetGeometry(std::string Name);

			//Load a skeletal animation
			CSkeletalAnimation *LoadAnimation(std::string Name, std::string Filename);

			//Get animation by name
			CSkeletalAnimation *GetAnimation(std::string Name);

			//Load skybox textures
			void LoadSkyboxTextures(std::string Front, std::string Back, std::string Left, std::string Right, std::string Up, std::string Down);

			//Set draw skybox on/off
			void ShowSkybox(bool Visible);

			//Set draw grid on/off
			void ShowGrid(bool Visible);

			//Set draw collision on/off
			void ShowCollision(bool Visible);

			//Set draw debug info on/off
			void ShowDebugInfo(bool Visible);

			//Set draw octree nodes on/off
			void ShowOctree(bool Visible);

			void SetUseOctree(bool UseOct);

			//Skybox
			//CSkybox Skybox;
			std::vector<CModel*> Skyboxes;

			//Render list
			std::vector<CGeometry*> RenderList;

			//Mapped container of geometry
			std::map<std::string, CGeometry*> Geometry;

			//Map of animations
			std::map<std::string, CSkeletalAnimation*> Animations;

			//Octree root node
			COctree Octree;

			//Set active camera
			void SetActiveCamera(CCamera *Cam) { ActiveCamera = Cam; }
			CCamera *GetActiveCamera() { return ActiveCamera; }

			void SetAmbientLight(CColour Amb) { AmbientLight = Amb; }
			void SetSunlight(CColour Sun) { Sunlight = Sun; }
			void SetSunPosition(CVector3 Pos) { SunPosition = Pos; }
			CColour GetAmbientLight() { return AmbientLight; }
			CColour GetSunlight() { return Sunlight; }
			CVector3 GetSunPosition() { return SunPosition; }

			//Load post-processing shader
			CShader *LoadPostShader(std::string Filename);
			CShader *GetPostShader();

		//private:

			//Lighting
			CColour AmbientLight;
			CColour Sunlight;
			CVector3 SunPosition;

			//Default cameras
			CCamera DefaultCamera;

			//Active camera
			CCamera *ActiveCamera;

			//Draw sky
			bool DrawSky;

			//Show grid
			bool DrawGrid;

			//Show collision boxes
			bool DrawCollision;

			//Show debug info
			bool DrawDebugInfo;

			//Show octree nodes
			bool DrawOctree;

			//Show bones
			bool DrawBones;

			//Use the octree
			bool UseOctree;

			std::string GetUniqueName();

			//Post-processing shader
			CShader *PostShader;

			//FBO objects
			GLuint FBO;
			GLuint RTTFBO;

			//Depth buffer
			GLuint DepthBuffer;

			//Textures
			CTexture FBOTexture;
			CTexture RTTTexture;

			//RTT display list
			GLuint RTTDisplayList;

			//Has the FBO been initialised
			bool FBOInitialised;

			CGparameter BloomAlpha;
			float CgTime;

			//Internal draw function
			void Draw();
	};
} //Namespace

#endif
