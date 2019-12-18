#include <sstream>
#include <algorithm>

#include "SYM_Octree.h"
#include "SYM_Engine.h"

namespace sym
{
    //Node count
    int NodeCount = 0;

    void COctree::Create(std::map<std::string, CGeometry*>& GeomList)
    {
        //Clear tree
        Clear();

        //Loop through geometry map
        for(std::map<std::string, CGeometry*>::iterator i = GeomList.begin(); i != GeomList.end(); i++)
        {
            //Merge bounding box
            AABB.Merge(&(*i).second->GetWorldAABB());

            //Add geometry
            //Geometry.push_back((*i).second);
        }

        //Make AABB a cube
        AABB.MakeCube();

		for(std::map<std::string, CGeometry*>::iterator i = GeomList.begin(); i != GeomList.end(); i++)
        {
			AddGeometry((*i).second);
		}

        //If there's too many geometries
        if((int)Geometry.size() > MaxGeometry)
        {
            //Split
            //Split();
            //Distribute();
        }
    }

    int COctree::GetNodeCount()
    {
        return NodeCount;
    }

    COctreeNode::COctreeNode()
    {
        MaxGeometry = 8;
        NodeID = NodeCount;
        NodeCount++;
        Parent = 0;

        //cout << "New octree node " << NodeID << endl;;
    }

    COctreeNode::COctreeNode(CBoundingBox Bounds, COctreeNode *ParentNode)
    {
        MaxGeometry = 8;
        NodeID = NodeCount;
        NodeCount++;
        AABB = Bounds;
		Parent = ParentNode;

        //cout << "New octree node " << NodeID << " - Size x: " << AABB.Size.x << " y: " << AABB.Size.y << " z: " << AABB.Size.z << endl;
        //cout << "Min x: " << AABB.Min.x << " y: " << AABB.Min.y << " z: " << AABB.Min.z << endl;
        //cout << "Max x: " << AABB.Max.x << " y: " << AABB.Max.y << " z: " << AABB.Max.z << endl;
    }

    COctreeNode::~COctreeNode()
    {
        for(std::list<COctreeNode*> ::iterator i = ChildNodes.begin(); i != ChildNodes.end(); i++)
        {
            delete *i;
        }
    }

    void COctreeNode::Clear()
    {
        //cout << "Clearing node " << NodeID << endl;

        for (int i = 0; i < (int)Geometry.size(); i++)
        {
            Geometry[i]->ClearOctreeNodes();
        }

        //Clear current nodes and geometry
        ChildNodes.clear();
        Geometry.clear();

        //Reset AABB
        AABB = CBoundingBox();

        //Reset node count
        NodeCount = 0;
    }

    void COctreeNode::AddGeometry(CGeometry* Geom)
    {
		//cout << "Request to add " << Geom->Name << endl;
        bool BelongsToChild = false;

		//Merge bounding box
        AABB.Merge(&Geom->GetWorldAABB());

        //Check if this node has children
        if(!ChildNodes.empty())
        {
            //Find out which node (potentially) contains this object
            for(std::list<COctreeNode*> ::iterator i = ChildNodes.begin(); i != ChildNodes.end(); i++)
            {
                //Check if geometry's AABB is inside the node's AABB
                if((*i)->AABB.Contains(&Geom->GetWorldAABB()))
                {
                    //If geom is not already in list
                    if(std::find((*i)->Geometry.begin(), (*i)->Geometry.end(), Geom) == (*i)->Geometry.end())
                    {
                        std::stringstream str;
                        str << "Adding geometry " << Geom->Name << " to node " << (*i)->NodeID;
                        //Engine->Logger.AddEntry(str.str());

                        //Add geometry to node
                        (*i)->AddGeometry(Geom);

                        //This geometry belongs in a child
                        BelongsToChild = true;
                    }
                }
            }

            //If the geometry isn't contained in one of the children
            if(!BelongsToChild)
            {
                //If geom is not already in list
                if(std::find(Geometry.begin(), Geometry.end(), Geom) == Geometry.end())
                {
                    std::stringstream str;
                    str << "Adding geometry " << Geom->Name << " to node " << NodeID;
                    //Engine->Logger.AddEntry(str.str());

                    //Add geometry to this node
                    Geometry.push_back(Geom);
                    Geom->AddOctreeNode(this);
                }
            }
        }
        //If this node has too many geometries
        else if((int)Geometry.size() > MaxGeometry)
        {
            //If geom is not already in list
            if(std::find(Geometry.begin(), Geometry.end(), Geom) == Geometry.end())
            {
                //Add geometry to this node
                Geometry.push_back(Geom);
                Geom->AddOctreeNode(this);

                std::stringstream str;
                str << "Node " << NodeID << " has too many geometries, splitting";
                //Engine->Logger.AddEntry(str.str());

                //Split node
                Split();
            }
        }
        else
        {
            //If geom is not already in list
            if(std::find(Geometry.begin(), Geometry.end(), Geom) == Geometry.end())
            {
                std::stringstream str;
                str << "Node " << NodeID << " has no children, adding geometry" << Geom->Name;
                //Engine->Logger.AddEntry(str.str());

                //Add geometry to this node
                Geometry.push_back(Geom);
                Geom->AddOctreeNode(this);
            }
        }
    }

