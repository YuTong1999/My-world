#pragma once
#include "mesh.h"
#include <assert.h>
#undef near
#undef far
#undef top

// 相机
struct Camera
{
	// 相机位置参数
	float radius = 9.0;

	//投影参数
	double fov = 60.0;
	double aspect = 1.0;
	//透视投影参数
	double near = 0.1;
	double far = 100.0;

	vec4 eye = vec4(0, 0.0, 1.0, 1.0);		//VRP参考点
	vec4 at = vec4(0.0, 0.0, 0.0, 1.0);	//VPN
	vec4 up = vec4(0, 1, 0, 0);			//VUP

	mat4 getViewMatrix()				//视图矩阵，照相机的视角
	{
		return LookAt(eye, at, up);		//将模型从世界坐标系转到相机坐标系
	}

	mat4 getProjectionMatrix()
	{
		mat4 projMatrix = mat4(1.0);	//投影矩阵
		//projMatrix = Ortho(-3, 3, -3, 3, -3, 3);		//透视投影
		projMatrix = Perspective(fov, aspect, near, far);		//透视投影
		return projMatrix;
	}
};

// 光源
struct Light
{
	vec4 position = vec4(1.0, 100.0, 1.0, 1.0);		//光源位置
	vec4 ambient = vec4(1.0, 1.0, 1.0, 1.0);		//环境光
	vec4 diffuse = vec4(1.0, 1.0, 1.0, 1.0);		//漫反射光
	vec4 specular = vec4(1.0, 1.0, 1.0, 1.0);		//镜面反射
};

// 材质
struct Material
{
	vec4 ambient = vec4(0.8f,0.8f,0.8f,1.0f);				//周围的
	vec4 diffuse = vec4(0.5f,0.5f,0.5f,1.0f);			//固有色
	vec4 specular = vec4(0.7f,0.7f,0.7f,1.0f);			//高光色
	vec4 emission = vec4(0, 0, 0, 1);							//反射
	float shininess = 10.0f;										//高光系数
};

class MatrixStack {
	int		_index;
    int		_size;
    mat4*	_matrices;

public:
	MatrixStack(int numMatrices = 100):_index(0), _size(numMatrices)
        { _matrices = new mat4[numMatrices]; }

    ~MatrixStack()
		{ delete[]_matrices; }

    void push(const mat4& m){
		assert( _index + 1 < _size );
		_matrices[_index++] = m;	 
    }

    mat4& pop(){
        assert(_index - 1 >= 0);
        _index--;
        return _matrices[_index];
    }
};

class Mesh_Painter
{
public:
	Mesh_Painter();
	~Mesh_Painter();
	GLfloat dd[16];
	Camera camera;
	Light light;
	Material material;
	int* theta_main1 = new int[6];
	vec3 tran1;
	// 
	vec3 p = (0.0, 4.99181, 1.0193009);
	MatrixStack	mvstack;
	bool robot_view = false;

	// void set_robot_view(){robot_view=true;};
	// void set_world_view(){robot_view=false;};

	void draw_meshes(float, float);
	void theta(float& x, float& y, float& z);
	void update_vertex_buffer();
	void update_texture();

	// void resetTheta();
	// void updateTheta(int axis, int sign);
	// void updateDelta(int sign);
	void init_shaders(std::string vs, std::string fs);
	void add_mesh(My_Mesh*);
	void clear_mehs();
	void set_th(int *ver)
	{
		theta_main1 = ver;
	}
	void set_tran(vec3 t)
	{
		tran1 = t;
	}
	void set_at(vec4 p)
	{
		camera.at = p;
	}
	void set_eye(vec4 p)
	{
		camera.eye = p;
	}
	vec4 get_up(){
		return camera.at;
	}
	vec4 get_at()
	{
		return camera.up;
	}
	vec4 get_eye(){
		return camera.eye;
	}
	void changeRobat()
	{
	}

private:

	void load_texture_STBImage(std::string file_name, GLuint& m_texName);

	std::vector<GLuint> textures_all;
	std::vector<GLuint> program_all;
	std::vector<GLuint> vao_all;
	std::vector<GLuint> buffer_all;
	std::vector<GLuint> vPosition_all;
	std::vector<GLuint> vColor_all;
	std::vector<GLuint> vTexCoord_all;
	std::vector<GLuint> vNormal_all;
	std::vector<GLuint> theta_all;
	std::vector<GLuint> trans_all;
	
	// std::vector<GLuint> projMatrix_all;		//投影变换矩阵
	// std::vector<GLuint> modelMatrix_all;		//模式变换矩阵
	// std::vector<GLuint> viewMatrix_all;		//视图变换矩阵
	
	std::vector<My_Mesh*> m_my_meshes_;
};

