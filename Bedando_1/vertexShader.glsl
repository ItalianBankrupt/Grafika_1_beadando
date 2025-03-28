#version 430 

layout(location = 0) in vec2 position;
uniform float offsetX;
uniform float offsetY;
uniform int moving;  
uniform float szakaszEltolas;

void main() {
    if(moving == 1) {
        gl_Position = vec4(position.x + (offsetX / 600) * 2.0f, position.y + (offsetY / 600) * 2.0f, 0.0, 1.0);
    }
    else {
        vec2 finalPosition = position;
        finalPosition.y += (szakaszEltolas /600 * 2.0f);
        gl_Position = vec4(finalPosition, 0.0, 1.0);
    }
        
}