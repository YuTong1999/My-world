#version 330 core

in  vec3 vPosition;
in  vec3 vColor;
in  vec3 vNormal;
in  vec2 vTexCoord;
// in  vec3 vFaceIndecies;

// 传给片元着色器的变量
// 模型
out vec4 color;
out vec2 texCoord;
out vec3 normal;
// 阴影
out vec3 pos;
out vec3 l_pos;

uniform vec3 theta;
uniform vec3 translation;
//uniform mat4 modelView;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 shadowMatrix;
uniform vec4 LightPosition;
uniform int type;

void main()
{
    const float  DegreesToRadians = 3.14159265 / 180.0;

    vec3 c = cos( DegreesToRadians * theta );
    vec3 s = sin( DegreesToRadians * theta );

    mat4 rx = mat4( 1.0, 0.0,  0.0, 0.0,
		    0.0, c.x, -s.x, 0.0,
		    0.0, s.x,  c.x, 0.0,
		    0.0, 0.0,  0.0, 1.0);

    mat4 ry = mat4( c.y, 0.0, s.y, 0.0,
		      0.0, 1.0, 0.0, 0.0,
		     -s.y, 0.0, c.y, 0.0,
		      0.0, 0.0, 0.0, 1.0 );

    // Workaround for bug in ATI driver
    ry[1][0] = 0.0;
    ry[1][1] = 1.0;

    mat4 rz = mat4( c.z, -s.z, 0.0, 0.0,
		            s.z, c.z, 0.0, 0.0,
		            0.0, 0.0, 1.0, 0.0,
		            0.0, 0.0, 0.0, 1.0);

    // Workaround for bug in ATI driver
    rz[2][2] = 1.0;

    color       = vec4(vColor, 0);
    texCoord    = vTexCoord;

 	gl_Position = vec4(vPosition, 1.0);
    gl_Position = rz * ry * rx * gl_Position;
    gl_Position = modelMatrix * gl_Position;        //层级变换
    gl_Position = gl_Position + vec4(translation, 0.0);         //模型坐标系到世界坐标系

    // vec4 v11(1.0, 0.0, 0.0, translation.x);    //平移矩阵
    // vec4 v22(0.0, 1.0, 0.0, translation.y);
    // vec4 v33(0.0, 0.0, 1.0, translation.z);
    // vec4 v44(0.0, 0.0, 0.0, 1.0);
    // mat4 temp(v11, v22, v33, v44);
    // mat4 modelMatrix = temp * (rz * ry * rx) ;          //模式变换矩阵
    // gl_Position = modelMatrix * vec4(vPosition, 1.0);          //局部坐标系->世界坐标系

    // 计算顶点最终位置，传给顶点着色器，判断是否是阴影
    vec4 v1;
    if(type == 1){
	    v1 = shadowMatrix * gl_Position;
    }
    else{
        v1 = gl_Position;
    }
	// 由于modelMatrix有可能为阴影矩阵，为了得到正确位置，我们需要做一次透视除法
	vec4 v2 = vec4(v1.xyz / v1.w, 1.0);
	//考虑相机和投影
	vec4 v3 = projMatrix * viewMatrix * v2; // gl_Position;
	gl_Position = v3; 

    pos = (viewMatrix * gl_Position).xyz;	//这三个矩阵的前三个变量，第四个变量知识齐次坐标的变量，对计算无用
    l_pos = (viewMatrix * LightPosition).xyz;
    normal = (viewMatrix * vec4(vNormal, 0.0)).xyz;

   
}
