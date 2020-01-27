#ifndef SHADER_H
#define SHADER_H

#include "../../utils/pointers.h"
#include "../../utils/types/types.h"
#include "../enum/graphics_enums.h"
#include "graphic_object.h"

class Shader : GraphicObject{
protected:
	ShaderStage stage;
	std::string source;
};

#endif //SHADER_H