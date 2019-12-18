#include <sstream>

#include "SYM_Mesh.h"
//#include "SYM_Shader.h"
#include "SYM_Engine.h"

namespace sym
{
	CMeshSubset::CMeshSubset()
	{
		Material = 0;
		VertexArray = 0;
		FaceIndexArray = 0;
		TexCoordArray = 0;
		VertexNormalArray = 0;
		Bones = 0;
	}

	CMeshSubset::~CMeshSubset()
	{
	    //if(VertexArray) delete VertexArray;
	    //if(FaceIndexArray) delete FaceIndexArray;
	    //if(TexCoordArray) delete TexCoordArray;
	    //if(VertexNormalArray) delete VertexNormalArray;
	}

	void CMeshSubset::Render()
	{
		if(Bones)
		{
			CGparameter CgParam = cgGetNamedEffectParameter(AnimShader.GetEffect(), "BoneMatrices");

			float *MatrixArray = new float[16 * (int)Bones->size()];
			int CurrentElement = 0;

			for(int i = 0; i < (int)Bones->size(); i++)
			{
				for(int j = 0; j < 16; j++)
				{
					CMatrix4 BoneInitMatrix = (*Bones)[i].OriginalMatrix;
					CMatrix4 BoneFinalMatrix = (*Bones)[i].GetMatrix();

					CMatrix4 Matrix = BoneInitMatrix.GetInverse() * BoneFinalMatrix;

					MatrixArray[CurrentElement++] = Matrix.GetAsFloatArray()[j];
				}
			}
			
			cgGLSetMatrixParameterArrayfc(CgParam, 0, (int)Bones->size(), MatrixArray);

			//Get the first render pass
			CGpass Pass = AnimShader.GetFirstPass();

			//Set current pass
			AnimShader.SetPass(Pass);

			//Draw geometry
			Draw();

			//Reset current pass
			AnimShader.ResetPass(Pass);
		}
		else
		{
			//If a material has been loaded
			if(Material)
			{
				//Apply the material
				Material->Apply();

				//If a shader has been bound
				if(Material->Shader)
				{
					//Get the first render pass
					CGpass Pass = Material->Shader->GetFirstPass();

					//Loop until all passes have finished
					while(Pass)
					{
						//Set current pass
						Material->Shader->SetPass(Pass);

						//Draw geometry
						Draw();

						//Reset current pass
						Material->Shader->ResetPass(Pass);

						//Get the next pass
						Pass = Material->Shader->GetNextPass(Pass);
					}
				}
				else
				{
					//Draw without shader
					Draw();
				}
			}
			else
			{
				//Enable colour material, turn off textures
				glEnable(GL_COLOR_MATERIAL);
				glDisable(GL_TEXTURE_2D);

				//Set colour to white
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

				//Draw without material
				Draw();
			}
		}

		//Set colour to white
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		//Turn off materials and textures
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
	}

	void CMeshSubset::RenderWireframe(CColour Colour)
	{
	    glDisable(GL_LIGHTING);
        glDisable(GL_COLOR_MATERIAL);

	    glColor3ub(Colour.r, Colour.g, Colour.b);

	    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        Draw();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glColor3f(1.0f, 1.0f, 1.0f);
	}

	bool CMeshSubset::SetMaterial(std::string Filename)
	{
		Material = Engine->ResourceManager.Materials.Get(Filename);

		if (!Material) return false;

		return true;
	}

	bool CMeshSubset::SetMaterial(CMaterial *Mat)
	{
		if (!Mat) return false;

		Material = Mat;

		return true;
	}

	void CMeshSubset::Draw()
	{
		//Set offset matrix
		glPushMatrix();
		glMultMatrixf(Matrix.GetAsFloatArray());

		//Enable vertex arrays
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);

		//Pointers to arrays
		glVertexPointer(3, GL_FLOAT, 0, VertexArray);
		glNormalPointer(GL_FLOAT, 0, VertexNormalArray);

