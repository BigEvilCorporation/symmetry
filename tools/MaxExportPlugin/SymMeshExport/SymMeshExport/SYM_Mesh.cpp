#include <iostream>
#include <sstream>

#include "SYM_Mesh.h"

namespace SYM
{
	SYM_MESHSUBSET *SYM_MESH::NewSubset()
	{
		//Create new subset
		SYM_MESHSUBSET *Subset = new SYM_MESHSUBSET;

		//Add to vector
		Subsets.push_back(*Subset);

		//Return pointer
		return Subset;
	}

	int SYM_MESH::Write(std::string Filename)
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

		//Open file for writing
		FILE *File = fopen(Filename.c_str(), "wb");

		//Check for errors
		if (!File) return -1;

		//////////////////////////////////////////////////////////////////
		//ROOT_SECTION
		MainSection_ID = ROOT_SECTION;
		MainSection_Length = 6;

		//Write ROOT_SECTION header
		fwrite(&MainSection_ID, 2, 1, File);

		//Write root section length
		fwrite(&MainSection_Length, 4, 1, File);

		//////////////////////////////////////////////////////////////////
		//OBJECTS

		//For all objects
		for (int i = 0; i < Subsets.size(); i++)
		{
			MainSection_ID = OBJECT;

			//Append '\0' to name
			Subsets[i].Name += '\0';
			Subsets[i].MaterialName += '\0';

			//Calculate OBJECT section length
			///       Header 		        (6)
			/// + 6 + Name (char)		    (Chars in name)
			/// + 6 + MatName (char)		(Chars in name)
			/// + 6 + NumVertices (int)     (4)
			/// + 6 + NumFaces (int)	    (4)
			/// + 6 + VerticesList	(float)	(3 * sizeof(float) * NumVertices)
			/// + 6 + FacesList (int)	    (36 * NumFaces)


			MainSection_Length =      42					            //All headers
                                    + 8					                //All single integers
                                    + Subsets[i].Name.size()			//Name
                                    + Subsets[i].MaterialName.size()
                                    + (44 * Subsets[i].Vertices.size())	//Vertices list
                                    + (36 * Subsets[i].Faces.size());//Face list

			//Write header
			fwrite(&MainSection_ID, 2, 1, File);

			//Write length
			fwrite(&MainSection_Length, 4, 1, File);

			//////////////////////////////////////////////////////////////////
			//OBJ_NAME
			SubSection_ID = OBJ_NAME;

			SubSection_Length = (int)Subsets[i].Name.size() + 6;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//Write object name
			fwrite(Subsets[i].Name.c_str(), 1, Subsets[i].Name.size(), File);

			//////////////////////////////////////////////////////////////////
			//NUM_VERTICES
			SubSection_ID = NUM_VERTICES;
			SubSection_Length = 10;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			int Size = Subsets[i].Vertices.size();

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

			Size = Subsets[i].Faces.size();

			//Write numfaces
			fwrite(&Size, 4, 1, File);

			//////////////////////////////////////////////////////////////////
			//VERTICES_LIST
			SubSection_ID = VERTICES_LIST;

			//Calculate section size
			SubSection_Length = (int)(44 * Subsets[i].Vertices.size()) + 6;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//For all vertices
			for (int j = 0; j < Subsets[i].Vertices.size(); j++)
			{
				//Write X
				fwrite(&Subsets[i].Vertices[j].x, 4, 1, File);

				//Write Y
				fwrite(&Subsets[i].Vertices[j].y, 4, 1, File);

				//Write Z
				fwrite(&Subsets[i].Vertices[j].z, 4, 1, File);

				for(int b = 0; b < 4; b++)
				{
					//Write Weight
					fwrite(&Subsets[i].Vertices[j].Weights[b], 4, 1, File);

					//Write BoneID
					fwrite(&Subsets[i].Vertices[j].BoneIDs[b], 4, 1, File);
				}
			}

			//////////////////////////////////////////////////////////////////
			//FACE_LIST
			SubSection_ID = FACE_LIST;

			//Calculate section size
			SubSection_Length = (36 * Subsets[i].Faces.size()) + 6;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//For all faces
			for (int j = 0; j < Subsets[i].Faces.size(); j++)
			{
				//Write A
				fwrite(&Subsets[i].Faces[j].Indices[0], 4, 1, File);

				//Write B
				fwrite(&Subsets[i].Faces[j].Indices[1], 4, 1, File);

				//Write C
				fwrite(&Subsets[i].Faces[j].Indices[2], 4, 1, File);

				//Write UV coord A
				fwrite(&Subsets[i].Faces[j].UVCoords[0].u, 4, 1, File);
				fwrite(&Subsets[i].Faces[j].UVCoords[0].v, 4, 1, File);

				//Write UV coord B
				fwrite(&Subsets[i].Faces[j].UVCoords[1].u, 4, 1, File);
				fwrite(&Subsets[i].Faces[j].UVCoords[1].v, 4, 1, File);

				//Write UV coord C
				fwrite(&Subsets[i].Faces[j].UVc.u, 4, 1, File);
				fwrite(&Subsets[i].Faces[j].UVc.v, 4, 1, File);
			}

			//////////////////////////////////////////////////////////////////
			//OBJ_MAT_NAME
			SubSection_ID = OBJ_MAT_NAME;

			SubSection_Length = (int)Subsets[i].MaterialName.size() + 6;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//Write object name
			fwrite(Subsets[i].MaterialName.c_str(), 1, Subsets[i].MaterialName.size(), File);
		}


