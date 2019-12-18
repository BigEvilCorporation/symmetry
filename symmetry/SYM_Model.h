#ifndef __SYM_MODEL
#define __SYM_MODEL

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#include "SYM_Geometry.h"

namespace sym
{
	class CModel : public CGeometry
	{
		public:
			CModel();
			~CModel();

			//Load mesh from .sym.mesh file
			bool Load(std::string ModelName, std::string Filename, int PhysType, bool UseResManager = true);

		protected:
			bool UseResourceManager;
	};

} // Namespace

#endif
