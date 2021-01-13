#include "mesh.h"
#include <sstream>
#include <fstream>
#include <iosfwd>
#include <algorithm>
#include <math.h>
#include <array>
#include <vector>

#ifdef __APPLE__
	#include <GLUT/glut.h>
#else
	#include <gl/GL.h>
#endif

//自定义最大最小值
#define MIN -99999
#define MAX 99999

My_Mesh::My_Mesh()
{
	vTranslation[0] = Theta[0] = 0;
	vTranslation[1] = Theta[1] = 0;
	vTranslation[2] = Theta[2] = 0;
	Theta[0] = 45;
}

My_Mesh::~My_Mesh()
{
}

void My_Mesh::load_obj(std::string obj_File)
{
	GLfloat x_min = MAX, y_min = MAX, z_min = MAX;
	GLfloat x_max = MIN, y_max = MIN, z_max = MIN;
	char ch;
	int aa, bb, cc, a1[4], b1[4], c1[4];
	// @TODO: 请在此添加代码实现对含有UV坐标的obj文件的读取
	// 将读取的数据存储为My_Mesh对象的属性值，可参考圆柱体的生成
	std::string str;
	std::ifstream fin(obj_File);
	if (fin.eof()){
		std::cout << "cannot read file" << str << std::endl;
		exit(0);
	}

	GLfloat x, y, z, r, g, b;
	
	while(fin >> str){
		//读入顶点坐标
		if(str=="v"){
			while(str == "v"){
				fin >> x >> y >> z;
				m_vertices_.push_back(x);
				m_vertices_.push_back(y);
				m_vertices_.push_back(z);

				x_min = (x < x_min) ? x  : x_min;
				y_min = (y < z_min) ? y  : y_min;
				z_min = (z < z_min) ? z  : z_min;

				x_max = (x > x_max) ? x  : x_max;
				y_max = (y > z_max) ? y  : y_max;
				z_max = (z > z_max) ? z  : z_max;
				fin >> str;
			}
			std::cout << "v" << std::endl;
		}

		//算出方块的最大最小值和中心
		this->m_min_box_ = point3f(x_min, y_min, z_min);
		this->m_max_box_ = point3f(x_max, y_max, z_max);
		this->m_center_ = point3f((x_min + x_max)/2, (y_min + y_max)/2, (z_min + z_max)/2);
		// std::cout << "min: " << x_min << ' ' << y_min << ' ' << z_min << ' ' << std::endl;
		// std::cout << "max: " << x_max << ' ' << y_max << ' ' << z_max << ' ' << std::endl;

		//读入法向坐标
		if(str == "vn"){
			while(str == "vn"){
				fin >> x >> y >> z;
				normal_to_color(x, y, z, r, g, b);
				m_normals_.push_back(x);
				m_normals_.push_back(y);
				m_normals_.push_back(z);
				// 这里采用法线来生成颜色，学生可以自定义自己的颜色生成方式
				m_color_list_.push_back(r);
				m_color_list_.push_back(g);
				m_color_list_.push_back(b);
				fin >> str;
			}
			std::cout << "vn" << std::endl;
		}

		//读入贴图坐标
		else if(str=="vt"){
			while(str == "vt"){
				fin >> x >> y;
				m_vt_list_.push_back(x);
				m_vt_list_.push_back(y);
				fin >> str;
			}
			std::cout << "vt" << std::endl;
		}

		else if(str == "f"){
			while(str == "f"){
				if(obj_File == "texture/dog.obj"){
					fin >> a1[0] >> ch >> b1[0] >> ch >> c1[0];
					fin >> a1[1] >> ch >> b1[1] >> ch >> c1[1];
					fin >> a1[2] >> ch >> b1[2] >> ch >> c1[2];
					fin >> a1[3] >> ch >> b1[3] >> ch >> c1[3];
					
					m_faces_.push_back(a1[0]);
					m_faces_.push_back(b1[0]);
					m_faces_.push_back(c1[0]);
					m_faces_.push_back(a1[1]);
					m_faces_.push_back(b1[1]);
					m_faces_.push_back(c1[1]);
					m_faces_.push_back(a1[2]);
					m_faces_.push_back(b1[2]);
					m_faces_.push_back(c1[2]);

					
					m_faces_.push_back(a1[0]);
					m_faces_.push_back(b1[0]);
					m_faces_.push_back(c1[0]);
					m_faces_.push_back(a1[2]);
					m_faces_.push_back(b1[2]);
					m_faces_.push_back(c1[2]);
					m_faces_.push_back(a1[3]);
					m_faces_.push_back(b1[3]);
					m_faces_.push_back(c1[3]);
				}
				else{
					for(int i = 0; i < 3; i++){
							fin >> aa >> ch >> bb >> ch >> cc;
							m_faces_.push_back(aa);
							m_faces_.push_back(bb);
							m_faces_.push_back(cc);
							/*x_min = (m_vertices_[(aa-1)/3] < x_min) ? m_vertices_[(aa-1)/3]  : x_min;
							y_min = (m_vertices_[(aa-1)/3 + 1] < z_min) ? m_vertices_[(aa-1)/3 + 1]  : y_min;
							z_min = (m_vertices_[(aa-1)/3 + 2] < z_min) ? m_vertices_[(aa-1)/3 + 2]  : z_min;

							x_max = (m_vertices_[(aa-1)/3] > x_max) ? m_vertices_[(aa-1)/3]  : x_max;
							y_max = (m_vertices_[(aa-1)/3 + 1] > z_max) ? m_vertices_[(aa-1)/3 + 1]  : y_max;
							z_max = (m_vertices_[(aa-1)/3 + 2] > z_max) ? m_vertices_[(aa-1)/3 + 2]  : z_max;*/
					}
				}
				fin >> str;
			}
			std::cout << "f" << std::endl;
		}
		
	}std::cout << obj_File << ": " << std::endl;
	std::cout << "center: " << m_center_.x << ' ' << m_center_.y << ' ' << m_center_.z << ' ' << std::endl;

	// save to txt file
	


};

