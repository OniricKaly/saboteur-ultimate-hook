
#ifndef __CINIFILE_H_
#define __CINIFILE_H_

#ifdef _WIN32

#if defined(_MSC_VER) && (_MSC_VER >= 1200) && (_MSC_VER < 1300)
#pragma warning(disable: 4786)
#endif

#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <set>
#include <string>
#include <string.h>
#include <wchar.h>
#include <algorithm>

#define INI_TOKEN_A_ANSI "\a"	
#define INI_TOKEN_B_ANSI "\b"	
#define INI_EMPTY_ANSI "*"		

class CIniFileA
{
public:
    static const char* const LF;
public:
    CIniFileA();
    ~CIniFileA();

    
    bool Save( const std::string& fileName );

    
    void Save( std::ostream& output );

    
    bool Load( const std::string& fileName , bool bMerge = false );

    
    void Load( std::istream& input , bool bMerge = false );

public:
    class CIniMergeA
    {
    public:
        explicit CIniMergeA(CIniFileA& ini):_ini(ini) {}
        std::istream &operator()(std::istream& input) const
        {
            _ini.Load( input , true );
            return input;
        }
    private:
        CIniFileA& _ini;
    };
public:
#ifdef _WIN32
    
#if defined(_MSC_VER) && (_MSC_VER <= 1400)
    struct ci_less_a;
#endif
#endif
    class CIniSectionA
    {
        friend class CIniFileA; 
#ifdef _WIN32
    
#if defined(_MSC_VER) && (_MSC_VER <= 1400)
        friend struct ci_less_a;

#endif
#endif
    public:
#ifdef _WIN32
    
#if defined(_MSC_VER) && (_MSC_VER <= 1400)
        struct ci_less_a;
#endif
#endif
        class CIniKeyA
        {
            friend class CIniSectionA; 
#ifdef _WIN32
    
#if defined(_MSC_VER) && (_MSC_VER <= 1400)
            friend struct ci_less_a;
#endif
#endif
        private: 
            CIniKeyA( CIniSectionA* pSection , const std::string& sKeyName );
            CIniKeyA( const CIniKeyA& ); 
            CIniKeyA& operator=(const CIniKeyA&); 
            ~CIniKeyA( );
        public:
            
            void SetValue( const std::string& sValue );
            
            std::string GetValue() const;
            
            
            bool SetKeyName( std::string sKeyName );
            
            std::string GetKeyName() const;
        private:
            
            CIniSectionA* m_pSection;
            
            std::string m_sKeyName;
            
            std::string m_sValue;
        }; 
        
        struct ci_less_a
        {
            bool operator() (const CIniKeyA* s1, const CIniKeyA* s2) const
            {
#ifndef _WIN32
                return strcasecmp(s1->m_sKeyName.c_str(), s2->m_sKeyName.c_str()) < 0;
#else
                return _stricmp(s1->m_sKeyName.c_str(), s2->m_sKeyName.c_str()) < 0;
#endif
            }
        };

        typedef std::set<CIniKeyA*,ci_less_a> KeyIndexA;

#ifdef _WIN32
        
#if defined(_MSC_VER) && (_MSC_VER >= 1200) && (_MSC_VER < 1300)
        friend class CIniKeyA;
#endif
#endif
    private: 
        CIniSectionA( CIniFileA* pIniFile , const std::string& sSectionName );
        CIniSectionA( const CIniSectionA& ); 
        CIniSectionA& operator=(const CIniSectionA&); 
        ~CIniSectionA( );
    public:
        
        CIniKeyA* AddKey( std::string sKeyName );
        
        void RemoveKey( CIniKeyA* pKey );
        
        void RemoveKey( std::string sKey );
        
        void RemoveAllKeys( );
        
        CIniKeyA* GetKey( std::string sKeyName ) const;
        
        const KeyIndexA& GetKeys() const;
        
        std::string GetKeyValue( std::string sKey ) const;
        
        void SetKeyValue( std::string sKey, const std::string& sValue );
        
        
        bool SetSectionName( std::string sSectionName );
        
        std::string GetSectionName() const;
    private:
        KeyIndexA::const_iterator _find_key( const std::string& sKeyName ) const;
        KeyIndexA::iterator _find_key( const std::string& sKeyName );
    private:
        
        CIniFileA* m_pIniFile;
        
        std::string m_sSectionName;
        
        KeyIndexA m_keys;
    }; 
    
    struct ci_less_a
    {
        bool operator() (const CIniSectionA* s1, const CIniSectionA* s2) const
        {
#ifndef _WIN32
            return strcasecmp(s1->m_sSectionName.c_str(), s2->m_sSectionName.c_str()) < 0;
#else
            return _stricmp(s1->m_sSectionName.c_str(), s2->m_sSectionName.c_str()) < 0;
#endif
        }
    };

