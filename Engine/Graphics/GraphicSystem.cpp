/******************************************************************************/
/*!
\file GraphicSystem.cpp
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
*/
/******************************************************************************/
#include "GraphicSystem.h"

#include "Graphics/WinWrapper.h"

//#include "SOIL/SOIL.h"
#include "glew/glew.h"
#include "glew/wglew.h"

#include "Sprite/Sprite.h"

GraphicSystem* GraphicSystem::instance = nullptr;

bool GraphicSystem::InitializeRenderingEnvironment()
{
	//create rendering window
	HWND mainHWND = WinWrapper::GetHWND();
	m_windowDC = GetDC(mainHWND);


	DEVMODE devMode = { 0 };
	devMode.dmSize = sizeof(DEVMODE);
	BOOL b = EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &devMode);
	if (b == 0)
		return false;


	//drawing surface format
	PIXELFORMATDESCRIPTOR pfdesc;
	memset(&pfdesc, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfdesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfdesc.nVersion = 1;
	pfdesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_GENERIC_ACCELERATED | PFD_DOUBLEBUFFER;
	pfdesc.iPixelType = PFD_TYPE_RGBA;
	pfdesc.cColorBits = (BYTE)devMode.dmBitsPerPel;//32; //24 bit color for front and back buffer
	pfdesc.cDepthBits = 24;//24 bit depth buffer - not used in this demo
	pfdesc.cStencilBits = 8; //8 bit stencil buffer - not used in this demo

	int pf = ChoosePixelFormat(m_windowDC, &pfdesc);//checks if the graphics card can support the pixel format requested
	if (pf == 0)
	{
		ReleaseDC(mainHWND, m_windowDC);
		return false;
	}


	BOOL ok = SetPixelFormat(m_windowDC, pf, &pfdesc);
	if (!ok)
	{
		ReleaseDC(mainHWND, m_windowDC);
		return false;
	}


	//set the OpenGL context
	m_wglDC = wglCreateContext(m_windowDC);
	if (!m_wglDC)
	{
		ReleaseDC(mainHWND, m_windowDC);
		return false;
	}


	ok = wglMakeCurrent(m_windowDC, m_wglDC);
	if (!ok)
	{
		wglDeleteContext(m_wglDC);
		ReleaseDC(mainHWND, m_windowDC);
		return false;
	}

	return true;
}

void GraphicSystem::CleanRenderingEnvironment()
{
	HWND mainHWND = WinWrapper::GetHWND();
	glDeleteVertexArrays(1, &m_squareMesh->VAOref);
	glDeleteVertexArrays(1, &m_debugCircleMesh->VAOref);
	glDeleteVertexArrays(1, &m_debugSquareMesh->VAOref);

	glDeleteBuffers(1, &m_squareMesh->VBOref);
	glDeleteBuffers(1, &m_debugCircleMesh->VBOref);
	glDeleteBuffers(1, &m_debugSquareMesh->VBOref);

	glDeleteBuffers(1, &m_squareMesh->EBOref);
	glDeleteBuffers(1, &m_debugCircleMesh->EBOref);
	glDeleteBuffers(1, &m_debugSquareMesh->EBOref);

	if (m_wglDC)
	{
		if (!wglMakeCurrent(NULL, NULL))
		{
			//INF_CORE_ERROR("wglMakeCurrent failure");
		}
	}
	if (!wglDeleteContext(m_wglDC))
	{
		//INF_CORE_ERROR("wglDeleteContext failure");
	}

	m_wglDC = NULL; // causing a crash on exit for some reason

	if (m_windowDC && !ReleaseDC(mainHWND, m_windowDC))
	{
		m_windowDC = NULL;
	}

}

void GraphicSystem::SwapBuffers()
{
	::SwapBuffers(m_windowDC); //using double buffering
}


