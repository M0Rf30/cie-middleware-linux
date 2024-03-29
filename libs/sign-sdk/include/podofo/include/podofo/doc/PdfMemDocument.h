/***************************************************************************
 *   Copyright (C) 2006 by Dominik Seichter                                *
 *   domseichter@web.de                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef _PDF_MEM_DOCUMENT_H_
#define _PDF_MEM_DOCUMENT_H_

#include "podofo/base/PdfDefines.h"
#include "podofo/base/PdfObject.h"

#include "PdfDocument.h"
#include "PdfFontCache.h"
#include <string>
#include <vector>


using namespace std;

namespace PoDoFo {

class PdfAcroForm;
class PdfDestination;
class PdfDictionary;
class PdfEncrypt;
class PdfFileSpec;
class PdfFont;
class PdfInfo;
class PdfNamesTree;
class PdfOutlines;
class PdfPage;
class PdfPagesTree;
class PdfParser;
class PdfRect;
class PdfWriter;

/** PdfMemDocument is the core class for reading and manipulating
 *  PDF files and writing them back to disk.
 *
 *  PdfMemDocument was designed to allow easy access to the object
 *  structur of a PDF file.
 *
 *  PdfMemDocument should be used whenever you want to change
 *  the object structure of a PDF file.
 *
 *  When you are only creating PDF files, please use PdfStreamedDocument
 *  which is usually faster for creating PDFs.
 *
 *  \see PdfDocument
 *  \see PdfStreamedDocument
 *  \see PdfParser
 *  \see PdfWriter
 */
class PODOFO_DOC_API PdfMemDocument : public PdfDocument {
    friend class PdfWriter;

 public:

    /** Construct a new (empty) PdfMemDocument
     */
    PdfMemDocument();
    
    /** Construct a PdfMemDocument from an existing PDF (on disk)
     *  \param pszFilename filename of the file which is going to be parsed/opened
     *
     *  This might throw a PdfError( ePdfError_InvalidPassword ) exception
     *  if a password is required to read this PDF.
     *  Call SetPassword with the correct password in this case.
     *  
     *  \see SetPassword
     */
    PdfMemDocument( const char* pszFilename );

#ifdef _WIN32
#if defined(_MSC_VER)  &&  _MSC_VER <= 1200			// nicht f�r Visualstudio 6
#else
    /** Construct a PdfMemDocument from an existing PDF (on disk)
     *  \param pszFilename filename of the file which is going to be parsed/opened
     *
     *  This might throw a PdfError( ePdfError_InvalidPassword ) exception
     *  if a password is required to read this PDF.
     *  Call SetPassword with the correct password in this case.
     *  
     *  This is an overloaded member function to allow working
     *  with unicode characters. On Unix systes you can also path
     *  UTF-8 to the const char* overload.
     *
     *  \see SetPassword
     */
    PdfMemDocument( const wchar_t* pszFilename );
#endif
#endif // _WIN32

    /** Close down/destruct the PdfMemDocument
     */
    virtual ~PdfMemDocument();

    /** Load a PdfMemDocument from a file
     *
     *  \param pszFilename filename of the file which is going to be parsed/opened
     *
     *  This might throw a PdfError( ePdfError_InvalidPassword ) exception
     *  if a password is required to read this PDF.
     *  Call SetPassword with the correct password in this case.
     *  
     *  \see SetPassword
     */
    void Load( const char* pszFilename );

#ifdef _WIN32
#if defined(_MSC_VER)  &&  _MSC_VER <= 1200			// nicht f�r Visualstudio 6
#else
    /** Load a PdfMemDocument from a file
     *
     *  \param pszFilename filename of the file which is going to be parsed/opened
     *
     *  This might throw a PdfError( ePdfError_InvalidPassword ) exception
     *  if a password is required to read this PDF.
     *  Call SetPassword with the correct password in this case.
     *  
     *  This is an overloaded member function to allow working
     *  with unicode characters. On Unix systes you can also path
     *  UTF-8 to the const char* overload.
     *
     *  \see SetPassword
     */
    void Load( const wchar_t* pszFilename );
#endif
#endif // _WIN32

