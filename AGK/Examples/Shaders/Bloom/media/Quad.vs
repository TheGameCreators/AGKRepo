    attribute vec3 position;
   
    varying vec2 uvVarying;
    uniform vec4 uvBounds0; // to adjust for atlas sub images
    uniform float agk_invert; // FBOs render upside down so flip the quad
	
    void main()
    {
        gl_Position = vec4(position.xy*vec2(1,agk_invert),0.5,1.0);
        uvVarying = (position.xy*vec2(0.5,-0.5) + 0.5) * uvBounds0.xy + uvBounds0.zw;
    }