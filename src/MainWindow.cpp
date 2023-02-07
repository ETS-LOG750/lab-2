#include "MainWindow.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>
#include <vector>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

MainWindow::MainWindow() :
	m_camera(m_windowWidth, m_windowHeight,
		glm::vec3(2.0,0.0,2.0),
		glm::vec3(0.0,0.0,0.0))
{
}

int MainWindow::Initialisation()
{
	// OpenGL version (usefull for imGUI and other libraries)
	const char* glsl_version = "#version 430 core";

	// glfw: initialize and configure
	// ------------------------------
	glfwInit();

	// Request OpenGL 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, "Labo 2", NULL, NULL);
	if (m_window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(m_window);
	InitializeCallback();

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return 2;
	}

	// imGui: create interface
	// ---------------------------------------
	// Setup Dear ImGui context
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;// Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Other openGL initialization
	// -----------------------------
	return InitializeGL();
}

void MainWindow::InitializeCallback() {
	glfwSetWindowUserPointer(m_window, reinterpret_cast<void*>(this));
	glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
		MainWindow* w = reinterpret_cast<MainWindow*>(glfwGetWindowUserPointer(window));
		w->FramebufferSizeCallback(width, height);
		});
	glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
		MainWindow* w = reinterpret_cast<MainWindow*>(glfwGetWindowUserPointer(window));
		w->MouseButtonCallback(button, action, mods);
		});
	
	glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos) {
		MainWindow* w = reinterpret_cast<MainWindow*>(glfwGetWindowUserPointer(window));
		w->CursorPositionCallback(xpos, ypos);
	});

}

int MainWindow::InitializeGL()
{
	const std::string directory = SHADERS_DIR;

	// Main shader loading
	bool mainShaderSuccess = true;
	m_mainShader = std::make_unique<ShaderProgram>();
	mainShaderSuccess &= m_mainShader->addShaderFromSource(GL_VERTEX_SHADER, directory + "basicShader.vert");
	mainShaderSuccess &= m_mainShader->addShaderFromSource(GL_FRAGMENT_SHADER, directory + "basicShader.frag");
	mainShaderSuccess &= m_mainShader->link();
	if (!mainShaderSuccess) {
		std::cerr << "Error when loading main shader\n";
		return 4;
	}

	// Create our VertexArrays Objects and VertexBuffer Objects
	glGenVertexArrays(NumVAOs, m_VAOs);
	glGenBuffers(NumBuffers, m_Buffers);
	initGeometrySphere();

	// Init GL properties
	glPointSize(10.0f);
	glEnable(GL_DEPTH_TEST);

	// Setup projection matrix (a bit hacky)
	FramebufferSizeCallback(m_windowWidth, m_windowHeight);

	return 0;
}

void MainWindow::RenderImgui()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//imgui 
	{
		ImGui::Begin("Labo 2");
		ImGui::Text("Rotations: ");
		// X
		bool rotationXN = ImGui::Button("X-"); 
		ImGui::SameLine(); 
		bool rotationXP = ImGui::Button("X+");
		// Y
		bool rotationYN = ImGui::Button("Y-"); 
		ImGui::SameLine(); 
		bool rotationYP = ImGui::Button("Y+");
		// Z
		bool rotationZN = ImGui::Button("Z-"); 
		ImGui::SameLine(); 
		bool rotationZP = ImGui::Button("Z+");
		ImGui::End();

		if(rotationXN) {
			std::cout << "Rotation -X\n";
		}
		if(rotationYN) {
			std::cout << "Rotation -Y\n";
		}
		if(rotationZN) {
			std::cout << "Rotation -Z\n";
		}
		if(rotationXP) {
			std::cout << "Rotation +X\n";
		}
		if(rotationYP) {
			std::cout << "Rotation +Y\n";
		}
		if(rotationZP) {
			std::cout << "Rotation +Z\n";
		}
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MainWindow::RenderScene(float delta_time)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind our vertex/fragment shaders
	glUseProgram(m_mainShader->programId());

	glm::mat4 modelMatrix = glm::mat4(1.0);
	glm::mat4 viewMatrix = m_camera.viewMatrix();
	glm::vec3 lightPosition = viewMatrix * glm::vec4(1.0);

	// ======
	// Send information to the shader
	glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
	// Note: optimized version of glm::transpose(glm::inverse(...))
	glm::mat3 normalMat = glm::inverseTranspose(glm::mat3(modelViewMatrix));
	m_mainShader->setMat4("mvMatrix", modelViewMatrix);
	m_mainShader->setMat4("projMatrix", m_camera.projectionMatrix());
	m_mainShader->setMat3("normalMatrix", normalMat);
	// Draw the sphere
	// Note: Because we are using an index buffer, we need to call glDrawElements instead
	// of glDrawArrays.
	glBindVertexArray(m_VAOs[Torus]);
	glDrawElements(GL_TRIANGLES, m_nbTri * 3, GL_UNSIGNED_INT, nullptr);
}


