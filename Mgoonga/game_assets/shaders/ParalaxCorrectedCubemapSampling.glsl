#version 430 core

// Uniforms
uniform samplerCube envMap;      // Environment cubemap

uniform mat4 WorldToLocal;       // World to local transformation matrix
uniform vec3 CameraWS;           // Camera position in world space
uniform vec3 CubemapPositionWS;  // Position of the cubemap in world space

// Inputs from vertex shader
in vec3 PositionWS;              // Position in world space
in vec3 NormalWS;                // Normal in world space

// Output
out vec4 FragColor;

void main() {
    // Compute the normalized direction from the position in world space to the camera position in world space
    vec3 DirectionWS = normalize(PositionWS - CameraWS);

    // Compute the reflection direction in world space
    vec3 ReflDirectionWS = reflect(DirectionWS, NormalWS);

    // Convert reflection direction to local unit parallax cube space
    // Transform the reflection direction to local space using the world-to-local transformation matrix
    vec3 RayLS = (mat3(WorldToLocal) * ReflDirectionWS);

    // Transform the position to local space using the world-to-local transformation matrix
    vec3 PositionLS = (WorldToLocal * vec4(PositionWS, 1.0)).xyz;

    // Define a unitary vector representing the unit box extents
    vec3 Unitary = vec3(1.0, 1.0, 1.0);

    // Calculate the intersection distances with the planes of the unit box
    vec3 FirstPlaneIntersect  = (Unitary - PositionLS) / RayLS;
    vec3 SecondPlaneIntersect = (-Unitary - PositionLS) / RayLS;

    // Find the furthest plane intersected by the ray
    vec3 FurthestPlane = max(FirstPlaneIntersect, SecondPlaneIntersect);

    // Compute the minimum distance to the intersection with the unit box
    float Distance = min(FurthestPlane.x, min(FurthestPlane.y, FurthestPlane.z));

    // Calculate the intersection position in world space
    vec3 IntersectPositionWS = PositionWS + ReflDirectionWS * Distance;

    // Adjust the reflection direction based on the intersection position and the position of the cubemap
    vec3 FinalReflDirectionWS = normalize(IntersectPositionWS - CubemapPositionWS);

    // Sample the cubemap texture using the computed reflection direction
    vec4 EnvColor = texture(envMap, FinalReflDirectionWS);

    // Output the color
    FragColor = EnvColor;
}
