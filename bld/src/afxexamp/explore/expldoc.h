#ifndef _EXPLDOC_H_INCLUDED
#define _EXPLDOC_H_INCLUDED

class CExploreDoc : public CDocument {
public:
    CStringList m_listTreeItems;
    CStringList m_listListItems;
    
    DECLARE_DYNCREATE( CExploreDoc )

public:
    CExploreDoc();

    virtual BOOL    OnNewDocument();
    virtual void    Serialize( CArchive &ar );
};

#endif /* _EXPLDOC_H_INCLUDED */
