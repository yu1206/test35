
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream> 
#include <io.h>
#include <Windows.h>
#include "opencv\cv.h"
#include "opencv\highgui.h"
#include <direct.h>
using namespace cv;
using namespace std;
typedef struct plateResult
{
	Rect rt;
	string name;
};
extern int readDir(char *dirName, vector<string> &filesName);
#define M_PI 3.14159265358979323846

extern void coordinates44(Point2f src, float angle, Point2f & dst);



// 旋转中心，坐标为车牌中心，旋转区域车牌区域
extern void J_Rotate44(Mat src, int x1, int y1, int x2, int y2, float angle, Mat dst,
	int & l1, int & t1, int & r1, int & b1, Point & pt5, Point & pt6, Point & pt7, Point & pt8);


int read_plate_x2y2_space_plate(char* plateRectTxt, char* plateNameTxt, map<string, vector<plateResult>>& srcPlate,
	int& picture_num, int& plate_num)
{



	fstream f_plate_pos;
	f_plate_pos.open(plateRectTxt, ios::in);
	if (!f_plate_pos.is_open())
	{
		cout << "x2y2文件数据打开错误！" << endl;
		system("pause");
		return false;
	}

	fstream f_plate_name;
	f_plate_name.open(plateNameTxt, ios::in);
	if (!f_plate_name.is_open())
	{
		cout << "文件数据打开错误！" << endl;
		system("pause");
		return false;
	}

	char line[256];
	while (f_plate_pos.getline(line, sizeof(line)))
	{


		plate_num++;

		plateResult vehRe;

		string image1;
		int x1, y1, x2, y2;
		stringstream str_s1(line);
		str_s1 >> image1 >> x1 >> y1 >> x2 >> y2;

		vehRe.rt.x = x1;
		vehRe.rt.y = y1;
		vehRe.rt.width = x2 - x1 + 1;
		vehRe.rt.height = y2 - y1 + 1;



		f_plate_name.getline(line, sizeof(line));
		stringstream str_s3(line);
		string image3;
		string plate_name;
		str_s3 >> image3 >> plate_name;
		vehRe.name = plate_name;

		if (strcmp(image1.c_str(), image3.c_str()) != 0)
		{
			printf("标记的样本不对!\n");
			printf("image1=%s,image3=%s\n", image1.c_str(), image3.c_str());
			system("pause");
		}

		srcPlate[image3].push_back(vehRe);
	}


	picture_num = srcPlate.size();


	f_plate_pos.close();
	f_plate_name.close();


	return 0;
}