GraphicSystem::GraphicSystem():
	m_windowDC { nullptr },
	m_wglDC { nullptr },
	worldWidth { 1280 },
	worldHeight { 720 },
	worldWidthMod { 1.f / worldWidth },
	worldHeightMod { 1.f / worldHeight },
	m_squareMesh { std::make_shared<Mesh>() },
	m_debugSquareMesh { std::make_shared<Mesh>() },
	m_debugCircleMesh{ std::make_shared<Mesh>() },
	m_coneMesh{ std::make_shared<Mesh>() },
  usingDefaultCamera{ true }
{
	InitializeRenderingEnvironment();
	//InitVerticesData_Programmable(m_totalNumberOfVertices, m_totalNumberOfTriangles, 
	//	m_test.VAO.m_elementArray,
	//	m_test.IA);

	InitVerticesData();

	GLint res = glewInit();
	if (res != GLEW_OK)
	{
		//Log it
		std::string error = "Failed to init GraphicSystem Error Code: ";
		error.append(std::to_string(res));
    //INF_CORE_ERROR(error);
		return;
	}
  

  //wglSwapIntervalEXT(enableVsync);
  ShaderMan.InitBasicShaders();


	glGenVertexArrays(1, &m_squareMesh->VAOref);
	glGenBuffers(1, &m_squareMesh->VBOref);
	glGenBuffers(1, &m_squareMesh->EBOref);

	glGenVertexArrays(1, &m_debugSquareMesh->VAOref);
	glGenBuffers(1, &m_debugSquareMesh->VBOref);
	glGenBuffers(1, &m_debugSquareMesh->EBOref);

	glGenVertexArrays(1, &m_debugCircleMesh->VAOref);
	glGenBuffers(1, &m_debugCircleMesh->VBOref);
	glGenBuffers(1, &m_debugCircleMesh->EBOref);

	glGenVertexArrays(1, &m_coneMesh->VAOref);
	glGenBuffers(1, &m_coneMesh->VBOref);
	glGenBuffers(1, &m_coneMesh->EBOref);

	BindBuffers(*m_squareMesh);
	BindBuffers(*m_debugSquareMesh);
	BindBuffers(*m_debugCircleMesh);
	BindBuffers(*m_coneMesh);

	//TexMan.LoadAllTexturesFromFile();

  auto prog = std::make_shared<ShaderProgram>();
  prog->Setup();
	FBOShader.SetProgram(prog);

}

size_t constexpr indiceCalc(size_t vertexNum, size_t numElePerVertex, size_t eleNum)
{
	return vertexNum * numElePerVertex + eleNum;
}