		//Texture UV coords use TEXTURE0
		if(glClientActiveTextureARB) glClientActiveTextureARB(GL_TEXTURE0_ARB);
		glTexCoordPointer(2, GL_FLOAT, 0, TexCoordArray);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		//Bone IDs use TEXTURE1
		if(glClientActiveTextureARB)
		{
			glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glTexCoordPointer(4, GL_FLOAT, 0, BoneIDArray);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

			//Vertex weights use TEXTURE2
			glClientActiveTextureARB(GL_TEXTURE2_ARB);
			glTexCoordPointer(4, GL_FLOAT, 0, VertexWeightArray);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		}

		//Draw
		glDrawArrays(GL_TRIANGLES, 0, (int)Faces.size() * 3);

		//Disable vertex arrays
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);

		if(glClientActiveTextureARB) glClientActiveTextureARB(GL_TEXTURE2_ARB);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_2D);

		if(glClientActiveTextureARB)
		{
			glClientActiveTextureARB(GL_TEXTURE1_ARB);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisable(GL_TEXTURE_2D);

			glClientActiveTextureARB(GL_TEXTURE0_ARB);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisable(GL_TEXTURE_2D);
		}

		glPopMatrix();
	}

	void CMeshSubset::Process()
	{
		//Calculate vertex and face normals
		CalculateNormals(Vertices, Faces);

		//Initialise arrays
		VertexArray = new GLfloat[Faces.size() * 9];
		TexCoordArray = new GLfloat[Faces.size() * 6];
		VertexNormalArray = new GLfloat[Faces.size() * 9];
		BoneIDArray = new GLfloat[(Faces.size() * 3) * 4];
		VertexWeightArray = new GLfloat[(Faces.size() * 3) * 4];

		//Keep track of indices
		int VertIndex = 0;
		int UVIndex = 0;
		int NormalIndex = 0;
		int BoneIndex = 0;
		int WeightIndex = 0;

		//For all faces
		for(int f = 0; f < (int)Faces.size(); f++)
		{
			//For each vertex
			for(int i = 0; i < 3; i++)
			{
				//Copy face indices to index array
				VertexArray[VertIndex++] = Vertices[Faces[f].Indices[i]].x;
				VertexArray[VertIndex++] = Vertices[Faces[f].Indices[i]].y;
				VertexArray[VertIndex++] = Vertices[Faces[f].Indices[i]].z;

				//Copy UV coords
				TexCoordArray[UVIndex++] = Faces[f].UVCoords[i].u;
				TexCoordArray[UVIndex++] = Faces[f].UVCoords[i].v;

				//Copy bone IDs and weights
				for(int b = 0; b < 4; b++)
				{
					BoneIDArray[BoneIndex++] = (float)Vertices[Faces[f].Indices[i]].BoneIDs[b];
					VertexWeightArray[WeightIndex++] = Vertices[Faces[f].Indices[i]].Weights[b];
				}

				//Copy normals
				if(Vertices[Faces[f].Indices[0]].Normal)
				{
					VertexNormalArray[NormalIndex++] = Vertices[Faces[f].Indices[i]].Normal->x;
					VertexNormalArray[NormalIndex++] = Vertices[Faces[f].Indices[i]].Normal->y;
					VertexNormalArray[NormalIndex++] = Vertices[Faces[f].Indices[i]].Normal->z;
				}
				else
				{
					VertexNormalArray[NormalIndex++] = 0.0f;
					VertexNormalArray[NormalIndex++] = 0.0f;
					VertexNormalArray[NormalIndex++] = 0.0f;
				}
			}
		}
	}

	CMesh::CMesh()
	{
		CurrentAnimation = 0;
		AnimationSpeed = 0.01f;
		CurrentKeyframeTime = 0.0f;
		AnimationRunning = false;
		AnimationLoop = true;
		DepthTest = true;

		//Font for rendering bone labels
		Font = new CFont;
	}

	CMesh::~CMesh()
	{
		//if (Font) delete Font;
	}

	void CMesh::SetDepthTest(bool Depth)
	{
		DepthTest = Depth;
	}

	void CMesh::NextFrame()
	{
		if(CurrentAnimation && AnimationRunning)
		{
			CurrentKeyframeTime += AnimationSpeed * (Engine->GetDeltaTime() * 0.1f);
			if(CurrentKeyframeTime > 1.0f)
			{
				if(AnimationLoop) CurrentKeyframeTime = 0.0f;
				else CurrentKeyframeTime = 1.0f;
			}
		}
	}
	void CMesh::PrevFrame()
	{
		if(CurrentAnimation && AnimationRunning)
		{
			CurrentKeyframeTime -= AnimationSpeed * (Engine->GetDeltaTime() * 0.1f);
			if(CurrentKeyframeTime < 0.0f)
			{
				if(AnimationLoop) CurrentKeyframeTime = 1.0f;
				else CurrentKeyframeTime = 0.0f;
			}
		}
	}

	void CMesh::StartAnimation()
	{
		AnimationRunning = true;
	}

	void CMesh::StopAnimation()
	{
		AnimationRunning = false;
	}

	void CMesh::ResetAnimation()
	{
		CurrentKeyframeTime = 0.0f;

		if(CurrentAnimation)
		{
			//Loop through bones
			for (int i = 0; i < (int)Bones.size(); i++)
			{
				Bones[i].CurrentKey = CurrentAnimation->GetKey(i, 0.0f);
			}

			Bones[0].Traverse();
		}
	}

	float CMesh::GetAnimationTime()
	{
		return CurrentKeyframeTime;
	}

	void CMesh::SetAnimationTime(float Time)
	{
		if(Time > 0.0f && Time < 1.0f) CurrentKeyframeTime = Time;
	}

	void CMesh::SetAnimationLooping(bool Looping)
	{
		AnimationLoop = Looping;
	}

	void CMesh::BindAnimation(CSkeletalAnimation *Anim)
	{
		if(Anim)
		{
			//Check bones match
			if(Anim->Keys.size() == Bones.size())
			{
				//Set animation pointer
				CurrentAnimation = Anim;
			}
			else
			{
				Engine->Logger.AddEntry("Error: Animation does not match bone structure");
				Engine->Console.PrintLine("> Error: Animation does not match bone structure");
			}
		}
	}

	void CMesh::SetAnimSpeed(float Speed)
	{
		AnimationSpeed = Speed;
	}

	void CMesh::Render()
	{
		if(CurrentAnimation && AnimationRunning && Bones.size() > 0)
		{
			//Loop through bones
			for (int i = 0; i < (int)Bones.size(); i++)
			{
				//Get current animation key
				CMatrix4 BoneMatrix = CurrentAnimation->GetKey(Bones[i].ID, CurrentKeyframeTime);

				//Set matrix
				Bones[i].CurrentKey = BoneMatrix;
			}

			Bones[0].Traverse();
		}

		//If depth testing is not enabled
		if(!DepthTest) glDisable(GL_DEPTH_TEST);

		//Loop through subsets
		for (int i = 0; i < (int)Subsets.size(); i++)
		{
			//Render
			Subsets[i].Render();
		}

		if(!DepthTest) glEnable(GL_DEPTH_TEST);
	}

	void CMesh::RenderWireframe(CColour Colour)
	{
		//Loop through subsets
		for (int i = 0; i < (int)Subsets.size(); i++)
		{
			//Render
			Subsets[i].RenderWireframe(Colour);
		}
	}

	void CMesh::RenderBones(CColour Colour)
	{
		glColor3ub(Colour.r, Colour.g, Colour.b);
		glLineWidth(4.0f);

		//Loop through bones
		for (int i = 0; i < (int)Bones.size(); i++)
		{
			if(Bones[i].ParentBone)
			{
				glColor3ub(Colour.r, Colour.g, Colour.b);
				glBegin(GL_LINES);
				CVector3 Position1 = Bones[i].GetMatrix().TransformVector(CVector3(0,0,0));
				CVector3 Position2 = Bones[i].ParentBone->GetMatrix().TransformVector(CVector3(0,0,0));
				glVertex3f(Position1.x, Position1.y, Position1.z);
				glVertex3f(Position2.x, Position2.y, Position2.z);
				glEnd();
			}

			std::stringstream Stream;
			Stream << "ID: " << i << " " << Bones[i].Name;
			if(Bones[i].ParentBone) Stream << " Parent:" << Bones[i].ID;

			glPushMatrix();
			CVector3 Position = Bones[i].GetMatrix().GetTranslation();
			glTranslatef(Position.x, Position.y, Position.z);
			Font->RenderText(Stream.str(), 0, 0);
			glPopMatrix();
		}

		glColor3ub(255, 255, 255);
		glLineWidth(1.0f);
	}

	void CMesh::TranslateVertices(CVector3 Translation)
	{
		for(int i = 0; i < Subsets.size(); i++)
		{
			for (int j = 0; j < Subsets[i].Vertices.size(); j++)
			{
				Subsets[i].Vertices[j] += Translation;
			}
		}
	}

	void CMesh::Scale(CVector3 Scl)
	{
	    //Refresh AABB
	    AABB = CBoundingBox();

		//Loop through subsets
		for (int i = 0; i < (int)Subsets.size(); i++)
		{
			//Loop through vertices
			for (int v = 0; v < (int)Subsets[i].Vertices.size(); v++)
			{
				//Scale vertex
				Subsets[i].Vertices[v] *= Scl;

				//Add to AABB
				AABB.AddVertex(Subsets[i].Vertices[v]);
			}

			//Process GL arrays
			Subsets[i].Process();
		}
	}

	bool CMesh::SetMaterial(std::string Filename)
	{
		//Set material on all subsets
		for (int i = 0; i < (int)Subsets.size(); i++)
		{
			if (!Subsets[i].SetMaterial(Filename)) return false;
		}

		return true;
	}

	bool CMesh::SetMaterial(CMaterial *Mat)
	{
		if(Mat)
		{
			//Set material on all subsets
			for (int i = 0; i < (int)Subsets.size(); i++)
			{
				if (!Subsets[i].SetMaterial(Mat)) return false;
			}
		}
		else return false;

		return true;
	}

	CMeshSubset *CMesh::NewSubset()
	{
		//Create new subset
		CMeshSubset *Subset = new CMeshSubset;

		//Add to vector
		Subsets.push_back(*Subset);

		//Return pointer
		return Subset;
	}

	void CBone::Traverse()
	{
		//If this is not the root bone
		if(ParentBone)
		{
			//Matrix = Current Key Matrix x Parent Matrix
			SetMatrix(CurrentKey * ParentBone->GetMatrix());
		}
		else
		{
			//Matrix = Original Matrix x Current Key Matrix
			SetMatrix(OriginalMatrix * CurrentKey);
		}

		//Traverse through children
		for(int i = 0; i < (int)ChildBones.size(); i++)
		{
			ChildBones[i]->Traverse();
		}
	}

	bool CMesh::Load(std::string Filename)
	{
		Font->LoadTTFFont("arial.ttf", 8);
		//Used for file seeking
		long int FileSize = 0;

		//Current MAIN section ID and length
		unsigned short MainSection_ID = 0;
		unsigned int MainSection_Length = 0;

		//Current SUB section ID and length
		unsigned short SubSection_ID = 0;
		unsigned int SubSection_Length = 0;
		unsigned int SubSection_Start = 0;

		//Current CHILD section ID and length
		unsigned short ChildSection_ID = 0;
		unsigned int ChildSection_Length = 0;
		unsigned int ChildSection_Start = 0;

		//Temporary char for reading in CHARs
		char TempChar = '\0';

		//Count number of chars
		int CharCount = 0;

		//Number of vertices/faces
		int NumVertices = 0;
		int NumFaces = 0;

		//Prefix with models path from engine config
		Filename = Engine->ModelsPath + Filename;

		//Open file for reading
		FILE *File = fopen(Filename.c_str(), "rb");

		//Check if file exists
		if (!File)
		{
		    Engine->Logger.AddEntry("Error: Could not load mesh " + Filename);
			Engine->Console.PrintLine("> Error: Could not load mesh " + Filename);
			return false;
		}

		//Determine filesize (Seek to end, get current position, rewind to start)
		fseek(File, 0, SEEK_END);
		FileSize = ftell(File);
		rewind(File);

		//Loop through file until end
		while (ftell(File) < FileSize)
		{
			//Read the section header
			fread (&MainSection_ID, 2, 1, File);

			//Read the length of the section
			fread (&MainSection_Length, 4, 1, File);

			//Determine section type
			switch (MainSection_ID)
			{
				case ROOT_SECTION:
					break;

				case OBJECT:
				{
					//Make a new temp object
					CMeshSubset TempSubset;

					//Store where the subsection was found, so we can return later
					SubSection_Start = ftell(File) - 6;

					//Zero the subsection ID and length, in case they were used previously
					SubSection_ID = 0;
					SubSection_Length = 0;

					//Loop from subsection Start to subsection End (Mainsection length, + the position it was found)
					while ((unsigned)ftell(File) < (MainSection_Length + SubSection_Start))
					{
						int ThisSectionStart = ftell(File);
						//Read the subchunk ID
						fread (&SubSection_ID, 2, 1, File);

						//Read the length of the subchunk
						fread (&SubSection_Length, 4, 1, File);

						//Determine SubChunk type using its ID
						switch (SubSection_ID)
						{
						    case OBJ_NAME:
								//Clear current name
								TempSubset.Name.clear();

								CharCount = 0;
								do
								{
									fread (&TempChar, 1, 1, File);
									TempSubset.Name += TempChar;
									CharCount++;
								}while(TempChar != '\0');

								//Remove '\0'
								TempSubset.Name = TempSubset.Name.substr(0, TempSubset.Name.size() - 1);

								break;

							case NUM_VERTICES:
								fread (&NumVertices, 4, 1, File);
								break;

							case NUM_FACES:
								fread (&NumFaces, 4, 1, File);
								break;

							case VERTICES_LIST:
							{
								//For all vertices
								for (int i = 0; i < NumVertices; i++)
								{
									//Create temp vertex
									CVector3 TempVertex;

									//Read X
									fread (&TempVertex.x, sizeof(float), 1, File);

									//Read Y
									fread (&TempVertex.y, sizeof(float), 1, File);

									//Read Z
									fread (&TempVertex.z, sizeof(float), 1, File);

									for(int b = 0; b < 4; b++)
									{
										//Read Weight
										fread (&TempVertex.Weights[b], 4, 1, File);

										//Read BoneID
										fread (&TempVertex.MaxBoneIDs[b], 4, 1, File);
									}

									//Push back to vector
									TempSubset.Vertices.push_back(TempVertex);

									//Add vertex to AABB
									AABB.AddVertex(CVector3(TempVertex.x, TempVertex.y, TempVertex.z));
								}

								break;
							}

							case FACE_LIST:
							{
								//For all faces
								for (int i = 0; i < NumFaces; i++)
								{
									//Create new temp face
									CFace TempFace;

									//Read A
									fread (&TempFace.Indices[0], 4, 1, File);

									//Read B
									fread (&TempFace.Indices[1], 4, 1, File);

									//Read C
									fread (&TempFace.Indices[2], 4, 1, File);

									//Read UVCoords[0]
									fread (&TempFace.UVCoords[0].u, 4, 1, File);
									fread (&TempFace.UVCoords[0].v, 4, 1, File);

									//Read UVCoords[1]
									fread (&TempFace.UVCoords[1].u, 4, 1, File);
									fread (&TempFace.UVCoords[1].v, 4, 1, File);

									//Read UVc
									fread (&TempFace.UVCoords[2].u, 4, 1, File);
									fread (&TempFace.UVCoords[2].v, 4, 1, File);

									//Push back the vector
									TempSubset.Faces.push_back(TempFace);
								}

								break;
							}

							case OBJ_MAT_NAME:
								//Clear current name
								TempSubset.MaterialName.clear();

								CharCount = 0;
								do
								{
									fread (&TempChar, 1, 1, File);
									TempSubset.MaterialName += TempChar;
									CharCount++;
								}while(TempChar != '\0');

								//Remove '\0'
								TempSubset.MaterialName = TempSubset.MaterialName.substr(0, TempSubset.MaterialName.size() - 1);

								break;

                            default:
									//Advance to next subchunk (Current Position, + SubChunk length, - the header (6))
									fseek(File, SubSection_Length - 6, SEEK_CUR);

								break;
						}
					}

					//Push object to the vector
					Subsets.push_back(TempSubset);

					break;
				}

				case BONE:
				{
					//Make a new temp bone
					CBone TempBone;

					//Store where the subsection was found, so we can return later
					SubSection_Start = ftell(File) - 6;

					//Zero the subsection ID and length, in case they were used previously
					SubSection_ID = 0;
					SubSection_Length = 0;

					//Loop from subsection Start to subsection End (Mainsection length, + the position it was found)
					while ((unsigned)ftell(File) < (MainSection_Length + SubSection_Start))
					{
						//Read the subchunk ID
						fread (&SubSection_ID, 2, 1, File);

						//Read the length of the subchunk
						fread (&SubSection_Length, 4, 1, File);

						//Determine SubChunk type using its ID
						switch (SubSection_ID)
						{
							case BONE_ID:
								fread (&TempBone.ID, 4, 1, File);
								break;

							case BONE_PARENT:
								fread (&TempBone.ParentID, 4, 1, File);
								break;

						    case BONE_NAME:
								//Clear current name
								TempBone.Name.clear();

								CharCount = 0;
								do
								{
									fread (&TempChar, 1, 1, File);
									TempBone.Name += TempChar;
									CharCount++;
								}while(TempChar != '\0');

								//Remove '\0'
								TempBone.Name = TempBone.Name.substr(0, TempBone.Name.size() - 1);

								break;

							case BONE_POS:
							{
								CVector3 Position;

								//Read X
								fread (&Position.x, sizeof(float), 1, File);

								//Read Y
								fread (&Position.y, sizeof(float), 1, File);

								//Read Z
								fread (&Position.z, sizeof(float), 1, File);

								TempBone.SetPosition(Position);

								break;
							}

							case BONE_ROT:
							{
								CQuaternion Rotation;

								//Read X
								fread (&Rotation.X, sizeof(float), 1, File);

								//Read Y
								fread (&Rotation.Y, sizeof(float), 1, File);

								//Read Z
								fread (&Rotation.Z, sizeof(float), 1, File);

								//Read W
								fread (&Rotation.W, sizeof(float), 1, File);

								TempBone.SetRotation(Rotation);

								break;
							}

							default:
									//Advance to next subchunk (Current Position, + SubChunk length, - the header (6))
									fseek(File, SubSection_Length - 6, SEEK_CUR);

								break;
						}
					}

					//Push bones to the vector
					//Bones[TempBone.ID] = TempBone;
					Bones.push_back(TempBone);

					break;
				}

				default:
					//Go to next section (Advance to the end of this section, minus 6)
					fseek(File, MainSection_Length - 6, SEEK_CUR);

					break;
			}
		}

		//Close the file
		fclose(File);

		for(int i = 0; i < (int)Bones.size(); i++)
		{
			//Set bone original matrix
			Bones[i].OriginalMatrix = Bones[i].GetMatrix();

			for(int j = 0; j < (int)Bones.size(); j++)
			{
				//If bone ID matches parent ID, set pointer
				if(Bones[i].ID == Bones[j].ParentID) Bones[j].ParentBone = &Bones[i];
			}
		}

		for(int i = 0; i < (int)Bones.size(); i++)
		{
			if(Bones[i].ParentBone) Bones[i].ParentBone->ChildBones.push_back(&Bones[i]);
		}

		//Process all subsets and load materials
		for (int i = 0; i < (int)Subsets.size(); i++)
		{
			//Load material from resource manager
            if(Subsets[i].MaterialName.size() > 0) Subsets[i].Material = Engine->ResourceManager.Materials.Get(Subsets[i].MaterialName);
            else Engine->Logger.AddEntry("Warning: Model '" + Filename + "' subset '" + Subsets[i].Name + "' has no material");

			//Convert Max bone IDs to correct indices
			if(Bones.size() > 0)
			{
				//Load skeletal animation shader
				Subsets[i].AnimShader.Load("skeletal_anim.cgfx");

				//Set skeletal animation parameters
				if(Subsets[i].Material)
					if(Subsets[i].Material->Texture)
						Subsets[i].AnimShader.SetTextureParam("DiffuseSampler", Subsets[i].Material->Texture);

				for(int v = 0; v < (int)Subsets[i].Vertices.size(); v++)
				{
					for(int b = 0; b < (int)Bones.size(); b++)
					{
						//If bone ID matches vertex's bone ID, set correct bone index
						for(int BoneIndex = 0; BoneIndex < 4; BoneIndex++)
						{
							if(Bones[b].ID == Subsets[i].Vertices[v].MaxBoneIDs[BoneIndex])
							{
								Subsets[i].Vertices[v].BoneIDs[BoneIndex] = b;
							}
						}
					}

					/*
					std::stringstream Stream;
					Stream << "\nMaxIDs: " << Subsets[i].Vertices[v].MaxBoneIDs[0] << "\t"
										 << Subsets[i].Vertices[v].MaxBoneIDs[1] << "\t"
										 << Subsets[i].Vertices[v].MaxBoneIDs[2] << "\t"
										 << Subsets[i].Vertices[v].MaxBoneIDs[3] << "\n";

					Stream << "BoneIDs: " << Subsets[i].Vertices[v].BoneIDs[0] << "\t"
										  << Subsets[i].Vertices[v].BoneIDs[1] << "\t"
										  << Subsets[i].Vertices[v].BoneIDs[2] << "\t"
										  << Subsets[i].Vertices[v].BoneIDs[3] << "\n";

					Stream << "Weights: " << Subsets[i].Vertices[v].Weights[0] << "\t"
										  << Subsets[i].Vertices[v].Weights[1] << "\t"
										  << Subsets[i].Vertices[v].Weights[2] << "\t"
										  << Subsets[i].Vertices[v].Weights[3] << "\n";

					Engine->Logger.AddEntry(Stream.str());
					*/
				}

				Subsets[i].Bones = &Bones;
			}

			//Process vertex arrays and calculate normals
			Subsets[i].Process();
		}

		OriginalBones = Bones;

		return true;
	}

	CBone *CMesh::GetBoneByName(std::string BoneName)
	{
		//Iterate through bones
		for(std::vector<CBone>::iterator i = Bones.begin(); i != Bones.end(); i++)
		{
			if((*i).Name == BoneName)
			{
				//Found it, return
				return &(*i);
			}
		}

		//Not found
		return 0;
	}

	CMeshSubset *CMesh::GetSubsetByName(std::string SubsetName)
	{
		//Iterate through subsets
		for(std::vector<CMeshSubset>::iterator i = Subsets.begin(); i != Subsets.end(); i++)
		{
			if((*i).Name == SubsetName)
			{
				//Found it, return
				return &(*i);
			}
		}

		//Not found
		return 0;
	}

	bool CMesh::Write(std::string Filename)
	{
		//Current MAIN section ID and length
		unsigned short MainSection_ID = 0;
		unsigned int MainSection_Length = 0;

		//Current SUB section ID and length
		unsigned short SubSection_ID = 0;
		unsigned int SubSection_Length = 0;
		unsigned int SubSection_Start = 0;

		//Current CHILD section ID and length
		unsigned short ChildSection_ID = 0;
		unsigned int ChildSection_Length = 0;
		unsigned int ChildSection_Start = 0;

		//Prefix with models path from engine config
		Filename = Engine->ModelsPath + Filename;

		//Open file for writing
		FILE *File = fopen(Filename.c_str(), "wb");

		//Check for errors
		if (!File) return false;

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
		for (int i = 0; i < (int)Subsets.size(); i++)
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
			/// + 6 + VerticesList	(float)	(5 * sizeof(float) * NumVertices)
			/// + 6 + FacesList (int)	    (3 * 4 * NumFaces)


			MainSection_Length =      42					            //All headers
                                    + 8					                //All single integers
                                    + (int)Subsets[i].Name.size()			//Name
                                    + (int)Subsets[i].MaterialName.size()
                                    + ((3 * (int)sizeof(float)) * (int)Subsets[i].Vertices.size())	//Vertices list
                                    + ((3 * 4) + (6 * sizeof(float)) * (int)Subsets[i].Faces.size());//Face list

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

			int Size = (int)Subsets[i].Vertices.size();

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

			Size = (int)Subsets[i].Faces.size();

			//Write numfaces
			fwrite(&Size, 4, 1, File);

			//////////////////////////////////////////////////////////////////
			//VERTICES_LIST
			SubSection_ID = VERTICES_LIST;

			//Calculate section size
			SubSection_Length = (int)((3 * sizeof(float)) * Subsets[i].Vertices.size()) + 6;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//For all vertices
			for (int j = 0; j < (int)Subsets[i].Vertices.size(); j++)
			{

				//Write X
				fwrite(&Subsets[i].Vertices[j].x, sizeof(float), 1, File);

				//Write Y
				fwrite(&Subsets[i].Vertices[j].y, sizeof(float), 1, File);

				//Write Z
				fwrite(&Subsets[i].Vertices[j].z, sizeof(float), 1, File);
			}

			//////////////////////////////////////////////////////////////////
			//FACE_LIST
			SubSection_ID = FACE_LIST;

			//Calculate section size
			SubSection_Length = (int)((3 * 4) + (6 * sizeof(float)) * (int)Subsets[i].Faces.size()) + 6;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//For all faces
			for (int j = 0; j < (int)Subsets[i].Faces.size(); j++)
			{
				//Write A
				fwrite(&Subsets[i].Faces[j].Indices[0], 4, 1, File);

				//Write B
				fwrite(&Subsets[i].Faces[j].Indices[1], 4, 1, File);

				//Write C
				fwrite(&Subsets[i].Faces[j].Indices[2], 4, 1, File);

				//Write UV coord A
				fwrite(&Subsets[i].Faces[j].UVCoords[0].u, sizeof(float), 1, File);
				fwrite(&Subsets[i].Faces[j].UVCoords[0].v, sizeof(float), 1, File);

				//Write UV coord B
				fwrite(&Subsets[i].Faces[j].UVCoords[1].u, sizeof(float), 1, File);
				fwrite(&Subsets[i].Faces[j].UVCoords[1].v, sizeof(float), 1, File);

				//Write UV coord C
				fwrite(&Subsets[i].Faces[j].UVCoords[2].u, sizeof(float), 1, File);
				fwrite(&Subsets[i].Faces[j].UVCoords[2].v, sizeof(float), 1, File);
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

		//Close the file
		fclose(File);

		return true;
	}

} //Namespace
