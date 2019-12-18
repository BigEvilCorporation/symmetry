#include "SYM_Level.h"
#include "SYM_Engine.h"

namespace sym
{
    bool CLevel::Write(std::string Filename)
    {
        //Current MAIN section ID and length
        int MainSection_ID = 0;
        int MainSection_Length = 0;

        //Current SUB section ID and length
        int SubSection_ID = 0;
        int SubSection_Length = 0;
        int SubSection_Start = 0;

        //Current CHILD section ID and length
        int ChildSection_ID = 0;
        int ChildSection_Length = 0;
        int ChildSection_Start = 0;

        //Prefix with scenes path from engine config
		Filename = Engine->ScenePath + Filename;

        Engine->Logger.AddEntry("Writing scene to disk: " + Filename);

        int TempInt = 0;
        float TempFloat = 0;

        //Open file for writing
        File = fopen ( Filename.c_str(), "wb" );

        //Check for errors
        if ( !File )
        {
            Engine->Logger.AddEntry("Error: Could not write scene file " + Filename);
            return false;
        }

        //////////////////////////////////////////////////////////////////
        //ROOT_SECTION
        MainSection_ID = S_ROOT_SECTION;
        MainSection_Length = 6;

        //Write ROOT_SECTION header
        fwrite ( &MainSection_ID, 2, 1, File );

        //Write root section length
        fwrite ( &MainSection_Length, 4, 1, File );

        //////////////////////////////////////////////////////////////////
        //FILE_DESC
        MainSection_ID = S_FILE_DESC;

        ///DEFAULT VALUES
        Name = "Default";
        Description = "Symmetry Engine scene";
        LoadingScreenFile = "default_loading.png";

        //Append '\0' to all strings
        Name += '\0';
        Description += '\0';
        LoadingScreenFile += '\0';

        ///Calculate section length

        ///        HEADER	(6)
        /// + 6 +  LEVES_NAME	(NumChars)
        /// + 6 +  LEVES_DESC	(NumChars)
        /// + 6 +  LOAD_SCREEN	(NumChars)
        /// + 30 + INTEGERS	(5 x 4)

        MainSection_Length = 	  24				        //Headers
                                + Name.size()			    //Level name
                                + Description.size()		//Level description
                                + LoadingScreenFile.size();	//Loading screen

        //Write header
        fwrite ( &MainSection_ID, 2, 1, File );

        //Write length
        fwrite ( &MainSection_Length, 4, 1, File );

            //////////////////////////////////////////////////////////////////
            //S_LEVEL_NAME
            Engine->Logger.AddEntry("Writing scene name");
            SubSection_ID = S_SCENE_NAME;
            SubSection_Length = Name.size() + 6;

            //Write header
            fwrite ( &SubSection_ID, 2, 1, File );

            //Write length
            fwrite ( &SubSection_Length, 4, 1, File );

            //Write object name
            fwrite ( Name.c_str(), 1, Name.size(), File );

            //////////////////////////////////////////////////////////////////
            //S_LEVES_DESC
            Engine->Logger.AddEntry("Writing scene description");
            SubSection_ID = S_SCENE_DESC;
            SubSection_Length = Description.size() + 6;

            //Write header
            fwrite ( &SubSection_ID, 2, 1, File );

            //Write length
            fwrite ( &SubSection_Length, 4, 1, File );

            //Write object name
            fwrite ( Description.c_str(), 1, Description.size(), File );

            //////////////////////////////////////////////////////////////////
            //S_LOAD_SCREEN
            Engine->Logger.AddEntry("Writing scene loading screen");
            SubSection_ID = S_LOAD_SCREEN;
            SubSection_Length = LoadingScreenFile.size() + 6;

            //Write header
            fwrite ( &SubSection_ID, 2, 1, File );

            //Write length
            fwrite ( &SubSection_Length, 4, 1, File );

            //Write object name
            fwrite ( LoadingScreenFile.c_str(), 1, LoadingScreenFile.size(), File );

        //////////////////////////////////////////////////////////////////
        //S_GLOBALS
        MainSection_ID = S_GLOBALS;

        ///DEFAULT VALUES
        TerrainFile = "default.sym.terrain";
        SkyboxFile = "default.sym.sky";
        ScriptFile = "default.lua";

        Amb_r = 254; Amb_g = 254; Amb_b = 254;
        Gravity = 50;

        //Append '\0' to all strings
        TerrainFile += '\0';
        SkyboxFile += '\0';
        ScriptFile += '\0';


        ///Calculate section length

        ///        HEADER	(6)
        /// + 6 + AMB_LIGHT	(12)
        /// + 6 + TERRAIN	(NumChars)
        /// + 6 + SKY_FILE	(NumChars)
        /// + 6 + SCRIPT	(NumChars)
        /// + 6 + GRAVITY	(4)

        MainSection_Length = 	  36			    //Headers
                                + 12			    //Ambient light
                                + TerrainFile.size()//Terrain file
                                + SkyboxFile.size()	//Skybox
                                + ScriptFile.size()	//Script
                                + 4;			    //Gravity

        //Write header
        fwrite ( &MainSection_ID, 2, 1, File );

        //Write length
        fwrite ( &MainSection_Length, 4, 1, File );

            //////////////////////////////////////////////////////////////////
            //S_AMBIENT_LIGHT
            Engine->Logger.AddEntry("Writing scene ambient light");
            SubSection_ID = S_AMBIENT_LIGHT;
            SubSection_Length = 18;

            //Write header
            fwrite(&SubSection_ID, 2, 1, File);

            //Write length
            fwrite(&SubSection_Length, 4, 1, File);

            //Write R
            fwrite(&Amb_r, 4, 1, File);

            //Write G
            fwrite(&Amb_g, 4, 1, File);

            //Write B
            fwrite(&Amb_b, 4, 1, File);

            //////////////////////////////////////////////////////////////////
            //S_TERRAIN_FILE
            Engine->Logger.AddEntry("Writing scene terrain file");
            SubSection_ID = S_TERRAIN_FILE;
            SubSection_Length = TerrainFile.size() + 6;

            //Write header
            fwrite ( &SubSection_ID, 2, 1, File );

            //Write length
            fwrite ( &SubSection_Length, 4, 1, File );

            //Write object name
            fwrite ( TerrainFile.c_str(), 1, TerrainFile.size(), File );

            //////////////////////////////////////////////////////////////////
            //S_SKY_FILE
            Engine->Logger.AddEntry("Writing scene sky file");
            SubSection_ID = S_SKY_FILE;
            SubSection_Length = SkyboxFile.size() + 6;

            //Write header
            fwrite ( &SubSection_ID, 2, 1, File );

            //Write length
            fwrite ( &SubSection_Length, 4, 1, File );

            //Write object name
            fwrite ( SkyboxFile.c_str(), 1, SkyboxFile.size(), File );

            //////////////////////////////////////////////////////////////////
            //S_SCRIPT_FILE
            Engine->Logger.AddEntry("Writing scene script file");
            SubSection_ID = S_SCRIPT_FILE;
            SubSection_Length = ScriptFile.size() + 6;

            //Write header
            fwrite ( &SubSection_ID, 2, 1, File );

            //Write length
            fwrite ( &SubSection_Length, 4, 1, File );

            //Write object name
            fwrite ( ScriptFile.c_str(), 1, ScriptFile.size(), File );

            //////////////////////////////////////////////////////////////////
            //S_GRAVITY
            Engine->Logger.AddEntry("Writing scene gravity");
            SubSection_ID = S_GRAVITY;
            SubSection_Length = 10;

            //Write header
            fwrite(&SubSection_ID, 2, 1, File);

            //Write length
            fwrite(&SubSection_Length, 4, 1, File);

            //Write R
            fwrite(&Gravity, 4, 1, File);

        //////////////////////////////////////////////////////////////
        //GEOMETRY

        //For all geometry
        for(std::map<std::string, CGeometry*> ::iterator g = Engine->Scene.Geometry.begin(); g != Engine->Scene.Geometry.end(); g++)
        {
            //If this geometry is a model
            if((*g).second->Type == SYM_MDL)
            {
                MainSection_ID = S_MODEL;

                Engine->Logger.AddEntry("Writing model " + (*g).second->Name);

                //Append '\0' to names
                std::string ModelName = (*g).second->Name + '\0';
                std::string ModelFilename = (*g).second->ModelFilename + '\0';

                //Calculate S_MODEL section length
                ///       Header 		(6)
                /// + 6 + Name 			(NumChars)
                /// + 6 + Filename		(NumChars)
                /// + 6 + OBJ_POS		(3 x float)
                /// + 6 + OBJ_ROT		(3 x float)
                /// + 6 + OBJ_SCL		(3 x float)
                /// + 6 + MASS          (float)

                MainSection_Length = 	  42		            //All headers
                                        + ModelName.size()		//Name
                                        + ModelFilename.size()	//Filename
                                        + (10 * sizeof(float));	//POS, ROT, SCL, MASS, UV_TILE/OFFSET/ROTATE

                //Write header
                fwrite(&MainSection_ID, 2, 1, File);

                //Write length
                fwrite(&MainSection_Length, 4, 1, File);

                    //////////////////////////////////////////////////////////////
                    //S_MODES_NAME
                    SubSection_ID = S_MODEL_NAME;

                    SubSection_Length = ModelName.size() + 6;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write object name
                    fwrite(ModelName.c_str(), 1, ModelName.size(), File);

                    //////////////////////////////////////////////////////////////
                    //S_MDS_FILENAME
                    SubSection_ID = S_MDL_FILENAME;

                    SubSection_Length = ModelFilename.size() + 6;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write object name
                    fwrite(ModelFilename.c_str(), 1, ModelFilename.size(), File);

                    //////////////////////////////////////////////////////////////////
                    //S_MODES_POS
                    SubSection_ID = S_MODEL_POS;
                    SubSection_Length = 3 * sizeof(float);

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write POS
                    TempFloat = (*g).second->GetPosition().x;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetPosition().y;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetPosition().z;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    //////////////////////////////////////////////////////////////////
                    //S_MODES_ROT
                    SubSection_ID = S_MODEL_ROT;
                    SubSection_Length = 3 * sizeof(float);

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write POS
                    TempFloat = (*g).second->GetRotation().x;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetRotation().y;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetRotation().z;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    //////////////////////////////////////////////////////////////////
                    //S_MODEL_SCL
                    SubSection_ID = S_MODEL_SCL;
                    SubSection_Length = 3 * sizeof(float);

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write SCALE
                    TempFloat = (*g).second->GetDimentions().x;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetDimentions().y;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetDimentions().z;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    //////////////////////////////////////////////////////////////////
                    //S_MODEL_MASS
                    SubSection_ID = S_MODEL_MASS;
                    SubSection_Length = sizeof(float);

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write MASS
                    TempFloat = (*g).second->Mass;
                    fwrite(&TempFloat, sizeof(float), 1, File);
            }
			else if((*g).second->Type == SYM_TERR)
			{
				MainSection_ID = S_TERRAIN;

                Engine->Logger.AddEntry("Writing terrain " + (*g).second->Name);

                //Append '\0' to names
                std::string TerrName = (*g).second->Name + '\0';
                std::string TerrHMapFilename = (*g).second->HMapFilename + '\0';
				std::string TerrMatFilename = (*g).second->MatFilename + '\0';

                //Calculate S_BRUSH section length
                ///       Header 		(6)
                /// + 6 + Name 			(NumChars)
                /// + 6 + HMapFilename	(NumChars)
				/// + 6 + MatFilename	(NumChars)
                /// + 6 + POS			(3 x float)
                /// + 6 + ROT			(3 x float)
                /// + 6 + SCALE			(3 x float)


                MainSection_Length = 	  42		            //All headers
                                        + TerrName.size()		//Name
										+ TerrHMapFilename.size()	//Heightmap
                                        + TerrMatFilename.size()	//Material
                                        + (9 * sizeof(float));	//POS, ROT, SIZE

                //Write header
                fwrite(&MainSection_ID, 2, 1, File);

                //Write length
                fwrite(&MainSection_Length, 4, 1, File);

                    //////////////////////////////////////////////////////////////
                    //S_TERR_NAME
                    SubSection_ID = S_TERR_NAME;

                    SubSection_Length = TerrName.size() + 6;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write object name
                    fwrite(TerrName.c_str(), 1, TerrName.size(), File);

					//////////////////////////////////////////////////////////////
                    //S_HMAP_FILENAME
                    SubSection_ID = S_HMAP_FILENAME;

                    SubSection_Length = TerrHMapFilename.size() + 6;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write object name
                    fwrite(TerrHMapFilename.c_str(), 1, TerrHMapFilename.size(), File);

					//////////////////////////////////////////////////////////////
                    //S_TERR_MATERIAL
                    SubSection_ID = S_TERR_MATERIAL;

                    SubSection_Length = TerrMatFilename.size() + 6;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write object name
                    fwrite(TerrMatFilename.c_str(), 1, TerrMatFilename.size(), File);

					//////////////////////////////////////////////////////////////////
                    //S_TERR_POS
                    SubSection_ID = S_TERR_POS;
                    SubSection_Length = 3 * sizeof(float);

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write POS
                    TempFloat = (*g).second->GetPosition().x;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetPosition().y;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetPosition().z;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    //////////////////////////////////////////////////////////////////
                    //S_TERR_ROT
                    SubSection_ID = S_TERR_ROT;
                    SubSection_Length = 3 * sizeof(float);

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write POS
                    TempFloat = (*g).second->GetRotation().x;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetRotation().y;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetRotation().z;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    //////////////////////////////////////////////////////////////////
                    //S_TERR_SCALE
                    SubSection_ID = S_TERR_SCALE;
                    SubSection_Length = 3 * sizeof(float);

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write SCALE
                    TempFloat = (*g).second->GetDimentions().x;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetDimentions().y;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetDimentions().z;
                    fwrite(&TempFloat, sizeof(float), 1, File);

			}
            else
            {
                //It's a brush

                MainSection_ID = S_BRUSH;

                Engine->Logger.AddEntry("Writing brush " + (*g).second->Name);

                //Append '\0' to names
                std::string BrushName = (*g).second->Name + '\0';
                std::string MatFilename = (*g).second->MatFilename + '\0';

                //Calculate S_BRUSH section length
                ///       Header 		(6)
                /// + 6 + Name 			(NumChars)
                /// + 6 + MatFilename	(NumChars)
                /// + 6 + TYPE          (Int)
                /// + 6 + BRUSH_POS		(3 x float)
                /// + 6 + BRUSH_ROT		(3 x float)
                /// + 6 + BRUSH_SZE		(3 x float)
				/// + 6 + UV_TILE		(2 x float)
				/// + 6 + UV_OFFSET		(2 x float)
				/// + 6 + UV_ROTATE		(float)

                MainSection_Length = 	  66		            //All headers
                                        + BrushName.size()		//Name
                                        + MatFilename.size()	//Material
                                        + 4                     //Type
                                        + (15 * sizeof(float));	//POS, ROT, SIZE, MASS

                //Write header
                fwrite(&MainSection_ID, 2, 1, File);

                //Write length
                fwrite(&MainSection_Length, 4, 1, File);

                    //////////////////////////////////////////////////////////////
                    //S_BRUSH_NAME
                    SubSection_ID = S_BRUSH_NAME;

                    SubSection_Length = BrushName.size() + 6;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write object name
                    fwrite(BrushName.c_str(), 1, BrushName.size(), File);

                    //////////////////////////////////////////////////////////////////
                    //S_TYPE
                    SubSection_ID = S_TYPE;
                    SubSection_Length = 10;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write R
                    fwrite(&(*g).second->Type, 4, 1, File);

                    //////////////////////////////////////////////////////////////
                    //S_MATERIAL
                    SubSection_ID = S_MATERIAL;

                    SubSection_Length = MatFilename.size() + 6;

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write object name
                    fwrite(MatFilename.c_str(), 1, MatFilename.size(), File);

                    //////////////////////////////////////////////////////////////////
                    //S_BRUSH_SIZE
                    SubSection_ID = S_BRUSH_SIZE;
                    SubSection_Length = 3 * sizeof(float);

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write POS
                    TempFloat = (*g).second->GetDimentions().x;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetDimentions().y;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetDimentions().z;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    //////////////////////////////////////////////////////////////////
                    //S_BRUSH_POS
                    SubSection_ID = S_BRUSH_POS;
                    SubSection_Length = 3 * sizeof(float);

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write POS
                    TempFloat = (*g).second->GetPosition().x;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetPosition().y;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetPosition().z;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    //////////////////////////////////////////////////////////////////
                    //S_BRUSH_ROT
                    SubSection_ID = S_BRUSH_ROT;
                    SubSection_Length = 3 * sizeof(float);

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write POS
                    TempFloat = (*g).second->GetRotation().x;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetRotation().y;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    TempFloat = (*g).second->GetRotation().z;
                    fwrite(&TempFloat, sizeof(float), 1, File);

                    //////////////////////////////////////////////////////////////////
                    //S_BRUSH_MASS
                    SubSection_ID = S_BRUSH_MASS;
                    SubSection_Length = sizeof(float);

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write MASS
                    TempFloat = (*g).second->Mass;
                    fwrite(&TempFloat, sizeof(float), 1, File);

					//////////////////////////////////////////////////////////////////
                    //S_UV_TILE
                    SubSection_ID = S_UV_TILE;
                    SubSection_Length = 2 * sizeof(float);

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write UV_TILE
					TempFloat = (*g).second->TextureTile.x;
                    fwrite(&TempFloat, sizeof(float), 1, File);

					TempFloat = (*g).second->TextureTile.y;
					fwrite(&TempFloat, sizeof(float), 1, File);

					//////////////////////////////////////////////////////////////////
                    //S_UV_OFFSET
                    SubSection_ID = S_UV_OFFSET;
                    SubSection_Length = 2 * sizeof(float);

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write UV_OFFSET
                    TempFloat = (*g).second->TextureOffset.x;
                    fwrite(&TempFloat, sizeof(float), 1, File);

					TempFloat = (*g).second->TextureOffset.y;
					fwrite(&TempFloat, sizeof(float), 1, File);

					//////////////////////////////////////////////////////////////////
                    //S_UV_ROTATE
                    SubSection_ID = S_UV_ROTATE;
                    SubSection_Length = sizeof(float);

                    //Write header
                    fwrite(&SubSection_ID, 2, 1, File);

                    //Write length
                    fwrite(&SubSection_Length, 4, 1, File);

                    //Write UV_ROTATION
                    TempFloat = (*g).second->TextureRotation.z;
                    fwrite(&TempFloat, sizeof(float), 1, File);
            }
		}

		//////////////////////////////////////////////////////////////////
		//S_SKYBOX
		MainSection_ID = S_SKYBOX;

		std::map<int, std::string> TexNames = Engine->Scene.Skybox.TextureNames;

		int NameSize = 0;

		for(std::map<int, std::string> ::iterator Texture = TexNames.begin(); Texture != TexNames.end(); Texture++)
		{
			//Add \0 to string
			(*Texture).second += '\0';

			//Add string size to count
			NameSize += (*Texture).second.size();

			//Add integer (type) and header to count
			NameSize += 10;
		}

		MainSection_Length = 6 + NameSize;

		//Write header
		fwrite ( &MainSection_ID, 2, 1, File );

		//Write length
		fwrite ( &MainSection_Length, 4, 1, File );

		for(std::map<int, std::string> ::iterator Texture = TexNames.begin(); Texture != TexNames.end(); Texture++)
		{
			//////////////////////////////////////////////////////////////////
			//S_SKY_TEXTURE
			Engine->Logger.AddEntry("Writing skybox texture");
			SubSection_ID = S_SKY_TEXTURE;
			SubSection_Length = (*Texture).second.size() + 10;

			//Write header
			fwrite ( &SubSection_ID, 2, 1, File );

			//Write length
			fwrite ( &SubSection_Length, 4, 1, File );

			//Write sky texture face
			int SkyFace = (*Texture).first;
			fwrite ( &SkyFace, 1, 4, File );

			//Write object name
			std::string TexName = (*Texture).second;
			fwrite (TexName.c_str(), 1, TexName.size(), File);
		}

        //Close the file
        fclose ( File );

        Engine->Logger.AddEntry("Scene " + Filename + " successfully written");

        return true;
    }