void GraphicSystem::InitVerticesData()
{
	m_squareMesh->VAO.m_elementPerVertex = 9;
	//P0
	m_squareMesh->VAO.PushVertex(
		{ -0.5f, -0.5f, 0.5f }, // position
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // color
		{ 0.0f, 0.0f, 0.0f },// uv
		{-1, -1, 1});// norm

	//P1
	m_squareMesh->VAO.PushVertex(
		{ 0.5f, -0.5f, 0.5f }, // position
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // color
		{ 1.0f, 0.0f, 0.0f },// uv
		{ 1, -1, 1 });// norm

	//P2					
	m_squareMesh->VAO.PushVertex(
		{ 0.5f,  0.5f, 0.5f }, // position
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // color
		{ 1.0f, 1.0f, 0.0f },// uv
		{ 1, 1, 1 });// norm

	//P3
	m_squareMesh->VAO.PushVertex(
		{ -0.5f,  0.5f, 0.5f }, // position
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // color
		{ 0.0f, 1.0f, 0.0f },// uv
		{ -1, 1, 1 });// norm

	m_squareMesh->VAO.PushVertex(
		{ -0.5f, -0.5f, -0.5f }, // position
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // color
		{ 0.0f, 1.0f, 0.0f },// uv
		{ -1, -1, -1 });// norm 

	m_squareMesh->VAO.PushVertex(
		{ 0.5f, -0.5f, -0.5f }, // position
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // color
		{ 0.0f, 1.0f, 0.0f },// uv
		{ 1, -1, -1 });// norm 

	m_squareMesh->VAO.PushVertex(
		{ 0.5f, 0.5f, -0.5f }, // position
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // color
		{ 0.0f, 1.0f, 0.0f },// uv
		{ 1, 1, -1 });// norm 

	m_squareMesh->VAO.PushVertex(
		{ -0.5f, 0.5f, -0.5f }, // position
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // color
		{ 0.0f, 1.0f, 0.0f },// uv
		{ -1, 1, -1 });// norm 


	*m_debugSquareMesh = *m_squareMesh;
	{
		// front
		m_squareMesh->IA.push_back(0);
		m_squareMesh->IA.push_back(1);
		m_squareMesh->IA.push_back(2);

		m_squareMesh->IA.push_back(2);
		m_squareMesh->IA.push_back(3);
		m_squareMesh->IA.push_back(0);

		// right
		m_squareMesh->IA.push_back(1);
		m_squareMesh->IA.push_back(5);
		m_squareMesh->IA.push_back(6);

		m_squareMesh->IA.push_back(6);
		m_squareMesh->IA.push_back(2);
		m_squareMesh->IA.push_back(1);

		// back
		m_squareMesh->IA.push_back(7);
		m_squareMesh->IA.push_back(6);
		m_squareMesh->IA.push_back(5);

		m_squareMesh->IA.push_back(5);
		m_squareMesh->IA.push_back(4);
		m_squareMesh->IA.push_back(7);

		// left
		m_squareMesh->IA.push_back(4);
		m_squareMesh->IA.push_back(0);
		m_squareMesh->IA.push_back(3);

		m_squareMesh->IA.push_back(3);
		m_squareMesh->IA.push_back(7);
		m_squareMesh->IA.push_back(4);

		// bottom
		m_squareMesh->IA.push_back(4);
		m_squareMesh->IA.push_back(5);
		m_squareMesh->IA.push_back(1);

		m_squareMesh->IA.push_back(1);
		m_squareMesh->IA.push_back(0);
		m_squareMesh->IA.push_back(4);

		// top
		m_squareMesh->IA.push_back(3);
		m_squareMesh->IA.push_back(2);
		m_squareMesh->IA.push_back(6);

		m_squareMesh->IA.push_back(6);
		m_squareMesh->IA.push_back(7);
		m_squareMesh->IA.push_back(3);
	}
	m_squareMesh->drawMode = GL_TRIANGLES;

	m_debugSquareMesh->IA.push_back(0);
	m_debugSquareMesh->IA.push_back(1);
	m_debugSquareMesh->IA.push_back(2);
	m_debugSquareMesh->IA.push_back(3);
	m_debugSquareMesh->IA.push_back(0);

	m_debugSquareMesh->IA.push_back(4);
	m_debugSquareMesh->IA.push_back(7);
	m_debugSquareMesh->IA.push_back(3);

	m_debugSquareMesh->IA.push_back(2);
	m_debugSquareMesh->IA.push_back(6);
	m_debugSquareMesh->IA.push_back(7);


	m_debugSquareMesh->IA.push_back(4);
	m_debugSquareMesh->IA.push_back(5);
	m_debugSquareMesh->IA.push_back(6);


	m_debugSquareMesh->IA.push_back(5);
	m_debugSquareMesh->IA.push_back(1);


	m_debugSquareMesh->drawMode = GL_LINE_STRIP;

	for (int i = 0; i < 20; i++)
	{
		float theta = i * 6.28f * 0.05f;
		m_debugCircleMesh->VAO.PushVertex(
			{ cosf(theta) * 1.f, sinf(theta) * 1.f, 1.0f }, // position
			{ 1.0f, 1.0f, 1.0f, 1.0f }, // color
			{ 1.0f, 1.0f, 0.0f },// uv
			{ 0, 0, 1 });// norm
		m_debugCircleMesh->IA.push_back(i);
	}
	m_debugCircleMesh->IA.push_back(0);
	m_debugCircleMesh->drawMode = GL_LINE_STRIP;


	m_coneMesh->VAO.PushVertex(
		{ 0.5f, 0, 0.0f }, // position
		{ 1.0f, 1.0f, 1.0f, 1.0f }, // color
		{ 0.0f, 0.0f, 0.0f },// uv
		{ 1, 0, 0 });// norm
	//m_coneMesh->IA.push_back(0);

	for (int i = 1; i <= 20; i++)
	{
		float theta = i * 6.28f * 0.05f; // * 0.05 = / 20
		m_coneMesh->VAO.PushVertex(
			{ -0.5f, cosf(theta) * 0.25f, sinf(theta) * 0.25f}, // position
			{ 1.0f, 1.0f, 1.0f, 1.0f }, // color
			{ 1.0f, 1.0f, 0.0f },// uv
			{ 0, cosf(theta), sinf(theta) });// norm
		//*
		m_coneMesh->IA.push_back(i);
		m_coneMesh->IA.push_back((i) % 20 + 1); // i - 1(to remove the central element) + 1 ( get next ele) % 20 + 1 (put back central ele)
		m_coneMesh->IA.push_back(0);
		//*/
	}
	m_coneMesh->drawMode = GL_TRIANGLES;
}

