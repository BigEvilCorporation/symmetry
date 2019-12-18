#pragma once

#include <vector>
#include <string>

class CInventoryItemBase
{
	public:
		CInventoryItemBase();
		~CInventoryItemBase();

		//Name
		void SetName(std::string ItemName);
		std::string GetName();

		//Item size
		void SetSize(int ItemSize);
		int GetSize();

	protected:
		//Name
		std::string Name;

		//Size
		int Size;
};

class CInventory
{
	public:
		CInventory();
		~CInventory();

		//Add item
		bool AddItem(CInventoryItemBase *Item);

		//Remove item
		bool RemoveItem(CInventoryItemBase *Item);

		//Get item by name
		CInventoryItemBase* GetItemByName(std::string Name);

		//Set max size (number of items, 0 for infinite)
		void SetMaxItems(int Size);

		//Set max size (size of items, 0 for infinite)
		void SetMaxSize(int Size);

		//Get current size
		int GetMaxItems();
		int GetMaxSize();
		int GetCurrentSize();
		int GetNumItems();

	private:
		//Vector of items
		std::vector<CInventoryItemBase*> Items;

		//Max size
		int MaxSize, MaxItems;

		//Current size
		int CurrentSize;
};
