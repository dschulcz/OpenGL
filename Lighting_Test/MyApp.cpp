#include "MyApp.h"
#include "SDL_GLDebugMessageCallback.h"
#include "ObjParser.h"

#include <imgui.h>

CMyApp::CMyApp()
{
}

CMyApp::~CMyApp()
{
}

void CMyApp::SetupDebugCallback()
{
	// engedélyezzük és állítsuk be a debug callback függvényt ha debug context-ben vagyunk 
	GLint context_flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
	if (context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		glDebugMessageCallback(SDL_GLDebugMessageCallback, nullptr);
	}
}

void CMyApp::InitShaders()
{
	m_programID = glCreateProgram();
	AssembleProgram( m_programID, "Vert_PosNormTex.vert", "Frag_LightingSkeleton.frag" );

}

void CMyApp::CleanShaders()
{
	glDeleteProgram( m_programID );
}

void CMyApp::InitGeometry()
{

	const std::initializer_list<VertexAttributeDescriptor> vertexAttribList =
	{
		{ 0, offsetof( Vertex, position ), 3, GL_FLOAT },
		{ 1, offsetof( Vertex, normal   ), 3, GL_FLOAT },
		{ 2, offsetof( Vertex, texcoord ), 2, GL_FLOAT },
	};

	// Quad 

	MeshObject<Vertex> quadMeshCPU;

	quadMeshCPU.vertexArray = 
	{
		{ glm::vec3( -1, -1, 0 ),glm::vec3( 0.0, 0.0, 1.0 ), glm::vec2( 0.0, 0.0 ) },
		{ glm::vec3(  1, -1, 0 ),glm::vec3( 0.0, 0.0, 1.0 ), glm::vec2( 1.0, 0.0 ) },
		{ glm::vec3( -1,  1, 0 ),glm::vec3( 0.0, 0.0, 1.0 ), glm::vec2( 0.0, 1.0 ) },
		{ glm::vec3(  1,  1, 0 ),glm::vec3( 0.0, 0.0, 1.0 ), glm::vec2( 1.0, 1.0 ) },
		{ glm::vec3(-1, -1, -2),glm::vec3(0.0, 0.0, 1.0), glm::vec2(2.0, 2.0) },
		{ glm::vec3(1, -1, -2),glm::vec3(0.0, 0.0, 1.0), glm::vec2(3.0, 2.0) },
		{ glm::vec3(-1,  1, -2),glm::vec3(0.0, 0.0, 1.0), glm::vec2(2.0, 3.0) },
		{ glm::vec3(1,  1, -2),glm::vec3(0.0, 0.0, 1.0), glm::vec2(3.0,3.0) }
	};

	quadMeshCPU.indexArray =
	{
		0, 1, 2,
		1, 3, 2,
		6, 5, 4,
		6,7,5, 
		2,3, 7,
		2,7,6,
		5,1,0	,
		4, 5, 0,
		3, 1, 5,
		3,5,7,	
		0,2,6,
		6,4,0

	};

	m_quadGPU = CreateGLObjectFromMesh( quadMeshCPU, vertexAttribList );

	// Suzanne

	MeshObject<Vertex> suzanneMeshCPU = ObjParser::parse("Assets/Suzanne.obj");

	m_SuzanneGPU = CreateGLObjectFromMesh( suzanneMeshCPU, vertexAttribList );

	// Sphere

	MeshObject<Vertex> sphereMeshCPU = ObjParser::parse("Assets/MarbleBall.obj");

	m_sphereGPU = CreateGLObjectFromMesh( sphereMeshCPU, vertexAttribList );
	
}

void CMyApp::CleanGeometry()
{
	CleanOGLObject( m_quadGPU );
	CleanOGLObject( m_SuzanneGPU );
	CleanOGLObject( m_sphereGPU );
}

void CMyApp::InitTextures()
{
	// diffuse texture
	glGenTextures( 1, &m_woodTextureID );
	TextureFromFile( m_woodTextureID, "Assets/Wood_Table_Texture.png" );
	SetupTextureSampling( GL_TEXTURE_2D, m_woodTextureID );

	glGenTextures( 1, &m_SuzanneTextureID );
	TextureFromFile( m_SuzanneTextureID, "Assets/wood.jpg" );
	SetupTextureSampling( GL_TEXTURE_2D, m_SuzanneTextureID );

	glGenTextures( 1, &m_sphereTextureID );
	TextureFromFile( m_sphereTextureID, "Assets/MarbleBall.png" );
	SetupTextureSampling( GL_TEXTURE_2D, m_sphereTextureID );
}