    /** Load a PdfMemDocument from a buffer in memory
     *
     *  \param pBuffer a memory area containing the PDF data
     *  \param lLen length of the buffer
     *
     *  This might throw a PdfError( ePdfError_InvalidPassword ) exception
     *  if a password is required to read this PDF.
     *  Call SetPassword with the correct password in this case.
     *  
     *  \see SetPassword
     */
    void Load( const char* pBuffer, long lLen );

    /** Load a PdfMemDocument from a PdfRefCountedInputDevice
     *
     *  \param rDevice the input device containing the PDF
     *
     *  This might throw a PdfError( ePdfError_InvalidPassword ) exception
     *  if a password is required to read this PDF.
     *  Call SetPassword with the correct password in this case.
     *  
     *  \see SetPassword
     */
    void Load( const PdfRefCountedInputDevice & rDevice );

    /** Writes the complete document to a file
     *
     *  \param pszFilename filename of the document 
     *
     *  \see Write
     *
     *  This is an overloaded member function for your convinience.
     */
    void Write( const char* pszFilename );

#ifdef _WIN32
    /** Writes the complete document to a file
     *
     *  \param pszFilename filename of the document 
     *
     *  \see Write
     *
     *  This is an overloaded member function to allow working
     *  with unicode characters. On Unix systes you can also path
     *  UTF-8 to the const char* overload.
     *
     *  This is an overloaded member function for your convinience.
     */
    void Write( const wchar_t* pszFilename );
#endif // _WIN32

    /** Writes the complete document to an output device
     *
     *  \param pDevice write to this output device
     */
    void Write( PdfOutputDevice* pDevice );

    /** Set the write mode to use when writing the PDF.
     *  \param eWriteMode write mode
     */
    void SetWriteMode( EPdfWriteMode eWriteMode ) { m_eWriteMode = eWriteMode; }

    /** Get the write mode used for wirting the PDF
     *  \returns the write mode
     */
    virtual EPdfWriteMode GetWriteMode() const { return m_eWriteMode; }

    /** Set the PDF Version of the document. Has to be called before Write() to
     *  have an effect.
     *  \param eVersion  version of the pdf document
     */
    void SetPdfVersion( EPdfVersion eVersion ) { m_eVersion = eVersion;}

	void SetIncrementalUpdates(bool bIncrementalUpdates);
	
    /** Get the PDF version of the document
     *  \returns EPdfVersion version of the pdf document
     */
    EPdfVersion GetPdfVersion() const { return m_eVersion; }

    /** If you try to open an encrypted PDF file, which requires
     *  a password to open, PoDoFo will throw a PdfError( ePdfError_InvalidPassword ) 
     *  exception. 
     *  
     *  If you got such an exception, you have to set a password
     *  which should be used for opening the PDF.
     *
     *  The usual way will be to ask the user for the password
     *  and set the password using this method.
     *
     *  PdfParser will immediately continue to read the PDF file.
     *
     *  \param sPassword a user or owner password which can be used to open an encrypted PDF file
     *                   If the password is invalid, a PdfError( ePdfError_InvalidPassword ) exception is thrown!
     */
    void SetPassword( const std::string & sPassword );

    /** Encrypt the document during writing.
     *
     *  \param userPassword the user password (if empty the user does not have 
     *                      to enter a password to open the document)
     *  \param ownerPassword the owner password
     *  \param protection several EPdfPermissions values or'ed together to set 
     *                    the users permissions for this document
     *  \param eAlgorithm the revision of the encryption algorithm to be used
     *  \param eKeyLength the length of the encryption key ranging from 40 to 128 bits 
     *                    (only used if eAlgorithm == ePdfEncryptAlgorithm_RC4V2)
     *
     *  \see PdfEncrypt
     */
    void SetEncrypted( const std::string & userPassword,
                       const std::string & ownerPassword, 
                       int protection = PdfEncrypt::ePdfPermissions_Print | 
                                        PdfEncrypt::ePdfPermissions_Edit |
                                        PdfEncrypt::ePdfPermissions_Copy |
                                        PdfEncrypt::ePdfPermissions_EditNotes | 
                                        PdfEncrypt::ePdfPermissions_FillAndSign |
                                        PdfEncrypt::ePdfPermissions_Accessible |
                                        PdfEncrypt::ePdfPermissions_DocAssembly |
                                        PdfEncrypt::ePdfPermissions_HighPrint,
                       PdfEncrypt::EPdfEncryptAlgorithm eAlgorithm = PdfEncrypt::ePdfEncryptAlgorithm_RC4V1, 
                       PdfEncrypt::EPdfKeyLength eKeyLength = PdfEncrypt::ePdfKeyLength_40 );

