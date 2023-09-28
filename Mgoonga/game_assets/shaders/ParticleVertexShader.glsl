#version 430

in layout(location=0) vec3 position;
in layout(location=6) mat4 ModelViewMatrix;
in layout(location=10) vec2 texOffset1;
in layout(location=11) vec2 texOffset2;
in layout(location=12) vec2 texCoordInfo;

out vec2 textureCoords1;
out vec2 textureCoords2;
out float blend;

uniform mat4 ProjectionMatrix;

void main()
{
	vec2 textureCoords = vec2( (position.x + 1.0f)/ 2.0f,(position.y+1.0f) /2.0f); // + vec2(0.5,0.5);
	
	textureCoords.y = 1.0f - textureCoords.y;
	textureCoords /= texCoordInfo.x; //4
	textureCoords1 = textureCoords + texOffset1;
	textureCoords2 = textureCoords + texOffset2;
	blend = texCoordInfo.y;

	gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(position,1.0);
}