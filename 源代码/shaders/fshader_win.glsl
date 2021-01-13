#version 330 core

in vec4 color;
in vec2 texCoord;
in vec3 normal;
in vec3 pos;            
in vec3 l_pos;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform float Shininess;
uniform int type;

out vec4 fColor;
out vec4 fNormal;

uniform sampler2D texture;
//type = 0时画模型，type = 1时画阴影
void main()
{
    if(type == 1){
        fColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else{
        vec3 N = normalize(normal);           //曲面法向
        vec3 L = normalize(l_pos - pos);    //入射方向
        vec3 V = normalize(-pos);           //视线方向
        vec3 R = reflect(-L,N);             //反射方向

        // 环境光分量I_a
        vec4 I_a = AmbientProduct;

        // @TODO: Task1 计算漫反射系数alpha和漫反射分量I_d
        float alpha = 0.0;
        alpha = max(dot(L, N), 0);
        vec4 I_d = alpha*DiffuseProduct;

        // @TODO: Task1 计算高光系数beta和镜面反射分量I_s
        float beta = 0.0;
        beta = pow(max(dot(R, V), 0) ,Shininess);
        vec4 I_s = beta * SpecularProduct;

        // @TODO: Task1 注意如果光源在背面则去除高光
        if( dot(L, N) < 0.0 ) {
            I_s = vec4(0.0, 0.0, 0.0, 1.0);
        }

        vec4 result = I_a + I_d + I_s;

        fColor = result * texture2D( texture, texCoord );
        fNormal = vec4(normal, 1.0);
    }
}

