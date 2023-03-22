#ifndef SHADOWMAPFBO_H 
#define SHADOWMAPFBO_H

#include "Texture.h"

 class ShadowMapFBO
 {
 public:
	 ShadowMapFBO();
	 ~ShadowMapFBO();
	 bool Init(unsigned int WindowWidth, unsigned int WindowHeight, bool needsCubeMap = false);
	 void BindForWriting();
	 void BindForReading(GLenum TextureUnit);
	 Texture GetTexture();
	 GLuint  ID() { return m_fbo; }
 private:
	 GLuint m_fbo;
	 Texture m_shadowMap;
 };


 #endif /* SHADOWMAPFBO_H */

