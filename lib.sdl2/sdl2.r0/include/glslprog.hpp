//============================================================================
//
// file: glslprog.hpp
//
//
//
//============================================================================
#ifndef __GLSLPROG_HPP__
#define __GLSLPROG_HPP__

#include "lib.hpp"

using std::string;
using glm::mat3;
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;

namespace lib {

////////
// Отображаемый элемент пространства
//
class VaoPiece
{
	private:
		GLuint vao;
		GLenum mode;    // GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES,
			// GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY, GL_TRIANGLE_STRIP,
			// GL_TRIANGLE_FAN, GL_TRIANGLES, GL_TRIANGLE_STRIP_ADJACENCY,
			// GL_TRIANGLES_ADJACENCY and GL_PATCHES

		GLsizei count;  // the number of elements to be rendered
	  GLenum type;    // GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT
		const GLvoid * indices; // a byte offset (cast to a pointer type) into
			// the buffer bound to GL_ELEMENT_ARRAY_BUFFER to start reading indices

		VaoPiece(const VaoPiece &);
		VaoPiece operator=(const VaoPiece &);
		
	public:
		VaoPiece(GLuint vao, GLenum mode, GLsizei count,
				GLenum type, const GLvoid * indices);
		~VaoPiece(void);
		GLuint vao_id(void);
		void draw(void);
};
////////
//
//
class GLSLProgram
{
	public:
		GLSLProgram(void);
		~GLSLProgram(void);
		void addShader(const string& fileName);
		void addTexture(const string& fn);
		void use(void);
		GLuint handle(void);
		void link(void);
		GLint getAttribLocation(const GLchar * name);
		void setUniform( const GLchar *name, bool val );
		void setUniform( const GLchar *name, GLint val );
		void setUniform( const GLchar *name, GLuint val );
		void setUniform( const GLchar *name, float val );
		void setUniform( const GLchar *name, float x,float y,float z);
		void setUniform( const GLchar *name, const vec2 & v);
		void setUniform( const GLchar *name, const vec3 & v);
		void setUniform( const GLchar *name, const vec4 & v);
		void setUniform( const GLchar *name, const mat3 & m);
		void setUniform( const GLchar *name, const mat4 & m);
		void draw(void);
		//void pushFace4v(const glm::vec3 coord[], GLfloat * texture);
		void pushFace4v(GLvoid * quad, GLvoid * texture);
		void debug_make_area();
	
	private:
		GLuint vao = 0;
		GLint gl_major_ver = 0;
		GLint gl_minor_ver = 0;
		GLuint id = 0;
		GLboolean ready = gl::FALSE_;
    std::map<string, GLint> uniLocs; // set of uniformLocations
    std::map<string, GLint> attribs; // set of attribs indexes
		std::list<VaoPiece *> pieses;
		std::list<GLuint> vao_list = {};
		// Размер массива из четырех векторов для отображения четырехугольника
		//GLsizeiptr SizeFace4v = (GLsizeiptr)(sizeof(glm::vec3) * 4);
		GLsizeiptr SizeFace4v = (GLsizeiptr)(sizeof(GLfloat) * 12);
		// Размер массива координат текстуры для 4-х вершин
		GLsizeiptr SizeTex4v = (GLsizeiptr)(sizeof(GLfloat) * 8);

		// обход вершин для построения четырехугольника из треугольников
		GLubyte Idx4[4] = {0, 1, 3, 2};
		GLsizeiptr SizeIdx4 = (GLsizeiptr)(sizeof(GLubyte) * 4);
		GLenum TypeIdx4 = gl::UNSIGNED_BYTE;
		GLenum ModeIdx4 = gl::TRIANGLE_STRIP;

		// Размер ВЕКТОРНОГО массива точек с координатами четырехугольника
		std::size_t const QuadVertexCount = 4;
		std::size_t const QuadSizeF32 = QuadVertexCount * sizeof(glm::vec3);

		GLSLProgram(const GLSLProgram &);
		GLSLProgram operator= (const GLSLProgram &);
		void listPushFace4v(GLuint vao);
		void fillAttribsList_modern(void);
		void fillAttribsList(void);
		string getExtension(const string& file_name);
		void attachShader(GLuint _shader);
		void checkShader(GLuint shader_handle);
		GLint getUniformLocation(const GLchar * name );
		string getProgramInfoLog(void);
		string getShaderInfoLog(GLuint shader_id);

};

} //namespace lib

#endif // __GLSLPROG_HPP__
