//地面上没有阴影，机器人动是扭曲的，天空盒子会出现空白和扭曲

#include "Angel.h"
#include "mesh.h"
#include <assert.h>
#include "Mesh_Painter.h"
#include "TriMesh.h"
#include <string>

float rotateAngle = 0;
float upAngle = 30;
int flag = 1;
GLuint programID;

// 窗口大小
struct Window
{
	int width = 800;
	int height = 800;
};
Window window;

int theta_main[6] = {
    0,    // 身体
    0,    // 头
    0,    // 右胳膊
    0,    // 左胳膊
    0,    // 右腿
    0,    // 左腿
};
vec3 tran(0.0, 0.0, 0.0);

//进入窗口时鼠标的最初位置
float lastX = window.width/2;
float lastY = window.height/2;
float PI = 3.1419926;

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

// 存储要贴纹理的物体
std::vector<My_Mesh*>	my_meshs;
// 存储纹理图片
Mesh_Painter* mp_;

void getRadius()
{

}

void init()
{
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_LIGHTING );
	//背景颜色白色
    glClearColor(1.0, 1.0, 1.0, 1.0 );
}

void change_flag()
{
	if(flag == 1)
		flag = -1;
	else
		flag = 1;
}

My_Mesh* my_mesh5;

void display()
{
#ifdef __APPLE__ // 解决 macOS 10.15 显示画面缩小问题
	glViewport(0, 0, window.width * 2, window.height * 2);
#endif
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//画出模型
	mp_->draw_meshes(upAngle, rotateAngle);
	glutSwapBuffers();
};

void mouse(int button, int state, int x, int y)
{
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		float xoffset = x - lastX;
		float yoffset = lastY - y;
		lastX = x;
		lastY = y;

		float sensitivity = 0.08;		//调整视角移动的快慢
		xoffset *= sensitivity;
		yoffset *= sensitivity;
		rotateAngle += xoffset;
		if(upAngle < 90.0f || upAngle > -90.0f)
			upAngle += yoffset;
		vec4 temp;
		temp.x = cos(rotateAngle) * cos(upAngle);
		temp.y = sin(upAngle);
		temp.z = sin(rotateAngle) * cos(upAngle);
		temp.w = 1.0;
		temp = normalize(temp);

		mp_->set_at(temp);
	}
}

