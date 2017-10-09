#include "ModuleFBXLoader.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleObjects.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment(lib, "Assimp/libx86/assimp.lib")

ModuleFBXLoader::ModuleFBXLoader(bool start_enabled): Module(start_enabled)
{
	name = "FBX Loader";
}

ModuleFBXLoader::~ModuleFBXLoader()
{
}

bool ModuleFBXLoader::Start()
{
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return true;
}

update_status ModuleFBXLoader::Update(float dt)
{
	//TODO -> place it in postUpdate method
	if (App->input->dropped)
	{
		LoadMesh(App->input->dropped_filedir);
		App->input->dropped = false;
	}

	//for (uint i = 0; i < meshes.size(); i++)
	//{
	//	meshes[i]->Draw();
	//}

	return UPDATE_CONTINUE;
}

update_status ModuleFBXLoader::postUpdate(float dt)
{
	if (App->input->dropped)
	{
		LoadMesh(App->input->dropped_filedir);
		App->input->dropped = false;
	}

	return UPDATE_CONTINUE;
}




bool ModuleFBXLoader::CleanUp()
{
	aiDetachAllLogStreams();
	return true;
}

BaseObject* ModuleFBXLoader::LoadMesh(const char* filename, char* texpath)
{
	bool ret = false;
	aiMesh* new_mesh = nullptr;
	const aiScene* scene = aiImportFile(filename, aiProcessPreset_TargetRealtime_MaxQuality);
	
	if (scene != nullptr && scene->HasMeshes())
	{
		//iterate all aiMesh structs
		for (uint i = 0; i < scene->mNumMeshes; i++)
		{
			m = new BaseObject(P_MESH, App->objects->wireframe_mode);
			new_mesh = scene->mMeshes[i];

			m->mesh.num_vertices = new_mesh->mNumVertices;
			m->mesh.vertices = new float[m->mesh.num_vertices * 3];
			memcpy(m->mesh.vertices, new_mesh->mVertices, sizeof(float) * m->mesh.num_vertices * 3);
		
			LOG("New mesh with %d vertices", m->mesh.num_vertices);
		
			if (new_mesh->HasFaces())
			{
				m->mesh.num_indices = new_mesh->mNumFaces * 3;
				m->mesh.indices = new uint[m->mesh.num_indices]; //Each face is a triangle
				
				for (uint j = 0; j < new_mesh->mNumFaces; j++)
				{
					if (new_mesh->mFaces[j].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else
					{
						memcpy(&m->mesh.indices[j * 3], new_mesh->mFaces[j].mIndices, 3 * sizeof(uint));
					}
				}

				if (new_mesh->HasTextureCoords(0))
				{
					m->mesh.tex_coords = new float[m->mesh.num_vertices * 2];
					for (uint i = 0; i< new_mesh->mNumVertices; i++)
					{
						m->mesh.tex_coords[i * 2] = new_mesh->mTextureCoords[0][i].x;
						m->mesh.tex_coords[i * 2+1] = new_mesh->mTextureCoords[0][i].y;
					}

					//Load the texture of the model
					if (texpath != nullptr)
					{
						App->textures->SetTexPath(texpath, m);
						m->mesh.id_image = App->textures->LoadTexture(m->mesh.tex_path);
					}
				}

				//Save space in VRAM and add the new mesh in the vector
				m->Init();
				m->id = App->objects->count++;
				App->objects->objects.push_back(m);
			}

			//Set the normals of the model
			if (new_mesh->HasNormals())
			{
				m->mesh.normals = new float[m->mesh.num_vertices * 3];
				memcpy(m->mesh.normals, new_mesh->mNormals, sizeof(float)*m->mesh.num_vertices * 3);
			}
		}
		aiReleaseImport(scene);
	}

	else
	{
		LOG("Error loading scene %s", filename);
	}

	return m;
}
