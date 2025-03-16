#version 430 

uniform int isLine;         
uniform vec2 center;        
uniform float radius;       
uniform bool intersection; 
out vec4 FragColor;

void main() {
    if (isLine == 1) {
        FragColor = vec4(0.0, 0.0, 1.0, 1.0); 
    } else {
        float d = distance(gl_FragCoord.xy, center);
        if (d > radius) {
            discard;  
        }

        vec3 colorCenter = vec3(1.0, 0.0, 0.0);  
        vec3 colorBorder = vec3(0.0, 1.0, 0.0);  

        if (intersection) {
            vec3 temp = colorCenter;
            colorCenter = colorBorder;
            colorBorder = temp;
        }

        float t = d / radius;
        vec3 interpColor = mix(colorCenter, colorBorder, t);

        float outlineThickness = 1.0;
        if (abs(d - radius) < outlineThickness) {
            interpColor = colorBorder; 
        }

        FragColor = vec4(interpColor, 1.0);
    }
}