void idle(void)
{ 
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
	float cameraspeed = 0.5f;
	switch (key)
	{
	// 通过按键改变相机的前后位置
	case 'z': mp_->camera.radius = mp_->camera.radius - cameraspeed;		
			  break;	
	case 'x': mp_->camera.radius = mp_->camera.radius + cameraspeed;
			  break;
	// 机器人行走
	case 'a': 
			theta_main[0] += 90;
			if (theta_main[0]>360) {theta_main[0]-=360;}
			mp_->set_th(theta_main);
			break;
	case 'd': 
			theta_main[0] -= 90;
			if (theta_main[0]<0) {theta_main[0]+=360;}
			mp_->set_th(theta_main);
			break;
	case 'w': 
			if(theta_main[2] >= 25 || theta_main[2] <= -25)
			  	change_flag();
			//右胳膊左腿向前旋转
			theta_main[2] += flag * 5;
			theta_main[5] += flag * 5;
			//左胳膊右腿向后旋转
			theta_main[3] -= flag * 5;
			theta_main[4] -= flag * 5;
			mp_->set_th(theta_main);
			if (theta_main[0] % 360 == 0){tran.z += 0.05; mp_->p.z += 0.05;}
			else if (theta_main[0] % 360 == 180){tran.z -= 0.05; mp_->p.z -= 0.05;}
			else if (theta_main[0] % 360 == 90 ){tran.x += 0.05; mp_->p.x += 0.05;}
			else if (theta_main[0] % 360 == 270){tran.x -= 0.05; mp_->p.x -= 0.05;}
			mp_->set_tran(tran);
			std::cout << "head: " << my_mesh5->get_center().x << " " << my_mesh5->get_center().y << " " << my_mesh5->get_center().z << std::endl;
			std::cout << "camera: " << mp_->camera.eye.x << " " << mp_->camera.eye.y << " " << mp_->camera.eye.z << std::endl;
			break;
	case 's': 
			if(theta_main[2] >= 25 || theta_main[2] <= -25)
			  	change_flag();
			//右胳膊左腿向前旋转
			theta_main[2] += flag * 5;
			theta_main[5] += flag * 5;
			//左胳膊右腿向后旋转
			theta_main[3] -= flag * 5;
			theta_main[4] -= flag * 5;
			mp_->set_th(theta_main);
			if (theta_main[0] % 360 == 0){tran.z -= 0.05; mp_->p.z -= 0.05;}
			else if (theta_main[0] % 360 == 180){tran.z += 0.05; mp_->p.z += 0.05;}
			else if (theta_main[0] % 360 == 90 ){tran.x -= 0.05; mp_->p.x -= 0.05;}
			else if (theta_main[0] % 360 == 270){tran.x += 0.05; mp_->p.x += 0.05;}
			mp_->set_tran(tran);
	std::cout << "head: " << my_mesh5->get_center().x << " " << my_mesh5->get_center().y << " " << my_mesh5->get_center().z << std::endl;
			break;
	//改变光源位置
	case 'i': mp_->light.position.x++;
			  break;	
	case 'j': mp_->light.position.x--; 
			  break;
	case 'o': mp_->light.position.y++;
			  break;
	case 'k': mp_->light.position.y--;
			  break;
	case 'p': mp_->light.position.z++;	
			  break;
	case 'l': mp_->light.position.z--;
			  break;
	//改变相机的角度
	case 'f': rotateAngle--;		//向左转
			  break;	
	case 'h': rotateAngle++; 		//向右转
			  break;
	case 'g': if(upAngle > -89){	//向下转
				upAngle--;
			}
			  break;
	case 't': if(upAngle < 89){		//向上转
				upAngle++;
			}
			  break;
	//视角切换
	case 'n': mp_->robot_view = true;
			  break;
	// 空格键初始化所有参数
	case ' ': 
		//相机初始化
		mp_->robot_view = false;
		mp_->camera.radius = 9.0;
		mp_->camera.fov = 60.0;
		mp_->camera.aspect = 1.0;
		mp_->camera.near = 0.1;
		mp_->camera.far = 100.0;
		mp_->camera.eye = vec4(0, 0, 1.0, 1.0);		//VRP参考点
		mp_->camera.at = vec4(0.0, 0.0, 0.0, 1.0);	//VPN
		mp_->camera.up = vec4(0, 1, 0, 0);			//VUP
		rotateAngle = 0.0; 
		upAngle = 30.0;
		//光源位置初始化
		mp_->light.position = vec4(1.0, 100.0, 1.0, 1.0);
		break;
	}
	glutPostRedisplay();
}

