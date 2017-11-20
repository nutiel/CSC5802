#pragma comment(lib, "nclgl.lib")

#include "../../NCLGL/window.h"
#include "Renderer.h"

int main() {

	int t, scene = 0;
	bool pause = false;

	Window w("CourseWork1", 1920, 1080, false);
	if (!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		t = w.GetTimer()->GetMS();

		if (w.GetKeyboard()->KeyDown(KEYBOARD_P)) {
			pause = !pause;
		}

		if (!pause && t%6000 < 2000) {
			scene++;
			scene = scene % 3;
		}

		if (w.GetKeyboard()->KeyDown(KEYBOARD_RIGHT)) {
			scene++;
			scene = scene % 3;
		}

		if (w.GetKeyboard()->KeyDown(KEYBOARD_LEFT)) {
			scene--;
			scene = scene % 3;
		}

		switch (scene) {
		case 0:
			renderer.UpdateScene(w.GetTimer()->GetTimedMS());
			renderer.RenderScene();
			break;
		case 1:
			break;
		case 2:
			break;
		}
		
	}

	return 0;
}