    bool CLevel::Load(std::string Filename)
    {
        //Current MAIN section ID and length
        int MainSection_ID = 0;
        int MainSection_Length = 0;

        //Current SUB section ID and length
        int SubSection_ID = 0;
        int SubSection_Length = 0;
        int SubSection_Start = 0;

        //Current CHILD section ID and length
        int ChildSection_ID = 0;
        int ChildSection_Length = 0;
        int ChildSection_Start = 0;

        //Prefix with scenes path from engine config
		Filename = Engine->ScenePath + Filename;

		Engine->Logger.AddEntry("Loading scene file " + Filename);

        //Open file for reading
        File = fopen ( Filename.c_str(), "rb" );

        //Check for errors
        if ( !File )
        {
            Engine->Logger.AddEntry("Error: Could not open scene file " + Filename);
            return false;
        }

        //Determine filesize (Seek to end, get current position, rewind to start)
        fseek ( File, 0, SEEK_END );
        FileSize = ftell ( File );
        rewind ( File );

        //Loop through file until end
        while ( ftell ( File ) < FileSize )
        {
            //Read the section header
            fread ( &MainSection_ID, 2, 1, File );

            //Read the length of the section
            fread ( &MainSection_Length, 4, 1, File );

            //Determine section type
            switch ( MainSection_ID )
            {
                case S_ROOT_SECTION:
                    ////cout << "\n+S_ROOT_SECTION \tPosition:" << ftell ( File ) - 6 << "\tLength: " << MainSection_Length << "\n|\n";

                    break;

                case S_FILE_DESC:
                    ////cout << "+--S_FILE_DESC \tPosition:" << ftell ( File ) - 6 << "\tLength: " << MainSection_Length << "\n|  |\n";

                    //Store where the subsection was found, so we can return later
                    SubSection_Start = ftell ( File ) - 6;

                    //Zero the subsection ID and length, in case they were used previously
                    SubSection_ID = 0;
                    SubSection_Length = 0;

                    //Loop from subsection Start to subsection End (Mainsection length, + the position it was found)
                    while ( ftell ( File ) < ( MainSection_Length + SubSection_Start ) )
                    {
                        //Read the subchunk ID
                        fread ( &SubSection_ID, 2, 1, File );

                        //Read the length of the subchunk
                        fread ( &SubSection_Length, 4, 1, File );

                        //Determine SubChunk type using its ID
                        switch ( SubSection_ID )
                        {
                            case S_SCENE_NAME:
                                ////cout << "|  +S_SCENE_NAME \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                //Clear current name
                                Name.clear();

                                CharCount = 0;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    Name += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy when using the string
                                Name = Name.substr(0, Name.size() - 1);

                                break;

                            case S_SCENE_DESC:
                                //cout << "|  +S_SCENE_DESC \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                //Clear current name
                                Description.clear();

                                CharCount = 0;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    Description += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy when using the string
                                Description = Description.substr(0, Description.size() - 1);

                                break;

                            case S_LOAD_SCREEN:
                                //cout << "|  +S_LOAD_SCREEN \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                //Clear current name
                                LoadingScreenFile.clear();

                                CharCount = 0;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    LoadingScreenFile += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy when using the string
                                LoadingScreenFile = LoadingScreenFile.substr(0, LoadingScreenFile.size() - 1);

                                break;

                            default:
                                //cout << "|  +**UNKNOWN \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\tID: 0x" << hex << SubSection_ID << dec << "\n|\n";

                                //Advance to next subchunk (Current Position, + SubChunk length, - the header (6))
                                fseek ( File, SubSection_Length - 6, SEEK_CUR );

                                break;
                        }
                    }

                    //Move to end of subsection before returning
                    //(where we found the subsection, + section length, - the header (6))
                    fseek ( File, SubSection_Start + MainSection_Length, SEEK_SET );

                    break;

                case S_GLOBALS:
                    //cout << "+--S_GLOBALS \tPosition:" << ftell ( File ) - 6 << "\tLength: " << MainSection_Length << "\n|  |\n";

                    //Store where the subsection was found, so we can return later
                    SubSection_Start = ftell ( File ) - 6;

                    //Zero the subsection ID and length, in case they were used previously
                    SubSection_ID = 0;
                    SubSection_Length = 0;

                    //Loop from subsection Start to subsection End (Mainsection length, + the position it was found)
                    while ( ftell ( File ) < ( MainSection_Length + SubSection_Start ) )
                    {
                        //Read the subchunk ID
                        fread ( &SubSection_ID, 2, 1, File );

                        //Read the length of the subchunk
                        fread ( &SubSection_Length, 4, 1, File );

                        //Determine SubChunk type using its ID
                        switch ( SubSection_ID )
                        {
                            case S_AMBIENT_LIGHT:
                                //cout << "|  +S_AMBIENT_LIGHT \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &Amb_r, 4, 1, File );
                                fread ( &Amb_g, 4, 1, File );
                                fread ( &Amb_b, 4, 1, File );

                                break;

                            case S_TERRAIN_FILE:
                                //cout << "|  +S_TERRAIN_FILE \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                //Clear current name
                                TerrainFile.clear();

                                CharCount = 0;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    TerrainFile += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy when using the string
                                TerrainFile = TerrainFile.substr(0, TerrainFile.size() - 1);

                                break;

                            case S_SKY_FILE:
                                //cout << "|  +S_SKY_FILE \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                //Clear current name
                                TerrainFile.clear();

                                CharCount = 0;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    SkyboxFile += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy when using the string
                                SkyboxFile = SkyboxFile.substr(0, SkyboxFile.size() - 1);

                                break;

                            case S_SCRIPT_FILE:
                                //cout << "|  +S_SCRIPT_FILE \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                //Clear current name
                                ScriptFile.clear();

                                CharCount = 0;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    ScriptFile += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy when using the string
                                ScriptFile = ScriptFile.substr(0, ScriptFile.size());

                                break;

                            case S_GRAVITY:
                                //cout << "|  +S_GRAVITY \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &Gravity, 4, 1, File );

                                break;

                            default:
                                //cout << "|  +**UNKNOWN \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\tID: 0x" << hex << SubSection_ID << dec << "\n|\n";

                                //Advance to next subchunk (Current Position, + SubChunk length, - the header (6))
                                fseek ( File, SubSection_Length - 6, SEEK_CUR );

                                break;
                        }
                    }

                    //Move to end of subsection before returning
                    //(where we found the subsection, + section length, - the header (6))
                    fseek ( File, SubSection_Start + MainSection_Length, SEEK_SET );

                    break;

                case S_MODEL:
                    //cout << "+--S_MODEL \tPosition:" << ftell ( File ) - 6 << "\tLength: " << MainSection_Length << "\n|  |\n";

                    TempModel = new SYM_TEMPMODEL;

                    TempModel->Scale = CVector3(1.0f, 1.0f, 1.0f);

                    //Store where the subsection was found, so we can return later
                    SubSection_Start = ftell ( File ) - 6;

                    //Zero the subsection ID and length, in case they were used previously
                    SubSection_ID = 0;
                    SubSection_Length = 0;

                    //Loop from subsection Start to subsection End (Mainsection length, + the position it was found)
                    while ( ftell ( File ) < ( MainSection_Length + SubSection_Start ) )
                    {
                        //Read the subchunk ID
                        fread ( &SubSection_ID, 2, 1, File );

                        //Read the length of the subchunk
                        fread ( &SubSection_Length, 4, 1, File );

                        //Determine SubChunk type using its ID
                        switch ( SubSection_ID )
                        {
                            case S_MODEL_NAME:
                                //cout << "|  +S_MODEL_NAME \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                //Clear current name
                                TempModel->Name.clear();

                                CharCount = 0;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    TempModel->Name += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy when using the string
                                TempModel->Name = TempModel->Name.substr(0, TempModel->Name.size() - 1);

                                break;

                            case S_MDL_FILENAME:
                                //cout << "|  +S_MDL_FILENAME \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                //Clear current name
                                TempModel->Filename.clear();

                                CharCount = 0;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    TempModel->Filename += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy when using the string
                                TempModel->Filename = TempModel->Filename.substr(0, TempModel->Filename.size() - 1);

                                break;

                            case S_MODEL_POS:
                                //cout << "|  +S_MODEL_POS \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &TempModel->Position.x, sizeof(float), 1, File );
                                fread ( &TempModel->Position.y, sizeof(float), 1, File );
                                fread ( &TempModel->Position.z, sizeof(float), 1, File );

                                break;

                            case S_MODEL_ROT:
                                //cout << "|  +S_MODEL_ROT \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &TempModel->Rotation.x, sizeof(float), 1, File );
                                fread ( &TempModel->Rotation.y, sizeof(float), 1, File );
                                fread ( &TempModel->Rotation.z, sizeof(float), 1, File );

                                break;

                            case S_MODEL_SCL:
                                //cout << "|  +S_MODEL_SCL \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &TempModel->Scale.x, sizeof(float), 1, File );
                                fread ( &TempModel->Scale.y, sizeof(float), 1, File );
                                fread ( &TempModel->Scale.z, sizeof(float), 1, File );

                                break;

                            case S_MODEL_MASS:
                                //cout << "|  +S_MODEL_MASS \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &TempModel->Mass, sizeof(float), 1, File );

                                break;

                            default:
                                //cout << "|  +**UNKNOWN \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\tID: 0x" << hex << SubSection_ID << dec << "\n|\n";

                                //Advance to next subchunk (Current Position, + SubChunk length, - the header (6))
                                fseek ( File, SubSection_Length - 6, SEEK_CUR );

                                break;
                        }
                    }

                    //Push back vector
                    Models.push_back(*TempModel);

                    //Delete temp object
                    delete TempModel;

                    //Move to end of subsection before returning
                    //(where we found the subsection, + section length, - the header (6))
                    fseek ( File, SubSection_Start + MainSection_Length, SEEK_SET );

                    break;

				case S_TERRAIN:
                    //cout << "+--S_TERRAIN \tPosition:" << ftell ( File ) - 6 << "\tLength: " << MainSection_Length << "\n|  |\n";

                    TempTerrain = new SYM_TEMPTERRAIN;

                    TempTerrain->Scale = CVector3(1.0f, 1.0f, 1.0f);

                    //Store where the subsection was found, so we can return later
                    SubSection_Start = ftell ( File ) - 6;

                    //Zero the subsection ID and length, in case they were used previously
                    SubSection_ID = 0;
                    SubSection_Length = 0;

                    //Loop from subsection Start to subsection End (Mainsection length, + the position it was found)
                    while ( ftell ( File ) < ( MainSection_Length + SubSection_Start ) )
                    {
                        //Read the subchunk ID
                        fread ( &SubSection_ID, 2, 1, File );

                        //Read the length of the subchunk
                        fread ( &SubSection_Length, 4, 1, File );

                        //Determine SubChunk type using its ID
                        switch ( SubSection_ID )
                        {
                            case S_TERR_NAME:
                                //cout << "|  +S_TERR_NAME \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                //Clear current name
                                TempTerrain->Name.clear();

                                CharCount = 0;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    TempTerrain->Name += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy when using the string
                                TempTerrain->Name = TempTerrain->Name.substr(0, TempTerrain->Name.size() - 1);

                                break;

                            case S_HMAP_FILENAME:
                                //cout << "|  +S_HMAP_FILENAME \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                //Clear current name
                                TempTerrain->HMapFilename.clear();

                                CharCount = 0;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    TempTerrain->HMapFilename += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy when using the string
                                TempTerrain->HMapFilename = TempTerrain->HMapFilename.substr(0, TempTerrain->HMapFilename.size() - 1);

                                break;

							case S_TERR_MATERIAL:
                                //cout << "|  +S_TERR_MATERIAL \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                //Clear current name
                                TempTerrain->Material.clear();

                                CharCount = 0;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    TempTerrain->Material += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy when using the string
                                TempTerrain->Material = TempTerrain->Material.substr(0, TempTerrain->Material.size() - 1);

                                break;

                            case S_TERR_POS:
                                //cout << "|  +S_TERR_POS \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &TempTerrain->Position.x, sizeof(float), 1, File );
                                fread ( &TempTerrain->Position.y, sizeof(float), 1, File );
                                fread ( &TempTerrain->Position.z, sizeof(float), 1, File );

                                break;

                            case S_TERR_ROT:
                                //cout << "|  +S_TERR_ROT \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &TempTerrain->Rotation.x, sizeof(float), 1, File );
                                fread ( &TempTerrain->Rotation.y, sizeof(float), 1, File );
                                fread ( &TempTerrain->Rotation.z, sizeof(float), 1, File );

                                break;

                            case S_TERR_SCALE:
                                //cout << "|  +S_TERR_SCALE \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &TempTerrain->Scale.x, sizeof(float), 1, File );
                                fread ( &TempTerrain->Scale.y, sizeof(float), 1, File );
                                fread ( &TempTerrain->Scale.z, sizeof(float), 1, File );

                                break;

                            default:
                                //cout << "|  +**UNKNOWN \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\tID: 0x" << hex << SubSection_ID << dec << "\n|\n";

                                //Advance to next subchunk (Current Position, + SubChunk length, - the header (6))
                                fseek ( File, SubSection_Length - 6, SEEK_CUR );

                                break;
                        }
                    }

                    //Push back vector
                    Terrains.push_back(*TempTerrain);

                    //Delete temp object
                    delete TempTerrain;

                    //Move to end of subsection before returning
                    //(where we found the subsection, + section length, - the header (6))
                    fseek ( File, SubSection_Start + MainSection_Length, SEEK_SET );

                    break;

                case S_BRUSH:
                    //cout << "+--S_BRUSH \tPosition:" << ftell ( File ) - 6 << "\tLength: " << MainSection_Length << "\n|  |\n";

                    TempBrush = new SYM_TEMPBRUSH;

					TempBrush->TextureTile = CVector3(1.0f, 1.0f, 1.0f);

                    //Store where the subsection was found, so we can return later
                    SubSection_Start = ftell ( File ) - 6;

                    //Zero the subsection ID and length, in case they were used previously
                    SubSection_ID = 0;
                    SubSection_Length = 0;

                    //Loop from subsection Start to subsection End (Mainsection length, + the position it was found)
                    while ( ftell ( File ) < ( MainSection_Length + SubSection_Start ) )
                    {
                        //Read the subchunk ID
                        fread ( &SubSection_ID, 2, 1, File );

                        //Read the length of the subchunk
                        fread ( &SubSection_Length, 4, 1, File );

                        //Determine SubChunk type using its ID
                        switch ( SubSection_ID )
                        {
                            case S_BRUSH_NAME:
                                //cout << "|  +S_BRUSH_NAME \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                //Clear current name
                                TempBrush->Name.clear();

                                CharCount = 0;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    TempBrush->Name += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy when using the string
                                TempBrush->Name = TempBrush->Name.substr(0, TempBrush->Name.size() - 1);

                                break;

                            case S_TYPE:
                                //cout << "|  +S_TYPE \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &TempBrush->Type, 4, 1, File );

                                break;

                            case S_MATERIAL:
                                //cout << "|  +S_MATERIAL \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                //Clear current name
                                TempBrush->Material.clear();

                                CharCount = 0;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    TempBrush->Material += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy when using the string
                                TempBrush->Material = TempBrush->Material.substr(0, TempBrush->Material.size() - 1);

                                break;

                            case S_BRUSH_SIZE:
                                //cout << "|  +S_BRUSH_SIZE \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &TempBrush->Dimentions.x, sizeof(float), 1, File );
                                fread ( &TempBrush->Dimentions.y, sizeof(float), 1, File );
                                fread ( &TempBrush->Dimentions.z, sizeof(float), 1, File );

                                break;

                            case S_BRUSH_POS:
                                //cout << "|  +S_BRUSH_POS \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &TempBrush->Position.x, sizeof(float), 1, File );
                                fread ( &TempBrush->Position.y, sizeof(float), 1, File );
                                fread ( &TempBrush->Position.z, sizeof(float), 1, File );

                                break;

                            case S_BRUSH_ROT:
                                //cout << "|  +S_BRUSH_ROT \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &TempBrush->Rotation.x, sizeof(float), 1, File );
                                fread ( &TempBrush->Rotation.y, sizeof(float), 1, File );
                                fread ( &TempBrush->Rotation.z, sizeof(float), 1, File );

                                break;

                            case S_BRUSH_MASS:
                                //cout << "|  +S_BRUSH_MASS \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &TempBrush->Mass, sizeof(float), 1, File );

                                break;

							case S_UV_TILE:
                                //cout << "|  +S_UV_TILE \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &TempBrush->TextureTile.x, sizeof(float), 1, File );
                                fread ( &TempBrush->TextureTile.y, sizeof(float), 1, File );

                                break;

							case S_UV_OFFSET:
                                //cout << "|  +S_UV_OFFSET \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &TempBrush->TextureOffset.x, sizeof(float), 1, File );
                                fread ( &TempBrush->TextureOffset.y, sizeof(float), 1, File );

                                break;

							case S_UV_ROTATE:
                                //cout << "|  +S_UV_ROTATION \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

                                fread ( &TempBrush->TextureRotation.z, sizeof(float), 1, File );

                                break;

                            default:
                                //cout << "|  +**UNKNOWN \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\tID: 0x" << hex << SubSection_ID << dec << "\n|\n";

                                //Advance to next subchunk (Current Position, + SubChunk length, - the header (6))
                                fseek ( File, SubSection_Length - 6, SEEK_CUR );

                                break;
                        }
                    }

                    //Push back vector
                    Brushes.push_back(*TempBrush);

                    //Delete temp object
                    delete TempBrush;

                    //Move to end of subsection before returning
                    //(where we found the subsection, + section length, - the header (6))
                    fseek ( File, SubSection_Start + MainSection_Length, SEEK_SET );

                    break;

				case S_SKYBOX:
                    //cout << "+--S_SKYBOX \tPosition:" << ftell ( File ) - 6 << "\tLength: " << MainSection_Length << "\n|  |\n";

					//Store where the subsection was found, so we can return later
                    SubSection_Start = ftell ( File ) - 6;

                    //Zero the subsection ID and length, in case they were used previously
                    SubSection_ID = 0;
                    SubSection_Length = 0;

                    //Loop from subsection Start to subsection End (Mainsection length, + the position it was found)
                    while ( ftell ( File ) < ( MainSection_Length + SubSection_Start ) )
                    {
                        //Read the subchunk ID
                        fread ( &SubSection_ID, 2, 1, File );

                        //Read the length of the subchunk
                        fread ( &SubSection_Length, 4, 1, File );

                        //Determine SubChunk type using its ID
                        switch ( SubSection_ID )
                        {
                            case S_SKY_TEXTURE:
							{
                                //cout << "|  +S_SKY_TEXTURE \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\n|  |\n";

								std::string TempString;
								int TempInt;

								fread ( &TempInt, 4, 1, File );

                                CharCount = 0;
                                do
                                {
                                    fread ( &TempChar, 1, 1, File );
                                    TempString += TempChar;
                                    CharCount++;
                                }
                                while ( TempChar != '\0' );

                                //Remove '\0' or things get messy when using the string
                                TempString = TempString.substr(0, TempString.size() - 1);

								//Add to textures map
								SkyTextures[TempInt] = TempString;

                                break;
							}

							default:
                                //cout << "|  +**UNKNOWN \tPosition:" << ftell ( File ) - 6 << "\tLength: " << SubSection_Length << "\tID: 0x" << hex << SubSection_ID << dec << "\n|\n";

                                //Advance to next subchunk (Current Position, + SubChunk length, - the header (6))
                                fseek ( File, SubSection_Length - 6, SEEK_CUR );

                                break;
						}

						//Move to end of subsection before returning
						//(where we found the subsection, + section length, - the header (6))
						fseek ( File, SubSection_Start + MainSection_Length, SEEK_SET );

						break;
					}

                default:
                    //cout << "+**UNKNOWN \tPosition:" << ftell ( File ) - 6 << "\tLength: " << MainSection_Length << "\tID: 0x" << hex << MainSection_ID << dec << "\n";

                    //Go to next section (Advance to the end of this section, minus 6)
                    fseek ( File, MainSection_Length - 6, SEEK_CUR );

                    break;
            }
        }

        //Close the file
        fclose(File);

        PostProcess();

        return true;
    }