int main( int argc, char **argv )
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(window.width, window.height);
	glutCreateWindow("2017301020_候雨彤_期末大作业");

	#ifdef WIN32
		glutInitContextVersion(3, 2);
		glutInitContextProfile(GLUT_CORE_PROFILE);
		glewExperimental = GL_TRUE;
		glewInit();
	#endif
	init();

	#ifdef __APPLE__ // 解决 macOS 10.15 显示画面缩小问题
		glViewport(0, 0, window.width * 2, window.height * 2);
	#endif
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glUseProgram(programID);

	mp_ = new Mesh_Painter;
	
	// @TODO: 参考实验4.1，读取wawa.obj和table.obj模型并贴图、设置绕y轴旋转
	// 房子
	My_Mesh* my_mesh1 = new My_Mesh;
	my_mesh1->load_obj("texture/cartooncastle.obj");			// 生成城堡
	my_mesh1->set_texture_file("texture/cartooncastle.jpg");	// 指定纹理图像文件
	my_mesh1->set_translate(1.0, -1.5, 1.0);						// 平移
	my_mesh1->set_theta(0, -90, 0);							// 旋转轴
	my_mesh1->set_theta_step(0, 0, 0);						// 旋转速度
	mp_->add_mesh(my_mesh1);
	my_meshs.push_back(my_mesh1);

	//蘑菇屋高
	My_Mesh* my_mesh2 = new My_Mesh;
	my_mesh2->load_obj("texture/toadstoolhouse.obj");		// 生成蘑菇屋1
	my_mesh2->set_texture_file("texture/toadstoolhouse.jpg");// 指定纹理图像文件
	my_mesh2->set_theta(0, 0, 0);							// 旋转轴
	my_mesh2->set_translate(-3.6, 0.01, 3.1);					// 平移
	my_mesh2->set_theta_step(0, 0, 0);						// 旋转速度
	mp_->add_mesh(my_mesh2);
	my_meshs.push_back(my_mesh2);

	//蘑菇屋矮
	My_Mesh* my_mesh3 = new My_Mesh;
	my_mesh3->load_obj("texture/toadstoolhouse2.obj");		// 生成蘑菇屋2
	my_mesh3->set_texture_file("texture/toadstoolhouse2.jpg");// 指定纹理图像文件
	my_mesh3->set_translate(-3.6, -0.14, -2.1);						// 平移
	my_mesh3->set_theta(0, 0, 0);							// 旋转轴
	my_mesh3->set_theta_step(0, 0, 0);						// 旋转速度
	mp_->add_mesh(my_mesh3);
	my_meshs.push_back(my_mesh3);

	//机器人身体
	My_Mesh* my_mesh4 = new My_Mesh;
	my_mesh4->load_obj("texture/Steve_Body.obj");			// 生成身体
	my_mesh4->set_texture_file("texture/Steve.png");		// 指定纹理图像文件
	my_mesh4->set_translate(0, 0, 0);				// 平移
	my_mesh4->set_theta(0, 90, 0);							// 旋转轴
	my_mesh4->set_theta_step(0, 0, 0);						// 旋转速度
	mp_->add_mesh(my_mesh4);	
	my_meshs.push_back(my_mesh4);

	//机器人头
	my_mesh5 = new My_Mesh;
	my_mesh5->load_obj("texture/Steve_Head.obj");			// 生成头
	my_mesh5->set_texture_file("texture/Steve.png");		// 指定纹理图像文件
	my_mesh5->set_translate(0, 0, 0);				// 平移
	my_mesh5->set_theta(0, 90, 0);							// 旋转轴
	my_mesh5->set_theta_step(0, 0, 0);						// 旋转速度
	mp_->add_mesh(my_mesh5);
	my_meshs.push_back(my_mesh5);

	//机器人右胳膊
	My_Mesh* my_mesh6 = new My_Mesh;
	my_mesh6->load_obj("texture/Steve_RArm.obj");			// 生成左胳膊
	my_mesh6->set_texture_file("texture/Steve.png");
	//point3f c = my_mesh6->get_center();		// 指定纹理图像文件
	my_mesh6->set_translate(0, 0, 0.0);				// 平移
	my_mesh6->set_theta(0, 90, 0);
	//my_mesh6->translate(0.0, -c.y, 0.0);							// 旋转轴
	my_mesh6->set_theta_step(0, 0, 0);						// 旋转速度
	mp_->add_mesh(my_mesh6);
	my_meshs.push_back(my_mesh6);

	//机器人左胳膊
	My_Mesh* my_mesh7 = new My_Mesh;
	my_mesh7->load_obj("texture/Steve_LArm.obj");			// 生成右胳膊
	my_mesh7->set_texture_file("texture/Steve.png");		// 指定纹理图像文件
	my_mesh7->set_translate(0, 0, 0.0);					// 平移
	my_mesh7->set_theta(0, 90, 0);							// 旋转轴
	my_mesh7->set_theta_step(0, 0, 0);						// 旋转速度
	mp_->add_mesh(my_mesh7);
	my_meshs.push_back(my_mesh7);

	//机器人右腿
	My_Mesh* my_mesh8 = new My_Mesh;
	my_mesh8->load_obj("texture/Steve_RLeg.obj");			// 生成右腿
	my_mesh8->set_texture_file("texture/Steve.png");		// 指定纹理图像文件
	my_mesh8->set_translate(0, 0, 0);				// 平移
	my_mesh8->set_theta(0, 90, 0);							// 旋转轴
	my_mesh8->set_theta_step(0, 0, 0);						// 旋转速度
	mp_->add_mesh(my_mesh8);
	my_meshs.push_back(my_mesh8);

	//机器人左腿
	My_Mesh* my_mesh9 = new My_Mesh;
	my_mesh9->load_obj("texture/Steve_LLeg.obj");			// 生成左腿
	my_mesh9->set_texture_file("texture/Steve.png");		// 指定纹理图像文件
	my_mesh9->set_translate(0, 0, 0);				// 平移
	my_mesh9->set_theta(0,90, 0);							// 旋转轴
	my_mesh9->set_theta_step(0, 0, 0);						// 旋转速度
	mp_->add_mesh(my_mesh9);
	my_meshs.push_back(my_mesh9);

	// 狗
	My_Mesh* my_mesh10 = new My_Mesh;
	my_mesh10->load_obj("texture/dog.obj");			// 生成房子
	my_mesh10->set_texture_file("texture/dog.jpg");	// 指定纹理图像文件
	my_mesh10->set_translate(-0.5, 0.3, 0.0);					// 平移
	my_mesh10->set_theta(90, 0, 0);					// 旋转轴
	my_mesh10->set_theta_step(0, 0, 0);					// 旋转速度
	mp_->add_mesh(my_mesh10);
	my_meshs.push_back(my_mesh10);

	//初始化天空盒子
	//前面
	float skysize = 800;
	My_Mesh* my_mesh11 = new My_Mesh;
	my_mesh11->generate_square();
	my_mesh11->set_texture_file("texture/hills_ft.tga");
	my_mesh11->set_theta(0, 0, 0);
	my_mesh11->set_translate(0.05, -6, 0.0);					// 平移
	my_mesh11->set_theta_step(0, 0, 0);					// 旋转速度
	my_meshs.push_back(my_mesh11);
	mp_->add_mesh(my_mesh11);

	//posz后面
	My_Mesh* my_mesh12 = new My_Mesh;
	my_mesh12->generate_square();
	my_mesh12->set_texture_file("texture/hills_bk.tga");
	my_mesh12->set_theta(0, 180, 0);
	my_mesh12->set_translate(0, -6, 0.0);					// 平移
	my_mesh12->set_theta_step(0, 0, 0);					// 旋转速度
	my_meshs.push_back(my_mesh12);
	mp_->add_mesh(my_mesh12);

	//posx左面
	My_Mesh* my_mesh13 = new My_Mesh;
	my_mesh13->generate_square();
	my_mesh13->set_texture_file("texture/hills_lf.tga");
	my_mesh13->set_theta(0, 90, 0);
	my_mesh13->set_translate(0, -6, 0.0);					// 平移
	my_mesh13->set_theta_step(0, 0, 0);					// 旋转速度
	my_meshs.push_back(my_mesh13);
	mp_->add_mesh(my_mesh13);

	//negx右面
	My_Mesh* my_mesh14 = new My_Mesh;
	my_mesh14->generate_square();
	my_mesh14->set_texture_file("texture/hills_rt.tga");
	my_mesh14->set_theta(0, 270, 0);
	my_mesh14->set_translate(0.1, -6, 0.0);					// 平移
	my_mesh14->set_theta_step(0, 0, 0);					// 旋转速度
	my_meshs.push_back(my_mesh14);
	mp_->add_mesh(my_mesh14);

	//posy下面
	My_Mesh* my_mesh15 = new My_Mesh;
	my_mesh15->generate_square();
	my_mesh15->set_texture_file("texture/grass.png");
	my_mesh15->set_theta(90, 90, 0);
	my_mesh15->set_translate(-10, 10.0-0.001, 0.0);					// 平移
	my_mesh15->set_theta_step(0, 0, 0);					// 旋转速度
	my_meshs.push_back(my_mesh15);
	mp_->add_mesh(my_mesh15);
	std::cout << "tiankonghe: " << my_mesh15->m_center_.y << std::endl;

	//negy上面
	My_Mesh* my_mesh16 = new My_Mesh;
	my_mesh16->generate_square();
	my_mesh16->set_texture_file("texture/hills_up.tga");
	my_mesh16->set_theta(-90, 0, 0);
	my_mesh16->set_translate(0, 4, -10);					// 平移
	my_mesh16->set_theta_step(0, 0, 0);					// 旋转速度
	my_meshs.push_back(my_mesh16);
	mp_->add_mesh(my_mesh16);

	std::string vshader, fshader;
	#ifdef __APPLE__
		vshader = "shaders/vshader_mac.glsl";
		fshader = "shaders/fshader_mac.glsl";
	#else
		vshader = "shaders/vshader_win.glsl";
		fshader = "shaders/fshader_win.glsl";
	#endif
	mp_->init_shaders(vshader.c_str(), fshader.c_str());
	mp_->update_vertex_buffer();
	mp_->update_texture();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutIdleFunc(idle);

	glutMainLoop();

	for (unsigned int i = 0; i < my_meshs.size(); i++)
	{
		delete my_meshs[i];
	}
	delete mp_;

	return 0;
}