    /** Encrypt the document during writing using a PdfEncrypt object
     *
     *  \param pEncrypt an encryption object that will be owned by PdfMemDocument
     */
    void SetEncrypted( const PdfEncrypt & pEncrypt );

    /** 
     * \returns true if this PdfMemDocument creates an encrypted PDF file
     */
    bool GetEncrypted() const { return (m_pEncrypt != NULL); }

    /** Returns wether this PDF document is linearized, aka
     *  weboptimized
     *  \returns true if the PDF document is linearized
     */
    bool IsLinearized() const { return m_bLinearized; }
    
    /** Get a reference to the sorted internal objects vector.
     *  \returns the internal objects vector.
     */
    const PdfVecObjects & GetObjects() const { return *(PdfDocument::GetObjects()); }

    /** Get a reference to the sorted internal objects vector.
     *  This is an overloaded function for your convinience.
     *  \returns the internal objects vector.
     */
    PdfVecObjects & GetObjects() { return *(PdfDocument::GetObjects()); }

    /** Get access to the internal Catalog dictionary
     *  or root object.
     *  
     *  \returns PdfObject the documents catalog or NULL 
     *                     if no catalog is available
     */
    PdfObject* GetCatalog()  { return PdfDocument::GetCatalog(); }

    /** Get access to the internal Catalog dictionary
     *  or root object.
     *  
     *  \returns PdfObject the documents catalog or NULL 
     *                     if no catalog is available
     */
    const PdfObject* GetCatalog() const { return PdfDocument::GetCatalog(); }

    /** Get the trailer dictionary
     *  which can be written unmodified to a pdf file.
     */
    const PdfObject* GetTrailer() const { return PdfDocument::GetTrailer(); }
    
    /** Get access to the StructTreeRoot dictionary
     *  \returns PdfObject the StructTreeRoot dictionary
     */
    PdfObject* GetStructTreeRoot() const { return GetNamedObjectFromCatalog( "StructTreeRoot" ); }

    /** Get access to the Metadata stream
     *  \returns PdfObject the Metadata stream (should be in XML, using XMP grammar)
     */
    PdfObject* GetMetadata() const { return GetNamedObjectFromCatalog( "Metadata" ); }

    /** Get access to the MarkInfo dictionary (ISO 32000-1:2008 14.7.1)
     *  \returns PdfObject the MarkInfo dictionary
     */
    PdfObject* GetMarkInfo() const { return GetNamedObjectFromCatalog( "MarkInfo" ); }

    /** Creates a PdfFont object from an existing font.
     *
     *  \param pObject a PdfObject that is a font

     *  \returns PdfFont* a pointer to a new PdfFont object.
     *           The returned object is owned by the PdfDocument.
     */
    PdfFont* GetFont( PdfObject* pObject );

    /** Copies one or more pages from another PdfMemDocument to this document
     *  \param rDoc the document to append
     *  \param inFirstPage the first page number to copy (0-based)
     *  \param inNumPages the number of pages to copy
     *  \returns this document
     */
    const PdfMemDocument & InsertPages( const PdfMemDocument & rDoc, int inFirstPage, int inNumPages );

    /** Deletes one or more pages from this document
     *  It does NOT remove any PdfObjects from memory - just the reference from the pages tree.
     *  If you want to delete resources of this page, you have to delete them yourself,
     *  but the resources might be used by other pages, too.
     *
     *  \param inFirstPage the first page number to delete (0-based)
     *  \param inNumPages the number of pages to delete
     *  \returns this document
     */
    void DeletePages( int inFirstPage, int inNumPages );

