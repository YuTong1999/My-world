﻿#include "Mesh_Painter.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Mesh_Painter::Mesh_Painter()
{
	for (int i=0; i<6; i++){
		theta_main1[i] = 0;
	}
}

Mesh_Painter::~Mesh_Painter()
{
}

GLfloat hi[6] = {
	2.994709,    // 身体
    2.013148,    // 头
    1.091053,    // 右胳膊
    1.091053,    // 左胳膊
    2.994708,    // 右腿
    2.994708,    // 左腿
};

double getRadian(float a)
{
	return a / 180.0 * 3.1415926;
}

void Mesh_Painter::draw_meshes(float upAngle, float rotateAngle)
{
	const float  Degrees = 3.14159265 / 180.0;
	mat4 modelMatrix = mat4(1.0);

	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{
		//设置相机的位置
		if (this->robot_view)
		{
			camera.eye = vec4(this->tran1.x - 3.1, this->tran1.y +1, this->tran1.z + 0.525, 1);
			camera.up = vec4(0.0, 1.0, 0.0, 0.0);
			camera.at = vec4(this->tran1.x - 3.1 + cos(getRadian(-theta_main1[0] + 90)), this->tran1.y + 1, this->tran1.z + 0.525 + sin(getRadian(-theta_main1[0] + 90)), 1.0);
		}
		else
		{
			float eyex = 0;		// 根据rotateAngle和upAngle设置x
			float eyey = 0;		// 根据upAngle设置y
			float eyez = 0;		// 根据rotateAngle设置z
			eyex = camera.radius * cos(upAngle * M_PI / 180) * sin(rotateAngle * M_PI / 180);
			eyey = camera.radius * sin(upAngle * M_PI / 180);
			eyez = camera.radius * cos(upAngle * M_PI / 180) * cos(rotateAngle * M_PI / 180);

			camera.eye = vec4(eyex, eyey, eyez, 1.0);
			camera.at = vec4(0.0, 0.0, 0.0, 1.0);
			camera.up = vec4(0.0, 1.0, 0.0, 0.0);
		}
		// 指定使用渲染器，不同的模型可以指定不同的渲染器
		// 这里我们使用的渲染器来自相同的文件，学生可以根据自己的爱好对不同的模型设定不同的渲染器
		glUseProgram(this->program_all[i]);
		mat4 viewMatrix = camera.getViewMatrix();				//观察矩阵
		mat4 projMatrix = camera.getProjectionMatrix();		//投影矩阵


		#ifdef __APPLE__
			glBindVertexArrayAPPLE(this->vao_all[i]);
		#else
			glBindVertexArray(this->vao_all[i]);
		#endif

 		glActiveTexture(GL_TEXTURE0);
 		glBindTexture(GL_TEXTURE_2D, this->textures_all[i]);// 该语句必须，否则将只使用同一个纹理进行绘制
		
		float x, y, z;
		this->m_my_meshes_[i]->get_theta(x, y, z);
		GLfloat  Theta[3] = {x, y, z};
		this->m_my_meshes_[i]->add_theta_step();
 		glUniform3fv(theta_all[i], 1, Theta);
		
		this->m_my_meshes_[i]->get_translate(x, y, z);
		GLfloat  vTranslation[3] = {x, y, z};
		glUniform3fv(trans_all[i], 1, vTranslation);

		GLuint modelMatrixID = glGetUniformLocation(this->program_all[i], "modelMatrix");
		// 机器人层级建模
		if(i == 3){
			float cos_y = cos ( Degrees * this->theta_main1[0]);
			float sin_y = sin ( Degrees * this->theta_main1[0]);
			mat4 tr = mat4( vec4(1.0, 0.0, 0.0, 0.0),
						    vec4(0.0, 1.0, 0.0, (-this->m_my_meshes_[i]->m_center_.y + 2.7 * hi[3])/dd[i]),
							vec4(0.0, 0.0, 1.0, 0.0),
							vec4(0.0, 0.0, 0.0, 1.0));
			mat4 ry1 = mat4( vec4(cos_y,  0.0, sin_y,  0.0),
							 vec4(  0.0,  1.0,   0.0,  0.0),
							 vec4(sin_y,  0.0, cos_y,  0.0),
							 vec4(  0.0,  0.0,   0.0,  1.0) );
			mat4 temp = mat4(vec4(1.0, 0.0, 0.0, this->tran1.x - 3),
							 vec4(0.0, 1.0, 0.0, this->tran1.y),
							 vec4(0.0, 0.0, 1.0, this->tran1.z),
							 vec4(0.0, 0.0, 0.0, 1.0));
			modelMatrix = temp * tr * ry1;
			glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, &modelMatrix[0][0]);
			// if(this->robot_view){
			// 	camera.eye = vec4(p, 1.0);
			// 	camera.eye = modelMatrix * camera.eye;
			// }
		}
		else if(i >= 4 && i <= 8){	
			this->mvstack.push(modelMatrix);   // 保存躯干变换矩阵
			float cos_x = cos ( Degrees * this->theta_main1[i-3]);
			float sin_x = sin ( Degrees * this->theta_main1[i-3]);
			mat4 tr;
			if(i == 7 || i == 8){
				tr = mat4( vec4(1.0, 0.0, 0.0, 0.0),
						    vec4(0.0, 1.0, 0.0, (-this->m_my_meshes_[i]->m_center_.y)/dd[i]),
							vec4(0.0, 0.0, 1.0, 0.0),
							vec4(0.0, 0.0, 0.0, 1.0));
				mat4 rx1 = mat4( vec4(1.0,   0.0,    0.0, 0.0),
							 vec4(0.0, cos_x, -sin_x, 0.0),
							 vec4(0.0, sin_x,  cos_x, 0.0),
							 vec4(0.0,   0.0,    0.0, 1.0) );
				modelMatrix *= rx1 * tr;
				mat4 tr1 = mat4( vec4(1.0, 0.0, 0.0, 0.0),
						    vec4(0.0, 1.0, 0.0, (this->m_my_meshes_[i]->m_center_.y)/dd[i]),
							vec4(0.0, 0.0, 1.0, 0.0),
							vec4(0.0, 0.0, 0.0, 1.0));
				modelMatrix = tr1 * modelMatrix;
			}
			else if(i == 5){
				float cos_z = cos ( Degrees * -90);
				float sin_z = sin ( Degrees * -90);
				mat4 rz1 = mat4( cos_z, -sin_z, 0.0, 0.0,
								sin_z, cos_z, 0.0, 0.0,
								0.0, 0.0, 1.0, 0.0,
								0.0, 0.0, 0.0, 1.0);
				modelMatrix *= rz1 * tr;

				tr = mat4( vec4(1.0, 0.0, 0.0, 1/dd[i]),
						    vec4(0.0, 1.0, 0.0, -5.787525/dd[i]),
							vec4(0.0, 0.0, 1.0, 0.0),
							vec4(0.0, 0.0, 0.0, 1.0));
				float cos_y = cos ( Degrees * this->theta_main1[i-3]);
				float sin_y = sin ( Degrees * this->theta_main1[i-3]);

				mat4 ry1 = mat4( cos_y, 0.0, sin_y, 0.0,
								0.0, 1.0, 0.0, 0.0,
								-sin_y, 0.0, cos_y, 0.0,
								0.0, 0.0, 0.0, 1.0 );
				modelMatrix *= ry1 * tr;
				mat4 tr1 = mat4( vec4(1.0, 0.0, 0.0, 0.0),
						    vec4(0.0, 1.0, 0.0, 7.887525/dd[i]),
							vec4(0.0, 0.0, 1.0, 0.0),
							vec4(0.0, 0.0, 0.0, 1.0));
				modelMatrix = tr1 * modelMatrix;
			}
			else if(i == 6){
				float cos_z = cos ( Degrees * 90);
				float sin_z = sin ( Degrees * 90);
				mat4 rz1 = mat4( cos_z, -sin_z, 0.0, 0.0,
								sin_z, cos_z, 0.0, 0.0,
								0.0, 0.0, 1.0, 0.0,
								0.0, 0.0, 0.0, 1.0);
				modelMatrix *= rz1 * tr;

				tr = mat4( vec4(1.0, 0.0, 0.0, -1/dd[i]),
						    vec4(0.0, 1.0, 0.0, -5.787525/dd[i]),
							vec4(0.0, 0.0, 1.0, 0.0),
							vec4(0.0, 0.0, 0.0, 1.0));
				float cos_y = cos ( -Degrees * this->theta_main1[i-3]);
				float sin_y = sin ( -Degrees * this->theta_main1[i-3]);

				mat4 ry1 = mat4( cos_y, 0.0, sin_y, 0.0,
								0.0, 1.0, 0.0, 0.0,
								-sin_y, 0.0, cos_y, 0.0,
								0.0, 0.0, 0.0, 1.0 );
				modelMatrix *= ry1 * tr;
				mat4 tr1 = mat4( vec4(1.0, 0.0, 0.0, 0.0),
						    vec4(0.0, 1.0, 0.0, 7.887525/dd[i]),
							vec4(0.0, 0.0, 1.0, 0.0),
							vec4(0.0, 0.0, 0.0, 1.0));
				modelMatrix = tr1 * modelMatrix;
			}
			else{
				mat4 rx1 = mat4( vec4(1.0,   0.0,    0.0, 0.0),
							 vec4(0.0, cos_x, -sin_x, 0.0),
							 vec4(0.0, sin_x,  cos_x, 0.0),
							 vec4(0.0,   0.0,    0.0, 1.0) );
				modelMatrix *= rx1;
			}
		 	glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, &modelMatrix[0][0]);
			modelMatrix = this->mvstack.pop(); // 恢复躯干变换矩阵
		}
		else if (i==9)	//dog
		{
			glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, &modelMatrix[0][0]);
		}
		else{
			mat4 p = mat4(1.0);
			glUniformMatrix4fv(modelMatrixID, 1, GL_TRUE, &p[0][0]);
		}

		//画模型
		//相机相关的矩阵
		GLuint viewMatrixID = glGetUniformLocation(this->program_all[i], "viewMatrix");
		GLuint projectMatrixID = glGetUniformLocation(this->program_all[i], "projMatrix");
		//光照
		GLuint LightPosition = glGetUniformLocation(this->program_all[i], "LightPosition");
		GLuint AmbientProduct = glGetUniformLocation(this->program_all[i], "AmbientProduct");
		GLuint DiffuseProduct = glGetUniformLocation(this->program_all[i], "DiffuseProduct");
		GLuint SpecularProduct = glGetUniformLocation(this->program_all[i], "SpecularProduct");
		GLuint Shininess = glGetUniformLocation(this->program_all[i], "Shininess");
		GLuint type = glGetUniformLocation(this->program_all[i], "type");
		GLuint shadow = glGetUniformLocation(this->program_all[i], "shadowMatrix");
		//相机
		glUniformMatrix4fv(viewMatrixID, 1, GL_TRUE, &viewMatrix[0][0]);
		glUniformMatrix4fv(projectMatrixID, 1, GL_TRUE, &projMatrix[0][0]);		//把modelMatrix的4*4矩阵传到与modelMatrixID关联的着色器变量中
		//光照
		vec4 ambient = light.ambient * material.ambient;
		vec4 diffuse = light.diffuse * material.diffuse;
		vec4 specular = light.specular * material.specular;
		glUniform4fv(LightPosition,  1, light.position);
		glUniform4fv(AmbientProduct,  1, ambient);
		glUniform4fv(DiffuseProduct,  1, diffuse);
		glUniform4fv(SpecularProduct,  1, specular);
		glUniform1f(Shininess, 1.0);
		glUniform1i(type, 0);
		
		mat4 shadowMatrix(1.0);
		glUniformMatrix4fv(shadow, 1, GL_TRUE, &shadowMatrix[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[i]->num_faces() * 3);

		//画阴影
		vec4 v1(-light.position.y, light.position.x, 0.0, 0);
		vec4 v2(0, 0, 0, 0);
		vec4 v3(0, light.position.z, -light.position.y, 0);
		vec4 v4(0, 1, 0, -light.position.y);
		shadowMatrix = mat4(v1, v2, v3, v4);
		
		glUniformMatrix4fv(shadow, 1, GL_TRUE, &shadowMatrix[0][0]);		//将新的模式矩阵传到着色器中
		glUniform1i(type, 1);
		
		if(i<10)
		glDrawArrays(GL_TRIANGLES, 0, this->m_my_meshes_[i]->num_faces() * 3);

		// glUniform1f(Shininess, light.shiniess);

		glUseProgram(0);
	}
	
};