void CMyApp::CleanTextures()
{
	glDeleteTextures( 1, &m_woodTextureID );
	glDeleteTextures( 1, &m_SuzanneTextureID );
	glDeleteTextures( 1, &m_sphereTextureID );
}

bool CMyApp::Init()
{
	SetupDebugCallback();

	// törlési szín legyen kékes
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	InitShaders();
	InitGeometry();
	InitTextures();

	//
	// egyéb inicializálás
	//
	glEnable(GL_CULL_FACE); 

	glEnable(GL_BLEND);  // kapcsoljuk be a hátrafelé néző lapok eldobását
	glCullFace(GL_BACK);    // GL_BACK: a kamerától "elfelé" néző lapok, GL_FRONT: a kamera felé néző lapok
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST); // mélységi teszt bekapcsolása (takarás)

	// kamera
	m_camera.SetView(
		glm::vec3(0.0, 7.0, 7.0),// honnan nézzük a színteret	   - eye
		glm::vec3(0.0, 0.0, 0.0),   // a színtér melyik pontját nézzük - at
		glm::vec3(0.0, 1.0, 0.0));  // felfelé mutató irány a világban - up


	return true;
}

void CMyApp::Clean()
{
	CleanShaders();
	CleanGeometry();
	CleanTextures();
}

void CMyApp::Update( const SUpdateInfo& updateInfo )
{
	m_ElapsedTimeInSec = updateInfo.ElapsedTimeInSec;

	m_camera.Update( updateInfo.DeltaTimeInSec );
}

void CMyApp::Render()
{
	// töröljük a frampuffert (GL_COLOR_BUFFER_BIT)...
	// ... és a mélységi Z puffert (GL_DEPTH_BUFFER_BIT)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// quad

	// - VAO beállítása
	glBindVertexArray( m_quadGPU.vaoID );

	// - Textúrák beállítása, minden egységre külön
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, m_woodTextureID );

	glUseProgram( m_programID );

	glm::mat4 matWorld = glm::identity<glm::mat4>();

	matWorld = glm::translate( matWorld, TABLE_POS ) 
			 * glm::rotate( glm::half_pi<float>(), glm::vec3( -1.0f,0.0,0.0) )
		     * glm::scale( matWorld, glm::vec3( TABLE_SIZE ) );


	glUniformMatrix4fv( ul( "world" ),    1, GL_FALSE, glm::value_ptr( matWorld ) );
	glUniformMatrix4fv( ul( "worldIT" ),  1, GL_FALSE, glm::value_ptr( glm::transpose( glm::inverse( matWorld ) ) ) );
	glUniformMatrix4fv( ul( "viewProj" ), 1, GL_FALSE, glm::value_ptr( m_camera.GetViewProj() ) );

	glUniform3fv(ul("cameraPos"), 1, glm::value_ptr(m_camera.GetEye()));
	glUniform3fv(ul("light_dir"), 1, glm::value_ptr(light_dir));
	glUniform3fv(ul("Ld"), 1, glm::value_ptr(light_col));
	glUniform3fv(ul("Ls"), 1, glm::value_ptr(light_col));
	glUniform1f(ul("shininess"), shininess);

	glUniform3fv(ul("coeffs"), 1, glm::value_ptr(attenuation));

	glUniform1f(ul("alpha"), alpha);
	// - textúraegységek beállítása
	glUniform1i( ul( "texImage" ), 0 );

	glDrawElements( GL_TRIANGLES,    
					m_quadGPU.count,			 
					GL_UNSIGNED_INT,
					nullptr );

	// Suzanne

	glBindVertexArray( m_SuzanneGPU.vaoID );

	// - Textúrák beállítása, minden egységre külön
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, m_SuzanneTextureID );

	matWorld = glm::translate( SUZANNE_POS ) ;
	matWorld = glm::scale(matWorld, glm::vec3(3.0f, 3.0f, 3.0f));
	glUniformMatrix4fv( ul( "world" ),    1, GL_FALSE, glm::value_ptr( matWorld ) );
	glUniformMatrix4fv( ul( "worldIT" ),  1, GL_FALSE, glm::value_ptr( glm::transpose( glm::inverse( matWorld ) ) ) );

	glDrawElements( GL_TRIANGLES,    
					m_SuzanneGPU.count,			 
					GL_UNSIGNED_INT,
					nullptr );

	// sphere

	glBindVertexArray( m_sphereGPU.vaoID );

	// - Textúrák beállítása, minden egységre külön
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, m_sphereTextureID );
	alpha = 0.5f;
	glUniform1f(ul("alpha"), alpha);
	alpha = 1.0f;
	matWorld = glm::translate( SPHERE_POS );
	matWorld = glm::scale(matWorld, glm::vec3(5.0f, 5.0f, 5.0f));
	glUniformMatrix4fv( ul( "world" ),    1, GL_FALSE, glm::value_ptr( matWorld ) );
	glUniformMatrix4fv( ul( "worldIT" ),  1, GL_FALSE, glm::value_ptr( glm::transpose( glm::inverse( matWorld ) ) ) );

	glDrawElements( GL_TRIANGLES,    
					m_sphereGPU.count,			 
					GL_UNSIGNED_INT,
					nullptr );

	// shader kikapcsolasa
	glUseProgram( 0 );

	// - Textúrák kikapcsolása, minden egységre külön
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, 0 );

	// VAO kikapcsolása
	glBindVertexArray( 0 );
}

