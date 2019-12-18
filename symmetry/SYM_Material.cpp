#include "SYM_Material.h"
#include "SYM_Engine.h"

namespace sym
{
	CColour::CColour()
	{
		//Initialise all values
		r = g = b = a = 255;
		rgba[0] = rgba[1] = rgba[2] = rgba[3] = 255;
	}

	CColour::CColour(int R, int G, int B)
	{
		r = R;
		g = G;
		b = B;
		a = 255;
		rgba[0] = R;
		rgba[1] = G;
		rgba[2] = B;
		rgba[3] = 255;
	}

	CColour::CColour(int R, int G, int B, int A)
	{
		r = R;
		g = G;
		b = B;
		a = A;
		rgba[0] = R;
		rgba[1] = G;
		rgba[2] = B;
		rgba[3] = A;
	}

	CMaterial::CMaterial()
	{
		//Initialise all values
		Texture = 0;
		Shader = 0;
		Video = 0;
		Shininess = 0;
		Transparency = 1.0f;
		UTile = VTile = 1.0f;
		UOffset = VOffset = 1.0f;
	}

	CMaterial::~CMaterial()
	{
	}

	bool CMaterial::Load(std::string Filename)
	{
		//Used for file seeking
		long int FileSize;

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
		char TempChar;

		//Count number of chars
		int CharCount;

		//Prefix filename with materials path
		Filename = Engine->MaterialsPath + Filename;

		//Open file for reading
		FILE *File = fopen(Filename.c_str(), "rb");

		//Check if file exists
		if (!File)
		{
		    Engine->Logger.AddEntry("Error: Could not load material " + Filename);
			Engine->Console.PrintLine("> Error: Could not load material " + Filename);
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
				case MATERIAL:
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
							case MAT_NAME:
								//Clear current name
								Name.clear();

								CharCount = 0;
								do
								{
									fread (&TempChar, 1, 1, File);
									Name += TempChar;
									CharCount++;
								}while(TempChar != '\0');

								//Remove '\0' or things get messy when using the string
								Name = Name.substr(0, Name.size());
								break;

							case DIFFUSE:
								fread (&Diffuse.r, 4, 1, File);
								fread (&Diffuse.g, 4, 1, File);
								fread (&Diffuse.b, 4, 1, File);

								//Convert to GLint colour
								Diffuse.rgba[0] = Diffuse.r;
								Diffuse.rgba[1] = Diffuse.g;
								Diffuse.rgba[2] = Diffuse.b;
								break;

                            case AMBIENT:
								fread (&Ambient.r, 4, 1, File);
								fread (&Ambient.g, 4, 1, File);
								fread (&Ambient.b, 4, 1, File);

								//Convert to GLint colour
								Ambient.rgba[0] = Ambient.r;
								Ambient.rgba[1] = Ambient.g;
								Ambient.rgba[2] = Ambient.b;
								break;

                            case SPECULAR:
								fread (&Specular.r, 4, 1, File);
								fread (&Specular.g, 4, 1, File);
								fread (&Specular.b, 4, 1, File);

								//Convert to GLint colour
								Specular.rgba[0] = Specular.r;
								Specular.rgba[1] = Specular.g;
								Specular.rgba[2] = Specular.b;
								break;

							case SHININESS:
								fread (&Shininess, 4, 1, File);
								break;

							case TRANSPARENCY:
								fread (&Transparency, 4, 1, File);
								//Transparency = (1.0f - (Transparency / 255.0f));
								Transparency = 1.0f;
								break;

							case TEXTURE:
								//Store where the subsection was found, so we can return later
								ChildSection_Start = ftell(File) - 6;

								//Zero the subsection ID and length, in case they were used previously
								ChildSection_ID = 0;
								ChildSection_Length = 0;

								//Loop from subsection Start to subsection End (Mainsection length, + the position it was found)
								while ((unsigned)ftell(File) < (SubSection_Length + ChildSection_Start))
								{
									//Read the subchunk ID
									fread (&ChildSection_ID, 2, 1, File);

									//Read the length of the subchunk
									fread (&ChildSection_Length, 4, 1, File);

									//Determine SubChunk type using its ID
									switch (ChildSection_ID)
									{
										case FILENAME:
											//Clear filename
											TextureFile.clear();

											CharCount = 0;
											do
											{
												fread (&TempChar, 1, 1, File);
												TextureFile += TempChar;
												CharCount++;
											}while(TempChar != '\0');

											//Remove '\0' or things get messy when using the string
											if(TextureFile.size() > 0) TextureFile = TextureFile.substr(0, TextureFile.size() - 1);
											break;

										case UTILE:
											fread (&UTile, sizeof(float), 1, File);
											break;

										case VTILE:
											fread (&VTile, sizeof(float), 1, File);
											break;

										case UOFFSET:
											fread (&UOffset, sizeof(float), 1, File);
											break;

										case VOFFSET:
											fread (&VOffset, sizeof(float), 1, File);
											break;

										default:
											//Advance to next subchunk (Current Position, + SubChunk length, - the header (6))
											fseek(File, ChildSection_Length - 6, SEEK_CUR);

											break;
									}
								}

								break;

								default:
								//Advance to next subchunk (Current Position, + SubChunk length, - the header (6))
								fseek(File, SubSection_Length - 6, SEEK_CUR);

								break;
						}
					}

