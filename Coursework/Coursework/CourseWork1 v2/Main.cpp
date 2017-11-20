#pragma comment(lib, "nclgl.lib")

#include "../../NCLGL/window.h"
#include "Renderer.h"

#define FADE_TIME 151

int main() {

	int t = 0, t_past = 0, scene = 0, scene_tmp = 0; // for switchScene: 0 is no switch, 1 is fade in and 2 is fade out
	bool pause = false, fadein = false, fadeout = false;
	//1920, 1080
	Window w("CourseWork1", 800, 600, false);
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

		//the first part is to ensure that the pressed button is read only once
		if (t - t_past > 500 && w.GetKeyboard()->KeyDown(KEYBOARD_PAUSE)) {
			t_past = t;
			pause = !pause;
			cout << "Pause " << pause << endl;
		}

		if (!pause && t - t_past > 7000) {
			t_past = t;
			scene_tmp++;
			scene_tmp = scene_tmp % 3;
			fadeout = true;
			cout << "Scene auto " << t << " " << scene_tmp << endl;
		}

		if (t - t_past < 500 && w.GetKeyboard()->KeyDown(KEYBOARD_RIGHT)) {
			t_past = t;
			scene_tmp++;
			scene_tmp = scene_tmp % 3;
			fadeout = true;
			cout << "Scene right " << scene << endl;
		}

		if (t - t_past < 500 && w.GetKeyboard()->KeyDown(KEYBOARD_LEFT)) {
			t_past = t;
			scene_tmp += 2;
			scene_tmp = scene_tmp % 3;
			fadeout = true;
			cout << "Scene left " << scene << endl;
		}

		t_past = t;

		switch (scene) {
		case 0:
			//cout << "Scene  " << scene << endl;
			if (fadeout) {
				//while (t - t_past < FADE_TIME) {
				while (renderer.getFade() < FADE_TIME) {
					//t = w.GetTimer()->GetMS();
					renderer.UpdateScene(w.GetTimer()->GetTimedMS(), 2);
					renderer.RenderScene();
				}
				//cout << renderer.getFade() << endl;//
				scene = scene_tmp;
				fadein = true;
				fadeout = false;
			}else if(fadein) {
				while (renderer.getFade() < FADE_TIME + 151) {
					//t = w.GetTimer()->GetMS();
					renderer.UpdateScene(w.GetTimer()->GetTimedMS(), 1);
					renderer.RenderScene();
				}
				fadein = false;
			}
			else {
				renderer.resetFade();
				renderer.UpdateScene(w.GetTimer()->GetTimedMS(), 0);
				renderer.RenderScene();
			}
			break;
		case 1:
			//cout << "Scene  " << scene << endl;
			if (fadeout) {
				while (renderer.getFade() < FADE_TIME) {
					//t = w.GetTimer()->GetMS();
					renderer.UpdateScene(w.GetTimer()->GetTimedMS(), 2);
					renderer.RenderScene();
				}
				//cout << renderer.getFade() << endl;//
				scene = scene_tmp;
				fadein = true;
				fadeout = false;
			}
			else if (fadein) {
				while (renderer.getFade() < FADE_TIME + 151) {
					//t = w.GetTimer()->GetMS();
					renderer.UpdateScene(w.GetTimer()->GetTimedMS(), 1);
					renderer.RenderScene();
				}
				fadein = false;
			}
			else {
				renderer.resetFade();
				renderer.UpdateScene(w.GetTimer()->GetTimedMS(), 0);
				renderer.RenderScene();
			}
			break;
		case 2:
			//cout << "Scene  " << scene << endl;
			if (fadeout) {
				while (renderer.getFade() < FADE_TIME) {
					//t = w.GetTimer()->GetMS();
					renderer.UpdateScene(w.GetTimer()->GetTimedMS(), 2);
					renderer.RenderScene();
				}
				//cout << renderer.getFade() << endl;//
				scene = scene_tmp;
				fadein = true;
				fadeout = false;
			}
			else if (fadein) {
				while (renderer.getFade() < FADE_TIME + 151) {
					//t = w.GetTimer()->GetMS();
					renderer.UpdateScene(w.GetTimer()->GetTimedMS(), 1);
					renderer.RenderScene();
				}
				fadein = false;
			}
			else {
				renderer.resetFade();
				renderer.UpdateScene(w.GetTimer()->GetTimedMS(), 0);
				renderer.RenderScene();
			}
			break;
		}
	}

	return 0;
}