//----------------------------------------------------------------------------
//
// file: tools.hpp
//
// Набор функций, необходимый  для создания контента и компиляции шейдера
// 
//----------------------------------------------------------------------------
#ifndef __TOOLS_HPP__
#define __TOOLS_HPP__

#define TR_WINDOW_WIDTH 800
#define TR_WINDOW_HEIGHT 600

// Размещение файлов шейдеров
#define TR_VERT_SHADER_FNAME "set/vert.glsl"
#define TR_FRAG_SHADER_FNAME "set/frag.glsl"
//#define TR_GEOM_SHADER_FNAME "set/geom.glsl"
#define TR_FONT_FNAME "set/FreeSans.ttf"

// Поддержка uint8_t, int32_t и т.д.
#include <stdint.h>

#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <string>
#include <locale>

#include "lib/gl_core_3_3.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

// http://libpng.sourceforge.net/index.html
#include <png.h>

#define ERR throw std::runtime_error

extern GLuint shaderProgram;

extern GLFWwindow* init_opengl(void);
extern std::vector<char> get_txt_chars(const std::string & fname);

// Получить изображение символа
extern FT_Glyph getGlyph(FT_Face face, uint32_t charcode);
 // Получить кернинг между двумя символа
extern FT_Pos getKerning(FT_Face face, uint32_t leftCharcode, uint32_t rightCharcode);

extern void savePNG(uint8_t *image, int32_t width, int32_t height);

#endif //__TOOLS_HPP__
