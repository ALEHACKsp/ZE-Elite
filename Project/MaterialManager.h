#ifndef CMat_H
#define CMat_H

class IMaterial;
class Color;


typedef IMaterial* Matptr;

class CMat
{
public:
	Matptr Chams[8];

	void Initialize();

	Matptr CreateMaterial(bool IgnoreZ, bool Flat, bool Wireframe = false, bool Shiny = false);
	Matptr CreateMaterialOther(bool IgnoreZ, bool Flat, bool Wireframe = false, bool Shiny = false);
	void ResetMaterial();
};

extern CMat gMat;

void ForceMaterial(Color Color, Matptr Material, bool ForceColor = true, bool ForceMat = true);

#endif