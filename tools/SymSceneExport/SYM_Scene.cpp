#include "SYM_Scene.h"

namespace sym
{
	int CScene::Write(std::string Filename)
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
		//Filename = Engine->ScenePath + Filename;

        //Engine->Logger.AddEntry("Writing scene to disk: " + Filename);

        int TempInt = 0;
        float TempFloat = 0;

        //Open file for writing
        FILE *File = fopen ( Filename.c_str(), "wb" );

        //Check for errors
        if ( !File )
        {
            //Engine->Logger.AddEntry("Error: Could not write scene file " + Filename);
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

        //////////////////////////////////////////////////////////////
        //GEOMETRY

        //For all geometry
        for(std::vector<CCustomBrush> ::iterator g = Geometry.begin(); g != Geometry.end(); g++)
        {
			MainSection_ID = S_CUSTOM_BRUSH;

            //Engine->Logger.AddEntry("Writing brush " + (*g).Name);

            //Append '\0' to names
            std::string BrushName = (*g).Name + '\0';
            std::string MatFilename = (*g).MaterialName + '\0';

            //Calculate S_BRUSH section length
            ///       Header 		(6)
            /// + 6 + Name 			(NumChars)
            /// + 6 + MatFilename	(NumChars)
            /// + 6 + BRUSH_POS		(3 x float)
            /// + 6 + BRUSH_ROT		(3 x float)
			/// + 6 + NUM_VERTS		(int)
			/// + 6 + NUM_FACE		(int)
			/// + 6 + VERTS			3x float (x NumVertices), vertex X, Y, Z
			/// + 6 + FACES			3x int + 6x float (x NumFaces)
			/// + 6 + PHYSICS		4

            MainSection_Length = 	  60		            //All headers
                                    + BrushName.size()		//Name
                                    + MatFilename.size()	//Material
                                    + (7 * 4)	//POS, ROT
									+ 8						//NUM_VERT + FACES
									+ (3 * 4) * (*g).Vertices.size()
									+ (9 * 4) * (*g).Faces.size()
									+ 4;

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
                //S_BRUSH_POS
                SubSection_ID = S_BRUSH_POS;
                SubSection_Length = 3 * sizeof(float) + 6;

                //Write header
                fwrite(&SubSection_ID, 2, 1, File);

                //Write length
                fwrite(&SubSection_Length, 4, 1, File);

                //Write POS
                TempFloat = (*g).Position.x;
                fwrite(&TempFloat, sizeof(float), 1, File);

                TempFloat = (*g).Position.y;
                fwrite(&TempFloat, sizeof(float), 1, File);

                TempFloat = (*g).Position.z;
                fwrite(&TempFloat, sizeof(float), 1, File);

                //////////////////////////////////////////////////////////////////
                //S_BRUSH_ROT
                SubSection_ID = S_BRUSH_ROT;
                SubSection_Length = 4 * sizeof(float) + 6;

                //Write header
                fwrite(&SubSection_ID, 2, 1, File);

                //Write length
                fwrite(&SubSection_Length, 4, 1, File);

                //Write POS
                TempFloat = (*g).Rotation.x;
                fwrite(&TempFloat, sizeof(float), 1, File);

                TempFloat = (*g).Rotation.y;
                fwrite(&TempFloat, sizeof(float), 1, File);

                TempFloat = (*g).Rotation.z;
                fwrite(&TempFloat, sizeof(float), 1, File);

				TempFloat = (*g).Rotation.w;
                fwrite(&TempFloat, sizeof(float), 1, File);

			//////////////////////////////////////////////////////////////////
			//NUM_VERTICES
			SubSection_ID = NUM_VERTICES;
			SubSection_Length = 10;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			int Size = (*g).Vertices.size();

			//Write numvertices
			fwrite(&Size, 4, 1, File);

			//////////////////////////////////////////////////////////////////
			//NUM_FACES
			SubSection_ID = NUM_FACES;
			SubSection_Length = 10;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			Size = (*g).Faces.size();

			//Write numfaces
			fwrite(&Size, 4, 1, File);

			//////////////////////////////////////////////////////////////////
			//VERTICES_LIST
			SubSection_ID = VERTICES_LIST;

			//Calculate section size
			SubSection_Length = (int)(12 * (*g).Vertices.size()) + 6;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//For all vertices
			for (int j = 0; j < (*g).Vertices.size(); j++)
			{

				//Write X
				fwrite(&(*g).Vertices[j].x, 4, 1, File);

				//Write Y
				fwrite(&(*g).Vertices[j].y, 4, 1, File);

				//Write Z
				fwrite(&(*g).Vertices[j].z, 4, 1, File);
			}

			//////////////////////////////////////////////////////////////////
			//FACE_LIST
			SubSection_ID = FACE_LIST;

			//Calculate section size
			SubSection_Length = (36 * (*g).Faces.size()) + 6;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//For all faces
			for (int j = 0; j < (*g).Faces.size(); j++)
			{
				//Write A
				fwrite(&(*g).Faces[j].Indices[0], 4, 1, File);

				//Write B
				fwrite(&(*g).Faces[j].Indices[1], 4, 1, File);

				//Write C
				fwrite(&(*g).Faces[j].Indices[2], 4, 1, File);

				//Write UV coord A
				fwrite(&(*g).Faces[j].UVCoords[0].u, 4, 1, File);
				fwrite(&(*g).Faces[j].UVCoords[0].v, 4, 1, File);

				//Write UV coord B
				fwrite(&(*g).Faces[j].UVCoords[1].u, 4, 1, File);
				fwrite(&(*g).Faces[j].UVCoords[1].v, 4, 1, File);

				//Write UV coord C
				fwrite(&(*g).Faces[j].UVc.u, 4, 1, File);
				fwrite(&(*g).Faces[j].UVc.v, 4, 1, File);
			}

			//////////////////////////////////////////////////////////////////
            //S_MODEL_PHYSICS
            SubSection_ID = S_MODEL_PHYSICS;
            SubSection_Length = 10;

            //Write header
            fwrite(&SubSection_ID, 2, 1, File);

            //Write length
            fwrite(&SubSection_Length, 4, 1, File);

			int Physics = 4;

            //Write PHYSICS
            fwrite(&Physics, 4, 1, File);
		}

		int Filesize = ftell(File);

        //Close the file
        fclose ( File );

        //Engine->Logger.AddEntry("Scene " + Filename + " successfully written");

        return Filesize;
    }
}