    /** Checks if printing this document is allowed.
     *  Every PDF consuming applications has to adhere this value!
     *
     *  \returns true if you are allowed to print this document
     *
     *  \see PdfEncrypt to set own document permissions.
     */
    inline virtual bool IsPrintAllowed() const; 

    /** Checks if modifiying this document (besides annotations, form fields or changing pages) is allowed.
     *  Every PDF consuming applications has to adhere this value!
     *
     *  \returns true if you are allowed to modfiy this document
     *
     *  \see PdfEncrypt to set own document permissions.
     */
    inline virtual bool IsEditAllowed() const;

    /** Checks if text and graphics extraction is allowed.
     *  Every PDF consuming applications has to adhere this value!
     *
     *  \returns true if you are allowed to extract text and graphics from this document
     *
     *  \see PdfEncrypt to set own document permissions.
     */
    inline virtual bool IsCopyAllowed() const;

    /** Checks if it is allowed to add or modify annotations or form fields
     *  Every PDF consuming applications has to adhere this value!
     *
     *  \returns true if you are allowed to add or modify annotations or form fields
     *
     *  \see PdfEncrypt to set own document permissions.
     */
    inline virtual bool IsEditNotesAllowed() const;

    /** Checks if it is allowed to fill in existing form or signature fields
     *  Every PDF consuming applications has to adhere this value!
     *
     *  \returns true if you are allowed to fill in existing form or signature fields
     *
     *  \see PdfEncrypt to set own document permissions.
     */
    inline virtual bool IsFillAndSignAllowed() const;

    /** Checks if it is allowed to extract text and graphics to support users with disabillities
     *  Every PDF consuming applications has to adhere this value!
     *
     *  \returns true if you are allowed to extract text and graphics to support users with disabillities
     *
     *  \see PdfEncrypt to set own document permissions.
     */
    inline virtual bool IsAccessibilityAllowed() const;

    /** Checks if it is allowed to insert, create, rotate, delete pages or add bookmarks
     *  Every PDF consuming applications has to adhere this value!
     *
     *  \returns true if you are allowed  to insert, create, rotate, delete pages or add bookmarks
     *
     *  \see PdfEncrypt to set own document permissions.
     */
    inline virtual bool IsDocAssemblyAllowed() const;

    /** Checks if it is allowed to print a high quality version of this document 
     *  Every PDF consuming applications has to adhere this value!
     *
     *  \returns true if you are allowed to print a high quality version of this document 
     *
     *  \see PdfEncrypt to set own document permissions.
     */
    inline virtual bool IsHighPrintAllowed() const;

    /** Tries to free all memory allocated by the given
     *  PdfObject (variables and streams) and reads
     *  it from disk again if it is requested another time.
     *
     *  This will only work if load on demand is used. Other-
     *  wise any call to this method will be ignored. Load on 
     *  demand is currently always enabled when using PdfMemDocument.
     *  If the object is dirty if will not be free'd.
     *
     *  \param rRef free all memory allocated by the object
     *              with this reference.
     *  \param bForce if true the object will be free'd
     *                even if IsDirty() returns true.
     *                So you will loose any changes made
     *                to this object.
     *
     *  This is an overloaded member for your convinience.
     */
    void FreeObjectMemory( const PdfReference & rRef, bool bForce = false );

    /** Tries to free all memory allocated by the given
     *  PdfObject (variables and streams) and reads
     *  it from disk again if it is requested another time.
     *
     *  This will only work if load on demand is used. Other-
     *  wise any call to this method will be ignored. Load on 
     *  demand is currently always enabled when using PdfMemDocument.
     *  If the object is dirty if will not be free'd.
     *
     *  \param pObj free object from memory
     *  \param bForce if true the object will be free'd
     *                even if IsDirty() returns true.
     *                So you will loose any changes made
     *                to this object.
     * 
     *  \see IsDirty
     */
    void FreeObjectMemory( PdfObject* pObj, bool bForce = false );

