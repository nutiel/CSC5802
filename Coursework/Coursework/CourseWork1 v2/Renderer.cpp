#include "Renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {

	//camera = new Camera(-65.0f, 0.0f, 0.0f, Vector3(RAW_WIDTH * HEIGHTMAP_X/2.0f, 4000, RAW_HEIGHT * HEIGHTMAP_Z + 500));
	camera = new Camera(0.0f, 230.0f, 0.0f, Vector3(0, 500, 0));
	quad = Mesh::GenerateQuad();
	Cube::CreateCube();
	Sphere::CreateSphere();
	Pyramid::CreatePyramid();

	//Sand texture used from https://naldzgraphics.net/free-seamless-sand-textures/
	desertTexture = SOIL_load_OGL_texture(TEXTUREDIR"5-desert-sand-seamless.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	SetTextureRepeating(desertTexture, true);

	barrenTexture = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	SetTextureRepeating(barrenTexture, true);

	barrenBump = SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	SetTextureRepeating(barrenBump, true);

	brickTexture = SOIL_load_OGL_texture(TEXTUREDIR"brick.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	SetTextureRepeating(brickTexture, true);

	brickBump = SOIL_load_OGL_texture(TEXTUREDIR"brickDOT3.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	SetTextureRepeating(brickBump, true);

	//---Shaders---//

	floorShader = new Shader(SHADERDIR"BumpVertex.glsl", SHADERDIR"BumpFragment.glsl");
	textShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	skyboxShader = new Shader(SHADERDIR"SkyboxVertex.glsl", SHADERDIR"SkyboxFragment.glsl");
	eyeShader = new Shader(SHADERDIR"SceneVertex.glsl", SHADERDIR"eyeFilter.glsl");
	sceneGraph = new Shader(SHADERDIR"SceneVertex.glsl", SHADERDIR"SceneFragment.glsl");
	particleShader = new Shader(SHADERDIR"particleVertex.glsl", SHADERDIR"particleFragment.glsl", SHADERDIR"particleGeometry.glsl");
	reflectShader = new Shader(SHADERDIR"PerPixelVertex.glsl", SHADERDIR"ReflectFragment.glsl");
	lightShader = new Shader(SHADERDIR"PerPixelVertex.glsl", SHADERDIR"PerPixelFragment.glsl");

	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");
	
	heightMap->SetTexture(barrenTexture);
	heightMap->SetBumpMap(barrenBump);
	heightMap->GenerateHeights(1.0, true);

	if (!textShader->LinkProgram() || !floorShader->LinkProgram() || !heightMap->GetTexture() || !heightMap->GetBumpMap()
		|| !skyboxShader->LinkProgram() || !eyeShader->LinkProgram() || !particleShader->LinkProgram() || !reflectShader->LinkProgram() 
		|| !lightShader->LinkProgram()) {
		return;
	}

	quad->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water3.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	quad->SetBumpMap(barrenBump);

	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg", TEXTUREDIR"rusted_up.jpg",
		TEXTUREDIR"rusted_down.jpg", TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	//picture used from: https://www.google.co.uk/url?sa=i&rct=j&q=&esrc=s&source=images&cd=&cad=rja&uact=8&ved=0ahUKEwjKzoWnp9XXAhVDL8AKHb9PDp0QjRwIBw&url=http%3A%2F%2Fkeywordsuggest.org%2Fgallery%2F80577.html&psig=AOvVaw36C-gyD9CRc5F1-0wuUrfS&ust=1511547424488281
	cubeMap2 = SOIL_load_OGL_cubemap(TEXTUREDIR"px.png", TEXTUREDIR"nx.png", TEXTUREDIR"py.png",
		TEXTUREDIR"ny.png", TEXTUREDIR"pz.png", TEXTUREDIR"nz.png", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	//picture used from: https://www.google.co.uk/url?sa=i&rct=j&q=&esrc=s&source=images&cd=&cad=rja&uact=8&ved=0ahUKEwi99ceLqtXXAhXID8AKHUCtAGcQjRwIBw&url=http%3A%2F%2Fkvkh.16mb.com%2Fportfolio%2Ftextures%2F&psig=AOvVaw0yG7zayzkctNeDmOd-WJ96&ust=1511548211728105
	cubeMap3 = SOIL_load_OGL_cubemap(TEXTUREDIR"spx.png", TEXTUREDIR"snx.png", TEXTUREDIR"spy.png",
		TEXTUREDIR"sny.png", TEXTUREDIR"spz.png", TEXTUREDIR"snz.png", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	if (!cubeMap || !quad -> GetTexture()) {
		return;
	}

	SetTextureRepeating(quad -> GetTexture(), true);

	light = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)), Vector4(1, 1, 1, 1), (RAW_WIDTH * HEIGHTMAP_X) / 2.0f);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	//---First Scene---//

	root1 = new SceneNode();
	root1->SetBoundingRadius(0.0f);
	//The cube around the camera used for post processing
	root1->AddChild(new Cube(camera->GetPosition()));
	root1->getChildren()[0]->setShader(eyeShader);
	root1->AddChild(new Sphere(light->GetPosition()));
	root1->getChildren()[1]->setShader(textShader);
	root1->getChildren()[1]->GetMesh()->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water2.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));

	//---Second Scene---//
	root2 = new SceneNode();
	root2->SetBoundingRadius(0.0f);
	//The cube around the camera used for post processing
	root2->AddChild(new Cube(camera->GetPosition()));
	root2->getChildren()[0]->setShader(eyeShader);
	root2->AddChild(new Sphere(light->GetPosition()));
	root2->getChildren()[1]->setShader(textShader);
	root2->getChildren()[1]->GetMesh()->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water2.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	root2->AddChild(new Pyramid(light->GetPosition()));
	root2->getChildren()[2]->setShader(textShader);
	root2->getChildren()[2]->GetMesh()->SetTexture(brickTexture);
	root2->getChildren()[2]->GetMesh()->SetBumpMap(brickBump);

	//---Third Scene---//
	root3 = new SceneNode();
	root3->SetBoundingRadius(0.0f);
	//The cube around the camera used for post processing
	root3->AddChild(new Cube(camera->GetPosition()));
	root3->getChildren()[0]->setShader(eyeShader);

	counter = 0.0f;
	frames = 0;
	fps = 0;
	seconds2 = 0;
	fade = 0;
	switchScene = 0;
	currentScene = 0;
	grow = 0;

	//A new particle emitter!
	emitter = new ParticleEmitter();

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	init = true;

	waterRotate = 0.0f;
}