void CMyApp::RenderGUI()
{
	// ImGui::ShowDemoWindow();
	if (ImGui::Begin("Lights")) {
		ImGui::SliderFloat("shininess", &shininess, 1.0f, 100.0f);
		if (ImGui::DragFloat3("light direction", glm::value_ptr(light_dir), 0.05f))
			light_dir = glm::normalize(light_dir);
		ImGui::ColorEdit3("light color", glm::value_ptr(light_col));

		ImGui::DragFloat3("attenuation", glm::value_ptr(attenuation), 0.1f);
	}
	ImGui::End();
}

GLint CMyApp::ul( const char* uniformName ) noexcept
{
	GLuint programID = 0;

	// Kérdezzük le az aktuális programot!
	// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGet.xhtml
	glGetIntegerv( GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>( &programID ) );
	// A program és a uniform név ismeretében kérdezzük le a location-t!
	// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetUniformLocation.xhtml
	return glGetUniformLocation( programID, uniformName );
}

// https://wiki.libsdl.org/SDL2/SDL_KeyboardEvent
// https://wiki.libsdl.org/SDL2/SDL_Keysym
// https://wiki.libsdl.org/SDL2/SDL_Keycode
// https://wiki.libsdl.org/SDL2/SDL_Keymod

void CMyApp::KeyboardDown(const SDL_KeyboardEvent& key)
{	
	if ( key.repeat == 0 ) // Először lett megnyomva
	{
		if ( key.keysym.sym == SDLK_F5 && key.keysym.mod & KMOD_CTRL )
		{
			CleanShaders();
			InitShaders();
		}
		if ( key.keysym.sym == SDLK_F1 )
		{
			GLint polygonModeFrontAndBack[ 2 ] = {};
			// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGet.xhtml
			glGetIntegerv( GL_POLYGON_MODE, polygonModeFrontAndBack ); // Kérdezzük le a jelenlegi polygon módot! Külön adja a front és back módokat.
			GLenum polygonMode = ( polygonModeFrontAndBack[ 0 ] != GL_FILL ? GL_FILL : GL_LINE ); // Váltogassuk FILL és LINE között!
			// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glPolygonMode.xhtml
			glPolygonMode( GL_FRONT_AND_BACK, polygonMode ); // Állítsuk be az újat!
		}
	}
	m_camera.KeyboardDown( key );
}

void CMyApp::KeyboardUp(const SDL_KeyboardEvent& key)
{
	m_camera.KeyboardUp( key );
}

// https://wiki.libsdl.org/SDL2/SDL_MouseMotionEvent

void CMyApp::MouseMove(const SDL_MouseMotionEvent& mouse)
{
	m_camera.MouseMove( mouse );
}

// https://wiki.libsdl.org/SDL2/SDL_MouseButtonEvent

void CMyApp::MouseDown(const SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseUp(const SDL_MouseButtonEvent& mouse)
{
}

// https://wiki.libsdl.org/SDL2/SDL_MouseWheelEvent

void CMyApp::MouseWheel(const SDL_MouseWheelEvent& wheel)
{
	m_camera.MouseWheel( wheel );
}


// a két paraméterben az új ablakméret szélessége (_w) és magassága (_h) található
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);
	m_camera.Resize( _w, _h );
}

