#ifndef _TEXTDOC_H_INCLUDED
#define _TEXTDOC_H_INCLUDED

class CTextDoc : public CDocument {
    DECLARE_DYNCREATE( CTextDoc )

public:
    CTextDoc();

    virtual void    Serialize( CArchive &ar );
};

#endif /* _TEXTDOC_H_INCLUDED */