		//////////////////////////////////////////////////////////////////
		//BONES

		//For all bones
		for (std::map<int, SYM_BONE>::iterator i = Bones.begin(); i != Bones.end(); i++)
		{
			MainSection_ID = BONE;

			//Append '\0' to name
			i->second.Name += '\0';

			//Calculate OBJECT section length
			///       Header 		(6)
			/// + 6 + ID (int)	    (4)
			/// + 6 + ParentID (int)(4)
			/// + 6 + Name (char)	(Chars in name)
			/// + 6 + Pos			(3x float)
			/// + 6 + Rot			(4x float)

			MainSection_Length =      36					//All headers
                                    + 8						//All single integers
                                    + i->second.Name.size()	//Name
                                    + (7 * sizeof(float));  //Pos/rot

			//Write header
			fwrite(&MainSection_ID, 2, 1, File);

			//Write length
			fwrite(&MainSection_Length, 4, 1, File);

			//////////////////////////////////////////////////////////////////
			//BONE_ID
			SubSection_ID = BONE_ID;
			SubSection_Length = 10;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//Write numfaces
			fwrite(&i->second.ID, 4, 1, File);

			//////////////////////////////////////////////////////////////////
			//BONE_PARENT
			SubSection_ID = BONE_PARENT;
			SubSection_Length = 10;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//Write numfaces
			fwrite(&i->second.ParentID, 4, 1, File);

			//////////////////////////////////////////////////////////////////
			//BONE_NAME
			SubSection_ID = BONE_NAME;

			SubSection_Length = i->second.Name.size() + 6;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//Write object name
			fwrite(i->second.Name.c_str(), 1, i->second.Name.size(), File);

			//////////////////////////////////////////////////////////////////
			//BONE_POS
			SubSection_ID = BONE_POS;

			//Calculate section size
			SubSection_Length = (3 * sizeof(float)) + 6;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//Write X
			fwrite(&i->second.Position.x, 4, 1, File);

			//Write Y
			fwrite(&i->second.Position.y, 4, 1, File);

			//Write Z
			fwrite(&i->second.Position.z, 4, 1, File);

			//////////////////////////////////////////////////////////////////
			//BONE_ROT
			SubSection_ID = BONE_ROT;

			//Calculate section size
			SubSection_Length = (4 * sizeof(float)) + 6;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//Write X
			fwrite(&i->second.Rotation.x, 4, 1, File);

			//Write Y
			fwrite(&i->second.Rotation.y, 4, 1, File);

			//Write Z
			fwrite(&i->second.Rotation.z, 4, 1, File);

			//Write W
			fwrite(&i->second.Rotation.w, 4, 1, File);
		}

		int Filesize = ftell(File);

		//Close the file
		fclose(File);

		return Filesize;
	}

} //Namespace