					break;

				default:
					//Go to next section (Advance to the end of this section, minus 6)
					fseek(File, MainSection_Length - 6, SEEK_CUR);

					break;
			}
		}

		//Close the file
		fclose(File);

		//If texture filename size is sane
		if(TextureFile.size() > 4)
		{
			//If texture ends with '.mpeg'
			if(TextureFile.substr(TextureFile.size() - 3, TextureFile.size()) == "MPG")
			{
				//Load video
				LoadVideo(TextureFile);
			}
			else
			{
				//Load texture
				LoadTexture(TextureFile);
			}
		}

		return true;
	}

	bool CMaterial::Write(std::string Filename)
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

		//Prefix filename with materials path
		Filename = Engine->MaterialsPath + Filename;

		//Open file for writing
		FILE *File = fopen(Filename.c_str(), "wb");

		//Check for errors
		if (!File)
		{
			Engine->Logger.AddEntry("Error opening file for writing: " + Filename);
			return false;
		}

		//////////////////////////////////////////////////////////////////
		//ROOT_SECTION
		MainSection_ID = ROOT_SECTION;
		MainSection_Length = 6;

		//Write ROOT_SECTION header
		fwrite(&MainSection_ID, 2, 1, File);

		//Write root section length
		fwrite(&MainSection_Length, 4, 1, File);

		MainSection_ID = MATERIAL;

		//Append '\0' to names
		//Name += '\0';
		//TextureFile += '\0';

		//Calculate MATERIAL section length
		///       Header 		(6)
		/// + 6 + Name (char)		(Chars in name)
		/// + 6 + MaterialID (int)	(4)
		/// + 6 + DiffuseColour (3xint)	(12)
		/// + 6 + AmbientColour (3xint)	(12)
		/// + 6 + SpecularColour (3xint)(12)
		/// + 6 + Shininess (int)	(4)
		/// + 6 + Transparency (int) 	(4)

		//Calculate TEXTURE section length
		///	+ Header		(6)
		/// + 6 + Filename (char) 	(Chars in name)
		/// + 6 + UTile (float)		(sizeof(float))
		/// + 6 + VTile (float)		(sizeof(float))
		/// + 6 + UOffset (float)	(sizeof(float))
		/// + 6 + VOffset (float)	(sizeof(float))

		//Calculate SHADER section length
		///	+ Header		(6)
		/// + 6 + Filename (char) 	(Chars in name)
		/// + 6 + FloatParams		(Chars + 4) x n
		/// + 6 + Float3Params		(Chars + 12) x n
		/// + 6 + Float4Params		(Chars + 16) x n
		/// + 6 + Sampler			(Chars) x n

		int ShaderLength = 0;

		if(Shader)
		{
			//Calculate shader section length
			ShaderLength += 6;						//Header
			ShaderLength += (int)Shader->CGFXFile.size();//Filename

			/*
			//All float params
			for(int i = 0; i < (int)Shader->FloatParams.size(); i++)
			{
				//Add header + float
				ShaderLength += 10;

				//Add semantic name
				ShaderLength += (int)Shader->FloatParams[i].Semantic.size();
			}

			//All float3 params
			for(int i = 0; i < (int)Shader->Float3Params.size(); i++)
			{
				//Add header + float3
				ShaderLength += 18;

				//Add semantic name
				ShaderLength += (int)Shader->Float3Params[i].Semantic.size();
			}

			//All float4 params
			for(int i = 0; i < (int)Shader->Float4Params.size(); i++)
			{
				//Add header + float4
				ShaderLength += 22;

				//Add semantic name
				ShaderLength += (int)Shader->Float4Params[i].Semantic.size();
			}

			//All float3 params
			for(int i = 0; i < (int)Shader->TextureParams.size(); i++)
			{
				//Add header
				ShaderLength += 6;

				//Add semantic name
				ShaderLength += (int)Shader->TextureParams[i].Semantic.size();

				//Add filename
				ShaderLength += (int)Shader->TextureParams[i].Filename.size();
			}
			*/
		}

		MainSection_Length = 	  72					//All headers
								+ 48					//All single integers
								+ (int)Name.size()		//Name
								+ (int)TextureFile.size()	//Texture
								+ (4 * (int)sizeof(float)			//All floats
								+ ShaderLength);			//Shader section

		//Write header
		fwrite(&MainSection_ID, 2, 1, File);

		//Write length
		fwrite(&MainSection_Length, 4, 1, File);

			//////////////////////////////////////////////////////////////
			//MAT_NAME
			SubSection_ID = MAT_NAME;

			SubSection_Length = (int)Name.size() + 6;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//Write object name
			fwrite(Name.c_str(), 1, Name.size(), File);

			//////////////////////////////////////////////////////////////////
			//DIFFUSE
			SubSection_ID = DIFFUSE;
			SubSection_Length = 18;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//Write R
			fwrite(&Diffuse.r, 4, 1, File);

			//Write G
			fwrite(&Diffuse.g, 4, 1, File);

			//Write B
			fwrite(&Diffuse.b, 4, 1, File);

			//////////////////////////////////////////////////////////////////
			//AMBIENT
			SubSection_ID = AMBIENT;
			SubSection_Length = 18;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//Write R
			fwrite(&Ambient.r, 4, 1, File);

			//Write G
			fwrite(&Ambient.g, 4, 1, File);

			//Write B
			fwrite(&Ambient.b, 4, 1, File);

			//////////////////////////////////////////////////////////////////
			//SPECULAR
			SubSection_ID = SPECULAR;
			SubSection_Length = 18;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//Write R
			fwrite(&Specular.r, 4, 1, File);

			//Write G
			fwrite(&Specular.g, 4, 1, File);

			//Write B
			fwrite(&Specular.b, 4, 1, File);

			//////////////////////////////////////////////////////////////////
			//SHININESS
			SubSection_ID = SHININESS;
			SubSection_Length = 10;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//Write Shininess
			fwrite(&Shininess, 4, 1, File);

			//////////////////////////////////////////////////////////////////
			//TRANSPARENCY
			SubSection_ID = TRANSPARENCY;
			SubSection_Length = 10;

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

			//Write Transparency
			fwrite(&Transparency, 4, 1, File);

			//////////////////////////////////////////////////////////////
			//TEXTURE

			SubSection_ID = TEXTURE;

			//Calculate TEXTURE section length
			///	+ Header		(6)
			/// + 6 + Filename (char) 	(Chars in name)
			/// + 6 + UTile (float)		(sizeof(float))
			/// + 6 + VTile (float)		(sizeof(float))
			/// + 6 + UOffset (float)	(sizeof(float))
			/// + 6 + VOffset (float)	(sizeof(float))

			SubSection_Length =   36					    //All headers
                                + (int)TextureFile.size()	    //Texture
                                + (4 * (int)sizeof(float));	//All floats

			//Write header
			fwrite(&SubSection_ID, 2, 1, File);

			//Write length
			fwrite(&SubSection_Length, 4, 1, File);

				//////////////////////////////////////////////////////////////
				//FILENAME

				ChildSection_ID = FILENAME;

				ChildSection_Length = (int)TextureFile.size() + 6;

				//Write header
				fwrite(&ChildSection_ID, 2, 1, File);

				//Write length
				fwrite(&ChildSection_Length, 4, 1, File);

				//Write object name
				fwrite(TextureFile.c_str(), 1, TextureFile.size(), File);

				//////////////////////////////////////////////////////////////////
				//UTILE
				ChildSection_ID = UTILE;
				ChildSection_Length = 10;

				//Write header
				fwrite(&ChildSection_ID, 2, 1, File);

				//Write length
				fwrite(&ChildSection_Length, 4, 1, File);

				//Write Transparency
				fwrite(&UTile, sizeof(float), 1, File);

				//////////////////////////////////////////////////////////////////
				//VTILE
				ChildSection_ID = VTILE;
				ChildSection_Length = 10;

				//Write header
				fwrite(&ChildSection_ID, 2, 1, File);

				//Write length
				fwrite(&ChildSection_Length, 4, 1, File);

				//Write Transparency
				fwrite(&VTile, sizeof(float), 1, File);

				//////////////////////////////////////////////////////////////////
				//UOFFSET
				ChildSection_ID = UOFFSET;
				ChildSection_Length = 10;

				//Write header
				fwrite(&ChildSection_ID, 2, 1, File);

				//Write length
				fwrite(&ChildSection_Length, 4, 1, File);

				//Write Transparency
				fwrite(&UOffset, sizeof(float), 1, File);

				//////////////////////////////////////////////////////////////////
				//UOFFSET
				ChildSection_ID = VOFFSET;
				ChildSection_Length = 10;

				//Write header
				fwrite(&ChildSection_ID, 2, 1, File);

				//Write length
				fwrite(&ChildSection_Length, 4, 1, File);

				//Write Transparency
				fwrite(&VOffset, sizeof(float), 1, File);

			if(Shader)
			{
				//////////////////////////////////////////////////////////////
				//SHADER
				SubSection_ID = SHADER;
				SubSection_Length = ShaderLength;

				//Write header
				fwrite(&SubSection_ID, 2, 1, File);

				//Write length
				fwrite(&SubSection_Length, 4, 1, File);

					//////////////////////////////////////////////////////////////
					//CGFX_FILENAME
					ChildSection_ID = CGFX_FILENAME;
					ChildSection_Length = (int)Shader->CGFXFile.size() + 6;

					//Write header
					fwrite(&ChildSection_ID, 2, 1, File);

					//Write length
					fwrite(&ChildSection_Length, 4, 1, File);

					//Write object name
					fwrite(Shader->CGFXFile.c_str(), 1, Shader->CGFXFile.size(), File);

					/*
					for(int i = 0; i < (int)Shader->FloatParams.size(); i++)
					{
						//////////////////////////////////////////////////////////////
						//PARAM_FLOAT
						ChildSection_ID = PARAM_FLOAT;
						ChildSection_Length = (int)Shader->FloatParams[i].Semantic.size() + 10;

						//Write header
						fwrite(&ChildSection_ID, 2, 1, File);

						//Write length
						fwrite(&ChildSection_Length, 4, 1, File);

						//Write name
						fwrite(Shader->FloatParams[i].Semantic.c_str(), 1, Shader->FloatParams[i].Semantic.size(), File);

						//Write float
						fwrite(&Shader->FloatParams[i].Values[0], sizeof(float), 1, File);
					}

					for(int i = 0; i < (int)Shader->Float3Params.size(); i++)
					{
						//////////////////////////////////////////////////////////////
						//PARAM_FLOAT3
						ChildSection_ID = PARAM_FLOAT3;
						ChildSection_Length = (int)Shader->Float3Params[i].Semantic.size() + 18;

						//Write header
						fwrite(&ChildSection_ID, 2, 1, File);

						//Write length
						fwrite(&ChildSection_Length, 4, 1, File);

						//Write name
						fwrite(Shader->Float3Params[i].Semantic.c_str(), 1, Shader->Float3Params[i].Semantic.size(), File);

						//Write floats
						fwrite(&Shader->Float3Params[i].Values[0], sizeof(float), 1, File);
						fwrite(&Shader->Float3Params[i].Values[1], sizeof(float), 1, File);
						fwrite(&Shader->Float3Params[i].Values[2], sizeof(float), 1, File);
					}

					for(int i = 0; i < (int)Shader->Float4Params.size(); i++)
					{
						//////////////////////////////////////////////////////////////
						//PARAM_FLOAT4
						ChildSection_ID = PARAM_FLOAT4;
						ChildSection_Length = (int)Shader->Float4Params[i].Semantic.size() + 22;

						//Write header
						fwrite(&ChildSection_ID, 2, 1, File);

						//Write length
						fwrite(&ChildSection_Length, 4, 1, File);

						//Write name
						fwrite(Shader->Float4Params[i].Semantic.c_str(), 1, Shader->Float4Params[i].Semantic.size(), File);

						//Write floats
						fwrite(&Shader->Float4Params[i].Values[0], sizeof(float), 1, File);
						fwrite(&Shader->Float4Params[i].Values[1], sizeof(float), 1, File);
						fwrite(&Shader->Float4Params[i].Values[2], sizeof(float), 1, File);
						fwrite(&Shader->Float4Params[i].Values[3], sizeof(float), 1, File);
					}

					for(int i = 0; i < (int)Shader->Float4Params.size(); i++)
					{
						//////////////////////////////////////////////////////////////
						//PARAM_SAMPLER
						ChildSection_ID = PARAM_SAMPLER;
						ChildSection_Length = (int)Shader->TextureParams[i].Semantic.size() + (int)Shader->TextureParams[i].Filename.size() + 6;

						//Write header
						fwrite(&ChildSection_ID, 2, 1, File);

						//Write length
						fwrite(&ChildSection_Length, 4, 1, File);

						//Write name
						fwrite(Shader->TextureParams[i].Semantic.c_str(), 1, Shader->TextureParams[i].Semantic.size(), File);
						
						//Write filename
						fwrite(Shader->TextureParams[i].Filename.c_str(), 1, Shader->TextureParams[i].Filename.size(), File);
					}
					*/
			}


		//Close the file
		fclose(File);

		return true;
	}

	void CMaterial::Apply()
	{
		//If video has been loaded
		if(Video)
		{
			//Grab next frame
			Video->ReadFrame();

			//Enable textures
			glEnable(GL_TEXTURE_2D);

			//Bind video frame texture
			glBindTexture(GL_TEXTURE_2D, Video->Texture->Texture);
		}

		//If texture has been loaded
		if(Texture)
		{
			//Enable textures
			glEnable(GL_TEXTURE_2D);

			//Bind texture
			glBindTexture(GL_TEXTURE_2D, Texture->Texture);

			//glColor4ub(255, 255, 255, 255);
		}

		//Enable colour materials
		glEnable(GL_COLOR_MATERIAL);

		glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4f(1.0f, 1.0f, 1.0f, Transparency);

		//Set colour materials
		//glMaterialiv(GL_FRONT, GL_AMBIENT, Ambient.rgba);
		//glMaterialiv(GL_FRONT, GL_DIFFUSE, Diffuse.rgba);
		//glMaterialiv(GL_FRONT, GL_SPECULAR, Specular.rgba);

		//TODO: Fix colours - glMaterialiv
		
		//Set shininess
		//glMateriali(GL_FRONT, GL_SHININESS, Shininess);
	}

	bool CMaterial::LoadTexture(std::string Filename)
	{
		//Load texture from resource manager
		Texture = Engine->ResourceManager.Textures.Get(Filename);

		//If texture was not loaded
		if(!Texture) return false;

		return true;
	}

	bool CMaterial::LoadVideo(std::string Filename)
	{
		//Load video from resource manager
		Video = Engine->ResourceManager.Videos.Get(Filename);

		//If video was not loaded
		if(!Video) return false;

		//Point texture to the video's texture
		Texture = Video->Texture;

		//Begin playback
		Video->Play();

		return true;
	}

	bool CMaterial::LoadShader(std::string Filename)
	{
		//Load shader from resource manager
		Engine->ResourceManager.Shaders.Get(Filename);

		//If shader was not loaded
		if(!Shader) return false;

		return true;
	}

	bool CMaterial::SetShader(CShader *Shad)
	{
		if(Shad) Shader = Shad;
		else return false;

		return true;
	}

	void CMaterial::SetAmbient(int r, int g, int b)
	{
		//Set ambient values if in range
		if(r > 0 && r < 255) Ambient.r = r;
		if(g > 0 && g < 255) Ambient.g = g;
		if(b > 0 && b < 255) Ambient.b = b;
	}

	void CMaterial::SetDiffuse(int r, int g, int b)
	{
		//Set diffuse values if in range
		if(r > 0 && r < 255) Diffuse.r = r;
		if(g > 0 && g < 255) Diffuse.g = g;
		if(b > 0 && b < 255) Diffuse.b = b;
	}

	void CMaterial::SetSpecular(int r, int g, int b)
	{
		//Set specular values if in range
		if(r > 0 && r < 255) Specular.r = r;
		if(g > 0 && g < 255) Specular.g = g;
		if(b > 0 && b < 255) Specular.b = b;
	}

	void CMaterial::SetShininess(int s)
	{
		if(s > 0 && s < 255) Shininess = s;
	}

	void CMaterial::SetTransparency(float t)
	{
		if(t > 0.0f && t < 1.0f) Transparency = t;
	}

} //Namespace
