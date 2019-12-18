#include "SYM_Terrain.h"
#include "SYM_Engine.h"
#include "SYM_Texture.h"
#include "SYM_Maths.h"

#include <sstream>

namespace sym
{
	CTerrain::CTerrain()
	{
		Type = SYM_TERR;
		MeshSubset = 0;
		Mesh = 0;
		MeshScale = 1;
		HeightScale = 1;
		Resolution = 2;
		Smoothing = 2;
	}

	CTerrain::~CTerrain()
	{
	    if(MeshSubset) delete MeshSubset;
	}

	bool CTerrain::Load(std::string TerrainName, std::string Filename)
	{
	    Name = TerrainName;

		//Parameter boundaries
		if(Resolution < 1) Resolution = 1;
		if(Resolution > 10) Resolution = 10;
		if(Smoothing < 0) Smoothing = 0;
		if(Smoothing > 8) Smoothing = 8;

		HMapFilename = Filename;

		//Prefix filename with heightmaps path from engine config
		Filename = Engine->HeightmapsPath + Filename;

		//Load the image
		SDL_Surface *Surface = IMG_Load(Filename.c_str());

		//Check if image data loaded ok
		if(Surface == 0)
		{
		    Engine->Logger.AddEntry("Error: Could not load heightmap file " + Filename);
			Engine->Console.PrintLine("> Error: Could not load heightmap file");
			return false;
		}

		//Get dimentions
		Width = Surface->w;
		Height = Surface->h;

		//Lock the SDL surface
		SDL_LockSurface(Surface);

		//Create new mesh
		Mesh = new CMesh;

		//Create new mesh subset
		MeshSubset = new CMeshSubset;

		//Temp vertex
		CVector3 TempVertex;

		//Load in vertices
		for (int x = 0; x < Width; x++)
		{
			for (int z = 0; z < Height; z++)
			{
				TempVertex.x = static_cast<float> (x) * MeshScale;
				TempVertex.y =	static_cast<float> (SDL_GetPixel(Surface, x, z)) * MeshScale * HeightScale;
				TempVertex.z = static_cast<float> (z) * MeshScale;

				//TempVertex.UVCoord.u = static_cast<float> (x) / Width;
				//TempVertex.UVCoord.v = static_cast<float> (z) / Height;

				MeshSubset->Vertices.push_back(TempVertex);
			}
		}

		//Free heightmap surface
		SDL_FreeSurface(Surface);

		//Smoothing algorithm
		for (int Pass = 0; Pass < Smoothing; Pass++)
		{
			float yd = (float)Width;

			for (int y = 1; y < Width - 1; y++)
			{
				for (int x = 1; x < Height - 1; x++)
				{
					MeshSubset->Vertices[x + yd].y = (	MeshSubset->Vertices[x - 1 + yd].y +
														MeshSubset->Vertices[x + 1 + yd].y +
														MeshSubset->Vertices[x + yd - Width].y +
														MeshSubset->Vertices[x + yd - Height].y) * 0.25f;
				}

				yd += Width;
			}
		}

		//Temp face
		CFace TempFace;

		//Calculate faces
		for (int x = 0; x < Width - 1; x += Resolution)
		{
			for (int z = 0; z < Height - 1; z += Resolution)
			{
				//First triangle
				TempFace.Indices[0] = x	* Width + z;
				TempFace.Indices[1] = x	* Width + (z + Resolution);
				TempFace.Indices[2] = (x + Resolution) * Width + z;

				MeshSubset->Faces.push_back(TempFace);

				//Second triangle
				TempFace.Indices[0] = (x + Resolution) * Width + (z + Resolution);
				TempFace.Indices[1] = (x + Resolution) * Width + z;
				TempFace.Indices[2] = x * Width + (z + Resolution);

				MeshSubset->Faces.push_back(TempFace);
			}
		}

		//Process vertex/face data
		MeshSubset->Process();

		//Add subset to mesh
		Mesh->Subsets.push_back(*MeshSubset);

		//Setup collision body
		CreateLevel(Mesh->Subsets[0].Vertices, Mesh->Subsets[0].Faces);

		//Set physics geometry
		SetGeometry(this);

		return true;
	}

} //Namespace