void Mesh_Painter::update_texture()
{
	this->textures_all.clear();

	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{
		GLuint textures;
		// 调用stb_image生成纹理
		glGenTextures(1, &textures);
		load_texture_STBImage(this->m_my_meshes_[i]->get_texture_file(), textures);
		std::cout << this->m_my_meshes_[i]->get_texture_file() << std::endl;

		// 将生成的纹理传给shader
		glBindTexture(GL_TEXTURE_2D, textures);
		glUniform1i(glGetUniformLocation(this->program_all[i], "texture"), 0);
		this->textures_all.push_back(textures);
	}
};

void My_Mesh::generate_square() {
	this->clear_data();
	vec4 vertices[4];
	vec3 n(0.0, 0.0, 1.0);
	//给出初始顶点位置，后面
	vertices[0] = vec4((vec3(-1.0, -1.0, -1.0)),1.0);
	vertices[1] = vec4(( vec3(1.0, -1.0, -1.0)), 1.0);
	vertices[2] = vec4((vec3(1.0, 1.0, -1.0)), 1.0);
	vertices[3] = vec4((vec3(-1.0, 1.0, -1.0)), 1.0);
	// 顶点坐标信息输入
	// 第一个三角面片的信息
	m_vertices_.push_back(vertices[0].x);
	m_vertices_.push_back(vertices[0].y);
	m_vertices_.push_back(vertices[0].z);
	m_vertices_.push_back(vertices[1].x);
	m_vertices_.push_back(vertices[1].y);
	m_vertices_.push_back(vertices[1].z);
	m_vertices_.push_back(vertices[3].x);
	m_vertices_.push_back(vertices[3].y);
	m_vertices_.push_back(vertices[3].z);

	// 第二个三角面片的信息
	m_vertices_.push_back(vertices[1].x);
	m_vertices_.push_back(vertices[1].y);
	m_vertices_.push_back(vertices[1].z);
	m_vertices_.push_back(vertices[2].x);
	m_vertices_.push_back(vertices[2].y);
	m_vertices_.push_back(vertices[2].z);
	m_vertices_.push_back(vertices[3].x);
	m_vertices_.push_back(vertices[3].y);
	m_vertices_.push_back(vertices[3].z);
	//找到盒子的边界
	this->m_min_box_ = point3f(-1, -1, -1);
	this->m_max_box_ = point3f(1, 1, 1);
	this->m_center_ = point3f(0.0, 0.0, 0);

	//盒子的法向量
	for(int i = 0; i < 6; i++){
		m_normals_.push_back(0.0);
		m_normals_.push_back(0.0);
		m_normals_.push_back(1.0);
	}

	// 颜色信息
	for(int i = 0; i < 18; i++){
		m_color_list_.push_back(0);
	}

	//设置纹理u,v坐标
	//0
	m_vt_list_.push_back(0.0);
	m_vt_list_.push_back(0.0);
	//1
	m_vt_list_.push_back(1.0);
	m_vt_list_.push_back(0.0);
	//3
	m_vt_list_.push_back(0.0);
	m_vt_list_.push_back(1.0);

	//1
	m_vt_list_.push_back(1.0);
	m_vt_list_.push_back(0.0);
	//2
	m_vt_list_.push_back(1.0);
	m_vt_list_.push_back(1.0);
	//3
	m_vt_list_.push_back(0.0);
	m_vt_list_.push_back(1.0);

	//放入面片顶点索引
	//三角形1
	m_faces_.push_back((m_vertices_.size() - 18)/ 3 + 1);		//顶点下标
	m_faces_.push_back((m_vt_list_.size() - 12)/2 + 1);			//纹理坐标
	m_faces_.push_back((m_normals_.size() - 6)/3 + 1);			//该顶点法向坐标
	m_faces_.push_back((m_vertices_.size() - 15)/ 3 + 1);
	m_faces_.push_back((m_vt_list_.size() - 10)/2 + 1);			//纹理坐标
	m_faces_.push_back((m_normals_.size() - 6)/3 + 1);			//该顶点法向坐标
	m_faces_.push_back((m_vertices_.size() - 12)/ 3 + 1);
	m_faces_.push_back((m_vt_list_.size() - 8)/2 + 1);			//纹理坐标
	m_faces_.push_back((m_normals_.size() - 6)/3 + 1);			//该顶点法向坐标
	//三角形2
	m_faces_.push_back((m_vertices_.size() - 9)/ 3 + 1);
	m_faces_.push_back((m_vt_list_.size() - 6)/2 + 1);			//纹理坐标
	m_faces_.push_back((m_normals_.size() - 3)/3 + 1);			//该顶点法向坐标
	m_faces_.push_back((m_vertices_.size() - 6)/ 3 + 1);
	m_faces_.push_back((m_vt_list_.size() - 4)/2 + 1);			//纹理坐标
	m_faces_.push_back((m_normals_.size() - 3)/3 + 1);			//该顶点法向坐标
	m_faces_.push_back((m_vertices_.size() - 3)/ 3 + 1);
	m_faces_.push_back((m_vt_list_.size() - 2)/2 + 1);			//纹理坐标
	m_faces_.push_back((m_normals_.size() - 3)/3 + 1);			//该顶点法向坐标
	//std::cout << "finish square reading" << std::endl;
}

