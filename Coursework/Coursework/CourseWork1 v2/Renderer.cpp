#include "Renderer.h"

Renderer::Renderer(Window & parent) : OGLRenderer(parent) {

	camera = new Camera(-65.0f, 0.0f, 0.0f, Vector3(RAW_WIDTH * HEIGHTMAP_X/2.0f, 4000, RAW_HEIGHT * HEIGHTMAP_Z + 500));
	quad = Mesh::GenerateQuad();
	Cube::CreateCube();
	Sphere::CreateSphere();
	UFO::CreateSphere();

	floorShader = new Shader(SHADERDIR"BumpVertex.glsl", SHADERDIR"BumpFragment.glsl");
	textShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	skyboxShader = new Shader(SHADERDIR"SkyboxVertex.glsl", SHADERDIR"SkyboxFragment.glsl");
	eyeShader = new Shader(SHADERDIR"SceneVertex.glsl", SHADERDIR"eyeFilter.glsl");//SceneFragment.glsl
	regularShader = new Shader(SHADERDIR"TexturedVertex.glsl", SHADERDIR"TexturedFragment.glsl");
	sceneGraph = new Shader(SHADERDIR"SceneVertex.glsl", SHADERDIR"SceneFragment.glsl");

	heightMap = new HeightMap(TEXTUREDIR"terrain.raw");
	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	heightMap->SetBumpMap(SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));

	if (!textShader->LinkProgram() || !floorShader->LinkProgram() || !heightMap->GetTexture() || !heightMap->GetBumpMap() 
		|| !skyboxShader->LinkProgram() || !eyeShader->LinkProgram() || !regularShader->LinkProgram()) {
		return;
	}

	basicFont = new Font(SOIL_load_OGL_texture(TEXTUREDIR"tahoma.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT), 16, 16);

	cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg", TEXTUREDIR"rusted_up.jpg",
		TEXTUREDIR"rusted_down.jpg", TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	//picture used from: https://www.google.co.uk/url?sa=i&rct=j&q=&esrc=s&source=images&cd=&cad=rja&uact=8&ved=0ahUKEwiowMTE0s3XAhUlDcAKHYZ4AU4QjRwIBw&url=https%3A%2F%2F3docean.net%2Fitem%2Fhdr-desert-skies%2F16042303&psig=AOvVaw1ptnkitm5UFFnGpmFQz1u5&ust=1511284188144421
	cubeMap2 = SOIL_load_OGL_cubemap(TEXTUREDIR"px.png", TEXTUREDIR"nx.png", TEXTUREDIR"py.png",
		TEXTUREDIR"ny.png", TEXTUREDIR"pz.png", TEXTUREDIR"nz.png", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);

	if (!cubeMap) {
		return;
	}

	SetTextureRepeating(heightMap->GetTexture(), true);
	SetTextureRepeating(heightMap->GetBumpMap(), true);

	light = new Light(Vector3((RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500.0f, (RAW_HEIGHT * HEIGHTMAP_Z / 2.0f)), Vector4(1, 1, 1, 1), (RAW_WIDTH * HEIGHTMAP_X) / 2.0f);
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	root = new SceneNode();
	root->SetBoundingRadius(0.0f);
	//The cube around the camera used for post processing
	root->AddChild(new Cube(camera->GetPosition()));
	root->getChildren()[0]->setShader(eyeShader);
	root->AddChild(new Sphere(light->GetPosition()));
	root->getChildren()[1]->setShader(regularShader);
	root->getChildren()[1]->GetMesh()->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water2.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
	UFO *u = new UFO(light->GetPosition());
	u->setShader(sceneGraph);
	root->AddChild(u);

	counter = 0.0f;
	frames = 0;
	fps = 0;
	seconds2 = 0;
	fade = 0;
	switchScene = false;
	currentScene = 0;

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	init = true;
}

Renderer ::~Renderer(void) {
	delete camera;
	delete heightMap;
	delete light;
	delete basicFont;
	delete skyboxShader;
	delete floorShader;
	delete textShader;
	delete eyeShader;
	delete regularShader;
	delete root;
	delete quad;

	currentShader = NULL;
	Cube::DeleteCube();
	Sphere::DeleteSphere();
}

/* switchScene - indicates whether we are switching into a scene, out of it or not switching
 * scene - is the number of the current scene*/
void Renderer::UpdateScene(float msec, int switchScene, int scene) {
	this->switchScene = switchScene;

	if (switchScene == 1 || switchScene == 2) {
		fade++;
		currentScene = scene;
	}

	if (this->switchScene == 2 && switchScene == 1) {
		DeleteChildren(root);
		//The cube around the camera used for post processing
		root->AddChild(new Cube(camera->GetPosition()));
		root->getChildren()[0]->setShader(eyeShader);
		root->AddChild(new Sphere(light->GetPosition()));
		root->getChildren()[1]->setShader(regularShader);
		root->getChildren()[1]->GetMesh()->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR"water2.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0));
		/*root->AddChild(new UFO(light->GetPosition()));
		root->getChildren()[2]->setShader(sceneGraph);*/
	}

	camera->UpdateCamera(msec);
	viewMatrix = camera->BuildViewMatrix();
	frameFrustum.FromMatrix(projMatrix * viewMatrix);
	
	seconds = msec;//Used for calculating fps
	seconds2 += msec;//used for object movement
	
	//Used for the box that covers the camera for post process effects
	Matrix4 *m = new Matrix4();
	m->ToIdentity();
	Vector3 *v = new Vector3(camera->GetPosition());
	m->SetPositionVector(*v);
	root->getChildren()[0]->SetTransform(*m);

	switch (scene) {
	case 0:
		//Moves the light in a circle above the scene
		light->SetPosition(Vector3(1500.0f*cos(seconds2 / 2000.0) + (RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500.0f, 1500.0f*sin(seconds2 / 2000.0) + (RAW_HEIGHT * HEIGHTMAP_X / 2.0f)));

		//Used to position the sphere at the position of the light
		m->ToIdentity();
		*v = light->GetPosition();
		m->SetPositionVector(*v);
		root->getChildren()[1]->SetTransform(*m);

		root->Update(msec);
		break;
	case 1:
		//Moves the light in a circle above the scene
		light->SetPosition(Vector3(1500.0f*cos(seconds2 / 2000.0) + (RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500.0f, 1500.0f*sin(seconds2 / 2000.0) + (RAW_HEIGHT * HEIGHTMAP_X / 2.0f)));

		//Used to position the sphere at the position of the light
		m->ToIdentity();
		*v = light->GetPosition();
		m->SetPositionVector(*v);
		root->getChildren()[1]->SetTransform(*m);

		root->Update(msec);
		break;
	case 2:
		//Moves the light in a circle above the scene
		light->SetPosition(Vector3(1500.0f*cos(seconds2 / 2000.0) + (RAW_HEIGHT * HEIGHTMAP_X / 2.0f), 500.0f, 1500.0f*sin(seconds2 / 2000.0) + (RAW_HEIGHT * HEIGHTMAP_X / 2.0f)));

		//Used to position the sphere at the position of the light
		m->ToIdentity();
		*v = light->GetPosition();
		m->SetPositionVector(*v);
		root->getChildren()[1]->SetTransform(*m);

		root->Update(msec);
		break;
	}

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

	BuildNodeLists(root);
	SortNodeLists();

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	switch (currentScene) {
	case 0:
		showFPS();
		drawSkyBox();
		drawTerrain();
		DrawNodes();
		break;
	case 1:
		showFPS();
		drawSkyBox();
		drawTerrain();
		DrawNodes();
		break;
	case 2:
		showFPS();
		drawSkyBox();
		drawTerrain();
		DrawNodes();
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

	DrawText("FPS: " + std::to_string(fps), Vector3(0, 0, 0), 16.0f);
	glUseProgram(0);
	glDepthMask(GL_FALSE);
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
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
		break;
	}

	UpdateShaderMatrices();
		

	quad->Draw();

	glUseProgram(0);
}

void Renderer::drawTerrain() {
	glDepthMask(GL_TRUE);

	SetCurrentShader(floorShader);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "bumpTex"), 1);

	glUniform3fv(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), 1, (float *)& camera->GetPosition());


	glBlendFunc(GL_SRC_ALPHA, GL_NONE);
	glEnable(GL_DEPTH_TEST);

	UpdateShaderMatrices();
	SetShaderLight(*light);

	heightMap->Draw();

	glUseProgram(0);
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

		n->Draw(*this);
	}

	glUseProgram(0);
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

void Renderer::DeleteChildren(SceneNode * from) {
	for (vector < SceneNode * >::const_iterator i = from->GetChildIteratorStart(); i != from->GetChildIteratorEnd(); ++i) {
		DeleteChildren((*i));
	}
}