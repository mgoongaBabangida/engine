#version 410

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexVelocity;
layout (location = 2) in float VertexStartTime;
layout (location = 3) in vec3 VertexInitialVelocity;

// Layout specifiers only available in OpenGL 4.4
/*layout( xfb_buffer = 0, xfb_offset=0 )*/ out vec3 Position;   // To transform feedback
/*layout( xfb_buffer = 1, xfb_offset=0 )*/ out vec3 Velocity;   // To transform feedback
/*layout( xfb_buffer = 2, xfb_offset=0 )*/ out float StartTime; // To transform feedback

out vec2 textureOffset1;	// To fragment/geometry shader
out vec2 textureOffset2;	// To fragment/geometry shader
out float blend;			// To fragment shader

uniform float Time;  // Simulation time
uniform float H;     // Elapsed time between frames
uniform vec4 Accel;  // Particle acceleration
uniform float ParticleLifetime;  // Particle lifespan
uniform vec4 StartPos;  // Starting position
uniform int NumRowsInTexture; //Number of rows in texture atlas

uniform mat4 MVP;

subroutine (RenderPassType)
void update() {

    // Update position & velocity for next frame
    Position = VertexPosition;
    Velocity = VertexVelocity;
    StartTime = VertexStartTime;

    if( Time >= StartTime ) {

        float age = Time - StartTime;

        if( age > ParticleLifetime ) {
            // The particle is past it's lifetime, recycle.
            Position = StartPos.xyz;
            Velocity = VertexInitialVelocity;
            StartTime = Time;
        } else {
            // The particle is alive, update.
            Position += Velocity * H;
            Velocity += Accel.xyz * H;
        }
    }
}

subroutine (RenderPassType)
void render() {

	float age = Time - VertexStartTime;
	float lifeFactor = age / ParticleLifetime;
	int stageCount = NumRowsInTexture * NumRowsInTexture;
	float atlasProgression = lifeFactor * stageCount;
	
	int index1 = int(floor(atlasProgression));
	int index2 = stageCount - 1 > 0 ? index1 : index1 + 1;
	blend = atlasProgression - index1;
	
	int column1 = index1 % NumRowsInTexture;
	int row1 = index1 / NumRowsInTexture;
	textureOffset1.x = float(column1) / float(NumRowsInTexture);
	textureOffset1.y = float(row1) / float(NumRowsInTexture);
	
	int column2 = index2 % NumRowsInTexture;
	int row2 = index2 / NumRowsInTexture;
	textureOffset2.x = float(column2) / float(NumRowsInTexture);
	textureOffset2.y = float(row2) / float(NumRowsInTexture);

    gl_Position = MVP * vec4(VertexPosition, 1.0);
}

void main()
{
    // This will call either render() or update()
    RenderPass();
}
