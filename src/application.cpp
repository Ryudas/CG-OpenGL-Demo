//#include "Image.h"
#include "window.h"
// Always include window first (because it includes glfw, which includes GL which needs to be included AFTER glew).
// Can't wait for modules to fix this stuff...
#include "disable_all_warnings.h"
#include "gl/shader.h"
#include "gl/texture.h"
#include "gl/cube_texture.h"
#include "prospective_camera.h"
#include "util3D/basic_geometry.h"
#include "util3D/animated_geometry.h"
#include "util3D/scene.h"
#include "util3D/mesh.h"
#include "util3D/group.h"
#include "util3D/directional_light.h"
#include "materials/solid_color_material.h"
#include "materials/blinn_phong_material.h"
#include "materials/skybox_material.h"
#include "materials/chrome_material.h"
#include "materials/water_material.h"
#include "materials/ground_material.h"
#include "materials/toon_material.h"
DISABLE_WARNINGS_PUSH()
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/string_cast.hpp>
DISABLE_WARNINGS_POP()
#include <functional>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>


class Application {
private:
	int terrain_toggle = 0;
	Window m_window;
	// Shader for default rendering and for depth rendering
	enum mouse_status {MOUSE_DISABLED, MOUSE_REENABLED, MOUSE_ACTIVE};
	mouse_status mouse_movement = MOUSE_DISABLED;
	glm::dvec2 oldCPos;
	Scene scene;
	std::shared_ptr<ProspectiveCamera> camera;
	std::shared_ptr<Transformable> group;


	std::shared_ptr<AnimatedGeometry> skin_arachnid;
	std::shared_ptr<AnimatedGeometry> sea;
	std::shared_ptr<AnimatedGeometry> terrain;


	std::shared_ptr<Transformable> gymbal_outer[4];
	std::shared_ptr<Transformable> gymbal_mid[4];
	std::shared_ptr<Transformable> gymbal_inner[4];

	std::shared_ptr<Transformable> eve_group;