    void COctreeNode::Split()
    {
        std::stringstream str;
        str << "Splitting octree node " << NodeID;
        //Engine->Logger.AddEntry(str.str());

        CVector3 Half = AABB.Size * 0.5f;
        CVector3 HalfX = CVector3(Half.x, 0, 0);
        CVector3 HalfY = CVector3(0, Half.y, 0);
        CVector3 HalfZ = CVector3(0, 0, Half.z);

        ChildNodes.push_back(new COctreeNode(CBoundingBox(AABB.Min,                    AABB.Min + Half             ), this));
        ChildNodes.push_back(new COctreeNode(CBoundingBox(AABB.Min + HalfX,            AABB.Max - Half + HalfX     ), this));
        ChildNodes.push_back(new COctreeNode(CBoundingBox(AABB.Min + HalfZ,            AABB.Min + Half + HalfZ     ), this));
        ChildNodes.push_back(new COctreeNode(CBoundingBox(AABB.Min + HalfX + HalfZ,    AABB.Max - HalfY            ), this));
        ChildNodes.push_back(new COctreeNode(CBoundingBox(AABB.Min + HalfY,            AABB.Max - HalfX - HalfZ    ), this));
        ChildNodes.push_back(new COctreeNode(CBoundingBox(AABB.Min + HalfY + HalfX,    AABB.Max - HalfZ            ), this));
        ChildNodes.push_back(new COctreeNode(CBoundingBox(AABB.Min + HalfY + HalfZ,    AABB.Max - HalfX            ), this));
        ChildNodes.push_back(new COctreeNode(CBoundingBox(AABB.Min + Half,             AABB.Max                    ), this));

        Distribute();
    }

    void COctreeNode::Distribute()
    {
        std::stringstream str;
        str << "Distributing octree node " << NodeID;
        //Engine->Logger.AddEntry(str.str());

        std::vector<CGeometry*> ToDelete;

        //Loop through geometry
        for(std::vector<CGeometry*> ::iterator g = Geometry.begin(); g != Geometry.end();)
        {
			//cout << "Searching for geometry " << (*g)->Name << endl;

            //Loop through child nodes
            for(std::list<COctreeNode*> ::iterator n = ChildNodes.begin(); n != ChildNodes.end(); n++)
            {
                //If node contains the geometry
                if((*n)->AABB.Contains(&(*g)->GetWorldAABB()))
                {
                    //cout << "Checking if geometry " << (*g)->Name << " is in list" << endl;
                    //If geom is not already in list
                    if(std::find((*n)->Geometry.begin(), (*n)->Geometry.end(), *g) == (*n)->Geometry.end())
                    {
                        std::stringstream str;
                        str << "DISTRIBUTE: Adding geometry " << (*g)->Name << " to node " << (*n)->NodeID;
                        //Engine->Logger.AddEntry(str.str());

                        //Add geometry to node
                        (*n)->AddGeometry((*g));

                        //Mark for deletion
                        if(std::find(ToDelete.begin(), ToDelete.end(), *g) == ToDelete.end())
                        {
                            //cout << "Adding " << (*g)->Name << " (" << (*g) << ") for deletion" << endl;
                            ToDelete.push_back(*g);
                        }
                    }
                }
            }

			++g;
        }

        //Delete marked geometries from this node
        for(std::vector<CGeometry*> ::iterator d = ToDelete.begin(); d != ToDelete.end();)
        {
            //cout << "Attempting to delete " << (*d)->Name << " (" << (*d) << ")" << endl;
            (*d)->DeleteOctreeNode(this);
            Geometry.erase(find(Geometry.begin(), Geometry.end(), *d));

            ++d;
        }
    }

	void COctreeNode::RemoveGeometry(CGeometry *Geom)
	{
		//Remove from this node
		if (std::find(Geometry.begin(), Geometry.end(), Geom) != Geometry.end())
			Geometry.erase(std::find(Geometry.begin(), Geometry.end(), Geom));
	}

    void COctreeNode::GeomHasMoved(CGeometry* Geom)
    {
		if(Parent)
		{
			//Remove from this node
			if (std::find(Geometry.begin(), Geometry.end(), Geom) != Geometry.end())
				Geometry.erase(std::find(Geometry.begin(), Geometry.end(), Geom));

			//Add to parent node for resorting
			Parent->AddGeometry(Geom);
		}
    }

    void COctreeNode::BuildRenderList(std::vector<CGeometry*> &GeomList, CFrustum *Frustum)
    {
        //Add this node's geometry to list, if not already listed
        for(std::vector<CGeometry*> ::iterator g = Geometry.begin(); g != Geometry.end(); g++)
        {
            if(std::find(GeomList.begin(), GeomList.end(), *g) == GeomList.end()) GeomList.push_back((*g));
        }

        //Iterate through child nodes
        for(std::list<COctreeNode*> ::iterator n = ChildNodes.begin(); n != ChildNodes.end(); n++)
        {
            //If node is in view frustum
            if(Frustum->CubeInFrustum((*n)->AABB.Centre, (*n)->AABB.Size.x * 0.5f))
            {
                //Add node's geometry
                (*n)->BuildRenderList(GeomList, Frustum);
            }
        }
    }

    void COctreeNode::Render(CFrustum *Frustum)
    {
        //Render this AABB
        AABB.Render();

        //Iterate through child nodes
        for(std::list<COctreeNode*> ::iterator n = ChildNodes.begin(); n != ChildNodes.end(); n++)
        {
            //If node is in view frustum
            if(Frustum->CubeInFrustum((*n)->AABB.Centre, (*n)->AABB.Size.x * 0.5f))
            {
                //Render node
                (*n)->Render(Frustum);
            }
        }
    }

} //Namespace
