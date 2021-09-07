/******************************************************************************/
/*!
\file GraphicSystem.h
\author Kennard Kee
\par Email: kennard.kee\@digipen.edu
\par DigiPen login: kennard.kee
\par Course: GAM200
\date 01/12/2019
\par        All content © 2019 DigiPen (SINGAPORE) Corporation,
            all rights reserved.
\brief
This holds the implementation for GraphicSystem class
GraphicSystem is needed for any graphics related APIs to work
it also holds managers to allow for easier access to graphics API around
the engine
This file also holds the declarations for various graphics related Managers
*/
/******************************************************************************/
#pragma once

// remove if we are switching to DLL
#define GLEW_STATIC 

#include <Windows.h>
#include <optional>
#include <map>
#include <memory>
#include <vector>
#include <future>
#include "Shaders/BasicShader.h"
#include "glew/glew.h"
#include "glm/glm/glm.hpp"
#include "Mesh/Mesh.h"
#include "Shaders/ShaderAttachment.h"
#include "Shaders/UniformBuffer.h"

#define SWAPBUFFER 1

#define DRAWFBO 2

#define USECAMERA 1


class GraphicSystem
{
public:

	enum struct E_RenderMode
	{
		DIRECT,
		FRAMEBUFFER
	};

private:
	GraphicSystem();
	~GraphicSystem();
	
	static GraphicSystem * instance;

	HDC m_windowDC;//global Window-device context
	HGLRC m_wglDC;//OpenGL-device context

	bool InitializeRenderingEnvironment();
	void CleanRenderingEnvironment();


	void SwapBuffers();

	unsigned int worldWidth;
	unsigned int worldHeight;
	float worldWidthMod;
	float worldHeightMod;


	void InitVerticesData();

	const GLint m_numberOfElementsPerVertex = { 9 }; //position = (3 floats)   +   4 color + 2 uv + 3 norm = (12 floats)

	BasicShader FBOShader;

	class ShaderManager
	{
	public:
		using ShaderName = std::string;
		using ShaderPtr = BasicShader::ShaderPtr;
    using ShaderProg = std::shared_ptr<ShaderProgram>;
		using Filename = std::string;
	private:
		std::map <ShaderName, ShaderPtr> m_shaderMap;
		std::map <ShaderPtr, ShaderName> m_reverseMap;

    std::map <UniformBuffer::UniformName, std::shared_ptr<UniformBuffer>> m_uniformBufferMap;

    size_t m_NextBindPoint = 0;
		ShaderManager() = default;
		~ShaderManager();
	public:
    void CreateUniformBuffer(UniformBuffer::UniformName name);

    //where respective systems should set uniform buffers
    void SetUniformBufferData(UniformBuffer::UniformName name, GLvoid* data, size_t size);
		std::string GetName(ShaderPtr shader);
		void InitBasicShaders();
		ShaderPtr GetShader(ShaderName name) const;

    ShaderPtr LoadShader(Filename vertexSource, Filename fragmentSource,
      ShaderName name, 
      std::vector<ShaderAttachment> attachmentList = std::vector<ShaderAttachment>{}); // load a shader from a file

		void LoadShaderNoRet(Filename vertexSource, Filename fragmentSource,
      ShaderName name, 
      std::vector<ShaderAttachment> attachmentList = std::vector<ShaderAttachment>{}); // load a shader from a file
		
    void RegisterShader(ShaderPtr shader, ShaderName name);
		void ReloadShaders();

		friend class GraphicSystem;
	}; 

  bool usingDefaultCamera;
  bool enableVsync = true;
public:

	using GSPtr = GraphicSystem *;
	static GraphicSystem * GetInstance();
	static void Exit();

  void ToggleVSYNC();

	std::shared_ptr<Mesh> m_squareMesh;
	std::shared_ptr<Mesh> m_coneMesh;

	std::shared_ptr<Mesh> m_debugSquareMesh;
	std::shared_ptr<Mesh> m_debugCircleMesh;

  //=======  type aliases  ========
	using ShaderName = ShaderManager::ShaderName;
  using ShaderPtr = ShaderManager::ShaderPtr;
	using Filename = std::string;

  //========  Managers  ========

	ShaderManager ShaderMan;

  bool m_enableDebugDraw = true;
	
	void SetWorldWidth(unsigned int width);
	void SetWorldHeight(unsigned int height);

	static glm::mat4 GetWorldTrans();

	GLuint GetVAO() const; // for legacy reasons

	void Update();

	void BindBuffers(const Mesh& mesh);

	void UpdateBegin();

	void UpdateEnd(unsigned int flags = SWAPBUFFER);

	void SetDebugDrawing();
	void SetStandardDrawing();
};

#define INF_GRAPHIC GraphicSystem::GetInstance()
#define INF_G_TEXMAN GraphicSystem::GetInstance()->TexMan
#define INF_G_LAYERMAN GraphicSystem::GetInstance()->LayerMan
#define INF_G_SHADERMAN GraphicSystem::GetInstance()->ShaderMan