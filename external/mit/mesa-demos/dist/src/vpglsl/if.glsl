
void main() {
    gl_FrontColor = gl_Color;
    gl_Position = gl_Vertex;
    if (gl_Position.x < 0.5)
	gl_FrontColor = vec4(1.0, 0.0, 0.0, 1.0);
}
