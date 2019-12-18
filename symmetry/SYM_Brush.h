#ifndef __SYM_BRUSH
#define __SYM_BRUSH

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#include "SYM_Geometry.h"
#include "SYM_Mesh.h"

namespace sym
{
	//Brush geometry
	class CBrush : public CGeometry
	{
		public:
			//Constructors
			CBrush();
			CBrush(std::string BrushName, int GeomType, int PhysType, CVector3 Dim = CVector3(16.0f, 16.0f, 16.0f));
			~CBrush();

			void Create(std::string BrushName, int GeomType, int PhysType, CVector3 Dim = CVector3(16.0f, 16.0f, 16.0f));

			//Resize brush dimentions
			void Resize(CVector3 Dim);

		private:
			//The mesh subset
			CMeshSubset *MeshSubset;

			//Create geometry types
			void CreateBoxGeometry(CVector3 Size);
			void CreatePlaneGeometry(CVector3 Size);
			void CreateCylinderGeometry(CVector3 Dimentions);
			void CreateSphereGeometry(CVector3 Dimentions);
	};

} //Namespace

#endif
