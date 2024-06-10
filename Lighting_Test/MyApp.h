#pragma once

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform2.hpp>

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

// Utils
#include "GLUtils.hpp"
#include "Camera.h"

struct SUpdateInfo
{
	float ElapsedTimeInSec = 0.0f; // Program indulása óta eltelt idő
	float DeltaTimeInSec   = 0.0f; // Előző Update óta eltelt idő
};

class CMyApp
{
public:
	CMyApp();
	~CMyApp();
	glm::vec3 CMyApp::GetUV(float u, float v);
	bool Init();
	void Clean();

	void Update( const SUpdateInfo& );
	void Render();
	void RenderGUI();

	void KeyboardDown(const SDL_KeyboardEvent&);
	void KeyboardUp(const SDL_KeyboardEvent&);
	void MouseMove(const SDL_MouseMotionEvent&);
	void MouseDown(const SDL_MouseButtonEvent&);
	void MouseUp(const SDL_MouseButtonEvent&);
	void MouseWheel(const SDL_MouseWheelEvent&);
	void Resize(int, int);
protected:
	void SetupDebugCallback();

	//
	// Adat változók
	//
	GLuint  vaoID = 0; // vertex array object erőforrás azonosító
	GLuint  vboID = 0; // vertex buffer object erőforrás azonosító
	GLuint  iboID = 0; // index buffer object erőforrás azonosító
	GLsizei count = 0; // mennyi indexet/vertexet kell rajzolnunk
	float m_ElapsedTimeInSec = 0.0f;
	float alpha = 1.0f;

	// Table params

	static constexpr float     TABLE_SIZE = 5.0f;
	static constexpr glm::vec3 TABLE_POS = glm::vec3( 0.0f, 0.0f, 0.0f );

	// Suzanne params

	static constexpr glm::vec3 SUZANNE_POS = glm::vec3( 0,5.0f, 0 );

	// Sphere params

	static constexpr glm::vec3 SPHERE_POS = glm::vec3( 0, 5.0f, 0 );

	// Kamera
	Camera m_camera;

	// Fényforrások

	//static constexpr glm::vec3 BUG_COLOR = glm::vec3( 0.53f, 1.0f, 0.3f );

	//
	// OpenGL-es dolgok
	//
	
	// uniform location lekérdezése
	GLint ul( const char* uniformName ) noexcept;

	// shaderekhez szükséges változók
	GLuint m_programID = 0; // shaderek programja

	// Fényforrás- ...
	// glm::vec4 m_lightPos = glm::vec4( 0.0f, 1.0f, 0.0f, 0.0f );
	// 
	// glm::vec3 m_La = glm::vec3(0.0, 0.0, 0.0 );
	// glm::vec3 m_Ld = glm::vec3(1.0, 1.0, 1.0 );
	// glm::vec3 m_Ls = glm::vec3(1.0, 1.0, 1.0 );
	// 
	// float m_lightConstantAttenuation    = 1.0;
	// float m_lightLinearAttenuation      = 0.0;
	// float m_lightQuadraticAttenuation   = 0.0;
	// 
	// // ... és anyagjellemzők
	// glm::vec3 m_Ka = glm::vec3( 1.0 );
	// glm::vec3 m_Kd = glm::vec3( 1.0 );
	// glm::vec3 m_Ks = glm::vec3( 1.0 );
	// 
	// float m_Shininess = 1.0;

	// Shaderek inicializálása, és törtlése
	void InitShaders();
	void CleanShaders();

	// Geometriával kapcsolatos változók

	OGLObject m_quadGPU = {};
	OGLObject m_SuzanneGPU = {};
	OGLObject m_sphereGPU = {};

	// Geometria inicializálása, és törtlése
	void InitGeometry();
	void CleanGeometry();

	// Textúrázás, és változói

	GLuint m_woodTextureID = 0;
	GLuint m_SuzanneTextureID = 0;
	GLuint m_sphereTextureID = 0;

	glm::vec3 light_dir = glm::normalize(glm::vec3( - 1.0f, -1.0f, -1.0f));
	glm::vec3 light_col = { 1,1,1 };
	float shininess = 50;

	glm::vec3 attenuation = { 1,1,1 };

	void InitTextures();
	void CleanTextures();
};