	std::shared_ptr<Transformable> _group, rotategroup;
public:
	Application()
			: m_window(glm::ivec2(1024, 1024), "Final Project", false),
			  oldCPos(0)
	{
		m_window.registerKeyCallback([this](int key, int scancode, int action, int mods) {
			if (action == GLFW_PRESS || action == GLFW_REPEAT)
				onKeyPressed(key, mods, action);
			else if (action == GLFW_RELEASE)
				onKeyReleased(key, mods);
		});
		m_window.registerMouseMoveCallback(std::bind(&Application::onMouseMove, this, std::placeholders::_1));
		m_window.registerMouseButtonCallback([this](int button, int action, int mods) {
			if (action == GLFW_PRESS)
				onMouseClicked(button, mods);
			else if (action == GLFW_RELEASE)
				onMouseReleased(button, mods);
		});

		std::cout << "size:" << sizeof(LightUniformData) << std::endl;
		std::cout << "offset:" << offsetof(LightUniformData, light_color) << std::endl;
		auto checkerboardtex = std::make_shared<Texture>("resources/textures/checkerboard.png");
		auto toontex = std::make_shared<Texture>("resources/textures/toon_map.png");
		auto toontex_1 = std::make_shared<Texture>("resources/textures/toon_map_blue.png");
		auto toontex_2 = std::make_shared<Texture>("resources/textures/toon_map_bw.png");
		auto toontex_3 = std::make_shared<Texture>("resources/textures/toon_map_carmine.png");
		auto toontex_4 = std::make_shared<Texture>("resources/textures/toon_map_db.png");
		auto toontex_5 = std::make_shared<Texture>("resources/textures/toon_map_grass.png");
		auto toontex_6 = std::make_shared<Texture>("resources/textures/toon_map_green.png");
		auto toontex_7 = std::make_shared<Texture>("resources/textures/toon_map_grun.png");
		auto toontex_8 = std::make_shared<Texture>("resources/textures/toon_map_lblue.png");
		auto toontex_9 = std::make_shared<Texture>("resources/textures/toon_map_red.png");
		auto toontex_10 = std::make_shared<Texture>("resources/textures/toon_map_sandy.png");
		auto toontex_11 = std::make_shared<Texture>("resources/textures/toon_map_sandy_b.png");
		auto toontex_12 = std::make_shared<Texture>("resources/textures/toon_map_sun.png");
		auto cube_tex = std::make_shared<CubeTexture>("resources/textures/skyboxes/skybox/");
		auto ground_tex = std::make_shared<Texture>("resources/textures/terrain_grad_g2.png");

		auto skybox_material = std::make_shared<SkyboxMaterial>(cube_tex, cube_tex);
		auto box_geometry = std::make_shared<BasicGeometry>("resources/skybox.obj");
		auto skybox_geometry = std::make_shared<BasicGeometry>("resources/skybox.obj");
		skybox_geometry->setVertexShader("shaders/skybox.vert.glsl");
		auto skybox = std::make_shared<Mesh>(skybox_geometry, skybox_material);

		std::shared_ptr<Geometry> dragon_geometry = std::make_shared<BasicGeometry>("resources/dragon.obj");


		// MATERIALS

		std::shared_ptr<Material> solid_material = std::make_shared<SolidColorMaterial>(glm::vec3(1.0f,0.0f,0.0f));

		std::shared_ptr<Material> blinn_phong_material = std::make_shared<BlinnPhongMaterial>(glm::vec3(0.5, 0.5, 0.5),
		                                                                                      10.0f,
		                                                                                      glm::vec3(0.8, 0.8, 0.8),
		                                                                                      checkerboardtex,
		                                                                                      toontex
		);


		std::shared_ptr<Material> blinn_phong_material_1 = std::make_shared<BlinnPhongMaterial>(glm::vec3(0.5, 0.5, 0.5),
			10.0f,
			glm::vec3(0.8, 0.8, 0.8),
			checkerboardtex,
			toontex_1
			);

		std::shared_ptr<Material> blinn_phong_material_2 = std::make_shared<BlinnPhongMaterial>(glm::vec3(0.5, 0.5, 0.5),
			10.0f,
			glm::vec3(0.8, 0.8, 0.8),
			checkerboardtex,
			toontex_2
			);

		std::shared_ptr<Material> blinn_phong_material_3 = std::make_shared<BlinnPhongMaterial>(glm::vec3(0.5, 0.5, 0.5),
			10.0f,
			glm::vec3(0.8, 0.8, 0.8),
			checkerboardtex,
			toontex_3
			);


		std::shared_ptr<Material> blinn_phong_material_4 = std::make_shared<BlinnPhongMaterial>(glm::vec3(0.5, 0.5, 0.5),
			10.0f,
			glm::vec3(0.8, 0.8, 0.8),
			checkerboardtex,
			toontex_4
			);

		std::shared_ptr<Material> blinn_phong_material_5 = std::make_shared<BlinnPhongMaterial>(glm::vec3(0.5, 0.5, 0.5),
			10.0f,
			glm::vec3(0.8, 0.8, 0.8),
			checkerboardtex,
			toontex_5
			);

		std::shared_ptr<Material> blinn_phong_material_6 = std::make_shared<BlinnPhongMaterial>(glm::vec3(0.5, 0.5, 0.5),
			10.0f,
			glm::vec3(0.8, 0.8, 0.8),
			checkerboardtex,
			toontex_6
			);

		std::shared_ptr<Material> blinn_phong_material_7 = std::make_shared<BlinnPhongMaterial>(glm::vec3(0.5, 0.5, 0.5),
			10.0f,
			glm::vec3(0.8, 0.8, 0.8),
			checkerboardtex,
			toontex_7
			);



		std::shared_ptr<Material> blinn_phong_material_8 = std::make_shared<BlinnPhongMaterial>(glm::vec3(0.5, 0.5, 0.5),
			10.0f,
			glm::vec3(0.8, 0.8, 0.8),
			checkerboardtex,
			toontex_8
			);

		std::shared_ptr<Material> blinn_phong_material_9 = std::make_shared<BlinnPhongMaterial>(glm::vec3(0.5, 0.5, 0.5),
			10.0f,
			glm::vec3(0.8, 0.8, 0.8),
			checkerboardtex,
			toontex_9
			);

		std::shared_ptr<Material> blinn_phong_material_10 = std::make_shared<BlinnPhongMaterial>(glm::vec3(0.5, 0.5, 0.5),
			10.0f,
			glm::vec3(0.8, 0.8, 0.8),
			checkerboardtex,
			toontex_10
			);

		std::shared_ptr<Material> toon_material_11 = std::make_shared<ToonMaterial>(glm::vec3(0.5, 0.5, 0.5),
			10.0f,
			glm::vec3(0.8, 0.8, 0.8),
			toontex_11,
			toontex
			);



		std::shared_ptr<Material> blinn_phong_material_12 = std::make_shared<BlinnPhongMaterial>(glm::vec3(0.5, 0.5, 0.5),
			10.0f,
			glm::vec3(0.8, 0.8, 0.8),
			checkerboardtex,
			toontex_12
			);





		std::shared_ptr<Material> arachnid_material = std::make_shared<BlinnPhongMaterial>(glm::vec3(1.0, 1.0, 1.0),
		                                                                                   479.818576f,
		                                                                                   glm::vec3(0.870858, 1.0, 0.988407),
		                                                                                   checkerboardtex,
		                                                                                   toontex
		);


		std::shared_ptr<Material> water_material = std::make_shared<WaterMaterial>(glm::vec3(0.976190, 0.976190, 0.976190),
		                                                                                900.0f,
		                                                                                glm::vec3(0.106332, 0.555170, 0.800000),
		                                                                                checkerboardtex,
		                                                                                toontex
		);
		auto ground_material = std::make_shared<GroundMaterial>(glm::vec3(0.05, 0.2, 0.0),
		                                                        5.0f,
		                                                        glm::vec3(0.106332, 0.555170, 0.020000),
		                                                        ground_tex, toontex);

		auto chrome_material = std::make_shared<ChromeMaterial>(cube_tex, toontex);

		auto eve_material = std::make_shared<ToonMaterial>(glm::vec3(0.5, 0.5, 0.5),
		                                                   10.0f,
		glm::vec3(0.8, 0.8, 0.8)
		,toontex_4, toontex);

		auto post_dof = std::make_shared<Shader>(VertexShader("shaders/postfx.vert.glsl"), FragmentShader("shaders/postfxDOF.frag.glsl"));
		

		// load eve

		std::shared_ptr<Mesh> eve_head = std::make_shared<Mesh>(
				std::make_shared<BasicGeometry>("resources/eve/eve_head.obj"),
				eve_material
		);

		std::shared_ptr<Mesh> eve_arms = std::make_shared<Mesh>(
				std::make_shared<BasicGeometry>("resources/eve/eve_arms.obj"),
				eve_material
		);

		std::shared_ptr<Mesh> eve_body = std::make_shared<Mesh>(
				std::make_shared<BasicGeometry>("resources/eve/eve_body.obj"),
				eve_material
		);


		// load temple

		std::shared_ptr<Mesh> temple = std::make_shared<Mesh>(
				std::make_shared<BasicGeometry>("resources/temple/temple.obj"),
				chrome_material
		);


		// load gymbal
		for(int i = 0; i < 4; i++)
		{
			gymbal_inner[i] = std::make_shared<Mesh>(
					std::make_shared<BasicGeometry>("resources/gymbal/gymbal_inner.obj"),
					chrome_material
			);

			gymbal_mid[i] = std::make_shared<Mesh>(
					std::make_shared<BasicGeometry>("resources/gymbal/gymbal_mid.obj"),
					chrome_material
			);

			gymbal_outer[i] = std::make_shared<Mesh>(
					std::make_shared<BasicGeometry>("resources/gymbal/gymbal_outer.obj"),
					chrome_material
			);
		}



		//Load skinned meshes

		skin_arachnid = std::make_shared<AnimatedGeometry>("resources/skin_arachnid");

		std::shared_ptr<Mesh> spidery_bub = std::make_shared<Mesh>(
				skin_arachnid,
				arachnid_material
		);


		sea = std::make_shared<AnimatedGeometry>("resources/skin_sea");

		std::shared_ptr<Mesh> sea_mesh = std::make_shared<Mesh>(
				sea,
				water_material
		);
		sea_mesh->scaling(glm::vec3(2, 1, 2));

		// load terrain


		terrain = std::make_shared<AnimatedGeometry>("resources/terrain");

		std::shared_ptr<Mesh> terrain_meshes = std::make_shared<Mesh>(
				terrain,
				ground_material
		);





		// load temple construction



		// load floaters

		std::shared_ptr<Mesh> floater_top = std::make_shared<Mesh>(
			std::make_shared<BasicGeometry>("resources/floaters/floater_top.obj"),
			blinn_phong_material
			);

		std::shared_ptr<Mesh> floater_mid = std::make_shared<Mesh>(
			std::make_shared<BasicGeometry>("resources/floaters/floater_mid.obj"),
			blinn_phong_material
			);

		std::shared_ptr<Mesh> floater_bottom = std::make_shared<Mesh>(
			std::make_shared<BasicGeometry>("resources/floaters/floater_bottom.obj"),
			blinn_phong_material
			);

		// SCENE SETUP

		// add terrain depending on toggle // this does not currently update (need to have scene remove or make insisible?


       
		scene.add(terrain_meshes);

		scene.add(sea_mesh);

		// Cameras and Lights

		camera = std::make_shared<ProspectiveCamera>();
		group = std::make_shared<Group>();
		auto campers = glm::perspective(glm::radians(80.0f), 1.0f, 0.1f, 500.0f);

		auto sun = std::make_shared<DirectionalLight>(campers, glm::vec3(1, 1, .8), glm::ivec2(1024, 1024));
		auto light2 = std::make_shared<DirectionalLight>(campers, glm::vec3(.5, .5, .5), glm::ivec2(1024, 1024));
		auto light = std::make_shared<DirectionalLight>(campers,glm::vec3(.5, .5, .5), glm::ivec2(1024, 1024));
		auto light3 = std::make_shared<DirectionalLight>(campers,glm::vec3(.5, .5, .5), glm::ivec2(1024, 1024));
		auto light4 = std::make_shared<DirectionalLight>(campers,glm::vec3(.5, .5, .5), glm::ivec2(1024, 1024));
		auto light5 = std::make_shared<DirectionalLight>(campers,glm::vec3(.5, .5, .5), glm::ivec2(1024, 1024));

		sun->translate(glm::vec3( 782.599548, 394.078827, 180.064880));
		sun->rotate(glm::vec3(-0.445000, 0.000000, -4.965050));
		scene.add(sun);
		light->translate(glm::vec3( -2.088730, 113.248100 ,2.967190));
		light->rotate(glm::vec3(-1.675000, 0.000000 ,-10.060158));
		scene.add(light);
		light2->translate(glm::vec3( -9.525551, 143.769882, 42.835293));
		light2->rotate(glm::vec3(-0.725001, 0.000000, -9.725164));
		scene.add(light2);
		light3->translate(glm::vec3(  34.916309, 136.132141, 6.175010));
		light3->rotate(glm::vec3(-0.605001, 0.000000, -7.895120));
		scene.add(light3);
		light4->translate(glm::vec3(  -9.821439, 133.884598, -35.387333));
		light4->rotate(glm::vec3(-0.530001, 0.000000, -6.095026));
		scene.add(light4);
		light5->translate(glm::vec3(   -42.900272, 135.240005, -0.399847));
		light5->rotate(glm::vec3(-0.690001, 0.000000 ,-4.745024));
		scene.add(light5);
		//camera -> add(light);
		spidery_bub->rotate(glm::vec3(0, 0, 1.5));
		spidery_bub->translate(glm::vec3(-1, -4, -4));
		camera ->add(spidery_bub);

		auto _subgroup = std::make_shared<Group>();
		//subgroup -> add(dragon);

		temple->translate(glm::vec3(0, 100, 0));

		gymbal_outer[0]->add(gymbal_mid[0]);
		gymbal_mid[0]->add(gymbal_inner[0]);
		gymbal_outer[0]->scaling(glm::vec3(1.f));
		gymbal_outer[0]->translate(glm::vec3(0, 190,0));

		gymbal_outer[1]->add(gymbal_mid[1]);
		gymbal_mid[1]->add(gymbal_inner[1]);
		gymbal_outer[1]->scaling(glm::vec3(2.f));
		gymbal_outer[1]->translate(glm::vec3(0, 70,0));
		group->add(gymbal_outer[1]);


		group->add(temple);
		group->add(gymbal_outer[0]);
		scene.add(group);


		// eve loading
		eve_group = std::make_shared<Group>();
		eve_group->add(eve_head);
		eve_group->add(eve_body);
		eve_group->add(eve_arms);

		eve_group->scaling(glm::vec3(4));
		eve_group->rotate(glm::vec3(0, -1.5, 1.5));
		eve_group->translate(glm::vec3(50, 0, 50));

		rotategroup = std::make_shared<Group>();
		rotategroup->add(eve_group);
		rotategroup->translate(glm::vec3(0, 20, 0));
		temple->add(rotategroup);


		camera->add(skybox);

		scene.add(camera);
		camera->addPostShader(post_dof);
		scene.update();
	}

