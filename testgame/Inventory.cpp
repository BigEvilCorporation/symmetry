#include "Inventory.h"

CInventoryItemBase::CInventoryItemBase()
{
	Size = 0;
}

CInventoryItemBase::~CInventoryItemBase()
{
}

void CInventoryItemBase::SetName(std::string ItemName)
{
	Name = ItemName;
}

std::string CInventoryItemBase::GetName()
{
	return Name;
}

void CInventoryItemBase::SetSize(int ItemSize)
{
	if(Size > -1) Size = ItemSize;
}

int CInventoryItemBase::GetSize()
{
	return Size;
}

CInventory::CInventory()
{
	MaxSize = 0;
	MaxItems = 0;
	CurrentSize = 0;
}

CInventory::~CInventory()
{
	//Clear items list
	Items.clear();
}

bool CInventory::AddItem(CInventoryItemBase *Item)
{
	//If space isn't infinite
	if(MaxSize != 0 && MaxItems != 0)
	{
		//Check there is space left
		if(GetNumItems() == GetMaxItems() && GetCurrentSize() + Item->GetSize() > GetMaxSize())
		{
			return false;
		}
		else
		{
			//Add item
			Items.push_back(Item);

			//Add its size
			CurrentSize += Item->GetSize();

			return true;
		}
	}
	else
	{
		//Space is infinite, just add item
		Items.push_back(Item);

		//Add its size
		CurrentSize += Item->GetSize();
	}

	return true;
}

bool CInventory::RemoveItem(CInventoryItemBase *Item)
{
	//Check item exists
	for(std::vector<CInventoryItemBase*>::iterator i = Items.begin(); i != Items.end(); i++)
	{
		if((*i) == Item)
		{
			//Item found, remove its size
			CurrentSize -= (*i)->GetSize();

			//Delete it
			Items.erase(i);

			return true;
		}
	}

	//Item not found
	return false;
}

CInventoryItemBase* CInventory::GetItemByName(std::string Name)
{
	//Check item exists
	for(std::vector<CInventoryItemBase*>::iterator i = Items.begin(); i != Items.end(); i++)
	{
		if((*i)->GetName() == Name)
		{
			//Item found, return it
			return (*i);
		}
	}

	//Item not found
	return 0;
}

void CInventory::SetMaxItems(int Size)
{
	if(Size > 0) MaxItems = Size;
}

void CInventory::SetMaxSize(int Size)
{
	if(Size > 0) MaxSize = Size;
}

int CInventory::GetMaxItems()
{
	return MaxItems;
}

int CInventory::GetMaxSize()
{
	return MaxSize;
}

int CInventory::GetCurrentSize()
{
	return CurrentSize;
}

int CInventory::GetNumItems()
{
	return (int)Items.size();
}