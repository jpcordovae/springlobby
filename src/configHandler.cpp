#include "configHandler.h"
#include "frame_get_usync.h"
#include <iostream>
#include "se_settings.h"
//TODO Team-Decision: might not be necessary because of shared config
#ifndef RUNMODE_STANDALONE
	// if extra include for springlobby unitsync method needed
#endif
//TODO find suitable place, at least guard it
#define LOCK_UNITSYNC wxCriticalSectionLocker lock_criticalsection(m_lock)
#define ASSERT_LOGIC(cond,msg) if(!(cond)){std::cerr << (std::string("logic ")+msg);}
#define ASSERT_RUNTIME(cond,msg) if(!(cond)){std::cerr << (std::string("runtime ")+msg);}

ConfigHandler* ConfigHandler::instance=0;

/* mostly copied from springlobbysource*/
bool ConfigHandler::LoadUnitSyncLib( const wxString& springdir, const wxString& unitsyncloc )
{
	if ( is_loaded ) return true;

	wxSetWorkingDirectory( wxT(".") );

	// Load the library.
	std::string loc = OptionsHandler.getUsyncLoc();//getUsyncLoc();
	std::cout <<( "Loading from: " + loc + "\n");

	// Check if library exists
	if ( !wxFileName::FileExists( _S(loc)) ) {
		std::cerr <<( "File not found: "+ loc );
		return false;
	}
	
	try {
		m_libhandle = new wxDynamicLibrary(_S(loc));
		if (!m_libhandle->IsLoaded()) {
			std::cout <<("wxDynamicLibrary created, but not loaded!");
			delete m_libhandle;
			m_libhandle = 0;
			
		}
	} catch(...) {
		m_libhandle = 0;
	}

	if (m_libhandle == 0) {
		std::cout <<( "Couldn't load the unitsync library" );
		return false;
	}

	is_loaded = true;

	// Load all function from library.
	try {
		 m_init = (InitPtr)GetLibFuncPtr("Init");
		    m_uninit = (UnInitPtr)GetLibFuncPtr("UnInit");
		h_GetSpringConfigInt = (GetSpringConfigInt)GetLibFuncPtr("GetSpringConfigInt");
		h_GetSpringConfigString = (GetSpringConfigString)GetLibFuncPtr("GetSpringConfigString");
		h_GetSpringConfigInt = (GetSpringConfigInt)GetLibFuncPtr("GetSpringConfigInt");
		h_SetSpringConfigString = (SetSpringConfigString)GetLibFuncPtr("SetSpringConfigString");
		h_SetSpringConfigInt = (SetSpringConfigInt)GetLibFuncPtr("SetSpringConfigInt");

	}
	catch ( ... ) {
		std::cout <<( "Failed to load Unitsync lib." );
		FreeUnitSyncLib();
		return false;
	}

	return true;
}


void ConfigHandler::FreeUnitSyncLib()
{
	LOCK_UNITSYNC;
	  if ( !is_loaded ) return;
	  m_uninit();

	  delete m_libhandle;
	  m_libhandle = 0;

	  is_loaded = false;
}

void* ConfigHandler::GetLibFuncPtr( const std::string& name )
{
	ASSERT_LOGIC( is_loaded, "Unitsync not loaded" );
	//if(m_libhandle != 0){
		void* ptr = m_libhandle->GetSymbol(_S(name));
	ASSERT_RUNTIME( ptr, "Couldn't load " + name + " from unitsync library" );
		return ptr;
}

ConfigHandler& ConfigHandler::GetInstance(){
	if (!instance){
		instance = new ConfigHandler();
		//TODO something a little more sophisticated
		if(!instance->LoadUnitSyncLib(wxT(""),wxT("")))
		{
			wxMessageBox(wxT("Unitsync could not be loaded. CRAASH!"), wxT(""), wxOK, 0);
			exit(1);
		}
	}
	return *instance;
}

void ConfigHandler::SetInt(std::string name, int value){
	LOCK_UNITSYNC;
	h_SetSpringConfigInt(name.c_str(),value);
}


void ConfigHandler::SetString(std::string name, std::string value){
	LOCK_UNITSYNC;
	h_SetSpringConfigString(name.c_str(),value.c_str());
}


std::string ConfigHandler::GetString(std::string name, std::string def) {
	LOCK_UNITSYNC;
	return h_GetSpringConfigString(name.c_str(),def.c_str());
}

int ConfigHandler::GetInt(std::string name, int def){
	//ASSERT_LOGIC( is_loaded, "Unitsync not loaded \n" );
	LOCK_UNITSYNC;
	return h_GetSpringConfigInt(name.c_str(),def);
}

ConfigHandler::~ConfigHandler()
{
	Deallocate();
}

void ConfigHandler::Deallocate()
{
	if (instance)
		delete instance;
	instance=0;
}

	