GraphicSystem::~GraphicSystem()
{
}

void GraphicSystem::SetWorldWidth(unsigned int width)
{
	worldWidth = width;
	worldWidthMod = 1.f / width;
}

void GraphicSystem::SetWorldHeight(unsigned int height)
{
	worldHeight = height;
	worldHeightMod = 1.f / height;
}

glm::mat4 GraphicSystem::GetWorldTrans()
{
	glm::mat4 mat;
	for (int i = 0; i < 4; ++i)
	{
		mat[i][i] = 1;
	}
	mat[0][0] = 2 * instance->worldWidthMod;
	mat[1][1] = 2 * instance->worldHeightMod;
	return mat;
}

GLuint GraphicSystem::GetVAO() const
{
	return m_squareMesh->VAOref;
}


void GraphicSystem::BindBuffers(const Mesh& mesh)
{
	//can be defined elsewhere
	GLint numberOfElementsPerPosition = 3;
	GLint numberOfElementsPerColor = 4;
	GLint num_Elements_UV = 2;

	glBindVertexArray(mesh.VAOref);

  //VERTICES
  //OpenGL allows to bind to different buffers at the same time as long as they have a different buffer types
  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBOref);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.VAO.GetNumberVertices() * m_numberOfElementsPerVertex,
    mesh.VAO.GetVertexArray(), GL_STATIC_DRAW);

	//INDICES
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBOref);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh.IA.size(),
		mesh.IA.data(), GL_STATIC_DRAW);


	//only position and color
	glVertexAttribPointer(0, numberOfElementsPerPosition, GL_FLOAT, GL_FALSE,
		m_numberOfElementsPerVertex * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	GLint stride = numberOfElementsPerPosition;
	glVertexAttribPointer(1, numberOfElementsPerColor, GL_FLOAT, GL_FALSE,
		m_numberOfElementsPerVertex * sizeof(GLfloat), (GLvoid*)(stride * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//
	stride = numberOfElementsPerColor + numberOfElementsPerPosition;
	glVertexAttribPointer(2, num_Elements_UV, GL_FLOAT, GL_TRUE,
		m_numberOfElementsPerVertex * sizeof(GLfloat), (GLvoid*)(stride * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	/*
	stride += num_Elements_UV;
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_TRUE,
		m_numberOfElementsPerVertex * sizeof(GLfloat), (GLvoid*)(stride * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);
	*/
}

void GraphicSystem::Update()
{
	UpdateBegin();
	
	UpdateEnd();
}
void GraphicSystem::UpdateBegin()
{
	//update opengl
	//if (Input::GetInstance()->IsKeyDown(INF_SPACE))
	//{
	//	ShaderMan.ReloadShaders();
	//}
	//if (Input::GetInstance()->IsKeyDown(INF_B))
	//{
	//	LayerMan.shadowTex = !LayerMan.shadowTex;
	//}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);//RGBA
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glViewport((GLint)0, (GLint)0, worldWidth,
		worldHeight);
}

void GraphicSystem::UpdateEnd(unsigned int flags)
{
  if (flags & SWAPBUFFER)
  {
    SwapBuffers();
  }
}

void GraphicSystem::SetDebugDrawing()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void GraphicSystem::SetStandardDrawing()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


GraphicSystem * GraphicSystem::GetInstance()
{
  if (!instance)
  {
    instance = new GraphicSystem();
  }
	return instance;
}

void GraphicSystem::Exit()
{
	if (instance)
	{
		instance->CleanRenderingEnvironment();
		delete instance;
		instance = nullptr;
	}
}

void GraphicSystem::ToggleVSYNC()
{
  enableVsync = !enableVsync;
  wglSwapIntervalEXT(enableVsync);
}


void Mesh::Clear()
{
	VAO.Clear();
	IA.clear();
}