int test35_1(int argc, char *argv[])
{
	

	string inputPath = "I:/calibration/16_双层黄牌1060/1";

	string inputp = "I:/calibration/16_双层黄牌1060/txt-right/0_plate_pos-right.txt";
	string inputn = "I:/calibration/16_双层黄牌1060/txt-right/0_plate_name-right.txt";

	string outputPath = "I:/calibration/16_双层黄牌1060/1-rotate-10";
	mkdir(outputPath.c_str());
	string outtxtp = "I:/calibration/16_双层黄牌1060/1-rotate-10/0_plate_pos-right.txt";
	string outtxtn = "I:/calibration/16_双层黄牌1060/1-rotate-10/0_plate_name-right.txt";
	

	/*string inputPath = argv[1];
	string inputp = argv[2];
	string inputn = argv[3];
	string outputPath = argv[4];
	mkdir(outputPath.c_str());
	string outtxtp = argv[5];
	string outtxtn = argv[6];*/


	/*fstream finReadp;
	finReadp.open(inputp, ios::in);
	if (!finReadp.is_open())
	{
		cout << "finRead 文件数据打开错误！" << endl;
		system("pause");
		return false;
	}

	fstream finReadn;
	finReadn.open(inputn, ios::in);
	if (!finReadn.is_open())
	{
		cout << "finRead 文件数据打开错误！" << endl;
		system("pause");
		return false;
	}
*/

	map<string, vector<plateResult>> srcPlate;
	int picNum = 0;  int plateNum = 0;
	read_plate_x2y2_space_plate((char*)inputp.c_str(), (char*)inputn.c_str(), srcPlate, picNum, plateNum);

	

	fstream finWritep;
	finWritep.open(outtxtp, ios::out);
	if (!finWritep.is_open())
	{
		cout << "finRead 文件数据打开错误！" << endl;
		system("pause");
		return false;
	}

	fstream finWriten;
	finWriten.open(outtxtn, ios::out);
	if (!finWriten.is_open())
	{
		cout << "finRead 文件数据打开错误！" << endl;
		system("pause");
		return false;
	}




	vector<string> v_img_;
	readDir((char*)inputPath.c_str(), v_img_);
	srand((unsigned)time(NULL));
	for (int i = 0; i < v_img_.size(); i++)
	{


		int deg1 = rand() % 10 + 1;
		int deg2 = -rand() % 10 - 1;
		

		/*v_img_[i] = "I:/calibration/6_韶关1800/1/1659.jpg";
		int deg1 = 1;
		int deg2 = -2;*/
	
		cout << v_img_[i] << endl;
		cout << "deg=" <<deg1<<","<<deg2<< endl;

		int npos = v_img_[i].find_last_of('/');
		int npos2 = v_img_[i].find_last_of('.');
		string name1 = v_img_[i].substr(npos + 1, npos2 - npos - 1);
		Mat img = imread(v_img_[i].c_str());
		if (img.data == NULL)
		{

			printf("图像为空!\n");
			cout << v_img_[i].c_str() << endl;
			system("pause");
		}

		


		
		int  degarr1[2] = { deg1, deg2 };
		for (int j = 0; j <2; j++)
		{

			int degree = degarr1[j];
			double radian = M_PI*degree*1.0 / 180;

			Mat dst = img.clone();
			
			Point pt5, pt6, pt7, pt8;
			
			int left = 0; int top = 0; int right = 0; int bottom = 0;

			vector<plateResult> vehRe;
			map<string, vector<plateResult>>::iterator key = srcPlate.find(name1);
			if (key != srcPlate.end())
			{
				vehRe = key->second;
			}
			else
			{
				//printf("图片和txt名字不一样!\n");
				//system("pause");
				continue;
			}
			for (int k = 0; k < vehRe.size(); k++)
			{
				plateResult vr = vehRe[k];
				int x1 = vr.rt.x; int y1 = vr.rt.y; int x2 = vr.rt.x + vr.rt.width - 1;
				int y2 = vr.rt.y + vr.rt.height;
				J_Rotate44(img, x1, y1, x2, y2, radian, dst, left, top, right, bottom, pt5, pt6, pt7, pt8);
			}
			
			

			char intstr[128];
			sprintf(intstr, "-%da%d",j,degree);

			string str1 = outputPath + "/" + name1 + intstr + ".jpg";
			imwrite(str1.c_str(), dst);

			
			string name3 = name1 + intstr;
			for (int k = 0; k < vehRe.size(); k++)
			{
				plateResult vr = vehRe[k];
				int x1 = vr.rt.x; int y1 = vr.rt.y; int x2 = vr.rt.x + vr.rt.width - 1;
				int y2 = vr.rt.y + vr.rt.height;
				string nameplate = vr.name;
				finWritep << name3 << " " << x1 << " " << y1 << " " << x2 << " " << y2 << endl;
				finWriten << name3 << " " << nameplate << endl;
			}
			
			
			
		}


		int jjjjjj = 90;

	}
	finWritep.close();
	finWriten.close();
	
	return 0;
}
int test35_2(int argc, char *argv[])
{


	string inputPath = "I:/calibration/16_双层黄牌1060/1";

	string inputp = "I:/calibration/16_双层黄牌1060/txt-right/0_plate_pos-right.txt";
	string inputn = "I:/calibration/16_双层黄牌1060/txt-right/0_plate_name-right.txt";

	string outputPath = "I:/calibration/16_双层黄牌1060/1-rotate-20";
	mkdir(outputPath.c_str());
	string outtxtp = "I:/calibration/16_双层黄牌1060/1-rotate-20/0_plate_pos-right.txt";
	string outtxtn = "I:/calibration/16_双层黄牌1060/1-rotate-20/0_plate_name-right.txt";

	/*string inputPath = argv[7];
	string inputp = argv[8];
	string inputn = argv[9];
	string outputPath = argv[10];
	mkdir(outputPath.c_str());
	string outtxtp = argv[11];
	string outtxtn = argv[12];*/


	/*fstream finReadp;
	finReadp.open(inputp, ios::in);
	if (!finReadp.is_open())
	{
		cout << "finRead 文件数据打开错误！" << endl;
		system("pause");
		return false;
	}

	fstream finReadn;
	finReadn.open(inputn, ios::in);
	if (!finReadn.is_open())
	{
		cout << "finRead 文件数据打开错误！" << endl;
		system("pause");
		return false;
	}*/


	map<string, vector<plateResult>> srcPlate;
	int picNum = 0;  int plateNum = 0;
	read_plate_x2y2_space_plate((char*)inputp.c_str(), (char*)inputn.c_str(), srcPlate, picNum, plateNum);



	fstream finWritep;
	finWritep.open(outtxtp, ios::out);
	if (!finWritep.is_open())
	{
		cout << "finRead 文件数据打开错误！" << endl;
		system("pause");
		return false;
	}

	fstream finWriten;
	finWriten.open(outtxtn, ios::out);
	if (!finWriten.is_open())
	{
		cout << "finRead 文件数据打开错误！" << endl;
		system("pause");
		return false;
	}




	vector<string> v_img_;
	readDir((char*)inputPath.c_str(), v_img_);
	srand((unsigned)time(NULL));
	for (int i = 0; i < v_img_.size(); i++)
	{
		int deg1 = rand() % 10 + 1+10;
		int deg2 = -rand() % 10 - 1-10;


		/*v_img_[i] = "I:/calibration/0_乌鲁木齐800/1/0024.jpg";
		int deg1 = 10;
		int deg2 = -10;*/

		cout << v_img_[i] << endl;
		cout << "deg=" << deg1 << "," << deg2 << endl;

		int npos = v_img_[i].find_last_of('/');
		int npos2 = v_img_[i].find_last_of('.');
		string name1 = v_img_[i].substr(npos + 1, npos2 - npos - 1);
		Mat img = imread(v_img_[i].c_str());
		if (img.data == NULL)
		{

			printf("图像为空!\n");
			cout << v_img_[i].c_str() << endl;
			system("pause");
		}





		int  degarr1[2] = { deg1, deg2 };
		for (int j = 0; j <2; j++)
		{

			int degree = degarr1[j];
			double radian = M_PI*degree*1.0 / 180;

			Mat dst = img.clone();

			Point pt5, pt6, pt7, pt8;

			int left = 0; int top = 0; int right = 0; int bottom = 0;

			vector<plateResult> vehRe;
			map<string, vector<plateResult>>::iterator key = srcPlate.find(name1);
			if (key != srcPlate.end())
			{
				vehRe = key->second;
			}
			else
			{
				//printf("图片和txt名字不一样!\n");
				//system("pause");
				continue;
			}
			for (int k = 0; k < vehRe.size(); k++)
			{
				plateResult vr = vehRe[k];
				int x1 = vr.rt.x; int y1 = vr.rt.y; int x2 = vr.rt.x + vr.rt.width - 1;
				int y2 = vr.rt.y + vr.rt.height;
				J_Rotate44(img, x1, y1, x2, y2, radian, dst, left, top, right, bottom, pt5, pt6, pt7, pt8);
			}



			char intstr[128];
			sprintf(intstr, "-%da%d", j, degree);

			string str1 = outputPath + "/" + name1 + intstr + ".jpg";
			imwrite(str1.c_str(), dst);


			string name3 = name1 + intstr;
			for (int k = 0; k < vehRe.size(); k++)
			{
				plateResult vr = vehRe[k];
				int x1 = vr.rt.x; int y1 = vr.rt.y; int x2 = vr.rt.x + vr.rt.width - 1;
				int y2 = vr.rt.y + vr.rt.height;
				string nameplate = vr.name;
				finWritep << name3 << " " << x1 << " " << y1 << " " << x2 << " " << y2 << endl;
				finWriten << name3 << " " << nameplate << endl;
			}



		}


		int jjjjjj = 90;

	}
	finWritep.close();
	finWriten.close();

	return 0;
}