    typedef std::set<CIniSectionA*,ci_less_a> SecIndexA;

#ifdef _WIN32
    
#if defined(_MSC_VER) && (_MSC_VER >= 1200) && (_MSC_VER < 1300)
    friend class CIniSectionA;
#endif
#endif
public:
    
    CIniSectionA* AddSection( std::string sSection );
    
    void RemoveSection( CIniSectionA* pSection );
    
    void RemoveSection( std::string sSection );
    
    void RemoveAllSections( );
    
    CIniSectionA* GetSection( std::string sSection ) const;
    
    const SecIndexA& GetSections() const;
    
    std::string GetKeyValue( const std::string& sSection, const std::string& sKey ) const;
    
    void SetKeyValue( const std::string& sSection, const std::string& sKey, const std::string& sValue );
    
    bool RenameSection( const std::string& sSectionName  , const std::string& sNewSectionName );
    
    bool RenameKey( const std::string& sSectionName  , const std::string& sKeyName , const std::string& sNewKeyName);
private:
    SecIndexA::const_iterator _find_sec( const std::string& sSection ) const;
    SecIndexA::iterator _find_sec( const std::string& sSection );
private:
    CIniFileA( const CIniFileA&); 
    CIniFileA& operator=(const CIniFileA&); 
    
    SecIndexA m_sections;
}; 


typedef CIniFileA::CIniMergeA CIniMergeA;
typedef CIniFileA::CIniSectionA CIniSectionA;
typedef CIniSectionA::CIniKeyA CIniKeyA;


typedef CIniFileA* PCINIA;
typedef CIniKeyA* PCINIKEYA;
typedef CIniSectionA* PCINISECA;


typedef CIniSectionA::KeyIndexA KeyIndexA;
typedef CIniFileA::SecIndexA SecIndexA;

std::ostream& operator<<(std::ostream& output, CIniFileA& obj);
std::istream& operator>>(std::istream& input, CIniFileA& obj);
std::istream& operator>>(std::istream& input, CIniMergeA merger);



#define INI_TOKEN_A_UNICODE L"\a"	
#define INI_TOKEN_B_UNICODE L"\b"	    
#define INI_EMPTY_UNICODE L"*"		

class CIniFileW
{
public:
    static const wchar_t* const LF;
public:
    CIniFileW();
    ~CIniFileW();

    
    bool Save( const std::wstring& fileName );

    
    void Save( std::wostream& output );

    
    bool Load( const std::wstring& fileName , bool bMerge = false );

    
    void Load( std::wistream& input , bool bMerge = false );

public:
    class CIniMergeW
    {
    public:
        explicit CIniMergeW(CIniFileW& ini):_ini(ini) {}
        std::wistream &operator()(std::wistream& input) const
        {
            _ini.Load( input , true );
            return input;
        }
    private:
        CIniFileW& _ini;
    };
public:
#ifdef _WIN32
    
#if defined(_MSC_VER) && (_MSC_VER <= 1400)
    struct ci_less_w;
#endif
#endif
    class CIniSectionW
    {
        friend class CIniFileW; 
#ifdef _WIN32
    
#if defined(_MSC_VER) && (_MSC_VER <= 1400)
        friend struct ci_less_w;
#endif
#endif
    public:
#ifdef _WIN32
    
#if defined(_MSC_VER) && (_MSC_VER <= 1400)
        struct ci_less_w;
#endif
#endif
        class CIniKeyW
        {
            friend class CIniSectionW; 
#ifdef _WIN32
    
#if defined(_MSC_VER) && (_MSC_VER <= 1400)
            friend struct ci_less_w;
#endif
#endif
        private: 
            CIniKeyW( CIniSectionW* pSection , const std::wstring& sKeyName );
            CIniKeyW( const CIniKeyW& ); 
            CIniKeyW& operator=(const CIniKeyW&); 
            ~CIniKeyW( );
        public:
            
            void SetValue( const std::wstring& sValue );
            
            std::wstring GetValue() const;
            
            
            bool SetKeyName( std::wstring sKeyName );
            
            std::wstring GetKeyName() const;
        private:
            
            CIniSectionW* m_pSection;
            
            std::wstring m_sKeyName;
            
            std::wstring m_sValue;
        }; 
        
        struct ci_less_w
        {
            bool operator() (const CIniKeyW* s1, const CIniKeyW* s2) const
            {
#ifndef _WIN32
                return wcscasecmp(s1->m_sKeyName.c_str(), s2->m_sKeyName.c_str()) < 0;
#else
                return _wcsicmp(s1->m_sKeyName.c_str(), s2->m_sKeyName.c_str()) < 0;
#endif
            }
        };

        typedef std::set<CIniKeyW*,ci_less_w> KeyIndexW;

#ifdef _WIN32
        
#if defined(_MSC_VER) && (_MSC_VER >= 1200) && (_MSC_VER < 1300)
        friend class CIniKeyW;
#endif
#endif
    private: 
        CIniSectionW( CIniFileW* pIniFile , const std::wstring& sSectionName );
        CIniSectionW( const CIniSectionW& ); 
        CIniSectionW& operator=(const CIniSectionW&); 
        ~CIniSectionW( );
    public:
        
