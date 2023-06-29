#version  430
subroutine vec2 Blur(int i, vec2 centerTexCoords, float pixelSize);
subroutine uniform Blur BlurFunction;

in layout(location=0) vec2 position;
in layout(location=1) vec2 texcoord;

uniform float targetWidth;
out vec2 blurTextureCoords[11];
out vec2 TexCoords;

subroutine( Blur )
vec2 HorizontalBlur(int i, vec2 centerTexCoords,float pixelSize)
{
	return centerTexCoords + vec2( pixelSize * i ,0.0f);
}

subroutine( Blur )
vec2 VerticalBlur(int i,vec2 centerTexCoords,float pixelSize)
{
	return centerTexCoords + vec2( 0.0f,pixelSize * i );
}

void main()
{
    gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);   //position y
    vec2 centerTexCoords = vec2(position.xy) * 0.5 + 0.5;
    float pixelSize = 1.0f /  targetWidth;

    for(int i=-5;i<5;++i)
    {
		blurTextureCoords[i+5] = BlurFunction(i,centerTexCoords,pixelSize);
    }
    //TexCoords = texcoord;
};