void My_Mesh::normal_to_color(float nx, float ny, float nz, float& r, float& g, float& b)
{
	r = float(std::min(std::max(0.5 * (nx + 1.0), 0.0), 1.0));
	g = float(std::min(std::max(0.5 * (ny + 1.0), 0.0), 1.0));
	b = float(std::min(std::max(0.5 * (nz + 1.0), 0.0), 1.0));
};

const VtList&  My_Mesh::get_vts()
{
	return this->m_vt_list_;
};

void My_Mesh::clear_data()
{
	m_vertices_.clear();		//顶点
	m_normals_.clear();			//法向量
	m_faces_.clear();			//面片
	m_color_list_.clear();		//颜色
	m_vt_list_.clear();			//纹理坐标
};

void My_Mesh::get_boundingbox(point3f& min_p, point3f& max_p) const
{
	min_p = this->m_min_box_;
	max_p = this->m_max_box_;
};

const STLVectorf&  My_Mesh::get_colors()
{
	return this->m_color_list_;
};

const VertexList& My_Mesh::get_vertices()
{
	return this->m_vertices_;
};

const NormalList& My_Mesh::get_normals()
{
	return this->m_normals_;
};

const FaceList&   My_Mesh::get_faces()
{
	return this->m_faces_;
};

int My_Mesh::num_faces()
{
	return this->m_faces_.size()/9;
};

int My_Mesh::num_vertices()
{
	return this->m_vertices_.size()/3;
};

const point3f& My_Mesh::get_center()
{
	return this->m_center_;
};

void My_Mesh::set_texture_file(std::string s)
{
	this->texture_file_name = s;
};

std::string My_Mesh::get_texture_file()
{
	return this->texture_file_name;
};

void My_Mesh::set_translate(float x, float y, float z)
{
	vTranslation[0] = x;
	vTranslation[1] = y;
	vTranslation[2] = z;

};
void My_Mesh::get_translate(float& x, float& y, float& z)
{
	x = vTranslation[0];
	y = vTranslation[1];
	z = vTranslation[2];
};

void My_Mesh::translate(float x, float y, float z)
{
	for(int i = 0; i < this->m_vertices_.size()/3; i++){
		this->m_vertices_[3 * i + 0] += x;
		this->m_vertices_[3 * i + 1] += y;
		this->m_vertices_[3 * i + 2] += z;
	}
};

void My_Mesh::set_theta(float x, float y, float z)
{
	Theta[0] = x;
	Theta[1] = y;
	Theta[2] = z;
};
void My_Mesh::get_theta(float& x, float& y, float& z)
{
	x = Theta[0];
	y = Theta[1];
	z = Theta[2];
};

void My_Mesh::theta(float x, float y, float z)
{
	for(int i = 0; i < this->m_vertices_.size()/3; i++){
		
		const float  DegreesToRadians = 3.14159265 / 180.0;
		vec3 theta_ = vec3(x, y, z);
		vec3 c;
		c.x = cos( DegreesToRadians * theta_.x );
		c.y = cos( DegreesToRadians * theta_.y );
		c.z = cos( DegreesToRadians * theta_.z );
		vec3 s;
		s.x = sin( DegreesToRadians * theta_.x );
		s.y = sin( DegreesToRadians * theta_.y );
		s.z = sin( DegreesToRadians * theta_.z );

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

		float x_ = this->m_vertices_[3 * i + 0];
		float y_ = this->m_vertices_[3 * i + 1];
		float z_ = this->m_vertices_[3 * i + 2];

		vec4 point = vec4(x_, y_, z_, 1.0);
		point =  rz * ry * rx * point;

		this->m_vertices_[3 * i + 0] = point.x;
		this->m_vertices_[3 * i + 1] = point.y;
		this->m_vertices_[3 * i + 2] = point.z;
	}
};

void My_Mesh::set_theta_step(float x, float y, float z)
{
	Theta_step[0] = x;
	Theta_step[1] = y;
	Theta_step[2] = z;
};

void My_Mesh::add_theta_step()
{
	Theta[0] = Theta[0] + Theta_step[0];
	Theta[1] = Theta[1] + Theta_step[1];
	Theta[2] = Theta[2] + Theta_step[2];
};