void Mesh_Painter::load_texture_STBImage(std::string file_name, GLuint& m_texName)
{
	int width, height, channels = 0;
	unsigned char *pixels = NULL;
	stbi_set_flip_vertically_on_load(true);
	pixels = stbi_load(file_name.c_str(), &width, &height, &channels, 0);

	// 调整行对齐格式
	if(width*channels%4!=0) glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	GLenum format = GL_RGB;
	// 设置通道格式
	switch (channels){
	case 1: format=GL_RED;break;
	case 3: format=GL_RGB;break;
	case 4: format=GL_RGBA;break;
	default: format=GL_RGB;break;
	}

	// 绑定纹理对象
	glBindTexture(GL_TEXTURE_2D, m_texName);

	// 指定纹理的放大，缩小滤波，使用线性方式，即当图片放大的时候插值方式
	// 将图片的rgb数据上传给opengl
	glTexImage2D(
		GL_TEXTURE_2D,	// 指定目标纹理，这个值必须是GL_TEXTURE_2D
		0,				// 执行细节级别，0是最基本的图像级别，n表示第N级贴图细化级别
		format,			// 纹理数据的颜色格式(GPU显存)
		width,			// 宽度。早期的显卡不支持不规则的纹理，则宽度和高度必须是2^n
		height,			// 高度。早期的显卡不支持不规则的纹理，则宽度和高度必须是2^n
		0,				// 指定边框的宽度。必须为0
		format,			// 像素数据的颜色格式(CPU内存)
		GL_UNSIGNED_BYTE,	// 指定像素数据的数据类型
		pixels			// 指定内存中指向图像数据的指针
	);
	
	// 生成多级渐远纹理，多消耗1/3的显存，较小分辨率时获得更好的效果
	// glGenerateMipmap(GL_TEXTURE_2D);

	// 指定插值方法
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// 恢复初始对齐格式
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	// 释放图形内存
	stbi_image_free(pixels);
};