        CIniKeyW* AddKey( std::wstring sKeyName );
        
        void RemoveKey( CIniKeyW* pKey );
        
        void RemoveKey( std::wstring sKey );
        
        void RemoveAllKeys( );
        
        CIniKeyW* GetKey( std::wstring sKeyName ) const;
        
        const KeyIndexW& GetKeys() const;
        
        std::wstring GetKeyValue( std::wstring sKey ) const;
        
        void SetKeyValue( std::wstring sKey, const std::wstring& sValue );
        
        
        bool SetSectionName( std::wstring sSectionName );
        
        std::wstring GetSectionName() const;
    private:
        KeyIndexW::const_iterator _find_key( const std::wstring& sKeyName ) const;
        KeyIndexW::iterator _find_key( const std::wstring& sKeyName );
    private:
        
        CIniFileW* m_pIniFile;
        
        std::wstring m_sSectionName;
        
        KeyIndexW m_keys;
    }; 
    
    struct ci_less_w
    {
        bool operator() (const CIniSectionW* s1, const CIniSectionW* s2) const
        {
#ifndef _WIN32
            return wcscasecmp(s1->m_sSectionName.c_str(), s2->m_sSectionName.c_str()) < 0;
#else
            return _wcsicmp(s1->m_sSectionName.c_str(), s2->m_sSectionName.c_str()) < 0;
#endif
        }
    };

    typedef std::set<CIniSectionW*,ci_less_w> SecIndexW;

#ifdef _WIN32
    
#if defined(_MSC_VER) && (_MSC_VER >= 1200) && (_MSC_VER < 1300)
    friend class CIniSectionW;
#endif
#endif
public:
    
    CIniSectionW* AddSection( std::wstring sSection );
    
    void RemoveSection( CIniSectionW* pSection );
    
    void RemoveSection( std::wstring sSection );
    
    void RemoveAllSections( );
    
    CIniSectionW* GetSection( std::wstring sSection ) const;
    
    const SecIndexW& GetSections() const;
    
    std::wstring GetKeyValue( const std::wstring& sSection, const std::wstring& sKey ) const;
    
    void SetKeyValue( const std::wstring& sSection, const std::wstring& sKey, const std::wstring& sValue );
    
    bool RenameSection( const std::wstring& sSectionName  , const std::wstring& sNewSectionName );
    
    bool RenameKey( const std::wstring& sSectionName  , const std::wstring& sKeyName , const std::wstring& sNewKeyName);
private:
    SecIndexW::const_iterator _find_sec( const std::wstring& sSection ) const;
    SecIndexW::iterator _find_sec( const std::wstring& sSection );
private:
    CIniFileW( const CIniFileW&); 
    CIniFileW& operator=(const CIniFileW&); 
    
    SecIndexW m_sections;
}; 


typedef CIniFileW::CIniMergeW CIniMergeW;
typedef CIniFileW::CIniSectionW CIniSectionW;
typedef CIniSectionW::CIniKeyW CIniKeyW;


typedef CIniFileW* PCINIW;
typedef CIniKeyW* PCINIKEYW;
typedef CIniSectionW* PCINISECW;


typedef CIniSectionW::KeyIndexW KeyIndexW;
typedef CIniFileW::SecIndexW SecIndexW;

std::wostream& operator<<(std::wostream& output, CIniFileW& obj);
std::wistream& operator>>(std::wistream& input, CIniFileW& obj);
std::wistream& operator>>(std::wistream& input, CIniMergeW merger);


#ifdef _UNICODE
#define INI_TOKEN_A INI_TOKEN_UNICODE
#define INI_TOKEN_B INI_TOKEN_UNICODE
#define INI_EMPTY INI_EMPTY_UNICODE
typedef CIniMergeW CIniMerge;
typedef CIniFileW CIniFile;
typedef CIniSectionW CIniSection;
typedef CIniKeyW CIniKey;
typedef PCINIW PCINI;
typedef PCINIKEYW PCINIKEY;
typedef PCINISECW PCINISEC;
typedef KeyIndexW KeyIndex;
typedef SecIndexW SecIndex;
#else
#define INI_TOKEN_A INI_TOKEN_ANSI
#define INI_TOKEN_B INI_TOKEN_ANSI
#define INI_EMPTY INI_EMPTY_ANSI
typedef CIniMergeA CIniMerge;
typedef CIniFileA CIniFile;
typedef CIniSectionA CIniSection;
typedef CIniKeyA CIniKey;
typedef PCINIA PCINI;
typedef PCINIKEYA PCINIKEY;
typedef PCINISECA PCINISEC;
typedef KeyIndexA KeyIndex;
typedef SecIndexA SecIndex;
#endif

#endif

