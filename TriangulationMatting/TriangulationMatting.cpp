#include <iostream>
#include <Eigen/Dense>
#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/core/hal/interface.h>

cv::Vec3b get_pixel(cv::Mat img, int x, int y) {
	return img.ptr<cv::Vec3b>(y)[x];
}

void set_pixel(cv::Mat img, cv::Vec3b pixel, int x, int y) {
	img.ptr<cv::Vec3b>(y)[x] = pixel;
}

cv::Mat triangulation_matting(cv::Mat fg_img_1, cv::Mat fg_img_2, cv::Mat bg_img_1, cv::Mat bg_img_2) {
	const int A_cols = 4;
	const int A_rows = 6;
	const int x_length = 4;
	const int b_length = 6;

	Eigen::MatrixXf A(A_rows, A_cols);
	Eigen::VectorXf X(x_length);
	Eigen::VectorXf b(b_length);

	A << 1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0;

	cv::Mat alpha(fg_img_1.rows, fg_img_1.cols, CV_8UC3);

	for (int x = 0; x < fg_img_1.cols; x++) {
		for (int y = 0; y < fg_img_1.rows; y++) {
			cv::Vec3b bg_1_pixel = get_pixel(bg_img_1, x, y);
			cv::Vec3b bg_2_pixel = get_pixel(bg_img_2, x, y);
			cv::Vec3b fg_1_pixel = get_pixel(fg_img_1, x, y);
			cv::Vec3b fg_2_pixel = get_pixel(fg_img_2, x, y);

			A(0, 3) = -bg_1_pixel[0];
			A(1, 3) = -bg_1_pixel[1];
			A(2, 3) = -bg_1_pixel[2];
			A(3, 3) = -bg_2_pixel[0];
			A(4, 3) = -bg_2_pixel[1];
			A(5, 3) = -bg_2_pixel[2];

			b(0) = fg_1_pixel[0] - bg_1_pixel[0];
			b(1) = fg_1_pixel[1] - bg_1_pixel[1];
			b(2) = fg_1_pixel[2] - bg_1_pixel[2];
			b(3) = fg_2_pixel[0] - bg_2_pixel[0];
			b(4) = fg_2_pixel[1] - bg_2_pixel[1];
			b(5) = fg_2_pixel[2] - bg_2_pixel[2];

			X = A.colPivHouseholderQr().solve(b);

			if (X(0) < 0) { X(0) = 0; }
			else if (X(0) > 255) { X(0) = 255; }
						
			if (X(1) < 0) { X(1) = 0; }
			else if (X(1) > 255) { X(1) = 255; }
						
			if (X(2) < 0) { X(2) = 0; }
			else if (X(2) > 255) { X(2) = 255; }
						
			if (X(3) < 0) { X(3) = 0; }
			else if (X(3) > 1) { X(3) = 1; }
			
			//set_pixel(alpha, cv::Vec3b((int)(X(3) * X(0)), (int)(X(3) * X(1)), (int)(X(3) * X(2))), x, y); //Use this piece of code if you would like to see the actual RGB image instead of a binarised image
			set_pixel(alpha, cv::Vec3b((int)(X(3) * 255), (int)(X(3) * 255), (int)(X(3) * 255)), x, y);
		}
	}

	return alpha;
}

int main()
{
	std::string input;

	std::cout << "Triangulation image matting, from the 1996 paper 'Blue Screen Matting' by Alvy Ray Smith and James F. Blinn. \n\n";
	std::cout << "Parameters: [image_1] [image_2] [image_1_background] [image_2_background] \n";

	while (std::getline(std::cin, input)) {
		std::istringstream sstream(input);
		std::vector<std::string> args(4);

		try
		{
			sstream >> args[0] >> args[1] >> args[2] >> args[3];
		}

		catch (const std::exception& e)
		{
			std::cout << "Invalid input. Parameters: [image_1] [image_2] [image_1_background] [image_2_background] \n";
			continue;
		}

		//cv::Mat fg_img_1 = cv::imread("C:/Projects/TriangulationMatting/TriangulationMatting/flowersA.jpg", cv::IMREAD_COLOR);
		//cv::Mat fg_img_2 = cv::imread("C:/Projects/TriangulationMatting/TriangulationMatting/flowersB.jpg", cv::IMREAD_COLOR);
		//cv::Mat bg_img_1 = cv::imread("C:/Projects/TriangulationMatting/TriangulationMatting/backA.jpg", cv::IMREAD_COLOR);
		//cv::Mat bg_img_2 = cv::imread("C:/Projects/TriangulationMatting/TriangulationMatting/backB.jpg", cv::IMREAD_COLOR);

		std::cout << "Computing alpha matte... \n";

		cv::Mat fg_img_1 = cv::imread(args[0], cv::IMREAD_COLOR);
		cv::Mat fg_img_2 = cv::imread(args[1], cv::IMREAD_COLOR);
		cv::Mat bg_img_1 = cv::imread(args[2], cv::IMREAD_COLOR);
		cv::Mat bg_img_2 = cv::imread(args[3], cv::IMREAD_COLOR);

		cv::Mat alpha = triangulation_matting(fg_img_1, fg_img_2, bg_img_1, bg_img_2);

		std::cout << "Done! \n";

		cv::imshow("Image 1", fg_img_1);
		cv::imshow("Image 2", fg_img_2);
		cv::imshow("Image 1 Background", bg_img_1);
		cv::imshow("Image 2 Background", bg_img_2);
		cv::imshow("Alpha", alpha);
		cv::imwrite("alpha.jpg", alpha);
		cv::waitKey(0);

		char c;
		std::cin >> c;
		std::cout << "Parameters: [image_1] [image_2] [image_1_background] [image_2_background] \n";
	}


	return 0;
}

