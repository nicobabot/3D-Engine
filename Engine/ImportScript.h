#ifndef _IMPORTSCRIPT_
#define _IMPORTSCRIPT_

// Info-> http://docs.go-mono.com/?link=root:/embed

#include "Globals.h"
#include <string>
#include <mono/jit/jit.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-gc.h>
#include <list>

class CSharpScript;
class ResourceScript;

class ImportScript
{
public:
	ImportScript();
	~ImportScript();

	bool InitScriptingSystem();

	bool Import(const char* file, uint uuid = 0);
	bool LoadResource(const char* file, ResourceScript* resourceScript);
	bool ReImportScript(std::string fileAssets, std::string uid_script, ResourceScript * resourceScript);
	MonoDomain* Load_domain();
	void Unload_domain();
	bool CreateNewScript(bool & active);
	int CompileScript(const char* file, std::string& libraryScript, const char* uid);
	CSharpScript* LoadScript_CSharp(std::string);
	CSharpScript* CreateCSharp(MonoImage* image);
	MonoClass* GetMonoClassFromImage(MonoImage* image, std::string& name_space, std::string& classname);

	MonoDomain* GetDomain() const;
	MonoImage* GetCulverinImage() const;
	std::string GetMonoPath() const;

	void SetCurrentScript(CSharpScript* current);

	bool IsNameUnique(std::string name) const;

private:
	void LinkFunctions();

	// FUNCTIONS ---------
	/* Debug - Console */
	static void ConsoleLog(MonoString* string);

	/* Input */
	static mono_bool KeyDown(MonoString* string);
	static mono_bool KeyUp(MonoString* string);
	static mono_bool KeyRepeat(MonoString* string);
	static mono_bool MouseButtonDown(int buttonmouse);
	static mono_bool MouseButtonUp(int buttonmouse);
	static mono_bool MouseButtonRepeat(int buttonmouse);

	/* Time */
	static float GetDeltaTime();

	/* GameObject */
	static MonoString* GetName(MonoObject* object);
	static void CreateGameObject(MonoObject* object);
	static MonoObject* GetComponent(MonoObject* object, MonoReflectionType* type);
	static MonoObject* GetPosition(MonoObject* object);
	static void SetPosition(MonoObject* vector3);

private:
	std::string nameNewScript;
	std::string mono_path;
	MonoDomain* domain = nullptr;
	MonoDomain* childDomain = nullptr;
	MonoImage* culverin_mono_image = nullptr;
	std::list<std::string> nameScripts;
	static CSharpScript* current;
};

#endif