void Mesh_Painter::update_vertex_buffer()
{
	this->vao_all.clear();
	this->buffer_all.clear();
	this->vPosition_all.clear();
	this->vColor_all.clear();
	this->vTexCoord_all.clear();
	this->vNormal_all.clear();

	// 顶点坐标，法线，颜色，纹理坐标到shader的映射
	for (unsigned int m_i = 0; m_i < this->m_my_meshes_.size(); m_i++)
	{
		int num_face = this->m_my_meshes_[m_i]->num_faces();
		int num_vertex = this->m_my_meshes_[m_i]->num_vertices();

		const VertexList& vertex_list = this->m_my_meshes_[m_i]->get_vertices();
		const NormalList& normal_list = this->m_my_meshes_[m_i]->get_normals();
		const FaceList&  face_list = this->m_my_meshes_[m_i]->get_faces();
		const STLVectorf& color_list = this->m_my_meshes_[m_i]->get_colors();
		const VtList& vt_list = this->m_my_meshes_[m_i]->get_vts();

		// 创建顶点数组对象
		GLuint vao;
		#ifdef __APPLE__
			glGenVertexArraysAPPLE(1, &vao);
			glBindVertexArrayAPPLE(vao);
		#else
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
		#endif

		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER,
			sizeof(vec3)*num_face * 3
			+ sizeof(vec3)*num_face * 3
			+ sizeof(vec3)*num_face * 3
			+ sizeof(vec2)*num_face * 3, NULL, GL_STATIC_DRAW);

		// ------------------------------------------------------------------------
		// 获得足够的空间存储坐标，颜色，法线以及纹理坐标等，并将它们映射给shader
		// Specify an offset to keep track of where we're placing data in our
		// vertex array buffer.  We'll use the same technique when we
		// associate the offsets with vertex attribute pointers.
		GLintptr offset = 0;
		point3f  p_center_ = this->m_my_meshes_[m_i]->get_center();
		point3f p_min_box_, p_max_box_;
		this->m_my_meshes_[m_i]->get_boundingbox(p_min_box_, p_max_box_);
		float d = p_min_box_.distance(p_max_box_);
		dd[m_i] = d;

		// if (p_min_box_.y < 0)
		// {
		// 	p_center_.y += -p_min_box_.y;
		// 	for (int i=0; i<vertex_list.size()/3; i++)
		// 	{
		// 		vertex_list[3 * i + 1] += -p_min_box_.y;
		// 	}
		// }

		// -------------------- 实现顶点到shader的映射 ------------------------------
		p_center_ = point3f(0.0, 0.0, 0.0);
		//std::cout << "s1" << std::endl;
		vec3* points = new vec3[num_face * 3];
		if(m_i == 0)			//放大城堡
		 	d = d/10;
		else if(m_i == 9)		//缩小狗
			d = d*2;
		else if(m_i >= 10 && m_i <= 15)	//放大天空盒
			d = 0.1;
		if(m_i == 3){
			for (int i = 0; i < num_face; i++)
			{
				// @TODO: 参考实验4.1，在这里添加代码实现顶点坐标到shader的映射
				//-1因为面片顶点下标表示时，是从1开始的但是读入的时候顶点下标是从0开始的
				int index = face_list[9 * i] - 1;
				points[3 * i] = vec3((vertex_list[index * 3 + 0] - p_center_.x) / d,
									(vertex_list[index * 3 + 1] - p_center_.y + (p_max_box_.y - p_min_box_.y)/2) / d,
									(vertex_list[index * 3 + 2] - p_center_.z) / d);

				index = face_list[9 * i + 3] - 1;
				points[3 * i + 1] = vec3((vertex_list[index * 3 + 0] - p_center_.x) / d,
										(vertex_list[index * 3 + 1] - p_center_.y + (p_max_box_.y - p_min_box_.y)/2) / d,
										(vertex_list[index * 3 + 2] - p_center_.z) / d);
							
				index = face_list[9 * i + 6] - 1;
				points[3 * i + 2] = vec3((vertex_list[index * 3 + 0] - p_center_.x) / d,
										(vertex_list[index * 3 + 1] - p_center_.y + (p_max_box_.y - p_min_box_.y)/2) / d,
										(vertex_list[index * 3 + 2] - p_center_.z) / d);
			}
		}
		for (int i = 0; i < num_face; i++)
		{
			// @TODO: 参考实验4.1，在这里添加代码实现顶点坐标到shader的映射
			//-1因为面片顶点下标表示时，是从1开始的但是读入的时候顶点下标是从0开始的
			int index = face_list[9 * i] - 1;
			points[3 * i] = vec3((vertex_list[index * 3 + 0] - p_center_.x) / d,
								(vertex_list[index * 3 + 1] - p_center_.y + (p_max_box_.y - p_min_box_.y)/2) / d,
								(vertex_list[index * 3 + 2] - p_center_.z) / d);

			index = face_list[9 * i + 3] - 1;
			points[3 * i + 1] = vec3((vertex_list[index * 3 + 0] - p_center_.x) / d,
									(vertex_list[index * 3 + 1] - p_center_.y + (p_max_box_.y - p_min_box_.y)/2) / d,
									(vertex_list[index * 3 + 2] - p_center_.z) / d);
						
			index = face_list[9 * i + 6] - 1;
			points[3 * i + 2] = vec3((vertex_list[index * 3 + 0] - p_center_.x) / d,
									(vertex_list[index * 3 + 1] - p_center_.y + (p_max_box_.y - p_min_box_.y)/2) / d,
									(vertex_list[index * 3 + 2] - p_center_.z) / d);
		}
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3)*num_face * 3, points);
		offset += sizeof(vec3) * num_face * 3;
		delete[] points;
		// ------------------------------------------------------------------------
		
		// -------------------- 实现法线到shader的映射 ------------------------------
		//std::cout << "s2" << " " << num_face << std::endl;
		points = new vec3[num_face * 3];
		for (int i = 0; i < num_face; i++)
		{
			// @TODO: 参考实验4.1，在这里添加代码实现法线到shader的映射
			int index = face_list[9 * i + 2] - 1;
			points[3 * i] = vec3((normal_list[index * 3 + 0]),
								(normal_list[index * 3 + 1]),
								(normal_list[index * 3 + 2]));
			
			index = face_list[9 * i + 5] - 1;
			points[3 * i + 1] = vec3((normal_list[index * 3 + 0]),
									(normal_list[index * 3 + 1]),
									(normal_list[index * 3 + 2]));
			
			index = face_list[9 * i + 8] - 1;
			points[3 * i + 2] = vec3((normal_list[index * 3 + 0]),
									(normal_list[index * 3 + 1]),
									(normal_list[index * 3 + 2]));
		}
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * num_face * 3, points);
		offset += sizeof(vec3) * num_face * 3;
		delete[] points;
		// ------------------------------------------------------------------------

		// -------------------- 实现颜色到shader的映射 ------------------------------
		//::cout << "s3" << std::endl;
		points = new vec3[num_face * 3];
		for (int i = 0; i < num_face; i++)
		{
			// @TODO: 参考实验4.1，在这里添加代码实现颜色到shader的映射
			int index = face_list[9 * i + 2] - 1;
			points[i * 3] = vec3(color_list[index * 3 + 0], color_list[index * 3 + 1], color_list[index * 3 + 2]);
			index = face_list[9 * i + 5] - 1;
			points[i * 3 + 1] = vec3(color_list[index * 3 + 0], color_list[index * 3 + 1], color_list[index * 3 + 2]);
			index  = face_list[9 * i + 8] - 1;
			points[i * 3 + 2] = vec3(color_list[index * 3 + 0], color_list[index * 3 + 1], color_list[index * 3 + 2]);
		}
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec3) * num_face * 3, points);
		offset += sizeof(vec3) * num_face * 3;
		delete[] points;
		// ------------------------------------------------------------------------

		// ------------------- 实现纹理坐标到shader的映射 -----------------------------
		//std::cout << "s4" << std::endl;
		vec2* points_2 = new vec2[num_face * 3];
		for (int i = 0; i < num_face; i++)
		{
			// @TODO: 参考实验4.1，在这里添加代码实现纹理坐标到shader的映射
			int index = face_list[9 * i + 1] - 1;
			points_2[3 * i] = vec2((vt_list[index * 2 + 0]),
								(vt_list[index * 2 + 1]));

			index = face_list[9 * i + 4] - 1;
			points_2[3 * i + 1] = vec2((vt_list[index * 2 + 0]),
									(vt_list[index * 2 + 1]));
			
			index = face_list[9 * i + 7] - 1;
			points_2[3 * i + 2] = vec2((vt_list[index * 2 + 0]),
									(vt_list[index * 2 + 1]));
		}
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(vec2) * num_face * 3, points_2);
		offset += sizeof(vec2) * num_face * 3;
		delete[] points_2;
		// ------------------------------------------------------------------------

		//std::cout << "s5" << std::endl;
		// 加载着色器并使用生成的着色器程序
		offset = 0;
		// 指定vPosition在shader中使用时的位置
		GLuint vPosition;
		vPosition = glGetAttribLocation(this->program_all[m_i], "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
		offset += sizeof(vec3) * num_face * 3;

		// 指定vNormal在shader中使用时的位置
		GLuint vNormal;
		vNormal = glGetAttribLocation(this->program_all[m_i], "vNormal");
		glEnableVertexAttribArray(vNormal);
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
		offset += sizeof(vec3) * num_face * 3;
		
		// 指定vColor在shader中使用时的位置
		GLuint vColor;
		vColor = glGetAttribLocation(this->program_all[m_i], "vColor");
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));
		offset += sizeof(vec3) * num_face * 3;
		
		//指定vTexCoord在shader中使用时的位置
		GLuint vTexCoord;
		vTexCoord = glGetAttribLocation(this->program_all[m_i], "vTexCoord");
		glEnableVertexAttribArray(vTexCoord);
		glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));

		this->vao_all.push_back(vao);
		this->buffer_all.push_back(buffer);
		this->vPosition_all.push_back(vPosition);
		this->vColor_all.push_back(vColor);
		this->vTexCoord_all.push_back(vTexCoord);
		this->vNormal_all.push_back(vNormal);

		//std::cout << "s6" << std::endl;
	}
};

void Mesh_Painter::init_shaders(std::string vs, std::string fs)
{
	this->program_all.clear();
	this->theta_all.clear();
	this->trans_all.clear();
	for (unsigned int i = 0; i < this->m_my_meshes_.size(); i++)
	{
		GLuint program = InitShader(vs.c_str(), fs.c_str());
		this->program_all.push_back(program);

		GLuint 	theta = glGetUniformLocation(program, "theta");
		GLuint  trans = glGetUniformLocation(program, "translation");

		theta_all.push_back(theta);
		trans_all.push_back(trans);
	}

};

void Mesh_Painter::add_mesh(My_Mesh* m)
{
	this->m_my_meshes_.push_back(m);
};

void Mesh_Painter::clear_mehs()
{
	this->m_my_meshes_.clear();

	this->textures_all.clear();
	this->program_all.clear();
	this->vao_all.clear();
	this->buffer_all.clear();
	this->vPosition_all.clear();
	this->vColor_all.clear();
	this->vTexCoord_all.clear();
	this->vNormal_all.clear();
};
