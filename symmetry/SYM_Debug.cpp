#include "SYM_Debug.h"
#include "SYM_Octree.h"
#include "SYM_Engine.h"

namespace sym
{
	bool CDebugInfo::Init()
	{
		//Load font
		if(!Arial10.LoadTTFFont("arial.ttf", 10))
		{
			Engine->Logger.AddEntry("Could not load debug font");
			return false;
		}

		//Set colour
		Arial10.SetColour(255, 255, 255);

		return true;
	}

	void CDebugInfo::RenderDebugInfo()
	{
		Stream.str("");
		Stream << "Frames per second: " << GetFPS();
		Arial10.RenderText(Stream.str(), 1.0f, (Engine->ScreenHeight) - 10.0f);

		Stream.str("");
		Stream << "Camera position: " << Engine->Scene.GetActiveCamera()->GetPosition().x << ", " << Engine->Scene.GetActiveCamera()->GetPosition().y << ", " << Engine->Scene.GetActiveCamera()->GetPosition().z;
		Arial10.RenderText(Stream.str(), 1.0f, (Engine->ScreenHeight) - 20.0f);

		Stream.str("");
		Stream << "Camera rotation: " << Engine->Scene.GetActiveCamera()->GetRotation().x << ", " << Engine->Scene.GetActiveCamera()->GetRotation().y << ", " << Engine->Scene.GetActiveCamera()->GetRotation().z;
		Arial10.RenderText(Stream.str(), 1.0f, (Engine->ScreenHeight) - 30.0f);

		Stream.str("");
		Stream << "Camera direction: " << Engine->Scene.GetActiveCamera()->GetForward().x << ", " << Engine->Scene.GetActiveCamera()->GetForward().y << ", " << Engine->Scene.GetActiveCamera()->GetForward().z;
		Arial10.RenderText(Stream.str(), 1.0f, (Engine->ScreenHeight) - 40.0f);

		Stream.str("");
		Stream << "Meshes: " << Engine->ResourceManager.Meshes.GetNumResources();
		Arial10.RenderText(Stream.str(), 1.0f, (Engine->ScreenHeight) - 50.0f);

		Stream.str("");
		Stream << "Geometry: " << Engine->Scene.Geometry.size();
		Arial10.RenderText(Stream.str(), 1.0f, (Engine->ScreenHeight) - 60.0f);

		Stream.str("");
		Stream << "Materials: " << Engine->ResourceManager.Materials.GetNumResources();
		Arial10.RenderText(Stream.str(), 1.0f, (Engine->ScreenHeight) - 70.0f);

		Stream.str("");
		Stream << "Textures: " << Engine->ResourceManager.Textures.GetNumResources();
		Arial10.RenderText(Stream.str(), 1.0f, (Engine->ScreenHeight) - 80.0f);

		Stream.str("");
		Stream << "Shaders: " << Engine->ResourceManager.Shaders.GetNumResources();
		Arial10.RenderText(Stream.str(), 1.0f, (Engine->ScreenHeight) - 90.0f);

		Stream.str("");
		Stream << "Octree size: X: " << Engine->Scene.GetWorldSize().Size.x << " Y: " << Engine->Scene.GetWorldSize().Size.y << " Z: " << Engine->Scene.GetWorldSize().Size.z;
		Arial10.RenderText(Stream.str(), 1.0f, (Engine->ScreenHeight) - 100.0f);

		Stream.str("");
		Stream << "Octree Min: X: " << Engine->Scene.GetWorldSize().Min.x << " Y: " << Engine->Scene.GetWorldSize().Min.y << " Z: " << Engine->Scene.GetWorldSize().Min.z;
		Arial10.RenderText(Stream.str(), 1.0f, (Engine->ScreenHeight) - 110.0f);

		Stream.str("");
		Stream << "Octree Max: X: " << Engine->Scene.GetWorldSize().Max.x << " Y: " << Engine->Scene.GetWorldSize().Max.y << " Z: " << Engine->Scene.GetWorldSize().Max.z;
		Arial10.RenderText(Stream.str(), 1.0f, (Engine->ScreenHeight) - 120.0f);

		Stream.str("");
		Stream << "Octree Centre: X: " << Engine->Scene.GetWorldSize().Centre.x << " Y: " << Engine->Scene.GetWorldSize().Centre.y << " Z: " << Engine->Scene.GetWorldSize().Centre.z;
		Arial10.RenderText(Stream.str(), 1.0f, (Engine->ScreenHeight) - 130.0f);

		Stream.str("");
		Stream << "Octree node count: " << Engine->Scene.Octree.GetNodeCount();
		Arial10.RenderText(Stream.str(), 1.0f, (Engine->ScreenHeight) - 140.0f);

		Stream.str("");
		Stream << "Render list size: " << Engine->Scene.RenderList.size();
		Arial10.RenderText(Stream.str(), 1.0f, (Engine->ScreenHeight) - 150.0f);

		Stream.str("");
		Stream << "Mouse: X: " << Engine->Input.GetMouseAbsoluteX() << " Y: " << Engine->Input.GetMouseAbsoluteY();
		Arial10.RenderText(Stream.str(), 1.0f, (Engine->ScreenHeight) - 160.0f);
	}


} //Namespace
