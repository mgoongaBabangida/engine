#ifndef SHADOWMAPFBO_H 
#define SHADOWMAPFBO_H


 #include <GL/glew.h>
#include "Texture.h"

 class ShadowMapFBO
 {
 public:
	 ShadowMapFBO();
	 ~ShadowMapFBO();
	 bool Init(unsigned int WindowWidth, unsigned int WindowHeight);
	 void BindForWriting();
	 void BindForReading(GLenum TextureUnit);
	 Texture GetTexture();
 private:
	 GLuint m_fbo;
	 //GLuint m_shadowMap;
	 Texture m_shadowMap;
 };


 #endif /* SHADOWMAPFBO_H */