	void update()
	{
		// This is your game loop
		// Put your real-time logic and rendering in here
		std::chrono::time_point expected_next_frame = std::chrono::system_clock::now();
		std::chrono::milliseconds update_interval(int(round(1000.0f/30.0f)));
		while (!m_window.shouldClose()) {
			m_window.updateInput();

			// animations
		for(int i = 0; i < 4; i++)
		{
			gymbal_inner[i]->rotate(glm::vec3(0.04, 0, 0));
			gymbal_mid[i]->rotate(glm::vec3(0, 0.01, 0));
			gymbal_outer[i]->rotate(glm::vec3(0.01, 0, 0));
		}
			rotategroup->rotate(glm::vec3(0, 0,0.1));

			scene.update();
			camera -> render();
			skin_arachnid -> updateFrame();


			sea->updateFrame();

			// Processes input and swaps the window buffer
			m_window.swapBuffers();
			std::chrono::milliseconds delta = std::chrono::duration_cast<std::chrono::milliseconds>(expected_next_frame - std::chrono::system_clock::now());
			if (delta > std::chrono::milliseconds(0)) {
				std::this_thread::sleep_for(delta);
				expected_next_frame += update_interval;
			} else {
				expected_next_frame = std::chrono::system_clock::now() + update_interval;
			}
		}
	}