Renderer ::~Renderer(void) {

	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);

	delete camera;
	delete heightMap;
	delete light;
	delete basicFont;
	delete skyboxShader;
	delete floorShader;
	delete textShader;
	delete eyeShader;
	delete regularShader;
	delete reflectShader;
	delete lightShader;
	delete root1;
	delete root2;
	delete root3;
	delete quad;
	delete emitter;


	currentShader = NULL;
	Cube::DeleteCube();
	Sphere::DeleteSphere();
	Pyramid::DeletePyramid();
}

/* switchScene - indicates whether we are switching into a scene, out of it or not switching
 * scene - is the number of the current scene*/
void Renderer::UpdateScene(float msec, int switchScene, int scene) {
	Matrix4 *transform = new Matrix4();
	if (this->switchScene == 2 && switchScene == 1) {
		perform_once = true;

	}
	else {
		perform_once = false;
	}

	this->switchScene = switchScene;

	if (switchScene == 1 || switchScene == 2) {
		fade++;
		currentScene = scene;
	}

	emitter->setAttatchPos(camera->GetPosition());
	emitter->Update(msec);
	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);
	
	waterRotate += msec / 1000.0f;
	seconds = msec;//Used for calculating fps
	seconds2 += msec;//used for object movement
	
	//Used for the box that covers the camera for post process effects
	Matrix4 *m = new Matrix4();
	m->ToIdentity();
	Vector3 *v = new Vector3(camera->GetPosition());
	m->SetPositionVector(*v);

	switch (currentScene) {
	case 0:
		if (perform_once) {
			camera->SetYaw(230.0);
			heightMap->SetTexture(barrenTexture);
			heightMap->SetBumpMap(barrenBump);
			heightMap->GenerateHeights(1.0f, true);
		}

		root1->getChildren()[0]->SetTransform(*m);

		//Used to position the sphere at the position of the light
		m->ToIdentity();
		*v = Vector3(1500.0f*cos(seconds2 / 2000.0) + (RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500.0f, 1500.0f*sin(seconds2 / 2000.0) + (RAW_HEIGHT * HEIGHTMAP_X / 2.0f));
		m->SetPositionVector(*v);
		root1->getChildren()[1]->SetTransform(*m);

		root1->Update(msec);
		break;
	case 1:

		*transform = Matrix4::Translation(Vector3(1550, 400, 2230)) * Matrix4::Rotation(90.0, Vector3(-1, 0, 0));

		if (perform_once) {
			camera->SetPosition(Vector3(2385, 330, 2015));
			camera->SetYaw(100.0);
			heightMap->SetTexture(desertTexture);
			heightMap->SetBumpMap(0);
			heightMap->GenerateHeights(1.0f, true);
		}

		root2->getChildren()[0]->SetTransform(*m);
		//Moves the light in a circle above the scene
		light->SetPosition(Vector3(1500.0f*cos(seconds2 / 2000.0) + (RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500.0f, 1500.0f*sin(seconds2 / 2000.0) + (RAW_HEIGHT * HEIGHTMAP_X / 2.0f)));
		light->SetRadius((RAW_WIDTH * HEIGHTMAP_X));
		//Used to position the sphere at the position of the light
		m->ToIdentity();
		*v = light->GetPosition();
		m->SetPositionVector(Vector3(1700,600,2415));
		root2->getChildren()[1]->SetTransform(*m);

		root2->getChildren()[2]->SetTransform(*transform);
		root2->getChildren()[2]->SetModelScale(Vector3(400,500,400));

		root2->Update(msec);
		break;
	case 2:

		//hellNode->Update(msec);

		if (perform_once) {
			camera->SetPosition(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)));
			heightMap->SetTexture(barrenTexture);
			heightMap->SetBumpMap(barrenBump);
			heightMap->GenerateHeights(0,  true);
		}
		
		if (grow < 1.0 && grow >= -1) {
			grow += 0.001;
			heightMap->GenerateHeights(grow, false);
		}

		root3->getChildren()[0]->SetTransform(*m);
		//Moves the light in a circle above the scene
		light->SetPosition(Vector3(1500.0f*cos(seconds2 / 2000.0) + (RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500.0f, 1500.0f*sin(seconds2 / 2000.0) + (RAW_HEIGHT * HEIGHTMAP_X / 2.0f)));
		root3->Update(msec);
		break;
	}
	delete transform;
	delete m;
	delete v;
}

