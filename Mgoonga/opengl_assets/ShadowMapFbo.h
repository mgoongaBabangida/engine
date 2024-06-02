#ifndef SHADOWMAPFBO_H 
#define SHADOWMAPFBO_H

#include "Texture.h"

//-------------------------------------------------------------------------------------
 class ShadowMapFBO
 {
 public:
	 ShadowMapFBO();
	 ~ShadowMapFBO();
	 bool Init(unsigned int WindowWidth, unsigned int WindowHeight, bool needsCubeMap = false);
	 bool InitCSM(unsigned int WindowWidth, unsigned int WindowHeight, int32_t _layers);

	 void BindForWriting();
	 void BindForReading(GLenum TextureUnit);
	 Texture GetTexture();
	 GLuint  ID()			 { return m_fbo; }
	 glm::ivec2 Size() { return { m_shadowMap.m_width , m_shadowMap.m_height }; }
 
 private:
	 GLuint m_fbo;
	 Texture m_shadowMap;
	 bool m_cubemap = false;
	 bool m_array = false;
 };


 #endif /* SHADOWMAPFBO_H */

