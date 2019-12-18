#ifndef __SYM_TERRAIN
#define __SYM_TERRAIN

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <string>
#include <vector>

#include <GL/gl.h>
#include <GL/glu.h>

#include "SYM_Geometry.h"

namespace sym
{
	class CTerrain : public CGeometry
	{
		public:
			//Constructor
			CTerrain();
			~CTerrain();

			//Load heightmap file
			bool Load(std::string TerrainName, std::string Filename);

		private:
			//Mesh subset
			CMeshSubset *MeshSubset;

			//Size of map
			int Width, Height;

			//Parameters
			float MeshScale, HeightScale;
			int Resolution, Smoothing;
	};

} //Namespace

#endif