int test35_3(int argc, char *argv[])
{


	//string inputPath = "I:/mtcnn-train/rotate1";

	string inputPath = "I:/mtcnn-train/rotate1";


	string outputsrc = "I:/mtcnn-train/rotateResult/src";
	mkdir(outputsrc.c_str());


	string outputsrc1 = "I:/mtcnn-train/rotateResult/src1";
	mkdir(outputsrc1.c_str());


	string outputdraw = "I:/mtcnn-train/rotateResult/draw";
	mkdir(outputdraw.c_str());

	

	

	




	vector<string> v_img_;
	readDir((char*)inputPath.c_str(), v_img_);
	srand((unsigned)time(NULL));
	for (int i = 0; i < v_img_.size(); i++)
	{
		int deg1 = rand() % 10 + 1 + 20;
		int deg2 = -rand() % 10 - 1 - 20;


		/*v_img_[i] = "I:/calibration/0_乌鲁木齐800/1/0024.jpg";
		int deg1 = 10;
		int deg2 = -10;*/

		cout << v_img_[i] << endl;
		cout << "deg=" << deg1 << "," << deg2 << endl;

		int npos = v_img_[i].find_last_of('/');
		int npos2 = v_img_[i].find_last_of('.');
		string name1 = v_img_[i].substr(npos + 1, npos2 - npos - 1);
		Mat img = imread(v_img_[i].c_str());
		if (img.data == NULL)
		{

			printf("图像为空!\n");
			cout << v_img_[i].c_str() << endl;
			system("pause");
		}

		fstream finRead;
		string inputPathtxt = inputPath + "/"+name1 + ".txt";
		finRead.open(inputPathtxt, ios::in);
		if (!finRead.is_open())
		{
			cout << "finRead1 文件数据打开错误！" << endl;
			system("pause");
			return false;
		}
		string str; int label; int x1, y1, width, height,x2,y2;
		finRead >> str >> label >> x1 >> y1 >> width >> height;
		x2 = x1 + width - 1;
		y2 = y1 + height - 1;
		
		finRead.close();

		int  degarr1[2] = { deg1, deg2 };
		for (int j = 0; j <2; j++)
		{


			int degree = degarr1[j];
			double radian = M_PI*degree*1.0 / 180;

			Mat dst = img.clone();

			Point pt5, pt6, pt7, pt8;

			int left = 0; int top = 0; int right = 0; int bottom = 0;

		
			J_Rotate44(img, x1, y1, x2, y2, radian, dst, left, top, right, bottom, pt5, pt6, pt7, pt8);
			



			char intstr[128];
			sprintf(intstr, "-%da%d", j, degree);

			string str1 = outputsrc + "/" + name1 + intstr + ".jpg";
			imwrite(str1.c_str(), dst);

			Mat drawimg = dst.clone();
			rectangle(drawimg, Point(left,top), Point(right,bottom), Scalar(0, 0, 255));
			string str2 = outputdraw + "/" + name1 + intstr + ".jpg";
			imwrite(str2.c_str(), drawimg);
			string outputtxt = outputsrc + "/" + name1 + intstr + ".txt";
			fstream finWrite;
			finWrite.open(outputtxt, ios::out);

			if (!finWrite.is_open())
			{
				cout << "finRead2 文件数据打开错误！" << endl;
				system("pause");
				return false;
			}

			finWrite << str1 << " " << left << " " << top << " " << right - left + 1 << " " << bottom - top + 1 << endl;
			finWrite.close();



			string str3 = outputsrc1 + "/" + name1 + intstr + ".jpg";
			imwrite(str3.c_str(), dst);

			string outputtxt1 = outputsrc1 + "/" + name1 + intstr + ".txt";
			fstream finWrite1;
			finWrite1.open(outputtxt1, ios::out);

			if (!finWrite1.is_open())
			{
				cout << "finRead3 文件数据打开错误！" << endl;
				system("pause");
				return false;
			}

			finWrite1 << str3 << " " << x1 << " " << y1 << " " << width << " " << height << endl;
			finWrite1.close();

		}


		int jjjjjj = 90;

	}
	

	return 0;
}
int test35(int argc, char *argv[])
{

	
	/*argv[1] = "I:/calibration/6_韶关1800/1";
	argv[2] =  "I:/calibration/6_韶关1800/txt-right/0_plate_pos-right.txt";
	argv[3] = "I:/calibration/6_韶关1800/txt-right/0_plate_name-right.txt";
	argv[4] = "I:/calibration/6_韶关1800/1-rotate-10";
	argv[5] = "I:/calibration/6_韶关1800/1-rotate-10/0_plate_pos-right.txt";
	argv[6] = "I:/calibration/6_韶关1800/1-rotate-10/0_plate_name-right.txt";

	argv[7] = "I:/calibration/6_韶关1800/1";
	argv[8] = "I:/calibration/6_韶关1800/txt-right/0_plate_pos-right.txt";
	argv[9] = "I:/calibration/6_韶关1800/txt-right/0_plate_name-right.txt";
	argv[10] = "I:/calibration/6_韶关1800/1-rotate-20";
	argv[11] = "I:/calibration/6_韶关1800/1-rotate-20/0_plate_pos-right.txt";
	argv[12] = "I:/calibration/6_韶关1800/1-rotate-20/0_plate_name-right.txt";*/



	//test35_1(argc, argv);


	//test35_2(argc, argv);

	// ken ni ya 
	test35_3(argc, argv);

	system("pause");

	return 0;
}








