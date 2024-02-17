    attribute vec3 position;
   
    uniform float agk_invert; // FBOs render upside down so flip the quad
	
    void main()
    {
        gl_Position = vec4(position.xy*vec2(1,agk_invert),0.5,1.0);
    }