    void CLevel::PostProcess()
    {
        Engine->Logger.AddEntry("Post-processing scene " + Name);

        //For all models
        for (int i = 0; i < Models.size(); i++)
        {
            //Load model, set position/rotation/mass
            Engine->Scene.AddModel(Models[i].Name, Models[i].Filename, PHYSICS_OBJ_BOX);
            Engine->Scene.GetGeometry(Models[i].Name)->Scale(Models[i].Scale);
            Engine->Scene.GetGeometry(Models[i].Name)->SetPosition(Models[i].Position);
            Engine->Scene.GetGeometry(Models[i].Name)->SetRotation(Models[i].Rotation);
            Engine->Scene.GetGeometry(Models[i].Name)->SetMass(Models[i].Mass);
        }

		//For all terrains
        for (int i = 0; i < Terrains.size(); i++)
        {
            //Load model, set position/rotation/mass
            if(Engine->Scene.AddTerrain(Terrains[i].Name, Terrains[i].HMapFilename))
			{
				Engine->Scene.GetGeometry(Terrains[i].Name)->Scale(Terrains[i].Scale);
				Engine->Scene.GetGeometry(Terrains[i].Name)->SetPosition(Terrains[i].Position);
				Engine->Scene.GetGeometry(Terrains[i].Name)->SetRotation(Terrains[i].Rotation);
				if(Terrains[i].Material.size() > 0) Engine->Scene.GetGeometry(Terrains[i].Name)->SetMaterial(Terrains[i].Material);
			}
        }

        //For all brushes
        for(int i = 0; i < Brushes.size(); i++)
        {
            //Switch type
            switch(Brushes[i].Type)
            {
                case SYM_PLANE:

                    //Add plane
                    Engine->Scene.AddPlane(Brushes[i].Name, LEVEL_COLLISION, Brushes[i].Dimentions);
                    //Engine->Scene.GetGeometry(Brushes[i].Name)->Scale(Brushes[i].Dimentions);
                    Engine->Scene.GetGeometry(Brushes[i].Name)->SetPosition(Brushes[i].Position);
                    break;

                case SYM_BOX:

                    //Add plane
                    Engine->Scene.AddBox(Brushes[i].Name, LEVEL_COLLISION, Brushes[i].Dimentions);
                    //Engine->Scene.GetGeometry(Brushes[i].Name)->Scale(Brushes[i].Dimentions);
                    Engine->Scene.GetGeometry(Brushes[i].Name)->SetPosition(Brushes[i].Position);
                    break;

                default:
                    //Unknown brush type
                    Engine->Logger.AddEntry("Error: Unknown brush type loaded");
                    break;
            }

            CGeometry *Geometry = Engine->Scene.GetGeometry(Brushes[i].Name);

            //Set brush rotation
            if(Geometry) Geometry->SetRotation(Brushes[i].Rotation);

            //Set brush material
            if(Brushes[i].Material.size() > 0) if(Geometry) Geometry->SetMaterial(Brushes[i].Material);

            //Set mass
            if(Geometry) Geometry->SetMass(Brushes[i].Mass);

			//Set UV tile/offset/rotation
			if(Geometry) Geometry->TextureTile = Brushes[i].TextureTile;
			if(Geometry) Geometry->TextureOffset = Brushes[i].TextureOffset;
			if(Geometry) Geometry->TextureRotation = Brushes[i].TextureRotation;
        }

		//Skybox
		if(!SkyTextures.empty()) Engine->Scene.Skybox.LoadSkyTextures(SkyTextures);
    }

} //Namespace
