#ifndef __SYM_RESOURCE
#define __SYM_RESOURCE

#ifdef SYM_DLL
#   define __declspec(dllexport)
#else
//#   define __declspec(dllimport)
#endif

#include <string>
#include <map>

#include "SYM_Engine.h"

namespace sym
{
	template<class TemplateClass>
	class CResourceManager
	{
		public:
			//Load + get pointer to resource by filename
			TemplateClass *Get(std::string Filename);

			//Create a new object
			TemplateClass *New(std::string Name);

			//Returns true if the resource exists
			bool Exists(std::string Name);

			//Check if resource is still in use and destroy it
			void Destroy();

			//Return number of resources
			unsigned int GetNumResources();

		private:
			//STL Map of objects
			std::map<std::string, TemplateClass*> Resources;
	};

	template<class TemplateClass>
	TemplateClass *CResourceManager<TemplateClass>::Get(std::string Filename)
	{
		//Does the resource exist?
		if(Resources.find(Filename) == Resources.end())
		{
			//Resource doesn't exist, attempt to load it
			TemplateClass *NewObject = new TemplateClass;

			if (!NewObject->Load(Filename))
			{
				//Could not be loaded
				delete NewObject;
				return 0;
			}
			else
			{
				//Loaded, add to map
				Resources[Filename] = NewObject;

				//Return pointer to object in map (NOT NewObject, it's a temporary)
				return Resources[Filename];
			}
		}
		else
		{
			//Resource already exists, return pointer to object
			return Resources[Filename];
		}
	}

	template<class TemplateClass>
	unsigned int CResourceManager<TemplateClass>::GetNumResources()
	{
		//Return the size of the map
		return Resources.size();
	}

	template<class TemplateClass>
	bool CResourceManager<TemplateClass>::Exists(std::string Name)
	{
		//Find the resource
		if (Resources.find(Name) == Resources.end()) return false;

		return true;
	}

} //Namespace

#endif