    /** 
     * \returns the parsers encryption object or NULL if the read PDF file was not encrypted
     */
    inline const PdfEncrypt* GetEncrypt() const;

 private:
	
	void GetUpdates(PdfVecObjects& updates);
	
    /** Get a dictioary from the catalog dictionary by its name.
     *  \param pszName will be converted into a PdfName
     *  \returns the dictionary if it was found or NULL
     */
    PdfObject* GetNamedObjectFromCatalog( const char* pszName ) const;

    /** Internal method to load all objects from a PdfParser object.
     *  The objects will be removed from the parser and are now
     *  owned by the PdfMemDocument.
     */
    void InitFromParser( PdfParser* pParser );

    /** Clear all internal variables
     */
    void Clear();

    /** Low level APIs for setting a viewer preference
     *  \param whichPrefs the dictionary key to set
     *  \param the object to be set
     */
    void SetViewerPreference( const PdfName& whichPref, const PdfObject & valueObj ) const;
    void SetViewerPreference( const PdfName& whichPref, bool inValue ) const;

 private:
    // Prevent use of copy constructor and assignment operator.  These methods
    // should never be referenced (given that code referencing them outside
    // PdfMemDocument won't compile), and calling them will result in a link error
    // as they're not defined.
    explicit PdfMemDocument(const PdfMemDocument&);
    PdfMemDocument& operator=(const PdfMemDocument&);

    bool            m_bLinearized;
    EPdfVersion     m_eVersion;

    PdfEncrypt*     m_pEncrypt;

    PdfParser*      m_pParser; ///< This will be temporarily initialized to a PdfParser object so that SetPassword can work
    EPdfWriteMode   m_eWriteMode;
	
	long m_nXRefOffset;
	
	bool m_bIncrementalUpdates;
	PdfObject* m_pOrgObjects[2000];
	vector<PdfObject*> m_orgObjectsVect;
	string m_orgBuffer;
};

// -----------------------------------------------------
// 
// -----------------------------------------------------
bool PdfMemDocument::IsPrintAllowed() const
{
    return m_pEncrypt ? m_pEncrypt->IsPrintAllowed() : true;
}

// -----------------------------------------------------
// 
// -----------------------------------------------------
bool PdfMemDocument::IsEditAllowed() const
{
    return m_pEncrypt ? m_pEncrypt->IsEditAllowed() : true;
}

// -----------------------------------------------------
// 
// -----------------------------------------------------
bool PdfMemDocument::IsCopyAllowed() const
{
    return m_pEncrypt ? m_pEncrypt->IsCopyAllowed() : true;
}

// -----------------------------------------------------
// 
// -----------------------------------------------------
bool PdfMemDocument::IsEditNotesAllowed() const
{
    return m_pEncrypt ? m_pEncrypt->IsEditNotesAllowed() : true;
}

// -----------------------------------------------------
// 
// -----------------------------------------------------
bool PdfMemDocument::IsFillAndSignAllowed() const
{
    return m_pEncrypt ? m_pEncrypt->IsFillAndSignAllowed() : true;
}

// -----------------------------------------------------
// 
// -----------------------------------------------------
bool PdfMemDocument::IsAccessibilityAllowed() const
{
    return m_pEncrypt ? m_pEncrypt->IsAccessibilityAllowed() : true;
}

// -----------------------------------------------------
// 
// -----------------------------------------------------
bool PdfMemDocument::IsDocAssemblyAllowed() const
{
    return m_pEncrypt ? m_pEncrypt->IsDocAssemblyAllowed() : true;
}

// -----------------------------------------------------
// 
// -----------------------------------------------------
bool PdfMemDocument::IsHighPrintAllowed() const
{
    return m_pEncrypt ? m_pEncrypt->IsHighPrintAllowed() : true;
}

// -----------------------------------------------------
// 
// -----------------------------------------------------
const PdfEncrypt* PdfMemDocument::GetEncrypt() const 
{ 
    return m_pEncrypt; 
}

};


#endif	// _PDF_MEM_DOCUMENT_H_
