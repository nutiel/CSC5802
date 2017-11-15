#pragma once
#include "../../nclgl/SceneNode.h"

class CubeMap : public SceneNode {
public:
	CubeMap(void);
	~CubeMap(void) {};
	virtual void Update(float msec);

	static void CreateCubeMap() {
		

	}
	static void DeleteCubeMap() {}

protected:
	Mesh * quad;
};
