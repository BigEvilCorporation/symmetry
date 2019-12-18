//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//*                    SYMMETRY ENGINE                      *
//*                                                         *
//*  Copyright (C) 2007 Matt Phillips, Kirt Rainford.       *
//*  mattphillips@mail.com   kirtrainford@hotmail.com       *
//*                                                         *
//*  SYM_3DS.cpp                                            *
//*                                                         *
//*  3DS model file loading, post processing and drawing    *
//* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#include <math.h>

#include "SYM_3DS.h"

bool SYM_3DS::Load3DSFile(string Filename)
{
	int i;
	
	//The file itself
	FILE *l_file = 0;

	//Filesize, and current position variables
	long int Filesize;
	long int FilePos = 0;

	//Variables for chunk ID's and lengths
	int l_chunk_id = 0;
	long int l_chunk_length = 0;
	
	int l_subchunk_id = 0;
	long int l_subchunk_length = 0;
	
	int l_childchunk_id = 0;
	long int l_childchunk_length = 0;

	//Used for searching any sub or child chunks
	long int SubChunkPos = 0;
	long int SubChunkFoundAt = 0;
	
	long int ChildChunkPos = 0;
	long int ChildChunkFoundAt = 0;

	//Variables used for reading in data
	char l_char;
	short int l_qty;
	int l_face_flags;

	//Next colour or map type
	int NextColourType;
	//int NextMapType;

	//First model/material is 0, correct initial count
	NumModels = 0;
	NumMaterials = 0;
	NumKeyframes = 0;
	int NumObjInfo = 0;

	//Open file, read-only binary mode
	l_file = fopen (Filename.c_str(), "rb");
	if(!l_file) return false;
	
	//Determine filesize (Seek to end, get current position, rewind to start)
	fseek(l_file, 0, SEEK_END);
	Filesize = ftell(l_file);
	rewind(l_file);

	//Loop through file until end
	while (ftell(l_file) < Filesize)
	{
		//Read the chunk header
		fread (&l_chunk_id, 2, 1, l_file);

		//Read the length of the chunk
		fread (&l_chunk_length, 4, 1, l_file);
		
		//Determine chunk type
		switch (l_chunk_id)
		{
			case MAIN3DS:
				//This chunk is the MAIN chunk (whole file). If this isn't found, this isn't a 3DS file :(
				//TODO: Implement failsafe if this chunk isn't found
				//cout << "FOUND Chunk: MAIN3DS" << "\t\t" << "ID: 0x" << hex << l_chunk_id << dec << "\t" << "Length: " << l_chunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";
				break;
			
			case EDIT3DS:
				//This chunk is the 3D EDITOR chunk :: Contains viewport data, etc, for 3D Studio Max
				//cout << "FOUND Chunk: EDIT3DS" << "\t\t" << "ID: 0x" << hex << l_chunk_id << dec << "\t" << "Length: " << l_chunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";
				break;

			case MASTER_SCALE:
				//This chunk is the 3D MASTER_SCALE chunk
				//cout << "FOUND Chunk: MASTER_SCALE" << "\t" << "ID: 0x" << hex << l_chunk_id << dec << "\t" << "Length: " << l_chunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";

				//Go to next chunk
				fseek(l_file, l_chunk_length - 6, SEEK_CUR);
				FilePos = ftell(l_file);
				break;

			case MATERIAL3DS:
				//This chunk is a MATERIAL chunk :: Contains many subchunks; material name, colour, texture filename, etc.
				//cout << "FOUND Chunk: MATERIAL" << "\t\t" << "ID: 0x" << hex << l_chunk_id << dec << "\t" << "Length: " << l_chunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";
				
				//Store current file position, so we can return later
				SubChunkFoundAt = ftell(l_file);
				
				//Store current file position as integer, used for loop
				SubChunkPos = ftell(l_file);
				
				//Zero the subchunk ID and length, in case they were used previously
				l_subchunk_id = 0;
				l_subchunk_length = 0;

				//Loop from Material chunk Start to Material chunk End (Subchunk length, + the position it was found)
				while (ftell(l_file) < (l_chunk_length + SubChunkFoundAt - 6))
				{
					 //Read the subchunk ID
					fread (&l_subchunk_id, 2, 1, l_file);
					
					//Read the length of the subchunk
					fread (&l_subchunk_length, 4, 1, l_file);
					
					//Determine SubChunk type using its ID
					switch (l_subchunk_id)
					{
						case MAT_NAME3DS:
							//This subchunk is the MATERIAL'S NAME
							//cout << "-->SubChunk: MAT_NAME" << "\t\t" << "ID: 0x" << hex << l_subchunk_id << dec << "\t" << "Length: " << l_subchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";

							//Initialise tiling floats
							Material[NumMaterials].TileV = 1;
							Material[NumMaterials].TileU = 1;
							
							//Read in Material Name
							i = 0;
							do
							{
								fread (&l_char, 1, 1, l_file);
								Material[NumMaterials].Name += l_char;
								i++;
							}while(l_char != '\0');
							
							break;
							
						case MAT_AMBIENT:
							//This subchunk defines that the material has an AMBIENT colour coming up
							//cout << "-->SubChunk: M_AMBIENT" << "\t\t" << "ID: 0x" << hex << l_subchunk_id << dec << "\t" << "Length: " << l_subchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";
							
							//NEXT COLOR_TRU VALUE IS AMBIENT
							NextColourType = COLOUR_AMBIENT;
							break;
							
						case MAT_DIFFUSE:
							//This subchunk defines that the material has a DIFFUSE colour coming up
							//cout << "-->SubChunk: M_DIFFUSE" << "\t\t" << "ID: 0x" << hex << l_subchunk_id << dec << "\t" << "Length: " << l_subchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";
							
							//NEXT COLOR_TRU VALUE IS DIFFUSE
							NextColourType = COLOUR_DIFFUSE;
							break;
							
						case MAT_SPECULAR:
							//This subchunk defines that the material has a SPECULAR colour coming up
							//cout << "-->SubChunk: M_SPECULAR" << "\t\t" << "ID: 0x" << hex << l_subchunk_id << dec << "\t" << "Length: " << l_subchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";
							
							//NEXT COLOR_TRU VALUE IS SPECULAR
							NextColourType = COLOUR_SPECULAR;
							break;
							
						case COLOR_TRU:
							//This subchunks defines the RBG COLOUR for whichever colour type was found last
							//cout << "-->SubChunk: M_COLOUR" << "\t\t" << "ID: 0x" << hex << l_subchunk_id << dec << "\t" << "Length: " << l_subchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";
							
							//Determine last colour type found
							switch (NextColourType)
							{
								case COLOUR_AMBIENT:
									//Read in 3-byte colour
									fread (&Material[NumMaterials].Ambient.r, 1, 1, l_file);
									fread (&Material[NumMaterials].Ambient.g, 1, 1, l_file);
									fread (&Material[NumMaterials].Ambient.b, 1, 1, l_file);

									//Convert to GLint colour
									Material[NumMaterials].Ambient.rgb[0] = Material[NumMaterials].Ambient.r;

									Material[NumMaterials].Ambient.rgb[1] = Material[NumMaterials].Ambient.g;

									Material[NumMaterials].Ambient.rgb[2] = Material[NumMaterials].Ambient.b;
									break;
									
								case COLOUR_DIFFUSE:
									fread (&Material[NumMaterials].Diffuse.r, 1, 1, l_file);
									fread (&Material[NumMaterials].Diffuse.g, 1, 1, l_file);
									fread (&Material[NumMaterials].Diffuse.b, 1, 1, l_file);

									//Convert to GLint colour
									Material[NumMaterials].Diffuse.rgb[0] = Material[NumMaterials].Diffuse.r;

									Material[NumMaterials].Diffuse.rgb[1] = Material[NumMaterials].Diffuse.g;

									Material[NumMaterials].Diffuse.rgb[2] = Material[NumMaterials].Diffuse.b;
									break;
									
								case COLOUR_SPECULAR:
									fread (&Material[NumMaterials].Specular.r, 1, 1, l_file);
									fread (&Material[NumMaterials].Specular.g, 1, 1, l_file);
									fread (&Material[NumMaterials].Specular.b, 1, 1, l_file);

									//Convert to GLint colour
									Material[NumMaterials].Specular.rgb[0] = Material[NumMaterials].Specular.r;

									Material[NumMaterials].Specular.rgb[1] = Material[NumMaterials].Specular.g;

									Material[NumMaterials].Specular.rgb[2] = Material[NumMaterials].Specular.b;
									break;
									
									default: break;
							}
							
							break;
							
							
						case SHINY_PERC:
							//This subchunk contains a PERCENTAGE OF SHININESS in an integer
							//cout << "-->SubChunk: SHINY_PERC" << "\t\t" << "ID: 0x" << hex << l_subchunk_id << dec << "\t" << "Length: " << l_subchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";
							
							//TODO: Implement this later
							//Skip subchunk and read next one
							fseek(l_file, l_subchunk_length - 6, SEEK_CUR);

							//fread (&Material[NumMaterials].Shininess, sizeof(byte), 1, l_file);
							break;
							
						case SHINY_STR_PERC:
							//This subchunk contains a PERCENTAGE OF STRENGTH OF SHININESS in an integer
							//cout << "-->SubChunk: SHINY_STR_PERC" << "\t" << "ID: 0x" << hex << l_subchunk_id << dec << "\t" << "Length: " << l_subchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";

							//TODO: Implement this later
							//Skip subchunk and read next one
							fseek(l_file, l_subchunk_length - 6, SEEK_CUR);

							//fread (&Material[NumMaterials].Shininess, sizeof(int), 1, l_file);
							break;
							
						case TRANS_PERC:
							//This subchunk contains a PERCENTAGE OF TRANSPARENCY in an integer
							//cout << "-->SubChunk: TRANS_PERC" << "\t\t" << "ID: 0x" << hex << l_subchunk_id << dec << "\t" << "Length: " << l_subchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";
							
							//TODO: Implement this later
							//Skip subchunk and read next one
							fseek(l_file, l_subchunk_length - 6, SEEK_CUR);
							//fread (&Material[NumMaterials].Transparency, 4, 1, l_file);
							break;
							
						case TRANS_FOFF_PERC:
							//This subchunk contains a PERCENTAGE OF FALLOFF TRANSPARENCY in an integer
							//cout << "-->SubChunk: TRANS_FOFF_PERC" << "\t" << "ID: 0x" << hex << l_subchunk_id << dec << "\t" << "Length: " << l_subchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";
							
							//TODO: Implement this later
							//Skip subchunk and read next one
							fseek(l_file, l_subchunk_length - 6, SEEK_CUR);
							break;
							
						case REF_BLUR_PERC:
							//This subchunk contains a PERCENTAGE OF TEXTURE BLUR in an integer
							//cout << "-->SubChunk: REF_BLUR_PERC" << "\t" << "ID: 0x" << hex << l_subchunk_id << dec << "\t" << "Length: " << l_subchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";
							
							//TODO: Implement this later
							//Skip subchunk and read next one
							fseek(l_file, l_subchunk_length - 6, SEEK_CUR);
							break;
							
						case PERC_INT:
							//This subchunk contains a percentage of some sort ;)
							//cout << "-->SubChunk: PERC_INT" << "\t\t" << "ID: 0x" << hex << l_subchunk_id << dec << "\t" << "Length: " << l_subchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";
							
							//TODO: Implement this later
							//Skip subchunk and read next one
							fseek(l_file, l_subchunk_length - 6, SEEK_CUR);
							break;
							
						case RENDER_TYPE:
							//This subchunk contains the RENDER TYPE
							//cout << "-->SubChunk: RENDER_TYPE" << "\t" << "ID: 0x" << hex << l_subchunk_id << dec << "\t" << "Length: " << l_subchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";
							
							//TODO: Implement this later
							//Skip subchunk and read next one
							fseek(l_file, l_subchunk_length - 6, SEEK_CUR);
							break;
							
							
						case MAT_TEXMAP:
							//This subchunk is the TEXTURE MAP chunk :: Contains child chunks with texture info
							//cout << "-->SubChunk: MAT_TEXMAP" << "\t\t" << "ID: 0x" << hex << l_subchunk_id << dec << "\t" << "Length: " << l_subchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";
							
							//Store current file position, so we can return later
							ChildChunkFoundAt = ftell(l_file);
				
							//Store current file position as integer, used for loop
							ChildChunkPos = ftell(l_file);
				
							//Zero the childchunk ID and length, in case they were used previously
							l_childchunk_id = 0;
							l_childchunk_length = 0;

							//Loop from child chunk Start to child chunk End (childchunk length, + the position it was found)
							while (ftell(l_file) < (l_subchunk_length + ChildChunkFoundAt - 6))
							{
					 			//Read the childchunk ID
								fread (&l_childchunk_id, 2, 1, l_file);
					
								//Read the length of the childchunk
								fread (&l_childchunk_length, 4, 1, l_file);
					
								//Determine ChildChunk type using its ID
								switch (l_childchunk_id)
								{
							
									case MAT_MAPNAME:
										//This subchunk is the TEXTURE MAP chunk :: Contains child chunks with texture info
										//cout << "----->Child: MAT_MAPNAME" << "\t" << "ID: 0x" << hex << l_childchunk_id << dec << "\t" << "Length: " << l_childchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";

										//Material has texture
										Material[NumMaterials].HasTexture = true;


										//Prefix filename with path
										//Material[NumMaterials].TextureFile = "data/";
											
										//Read in Texture Map Filename
										i = 0;
										do
										{
										fread (&l_char, 1, 1, l_file);
										Material[NumMaterials].TextureFile += l_char;
										i++;
										}while(l_char != '\0');
										
										break;

									case MAT_VTILE:
										//This subchunk is the TEXTURE MAP chunk :: Contains child chunks with texture info
										//cout << "----->Child: MAT_VTILE" << "\t" << "ID: 0x" << hex << l_childchunk_id << dec << "\t" << "Length: " << l_childchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";

										fread (&Material[NumMaterials].TileV, sizeof(float), 1, l_file);

										Material[NumMaterials].TileV += 1.0;

										break;

									case MAT_UTILE:
										//This subchunk is the TEXTURE MAP chunk :: Contains child chunks with texture info
										//cout << "----->Child: MAT_UTILE" << "\t" << "ID: 0x" << hex << l_childchunk_id << dec << "\t" << "Length: " << l_childchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";

										fread (&Material[NumMaterials].TileU, sizeof(float), 1, l_file);

										Material[NumMaterials].TileU += 1.0;

										break;
									
									default:
										//Unknown chunk found. Ignore it, skip to it's end and read the next chunk
										//cout << "----->Child: *UNKNOWN*" << "\t\t" << "ID: 0x" << hex << l_childchunk_id << dec << "\t" << "Length: " << l_childchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";
							
										//Advance to next childchunk (Current Position, + SubChunk length, - the header (6))
										fseek(l_file, l_childchunk_length - 6, SEEK_CUR);
										break;
								}
							}
							
							//Move to end of childchunk before returning
							//(where we found the childchunk, + subchunk length, - the header (6))
							fseek(l_file, ChildChunkFoundAt + l_subchunk_length, SEEK_SET);
							
							break;
							
						default:
							//Unknown chunk found. Ignore it, skip to it's end and read the next chunk
							//cout << "-->SubChunk: *UNKNOWN*" << "\t\t" << "ID: 0x" << hex << l_subchunk_id << dec << "\t" << "Length: " << l_subchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";
							
							//Advance to next subchunk (Current Position, + SubChunk length, - the header (6))
							fseek(l_file, l_subchunk_length - 6, SEEK_CUR);
							break;
					}
				}

				//Increase number of materials found
				NumMaterials++;

				//Move to end of material subchunk before returning
				//(where we found the subchunk, + chunk length, - the header (6))
				fseek(l_file, SubChunkFoundAt + l_chunk_length - 6, SEEK_SET);
				
				break;

			case OBJECT3DS:
				//This chunk is an OBJECT chunk :: Contains the name of a mesh
				//cout << "FOUND Chunk: OBJECT" << "\t\t" << "ID: 0x" << hex << l_chunk_id << dec << "\t" << "Length: " << l_chunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";

				//Store current file position, so we can return later
				SubChunkFoundAt = ftell(l_file);
				
				//Store current file position as integer, used for loophut.3DS
				SubChunkPos = ftell(l_file);
				
				//Zero the subchunk ID and length, in case they were used previously
				l_subchunk_id = 0;
				l_subchunk_length = 0;

				//Object name is within this chunk, read BEFORE looking for subchunks
				//Read in object name, character at a time, until a terminating '\0' is found
				i = 0;
				do
				{
					fread (&l_char, 1, 1, l_file);
					Model[NumModels].Name += l_char;
					i++;
				}while(l_char != '\0');

				//Loop from Material chunk Start to Material chunk End (Subchunk length, + the position it was found)
				while (ftell(l_file) < (l_chunk_length + SubChunkFoundAt - 6))
				{
					 //Read the subchunk ID
					fread (&l_subchunk_id, 2, 1, l_file);
					
					//Read the length of the subchunk
					fread (&l_subchunk_length, 4, 1, l_file);
					
					//Determine SubChunk type using its ID
					switch (l_subchunk_id)
					{
						case TRIG_MESH:
							//This chunk is an EMPTY, ROOT PARENT NODE chunk :: Defines the top level of the node hierarchy
							//cout << "-->SubChunk: TRIG_MESH" << "\t\t" << "ID: 0x" << hex << l_subchunk_id << dec << "\t" << "Length: " << l_subchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";
							break;
							
						case VERT_LIST:
							//Vertices list chunk
							//cout << "-->SubChunk: VERT_LIST" << "\t\t" << "ID: 0x" << hex << l_subchunk_id << dec << "\t" << "Length: " << l_subchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";
							
							fread (&l_qty, 2, 1, l_file);
							Model[NumModels].NumVertices = l_qty;
			
							for (i = 0; i < l_qty; i++)
							{
								fread (&Model[NumModels].Vertex[i].x, sizeof(float), 1, l_file);
								fread (&Model[NumModels].Vertex[i].y, sizeof(float), 1, l_file);
								fread (&Model[NumModels].Vertex[i].z, sizeof(float), 1, l_file);
							}
							break;
							
						case FACE_DESC:
							//Faces description chunk
							//cout << "-->SubChunk: FACE_DESC" << "\t\t" << "ID: 0x" << hex << l_subchunk_id << dec << "\t" << "Length: " << l_subchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";
							
							fread (&l_qty, 2, 1, l_file);
							Model[NumModels].NumPolygons = l_qty;
			
							for (i = 0; i < l_qty; i++)
							{
								fread (&Model[NumModels].Face[i].a, 2, 1, l_file);
								fread (&Model[NumModels].Face[i].b, 2, 1, l_file);
								fread (&Model[NumModels].Face[i].c, 2, 1, l_file);
								fread (&l_face_flags, 2, 1, l_file);
							}
							break;
							
						case TEX_VERTS:
							//Mapping coordinates list chunk
							//cout << "-->SubChunk: TEX_VERTS" << "\t\t" << "ID: 0x" << hex << l_subchunk_id << dec << "\t" << "Length: " << l_subchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";

							fread (&l_qty, 2, 1, l_file);

							for (i = 0; i < l_qty; i++)
							{
								fread (&Model[NumModels].UVCoord[i].u, sizeof (float), 1, l_file);
								fread (&Model[NumModels].UVCoord[i].v, sizeof (float), 1, l_file);
							}

							break;

						case FACE_MAT:
							//Mapping coordinates list chunk
							//cout << "-->SubChunk: FACE_MAT" << "\t\t" << "ID: 0x" << hex << l_subchunk_id << dec << "\t" << "Length: " << l_subchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";

							//Model has material
							Model[NumModels].HasMaterial = true;

							//Read in Material Name
							i = 0;
							do
							{
								fread (&l_char, 1, 1, l_file);
								Model[NumModels].MaterialName += l_char;
								i++;
							}while(l_char != '\0');

							//Read in number of indexes coming up
							fread (&Model[NumModels].NumMappedFaces, 2, 1, l_file);

							for (i = 0; i < Model[NumModels].NumMappedFaces; i++)
							{
								fread (&Model[NumModels].MaterialFaces[i], 2, 1, l_file);
							}
							
							break;
							

						default:
							//Unknown chunk found. Ignore it, skip to it's end and read the next chunk
							//cout << "-->SubChunk: *UNKNOWN*" << "\t\t" << "ID: 0x" << hex << l_subchunk_id << dec << "\t" << "Length: " << l_subchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";
							
							//Advance to next subchunk (Current Position, + SubChunk length, - the header (6))
							fseek(l_file, l_subchunk_length - 6, SEEK_CUR);
							break;
					}
				}

				//Increase number of models found
				NumModels++;

				//Move to end of material subchunk before returning
				//(where we found the subchunk, + chunk length, - the header (6))
				fseek(l_file, SubChunkFoundAt + l_chunk_length - 6, SEEK_SET);
				
				break;

			case KEYFRAME3DS:
				//This chunk is a MATERIAL chunk :: Contains many subchunks; material name, colour, texture filename, etc.
				//cout << "FOUND Chunk: KEYFRAME3DS" << "\t" << "ID: 0x" << hex << l_chunk_id << dec << "\t" << "Length: " << l_chunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";
				
				//Store current file position, so we can return later
				SubChunkFoundAt = ftell(l_file);
				
				//Store current file position as integer, used for loop
				SubChunkPos = ftell(l_file);
				
				//Zero the subchunk ID and length, in case they were used previously
				l_subchunk_id = 0;
				l_subchunk_length = 0;

				Keyframe[NumKeyframes].NumObjInfo = 0;

				//Loop from Material chunk Start to main chunk End (Subchunk length, + the position it was found)
				while (ftell(l_file) < (SubChunkFoundAt + l_chunk_length - 6))
				{
					 //Read the subchunk ID
					fread (&l_subchunk_id, 2, 1, l_file);
					
					//Read the length of the subchunk
					fread (&l_subchunk_length, 4, 1, l_file);
					
					//Determine SubChunk type using its ID
					switch (l_subchunk_id)
					{
						case FRAMES:
							//This subchunk contains the FRAMES
							//cout << "-->SubChunk: FRAMES" << "\t\t" << "ID: 0x" << hex << l_subchunk_id << dec << "\t" << "Length: " << l_subchunk_length << "\t" << "Pos: " << ftell(l_file);
							
							fread (&Keyframe[NumKeyframes].StartFrame, sizeof(unsigned int), 1, l_file);
							fread (&Keyframe[NumKeyframes].Endframe, sizeof(unsigned int), 1, l_file);

							//cout << " Value: Start(" << Keyframe[NumKeyframes].StartFrame << ") End(" << Keyframe[NumKeyframes].Endframe << ")\n";

							break;

						case MESH_INFO:
							//This subchunk is the MESH_INFO chunk :: Contains child chunks with keyframe info
							//cout << "-->SubChunk: MESH_INFO" << "\t\t" << "ID: 0x" << hex << l_subchunk_id << dec << "\t" << "Length: " << l_subchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";
							
							//Store current file position, so we can return later
							ChildChunkFoundAt = ftell(l_file);
				
							//Store current file position as integer, used for loop
							ChildChunkPos = ftell(l_file);
				
							//Zero the childchunk ID and length, in case they were used previously
							l_childchunk_id = 0;
							l_childchunk_length = 0;

							//Loop from child chunk Start to child chunk End (childchunk length, + the position it was found)
							while (ftell(l_file) < (l_subchunk_length + ChildChunkFoundAt - 6))
							{
					 			//Read the childchunk ID
								fread (&l_childchunk_id, 2, 1, l_file);
					
								//Read the length of the childchunk
								fread (&l_childchunk_length, 4, 1, l_file);
					
								//Determine ChildChunk type using its ID
								switch (l_childchunk_id)
								{
									case OBJNAME:
										//This subchunk is the OBJNAME chunk
										//cout << "----->Child: OBJNAME" << "\t\t" << "ID: 0x" << hex << l_childchunk_id << dec << "\t" << "Length: " << l_childchunk_length << "\t" << "Pos: " << ftell(l_file);

										//Read in name
										i = 0;
										do
										{
										fread (&l_char, 1, 1, l_file);
										Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Name += l_char;
										i++;
										}while(l_char != '\0');

										fread (&Keyframe[NumKeyframes].Unknown_Data, sizeof(unsigned short) * 3, 1, l_file);

										//cout << " Value: " << Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Name << "\n";
										
										break;

									case PIVOT_PT:
										//This subchunk is the PIVOT_PT chunk
										//cout << "----->Child: PIVOT_PT" << "\t\t" << "ID: 0x" << hex << l_childchunk_id << dec << "\t" << "Length: " << l_childchunk_length << "\t" << "Pos: " << ftell(l_file);

										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].PivotX, sizeof(float), 1, l_file);
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].PivotY, sizeof(float), 1, l_file);
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].PivotZ, sizeof(float), 1, l_file);

										//cout << " Value: X(" << Keyframe[NumKeyframes].ObjInfo[NumObjInfo].PivotX << ") Y(" << Keyframe[NumKeyframes].ObjInfo[NumObjInfo].PivotY << ") Z(" << Keyframe[NumKeyframes].ObjInfo[NumObjInfo].PivotZ << ")\n";

										break;

									case TRACK_POS:
										//This subchunk is the TRACK_POS chunk
										//cout << "----->Child: TRACK_POS" << "\t\t" << "ID: 0x" << hex << l_childchunk_id << dec << "\t" << "Length: " << l_childchunk_length << "\t" << "Pos: " << ftell(l_file);

										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Position.Flags, sizeof(unsigned short), 1, l_file);
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Position.Unknown_Data, sizeof(unsigned short) * 4, 1, l_file);
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Position.NumKeys, sizeof(unsigned short), 1, l_file);
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Position.Unknown_Data2, sizeof(unsigned short), 1, l_file);

										for (int i = 0; i < Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Position.NumKeys; i++)
										{
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Position.Keys[i].FrameNum, sizeof(unsigned short), 1, l_file);
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Position.Keys[i].Unknown_Data, sizeof(long int), 1, l_file);
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Position.Keys[i].PosX, sizeof(float), 1, l_file);
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Position.Keys[i].PosY, sizeof(float), 1, l_file);
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Position.Keys[i].PosZ, sizeof(float), 1, l_file);
										}

										//cout << " NumKeys: " << Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Position.NumKeys << "\n";

										break;

									case TRACK_ROT:
										//This subchunk is the TRACK_ROT chunk
										//cout << "----->Child: TRACK_ROT" << "\t\t" << "ID: 0x" << hex << l_childchunk_id << dec << "\t" << "Length: " << l_childchunk_length << "\t" << "Pos: " << ftell(l_file);

										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Rotation.Flags, sizeof(unsigned short), 1, l_file);
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Rotation.Unknown_Data, sizeof(unsigned short) * 4, 1, l_file);
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Rotation.NumKeys, sizeof(unsigned short), 1, l_file);
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Rotation.Unknown_Data2, sizeof(unsigned short), 1, l_file);

										for (int i = 0; i < Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Rotation.NumKeys; i++)
										{
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Rotation.Keys[i].FrameNum, sizeof(unsigned short), 1, l_file);
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Rotation.Keys[i].Unknown_Data, sizeof(long int), 1, l_file);
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Rotation.Keys[i].RotationRadian, sizeof(float), 1, l_file);
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Rotation.Keys[i].AxisX, sizeof(float), 1, l_file);
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Rotation.Keys[i].AxisY, sizeof(float), 1, l_file);
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Rotation.Keys[i].AxisZ, sizeof(float), 1, l_file);
										}

										//cout << " NumKeys: " << Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Rotation.NumKeys << "\n";

										break;

									case TRACK_SCL:
										//This subchunk is the TRACK_SCL chunk
										//cout << "----->Child: TRACK_SCL" << "\t\t" << "ID: 0x" << hex << l_childchunk_id << dec << "\t" << "Length: " << l_childchunk_length << "\t" << "Pos: " << ftell(l_file);

										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Scale.Flags, sizeof(unsigned short), 1, l_file);
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Scale.Unknown_Data, sizeof(unsigned short) * 4, 1, l_file);
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Scale.NumKeys, sizeof(unsigned short), 1, l_file);
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Scale.Unknown_Data2, sizeof(unsigned short), 1, l_file);

										for (int i = 0; i < Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Position.NumKeys; i++)
										{
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Scale.Keys[i].FrameNum, sizeof(unsigned short), 1, l_file);
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Scale.Keys[i].Unknown_Data, sizeof(long int), 1, l_file);
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Scale.Keys[i].ScaleX, sizeof(float), 1, l_file);
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Scale.Keys[i].ScaleY, sizeof(float), 1, l_file);
										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Scale.Keys[i].ScaleZ, sizeof(float), 1, l_file);
										}

										//cout << " NumKeys: " << Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Track_Scale.NumKeys << "\n";

										break;

									case NODE_ID:
										//This subchunk is the NODE_ID chunk
										//cout << "----->Child: NODE_ID" << "\t\t" << "ID: 0x" << hex << l_childchunk_id << dec << "\t" << "Length: " << l_childchunk_length << "\t" << "Pos: " << ftell(l_file);

										fread (&Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Node_ID, sizeof(unsigned short), 1, l_file);

										//cout << " Value: " << Keyframe[NumKeyframes].ObjInfo[NumObjInfo].Node_ID << "\n";

										break;

									default:
										//Unknown chunk found. Ignore it, skip to it's end and read the next chunk
										//cout << "----->Child: *UNKNOWN*" << "\t\t" << "ID: 0x" << hex << l_childchunk_id << dec << "\t" << "Length: " << l_childchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";
							
										//Advance to next childchunk (Current Position, + SubChunk length, - the header (6))
										fseek(l_file, l_childchunk_length - 6, SEEK_CUR);
										break;
								}
							}

							Keyframe[NumKeyframes].NumObjInfo++;
							
							//Move to end of childchunk before returning
							//(where we found the childchunk, + subchunk length, + the header (6))
							fseek(l_file, ChildChunkFoundAt + l_subchunk_length, SEEK_SET);
							
							break;

						default:
							//Unknown chunk found. Ignore it, skip to it's end and read the next chunk
							//cout << "-->SubChunk: *UNKNOWN*" << "\t\t" << "ID: 0x" << hex << l_subchunk_id << dec << "\t" << "Length: " << l_subchunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";
							
							//Advance to next subchunk (Current Position, + SubChunk length, - the header (6))
							fseek(l_file, l_subchunk_length - 6, SEEK_CUR);
							break;
					}
				}

				//Increase number of keyframes found
				NumKeyframes++;

				//Move to end of material subchunk before returning
				//(where we found the subchunk, + chunk length, - the header (6))
				fseek(l_file, SubChunkFoundAt + l_chunk_length - 6, SEEK_SET);
				
				break;
				
			default:
				//cout << "FOUND Chunk: *UNKNOWN*" << "\t\t" << "ID: 0x" << hex << l_chunk_id << dec << "\t" << "Length: " << l_chunk_length << "\t" << "Pos: " << ftell(l_file) << "\n";
				
				//Go to next chunk
				fseek(l_file, l_chunk_length - 6, SEEK_CUR);
				FilePos = ftell(l_file);
				break;
		}
	}
	
	//End of file, close stream
	fclose (l_file);


	//For all models
	for (int i = 0; i < NumModels; i++)
	{
		//Search through materials
		for (int j = 0; j < NumMaterials; j++)
		{
			//Check if names match
			if (Model[i].MaterialName == Material[j].Name)
			{
				//Assign model the material's number
				Model[i].MaterialNumber = j;

				//No point continuing the search
				break;
			}
		}
	}

		
	return true;
}
