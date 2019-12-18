#include "wxPointProperty.h"

WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP(wxVector3f,wxVectorProperty,wxVector3f())


class wxVectorPropertyClass : public wxPGPropertyWithChildren
{
    WX_PG_DECLARE_PROPERTY_CLASS()
public:

    wxVectorPropertyClass( const wxString& label,
                           const wxString& name,
                           const wxVector3f& value );
    virtual ~wxVectorPropertyClass ();

    WX_PG_DECLARE_PARENTAL_TYPE_METHODS()
    WX_PG_DECLARE_PARENTAL_METHODS()

protected:
    wxVector3f          m_value;
};


WX_PG_IMPLEMENT_PROPERTY_CLASS(wxVectorProperty,wxBaseParentProperty,
                               wxVector3f,const wxVector3f&,TextCtrl)


wxVectorPropertyClass::wxVectorPropertyClass( const wxString& label,
                                              const wxString& name,
                                              const wxVector3f& value)
    : wxPGPropertyWithChildren(label,name)
{
    wxPG_INIT_REQUIRED_TYPE(wxVector3f)
    DoSetValue((void*)&value);
    AddChild( wxFloatProperty(wxT("X"),wxPG_LABEL, value.x) );
    AddChild( wxFloatProperty(wxT("Y"),wxPG_LABEL, value.y) );
    AddChild( wxFloatProperty(wxT("Z"),wxPG_LABEL, value.z) );
}

wxVectorPropertyClass::~wxVectorPropertyClass() { }

void wxVectorPropertyClass::DoSetValue( wxPGVariant value )
{
    wxVector3f* pObj = (wxVector3f*)wxPGVariantToVoidPtr(value);
    m_value = *pObj;
    RefreshChildren();
}

wxPGVariant wxVectorPropertyClass::DoGetValue() const
{
    return wxPGVariant((void*)&m_value);
}

void wxVectorPropertyClass::RefreshChildren()
{
    if ( !GetCount() ) return;
    Item(0)->DoSetValue( m_value.x );
    Item(1)->DoSetValue( m_value.y );
    Item(2)->DoSetValue( m_value.z );
}

void wxVectorPropertyClass::ChildChanged( wxPGProperty* p )
{
    switch ( p->GetIndexInParent() )
    {
        case 0: m_value.x = p->DoGetValue().GetDouble(); break;
        case 1: m_value.y = p->DoGetValue().GetDouble(); break;
        case 2: m_value.z = p->DoGetValue().GetDouble(); break;
    }
}