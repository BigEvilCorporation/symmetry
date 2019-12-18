#ifndef __CLevel
#define __CLevel

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#include <iostream>
#include <string>
#include <vector>

#include "SYM_Material.h"
#include "SYM_Geometry.h"
#include "SYM_Model.h"
#include "SYM_Brush.h"

#include <GL/gl.h>
#include <GL/glu.h>

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

#define S_BRUSH             0xC100  //No data, header to a brush
#define     S_BRUSH_NAME    0xC101  //'\0' terminated CHAR string, name of model
#define     S_TYPE	        0xC102  //UNSIGNED SHORT INT, type of brush
#define     S_MATERIAL      0xC103  //'\0' terminated CHAR string, filename of material
#define     S_BRUSH_SIZE    0xC104  //3x FLOAT, dimentions, depending on type
#define     S_BRUSH_POS	    0xC105  //3x FLOAT, model position X, Y and Z
#define     S_BRUSH_ROT	    0xC106  //3x FLOAT, model rotation, X, Y and Z
#define     S_BRUSH_MASS    0xC107  //FLOAT, brush mass

#define		S_UV_TILE		0xC200	//2x FLOAT, U and V tiling
#define		S_UV_OFFSET		0xC201	//2x FLOAT, U and V tiling
#define		S_UV_ROTATE		0xC202	//FLOAT, UV rotation

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
    struct SYM_TEMPMODEL
    {
            std::string Name;
            std::string Filename;
            CVector3 Position;
            CVector3 Rotation;
            CVector3 Scale;
            float Mass;
    };

	struct SYM_TEMPTERRAIN
    {
            std::string Name;
            std::string HMapFilename;
			std::string Material;
            CVector3 Position;
            CVector3 Rotation;
            CVector3 Scale;
    };

    struct SYM_TEMPBRUSH
    {
            std::string Name;
            int Type;
            std::string Material;
            CVector3 Dimentions;
            CVector3 Position;
            CVector3 Rotation;
			CVector3 TextureOffset;
			CVector3 TextureTile;
			CVector3 TextureRotation;
            float Mass;
    };

    class CLevel
    {
        public:
            //Level name
			std::string Name;

            //Level description
			std::string Description;

            //Loading screen
            std::string LoadingScreenFile;

            //Ambient light
            float Amb_r, Amb_g, Amb_b;

            //Terrain filename
            std::string TerrainFile;

            //Skybox filename
            std::string SkyboxFile;

            //Script filename
            std::string ScriptFile;

            //Gravity
            int Gravity;

            //Object list
            std::vector <SYM_TEMPMODEL> Models;
			std::vector <SYM_TEMPTERRAIN> Terrains;
            std::vector <SYM_TEMPBRUSH> Brushes;
            SYM_TEMPMODEL *TempModel;
			SYM_TEMPTERRAIN *TempTerrain;
            SYM_TEMPBRUSH *TempBrush;

			//Skybox
			std::map<int, std::string> SkyTextures;

            /*
            //Spotlight list
            vector <SYM_SPOTLIGHT> Spotlights;

            //Spawn point list
            vector <SYM_SPAWNPOINT> SpawnPoints;

            //Pickups list
            vector <SYM_PICKUP> Pickups;

            //Ambient sounds list
            vector <SYM_SOUND> Amb_Sounds;
            */

            //Functions
            bool Load(std::string Filename);
            bool Write(std::string Filename);
            void PostProcess();

        private:
            //The file itself
            FILE * File;

            //Used for file seeking
            int FileSize;
            int FilePos;

            //Object count
            int ObjectCount;

            //Material count
            int MaterialCount;

            //Temporary char for reading in CHARs
            char TempChar;

            //Count number of chars
            int CharCount;

            //Temporary string for writing Section IDs
            std::string SectionID;

            //Temporary stream for writing scene data
            std::string TempFileString;
    };

} //Namespace

#endif