void Renderer::RenderScene() {

	counter += seconds;
	frames++;

	if (counter >= 1000.0f) {
		counter = 0.0f;
		fps = frames;
		frames = 0;
	}

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	switch (currentScene) {
	case 0:
		BuildNodeLists(root1);
		SortNodeLists();

		drawSkyBox();
		drawTerrain();
		DrawWater();
		DrawNodes();
		showFPS();

		break;
	case 1:
		BuildNodeLists(root2);
		SortNodeLists();

		drawSkyBox();
		drawTerrain();
		DrawNodes();
		DrawParticles();
		showFPS();
		break;
	case 2:
		BuildNodeLists(root3);
		SortNodeLists();

		drawSkyBox();
		drawTerrain();
		DrawNodes();
		showFPS();
		break;
	}

	ClearNodeLists();
	SwapBuffers();
}


void Renderer::showFPS() {
	SetCurrentShader(textShader);
	glUniform1i(glGetUniformLocation(textShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(textShader->GetProgram(), "bumpTex"), 1);

	glUniform3fv(glGetUniformLocation(textShader->GetProgram(), "cameraPos"), 1, (float *)& camera->GetPosition());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	DrawText("FPS: " + std::to_string(fps), Vector3(0, 0, 0), 16.0f);

	DrawText("Camera Pos: " + std::to_string((int)camera->GetPosition().x) + ", " +
		std::to_string((int)camera->GetPosition().y) + ", " +
		std::to_string((int)camera->GetPosition().z), Vector3(0, 16.0, 0), 16.0f);
	DrawText("Yaw: " + std::to_string((int)camera->GetYaw()) + " Pitch: " + std::to_string((int)camera->GetPitch()), Vector3(0, 32.0, 0), 16.0f);
	glUseProgram(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_TRUE);
}

void Renderer::drawSkyBox() {
	SetCurrentShader(skyboxShader);

	switch (currentScene) {
	case 0:
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
		break;
	case 1:
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap2);
		break;
	case 2:
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap3);
		break;
	}

	UpdateShaderMatrices();
		

	quad->Draw();

	glUseProgram(0);
}