	// In here you can handle key presses
	// key - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__keys.html
	// mods - Any modifier keys pressed, like shift or control
	void onKeyPressed(int key, int mods, int action)
	{
		switch (key) {
			case GLFW_KEY_ESCAPE:
				if(action == GLFW_PRESS)
				{
					mouse_movement = mouse_movement == MOUSE_DISABLED ? MOUSE_REENABLED : MOUSE_DISABLED;
					m_window.setMouseCapture();
				}
				break;
			case GLFW_KEY_W:
				camera -> translate(glm::orientate3(camera -> getRotation())*glm::vec3(0,0,-2));
				break;
			case GLFW_KEY_D:
				camera -> translate(glm::orientate3(camera -> getRotation())*glm::vec3(2,0,0));
				break;
			case GLFW_KEY_A:
				camera -> translate(glm::orientate3(camera -> getRotation())*glm::vec3(-2,0,0));
				break;
			case GLFW_KEY_S:
				camera -> translate(glm::orientate3(camera -> getRotation())*glm::vec3(0,0,2));
				break;
			case GLFW_KEY_Q:
				camera -> translate(glm::vec3(0,1,0));
				break;
			case GLFW_KEY_Z:
				camera -> translate(glm::vec3(0,-1,0));
				break;
			case GLFW_KEY_X:
				camera->toggleXRay();
				break;
			case GLFW_KEY_T:
				terrain->updateFrame();
				break;
			case GLFW_KEY_P:
				glm::vec3 pos = camera->getWorldPosition();
				printf("%f %f %f\n", pos.x, pos.y, pos.z);
				glm::vec3 rotation = camera->getRotation();
				printf("%f %f %f\n", rotation.x, rotation.y, rotation.z);
		}
		//std::cout << "Key pressed: " << key << std::endl;
	}