int MainWindow::RenderLoop()
{
	float time = glfwGetTime();
	while (!glfwWindowShouldClose(m_window))
	{
		// Compute delta time between two frames
		float new_time = glfwGetTime();
		const float delta_time = new_time - time;
		time = new_time;

		// Check inputs: Does ESC was pressed?
		if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(m_window, true);
		if(!m_imGuiActive) {
			m_camera.keybordEvents(m_window, delta_time);
		}

		RenderScene(delta_time);
		RenderImgui();

		// Show rendering and get events
		glfwSwapBuffers(m_window);
		m_imGuiActive = ImGui::IsAnyItemActive();
		glfwPollEvents();
	}

	// Cleanup
	glfwDestroyWindow(m_window);
	glfwTerminate();

	return 0;
}

void MainWindow::initGeometrySphere()
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	// Base positions
	const int count = 20;
	const float PI = 3.14159265358979323846f;
	const float offset = PI * 2.0 / count;
	for (int i = 0; i < count + 1; i++) {
		// Calcul des positions
		glm::vec3 p1bot = glm::vec3(
			0.7 * std::cos(offset * i), 
			-0.7, 
			0.7 *  std::sin(offset * i)
		);
		glm::vec3 p1top = glm::vec3(
			0.7 * std::cos(offset * i),
			0.7,
			0.7 * std::sin(offset * i)
		);
		positions.push_back(p1top);
		positions.push_back(p1bot);

		// Calcul des normales a chaque sommets
		glm::vec3 n1 = glm::normalize(p1bot - glm::vec3(0, -0.7, 0));
		normals.push_back(n1);
		normals.push_back(n1);
	}

	// Generation des indices
	std::vector<glm::uvec3> indices;
	for (int i = 0; i < count; i++) {
		int id = i * 2;
		indices.push_back(
			glm::uvec3(id, id + 1, id + 3)
		);
		indices.push_back(
			glm::uvec3(id + 2, id, id + 3)
		);
	}
	m_nbTri = indices.size();

	// VBO: position et normal
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[Position]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[Normal]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
	// EBO: indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[Indices]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::uvec3) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// Set VAO
	glBindVertexArray(m_VAOs[Torus]);
	// - positions
	int loc = m_mainShader->attributeLocation("vPosition");
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[Position]);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(loc);
	// - normals
	loc = m_mainShader->attributeLocation("vNormal");
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[Normal]);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_TRUE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(loc);
	// - indices
	// Note: besoin uniquement de le bind, les indices seront lu pendant le rendu
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[Indices]);
}

void MainWindow::FramebufferSizeCallback(int width, int height) {
	m_windowWidth = width;
	m_windowHeight = height;
	glViewport(0, 0, width, height);
	m_camera.viewportEvents(width, height);
}

void MainWindow::CursorPositionCallback(double xpos, double ypos) {
	if(!m_imGuiActive) {
		int state = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT);
		m_camera.mouseEvents(glm::vec2(xpos, ypos), state == GLFW_PRESS);
	}
}

void MainWindow::MouseButtonCallback(int button, int action, int mods)
{
	if(!m_imGuiActive) {
		// Debug code: please remove it
		std::cout << "Mouse callback: \n";
		std::cout << " - Left? " << (button == GLFW_MOUSE_BUTTON_LEFT ? "true" : "false") << "\n";
		std::cout << " - Pressed? " << (action == GLFW_PRESS ? "true" : "false") << "\n";
		std::cout << " - Shift? " << (mods == GLFW_MOD_SHIFT ? "true" : "false") << "\n";
	}
}