void Renderer::drawTerrain() {
	
	if (currentScene == 0) {
		glDepthMask(GL_TRUE);

		SetCurrentShader(lightShader);
		SetShaderLight(*light);

		glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float *)& camera->GetPosition());

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);

		modelMatrix.ToIdentity();
		textureMatrix.ToIdentity();

		UpdateShaderMatrices();

		heightMap->Draw();

		glUseProgram(0);
		glDepthMask(GL_FALSE);
	}
	else {
		SetCurrentShader(floorShader);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);

		glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float *)& camera->GetPosition());


		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);

		UpdateShaderMatrices();
		SetShaderLight(*light);

		heightMap->Draw();

		glUseProgram(0);
	}
}

void Renderer::DrawText(const std::string &text, const Vector3 &position, const float size, const bool perspective) {
	//Create a new temporary TextMesh, using our line of text and our font
	TextMesh* mesh = new TextMesh(text, *basicFont);

	Matrix4 temp_modelMatrix = modelMatrix;
	Matrix4 temp_viewMatrix = viewMatrix;
	Matrix4 temp_projMatrix = projMatrix;
	//This just does simple matrix setup to render in either perspective or
	//orthographic mode, there's nothing here that's particularly tricky.
	if (perspective) {
		modelMatrix = Matrix4::Translation(position) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix = camera->BuildViewMatrix();
		projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);
	}
	else {
		//In ortho mode, we subtract the y from the height, so that a height of 0
		//is at the top left of the screen, which is more intuitive
		//(for me anyway...)
		modelMatrix = Matrix4::Translation(Vector3(position.x, height - position.y, position.z)) * Matrix4::Scale(Vector3(size, size, 1));
		viewMatrix.ToIdentity();
		projMatrix = Matrix4::Orthographic(-1.0f, 1.0f, (float)width, 0.0f, (float)height, 0.0f);
	}
	//Either way, we update the matrices, and draw the mesh
	UpdateShaderMatrices();
	mesh->Draw();

	modelMatrix = temp_modelMatrix;
	viewMatrix = temp_viewMatrix;
	projMatrix = temp_projMatrix;

	UpdateShaderMatrices();
	delete mesh; //Once it's drawn, we don't need it anymore!
}

