/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SRC_GUI_BASEDATAVIEWMODEL_H_
#define SRC_GUI_BASEDATAVIEWMODEL_H_

#include <wx/dataview.h>
#include <list>

#include <climits>

#define DEFAULT_COLUMN UINT_MAX


template <class DataType>
class BaseDataViewModel : public wxDataViewModel
{
public:
	BaseDataViewModel();
	virtual ~BaseDataViewModel();

public:
	//Overriden methods from wxDataViewModel
	virtual void GetValue(wxVariant& , const wxDataViewItem& , unsigned int ) const override = 0;
	virtual wxDataViewItem GetParent(const wxDataViewItem& ) const override;
	virtual unsigned int GetChildren(const wxDataViewItem& , wxDataViewItemArray& ) const override;
	virtual int Compare(const wxDataViewItem& , const wxDataViewItem& , unsigned int , bool ) const override;
	virtual bool IsListModel() const override;
	virtual bool GetAttr(const wxDataViewItem&, unsigned int, wxDataViewItemAttr&) const override;

public:
	//Custom methods
	bool AddItem(const DataType&);
	bool RemoveItem(const DataType&);
	bool ContainsItem(const DataType&) const;
	void Clear();
	void UpdateItem(const DataType&);
	int GetItemsCount() const;
	std::list<const DataType*>& GetItemsContainer();

public:
	//These methods from wxDataViewModel does not require to be overriden in derived classes
	unsigned int GetColumnCount() const override;
	bool IsContainer(const wxDataViewItem& ) const override;
	bool SetValue(const wxVariant& , const wxDataViewItem& , unsigned int ) override;
	wxString GetColumnType(unsigned int ) const override;

private:
	std::list<const DataType*> m_ModelData;
};

/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// Implementation ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

template <class DataType>
BaseDataViewModel<DataType>::BaseDataViewModel()
{
	// TODO Auto-generated constructor stub
}

template <class DataType>
BaseDataViewModel<DataType>::~BaseDataViewModel()
{
	// TODO Auto-generated destructor stub
}

template <class DataType>
int BaseDataViewModel<DataType>::Compare(const wxDataViewItem& item1, const wxDataViewItem& item2,
					 unsigned int column, bool ascending) const
{
	return wxDataViewModel::Compare(item1, item2, column, ascending);
}

template <class DataType>
wxDataViewItem BaseDataViewModel<DataType>::GetParent(const wxDataViewItem& /*item*/) const
{
	return wxDataViewItem(nullptr); //Root is only one possible parent
}

template <class DataType>
unsigned int BaseDataViewModel<DataType>::GetChildren(const wxDataViewItem& item,
						      wxDataViewItemArray& children) const
{
	if (item.IsOk() == true) { //Return items only for root!
		return 0;
	} else {

		for (auto dataItem : m_ModelData) {
			children.Add(wxDataViewItem(const_cast<DataType*>(dataItem)));
		}
		return children.GetCount();
	}
}

template <class DataType>
bool BaseDataViewModel<DataType>::AddItem(const DataType& data)
{
	m_ModelData.push_back(&data);

	//Inform model about new item
	ItemAdded(wxDataViewItem(nullptr), wxDataViewItem(const_cast<DataType*>(&data)));
	return true;
}

template <class DataType>
bool BaseDataViewModel<DataType>::RemoveItem(const DataType& data)
{
	m_ModelData.remove(&data);

	//Inform model about deleted item
	ItemDeleted(wxDataViewItem(nullptr), wxDataViewItem(const_cast<DataType*>(&data)));
	return true;
}

template <class DataType>
unsigned int BaseDataViewModel<DataType>::GetColumnCount() const
{
	return 0; //This value does not matters
}

template <class DataType>
bool BaseDataViewModel<DataType>::IsContainer(const wxDataViewItem& item) const
{
	if (item.IsOk() == true) { //Only root can containt items
		return false;
	} else {
		return true;
	}
}

template <class DataType>
bool BaseDataViewModel<DataType>::SetValue(const wxVariant& ,
					   const wxDataViewItem& , unsigned int )
{
	return true; //Dymmy method
}

//Needed to inform model that there will be no expanders (make view looks like list)
template <class DataType>
inline bool BaseDataViewModel<DataType>::IsListModel() const
{
	return true;
}

template <class DataType>
inline bool BaseDataViewModel<DataType>::ContainsItem(const DataType& checkedItem) const
{
	const DataType* checkItemPointer = &checkedItem;
	for (const DataType* item : m_ModelData) {
		if (item == checkItemPointer) {
			return true;
		}
	}
	return false;
}

template <class DataType>
inline void BaseDataViewModel<DataType>::Clear()
{
	m_ModelData.clear();
	Cleared();
}

template <class DataType>
inline void BaseDataViewModel<DataType>::UpdateItem(const DataType& item)
{
	if (ContainsItem(item) == true) {
		//FIXME: Maybe update item in m_ModelData. At this moment it stores pointer and does not need to be updated
		ItemChanged(wxDataViewItem(const_cast<DataType*>(&item)));
	} else {
		return;
	}
}

template<class DataType>
inline bool BaseDataViewModel<DataType>::GetAttr(const wxDataViewItem&,
		unsigned int, wxDataViewItemAttr&) const {
	return false;
}

template<class DataType>
inline std::list<const DataType*>& BaseDataViewModel<DataType>::GetItemsContainer() {
	return m_ModelData;
}

template <class DataType>
wxString BaseDataViewModel<DataType>::GetColumnType(unsigned int ) const
{
	return wxEmptyString; //Does nothing
}

template <class DataType>
int BaseDataViewModel<DataType>::GetItemsCount() const
{
	return static_cast<int>(m_ModelData.size());
}

#endif /* SRC_GUI_BASEDATAVIEWMODEL_H_ */