	// In here you can handle key releases
	// key - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__keys.html
	// mods - Any modifier keys pressed, like shift or control
	void onKeyReleased(int key, int mods)
	{
		//std::cout << "Key released: " << key << std::endl;
	}

	// If the mouse is moved this function will be called with the x, y screen-coordinates of the mouse
	void onMouseMove(const glm::dvec2& cursorPos)
	{
		if(mouse_movement == MOUSE_DISABLED) return;
		else if(mouse_movement == MOUSE_REENABLED)
		{
			mouse_movement = MOUSE_ACTIVE;
			oldCPos = cursorPos;
			return;
		}
		else
		{
			glm::dvec2 delta = cursorPos - oldCPos;
			oldCPos = cursorPos;
			camera -> mouseRotate(delta.x, delta.y);
		}
	}

	// If one of the mouse buttons is pressed this function will be called
	// button - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__buttons.html
	// mods - Any modifier buttons pressed
	void onMouseClicked(int button, int mods)
	{
		// std::cout << "Pressed mouse button: " << button << std::endl;
	}

	// If one of the mouse buttons is released this function will be called
	// button - Integer that corresponds to numbers in https://www.glfw.org/docs/latest/group__buttons.html
	// mods - Any modifier buttons pressed
	void onMouseReleased(int button, int mods)
	{
		// std::cout << "Released mouse button: " << button << std::endl;
	}
};


int main()
{
	Application app;
	app.update();

	return 0;
}