void Renderer::DrawNode(SceneNode * n) {

	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 1);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (n->GetMesh()) {	
		SetCurrentShader(n->getShader());
		UpdateShaderMatrices();
		Matrix4 transform = n->GetWorldTransform() * Matrix4::Scale(n->GetModelScale());

		glUniformMatrix4fv(glGetUniformLocation(currentShader->GetProgram(), "modelMatrix"), 1, false, (float *)& transform);

		glUniform4fv(glGetUniformLocation(currentShader->GetProgram(), "nodeColour"), 1, (float *)& n->GetColour());

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "useTexture"), (int)n->GetMesh()->GetTexture());

		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "iTime"), fade/100);

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "switchScene"), switchScene);

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "scene"), currentScene);

		glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float *)& camera->GetPosition());

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 2);

		n->Draw(*this);
	}

	glUseProgram(0);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void Renderer::BuildNodeLists(SceneNode * from) {
	if (frameFrustum.InsideFrustum(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() - camera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);
		}
		else {
			nodeList.push_back(from);
		}
	}
	for (vector < SceneNode * >::const_iterator i =
		from->GetChildIteratorStart();
		i != from->GetChildIteratorEnd(); ++i) {
		BuildNodeLists((*i));
	}
}

void Renderer::SortNodeLists() {
	std::sort(transparentNodeList.begin(),
		transparentNodeList.end(),
		SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(),
		nodeList.end(),
		SceneNode::CompareByCameraDistance);

}

void Renderer::DrawNodes() {
	for (vector < SceneNode * >::const_iterator i = nodeList.begin();
		i != nodeList.end(); ++i) {
		DrawNode((*i));
	}

	for (vector < SceneNode * >::const_reverse_iterator i =
		transparentNodeList.rbegin();
		i != transparentNodeList.rend(); ++i) {
		DrawNode((*i));
	}
}

void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}

int Renderer::getFade() {
	return fade;
}

void Renderer::resetFade() {
	fade = 0;
}

void Renderer::DrawParticles() {
	SetCurrentShader(particleShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);

	SetShaderParticleSize(emitter->GetParticleSize());
	emitter->SetParticleSize(10.0f);
	emitter->SetParticleVariance(1.0f);
	emitter->SetLaunchParticles(160.0f);
	emitter->SetParticleSpeed(0.3f);
	emitter->SetParticleLifetime(2000.0f);;
	UpdateShaderMatrices();

	emitter->Draw();
	glUseProgram(0);
}

void Renderer::SetShaderParticleSize(float f) {
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "particleSize"), f);
}

void Renderer::DrawWater() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SetCurrentShader(reflectShader);
	SetShaderLight(*light);
	glUniform3fv(glGetUniformLocation(currentShader -> GetProgram(), "cameraPos"), 1, (float *)& camera -> GetPosition());
	
	glUniform1i(glGetUniformLocation(currentShader -> GetProgram(), "diffuseTex"), 0);
	
	glUniform1i(glGetUniformLocation(currentShader -> GetProgram(), "cubeTex"), 2);
	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	
	float heightX = (RAW_WIDTH * HEIGHTMAP_X / 2.0f);
	
	float heightY = 256 * HEIGHTMAP_Y / 5.0f;
	
	float heightZ = (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f);
	
	Matrix4 modelt = modelMatrix;
	Matrix4 texturet = textureMatrix;
	modelMatrix =
	Matrix4::Translation(Vector3(heightX, heightY - 200, heightZ)) *
	Matrix4::Scale(Vector3(heightX, 1, heightZ)) *
	Matrix4::Rotation(90, Vector3(1.0f, 0.0f, 0.0f));
	
	textureMatrix = Matrix4::Scale(Vector3(10.0f, 10.0f, 10.0f)) *
	Matrix4::Rotation(waterRotate, Vector3(0.0f, 0.0f, 1.0f));
	
	
	UpdateShaderMatrices();
	
	quad -> Draw();
	
	glUseProgram(0);

	modelMatrix = modelt;
	textureMatrix = texturet;

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}